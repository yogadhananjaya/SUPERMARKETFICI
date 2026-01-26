#ifndef UI_H
#define UI_H
#include "ui_config.h"

// ... (fungsi lain tetap sama) ...
void formatRupiah(long number, char *buffer);
int getConfirmation(int x, int y, const char *msg);
void setPinkTheme();
void textNormal();
void textHighlightTheme();
void gotoxy(int x, int y);
void setCursorVisible(BOOL visible);
void hideCursor();
void maximizeConsole();
void goFullscreen();
void drawFullFrame();
void drawHeader();
void drawBaseLayout(const char* sidebarTitle);
void printMenuItem(int x, int y, char* text, int isSelected);
void drawNavigationLegend(const char* legend);
void drawHomeLogo(int role);
void clearRightContent();
void printCenterRight(int y, const char* text);
int getCenterXForTable(int tableWidth);
void drawTableBox(int startX, int startY, int width, int height);
void drawShadowBox(int x, int y, int w, int h);
void drawFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight);
// NEW
void drawSimpleFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight);

void drawBreadcrumbs(const char* path);
void drawSummaryCard(int x, int y, const char* title, int value, const char* unit);
void showDashboardHome(int role);
void loadingAnimation();
void showErrorAndWait(int x, int y, const char* message);
void drawPerformanceVisual(int x, int y, int percent);
int isNumeric(const char* str);
int getString(char *buffer, int maxLen);
int getPassword(char *buffer, int maxLen, int inputX, int inputY, int* isVisible);
void getValidatedString(char *buffer, int maxLen, int x, int y);
long getValidatedNumber(int x, int y);
void getValidatedPhoneNumber(char *buffer, int maxLen, int x, int y, int checkType, int ignoreID);
int loginScreen(int *loggedIndex);

#endif