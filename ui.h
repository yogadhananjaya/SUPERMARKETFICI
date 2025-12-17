#ifndef UI_H
#define UI_H

#include "config.h"

void setPinkTheme();
void textNormal();
void maximizeConsole();
void removeScrollbar();
void goFullscreen();
void gotoxy(int x, int y);
void hideCursor();
void setCursorVisible(BOOL visible);
void drawFullFrame();
void drawHeader();
void clearRightContent();
void printCenterRight(int y, const char* text);
int getCenterXForTable(int tableWidth);
void drawBaseLayout(const char* sidebarTitle);
void printMenuItem(int x, int y, char* text, int isSelected);
void drawFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight);
void drawTableBox(int startX, int startY, int width, int height);
void loadingAnimation();
void drawMainMenuStatic();
void updateMainMenuOptions(int selected);
void drawNavigationLegend(const char* legend);
void getString(char *buffer, int maxLen);
long getLongInput(int inputX, int inputY);
void getPassword(char *buffer, int maxLen, int inputX, int inputY, int* isVisible);
int loginScreen(int *loggedIndex);
void getValidatedString(char *buffer, int maxLen, int x, int y);
long getValidatedNumber(int x, int y);
void getValidatedPhoneNumber(char *buffer, int maxLen, int x, int y);

#endif