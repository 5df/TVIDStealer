
#include "log.h"
#include <Windows.h>
#include <stdio.h>

FILE *logfile;

void log_open(char *name){
    logfile = fopen(name,"a+");
    if(!logfile){
        logfile=stdout;
    }
}

void logify(char *where, int why, int status){
    char error[64];

    if(status==EERRO || status==ECRIT){
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                      0,why,0,error,64,NULL);
    }
    switch(status){
    case EERRO:
        fprintf(logfile,"%s %s error: %s\n","[-]",where,error);
        break;
    case ECRIT:
        fprintf(logfile,"%s %s error: %s\n","[!]",where,error);
        break;
    case EINFO:
        fprintf(logfile,"%s %s\n","[*]",where);
        break;
    case ESUCC:
        fprintf(logfile,"%s %s: ok\n","[+]",where);
        break;
    }
}

void log_close(){
    if(logfile)
        fclose(logfile);
}
