#include "registry.h"
#include "log.h"

BOOL add_registry_key(char *keyname, DWORD value){
    HKEY key;
    LONG result;
    DWORD valuelen=sizeof(value);

    if(check_registry_key(keyname)){
        result=RegOpenKeyEx(REG_ROOT,TEXT(REG_SUB_KEY),0,KEY_WRITE,&key);
        if(ERROR_SUCCESS!=result){
            logify("RegOpenKeyEx",result,EERRO);
            return FALSE;
        }
    }else{
        result=RegCreateKeyEx(REG_ROOT,TEXT(REG_SUB_KEY),0,NULL,REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,NULL,&key,NULL);
        if(ERROR_SUCCESS!=result){
            logify("RegCreateKeyEx",result,EERRO);
            return FALSE;
        }
    }

    result=RegSetValueEx(key,keyname,0,REG_DWORD,(BYTE *)&value,valuelen);
    if(ERROR_SUCCESS!=result){
        logify("RegSetValueEx",result,EERRO);
        return FALSE;
    }
    RegCloseKey(key);
    return TRUE;
}

BOOL check_registry_key(char *keyname){
    HKEY key;
    LONG result;

    result=RegOpenKeyEx(REG_ROOT,TEXT(REG_SUB_KEY),0,KEY_READ,&key);
    if(ERROR_SUCCESS!=result){
        logify("RegOpenKeyEx",result,EERRO);
        return FALSE;
    }

    result=RegQueryValueEx(key,keyname,0,NULL,NULL,NULL);
    if(ERROR_SUCCESS!=result){
        logify("RegGetValue",result,EERRO);
        return FALSE;
    }
    RegCloseKey(key);
    return TRUE;
}

