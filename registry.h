#include <stdio.h>
#include <stdlib.h>

#include <Windows.h>
#include <Winreg.h>

/* Warning ! If not run as administrator (or system), the key will be putted in HKEY_USER/<USER-SID>_Classes/VirtualStore/Machine
 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa965884(v=vs.85).aspx
 */
#define REG_ROOT HKEY_LOCAL_MACHINE
/* Win32 registry redirector and registry reflection take care of putting the key in "Wow6432Node" if needed
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms724072(v=vs.85).aspx
 */
#define REG_SUB_KEY "Software\\OCS Inventory NG\\Agent\\PluginTV"

/* Key name */
#define REG_KEY_NAME "ClientID"

BOOL add_registry_key(DWORD value);
BOOL check_registry_key();
void reg_error(char *src, LONG code);
