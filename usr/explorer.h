#ifndef _EXPLORER_H
#define _EXPLORER_H

enum ITEMTYPE {
    IT_FOLDER,//A folder contains sub-files.
    IT_TXT,//Text files that can be edited.
    IT_BIN,//Binary files.
    IT_OTHER //None of the above.
};

struct itemUi {
    char name[32];//Name + ext.
    enum ITEMTYPE type;//The file type.
    int size;//The file size.
    int time;//Last fetch time.
    int row, col;//The row and column where it should been displayed.
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