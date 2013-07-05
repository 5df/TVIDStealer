#include <stdlib.h>

#include <Windows.h>
#include <Winuser.h> /* Enum*Window* | FindWindow* | SendMessage */

#define SLEEP 1000 /* Temps d'attente en milisecondes entre les checks */
#define TIMEOUT 120 /* timeout (en SLEEP) pour la recherche de teamviewer, 0 equivaut à ne jamais timeout */
#define TV_ID_LEN 12 /* Taille d'un id teamviewer (eg. "000 111 222") */
#define TV_PASS_LEN 5 /* Taille d'un password teamviewer (eg "3610") */
#define TV_BUFF_LEN 12

#define TV_WINDOW_NAME_MAXLEN 25

char *tvWindowNames[TV_WINDOW_NAME_MAXLEN]={\
                    "TeamViewer QuickSupport",\
                    "TeamViewer",
                    NULL
};
