#include <stdio.h>
#include <stdlib.h>

#include <Windows.h>
#include <Winuser.h> /* Enum*Window* | FindWindow* | SendMessage */

#define TV_PROCESS_NAME "TeamViewer QuickSupport" /* Nom du processus TeamViewer */
#define SLEEP 1000 /* Temps d'attente en milisecondes entre les checks */
#define TIMEOUT 120 /* timeout (en SLEEP) pour la recherche de teamviewer, 0 equivaut à ne jamais timeout */
#define TV_ID_LEN 12 /* Taille d'un id teamviewer (eg. "000 111 222") */

BOOL is_tvId(char *sid, int *id);
BOOL CALLBACK find_tvId (HWND hwnd, LPARAM lParam);
