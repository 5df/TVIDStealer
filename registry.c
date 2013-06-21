#include "registry.h"

BOOL add_registry_key(DWORD value){
    HKEY key;
    LONG result;
    DWORD valuelen=sizeof(value);

    result=RegCreateKeyEx(REG_ROOT,TEXT(REG_SUB_KEY),0,NULL,REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,NULL,&key,NULL);
    if(ERROR_SUCCESS!=result){
        reg_error("RegCreateKeyEx",result);
        return FALSE;
    }

    result=RegSetValueEx(key,TEXT(REG_KEY_NAME),0,REG_DWORD,(BYTE *)&value,valuelen);
    if(ERROR_SUCCESS!=result){
        reg_error("RegSetValueEx", result);
        return FALSE;
    }
    RegCloseKey(key);
    return TRUE;
}

BOOL check_registry_key(){
    HKEY key;
    LONG result;

    result=RegOpenKeyEx(REG_ROOT,TEXT(REG_SUB_KEY),0,KEY_READ,&key);
    if(ERROR_SUCCESS!=result){
        reg_error("RegOpenKeyEx", result);
        return FALSE;
    }

    result=RegQueryValueEx(key,TEXT(REG_KEY_NAME),0,NULL,NULL,NULL);
    if(ERROR_SUCCESS!=result){
        reg_error("RegGetValue", result);
        return FALSE;
    }
    RegCloseKey(key);
    return TRUE;
}


void reg_error(char *src, LONG code){
    char error[64];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                      0,code,0,error,64,NULL);
    fprintf(stderr,"[-] %s:%s\n",src,error);
}

