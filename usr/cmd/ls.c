#include "../terminal.h"
#include "ls.h"
#include <driver/vga.h>
#include <zjunix/fs/fat.h>

extern int terminalCurX, terminalCurY;

void ls(char *path){
    struct dir_entry_attr entry;
    char name[32];
    char *p = path;
    unsigned int r;
    FS_FAT_DIR dir;

    if (fs_open_dir(&dir, p)) {
        terminalPrint("open dir failed : No such directory!\n",
            terminalCurX, terminalCurY);
    }

readdir:
    r = fs_read_dir(&dir, (unsigned char *)&entry);
    if (1 != r) {
        if (-1 == r) {
            terminalPrint("\n", terminalCurX, terminalCurY);
        } else {
            get_filename((unsigned char *)&entry, name);
            if (entry.attr == 0x10){
                terminalPrint(name, terminalCurX, terminalCurY);
                terminalPrint("/", terminalCurX, terminalCurY);
            }
            else
                terminalPrint(name, terminalCurX, terminalCurY);

            terminalPrint("\n", terminalCurX, terminalCurY);
            goto readdir;
        }
    }

    finishLine();
}