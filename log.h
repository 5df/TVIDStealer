


#define EINFO 1
#define EERRO 2
#define ECRIT 3
#define ESUCC 4

void log_open(char *name);
void logify(char *where, int why, int status);
void log_close();
