#ifndef _EXPLORER_H
#define _EXPLORER_H

enum ITEMTYPE {
    IT_FOLDER,
    IT_TXT,
    IT_BIN,
    IT_OTHER
};

struct itemUi {
    char name[32];
    enum ITEMTYPE type;
    int size;
    int row, col;
};

void explorer();
void explorerBackground();
void explorerKey();

void refreshDir();
void showDir();
void sidePanel();
void closeExplorer();

void clearMem();
int getExt(char *name);

#endif