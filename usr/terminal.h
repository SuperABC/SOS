#ifndef _TERMINAL_H
#define _TERMINAL_H

void terminal();
void terminalBackground();
void terminalKey(int k);

void terminalPrint(char *s, int x, int y);
void rollScrn();
void finishLine();
void interpreteCmd();
void closeTerminal();
char *cmdInst(char *cmd);
char *cmdPara(char *cmd);

#endif