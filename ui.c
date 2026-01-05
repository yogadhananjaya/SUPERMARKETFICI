#include "ui.h"
#include "globals.h"
#include "ui_config.h"
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>

// ==============================================================================
// 1. KONFIGURASI WARNA & TEMA
// ==============================================================================

void setPinkTheme() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hOut, &csbi);
    csbi.ColorTable[13] = RGB_PRIMARY_BG;
    csbi.ColorTable[15] = RGB_PRIMARY_FG;
    SetConsoleScreenBufferInfoEx(hOut, &csbi);
}

void textNormal() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), THEME_PRIMARY_COLOR);
}

void textHighlightTheme() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), THEME_HIGHLIGHT_COLOR);
}

// ==============================================================================
// 2. UTILITAS LAYAR (WINDOW & CURSOR)
// ==============================================================================

void gotoxy(int x, int y) {
    if (x < 0) x = 0; if (y < 0) y = 0;
    COORD coord = {(short)x, (short)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setCursorVisible(BOOL visible) {
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
    ci.bVisible = visible;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

void hideCursor() { setCursorVisible(FALSE); }

void maximizeConsole() {
    ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
    Sleep(100);
}

void goFullscreen() {
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1C, 0, 0);
    keybd_event(VK_RETURN, 0x1C, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
}

COORD getCurrentCursorPosition() {
    COORD coord = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        coord = csbi.dwCursorPosition;
    }
    return coord;
}

// ==============================================================================
// 3. DRAWING FRAME & LAYOUT UTAMA
// ==============================================================================

void drawTableBox(int startX, int startY, int width, int height) {
    textNormal();
    for(int x = startX+1; x < startX+width; x++) {
        gotoxy(x, startY); printf("%c", CH_HLINE);
        gotoxy(x, startY+height); printf("%c", CH_HLINE);
    }
    for(int y = startY+1; y < startY+height; y++) {
        gotoxy(startX, y); printf("%c", CH_VLINE);
        gotoxy(startX+width, y); printf("%c", CH_VLINE);
    }
    gotoxy(startX, startY); printf("%c", CH_TOP_LEFT);
    gotoxy(startX+width, startY); printf("%c", CH_TOP_RIGHT);
    gotoxy(startX, startY+height); printf("%c", CH_BOT_LEFT);
    gotoxy(startX+width, startY+height); printf("%c", CH_BOT_RIGHT);
}

void drawShadowBox(int x, int y, int w, int h) {
    drawTableBox(x, y, w, h);
    for(int i = 1; i <= h; i++) {
        gotoxy(x + w + 1, y + i); printf("%c", 176);
    }
    for(int i = 1; i <= w; i++) {
        gotoxy(x + i + 1, y + h + 1); printf("%c", 176);
    }
}

void drawFullFrame() {
    updateScreenSize();
    int r = screenWidth-1, b = screenHeight-1;
    textNormal();
    for (int x = 0; x <= r; x++) {
        gotoxy(x, 0); printf("%c", CH_HLINE);
        gotoxy(x, b); printf("%c", CH_HLINE);
        gotoxy(x, HEADER_HEIGHT); printf("%c", CH_HLINE);
    }
    for (int y = 0; y <= b; y++) {
        gotoxy(0, y); printf("%c", CH_VLINE);
        gotoxy(r, y); printf("%c", CH_VLINE);
        if(isSidebarActive && y > HEADER_HEIGHT) {
            gotoxy(SIDEBAR_WIDTH, y); printf("%c", CH_VLINE);
        }
    }
    gotoxy(0, 0); printf("%c", CH_TOP_LEFT);
    gotoxy(r, 0); printf("%c", CH_TOP_RIGHT);
    gotoxy(0, b); printf("%c", CH_BOT_LEFT);
    gotoxy(r, b); printf("%c", CH_BOT_RIGHT);
    if(isSidebarActive) {
        gotoxy(SIDEBAR_WIDTH, HEADER_HEIGHT); printf("%c", CH_TEE_TOP);
        gotoxy(SIDEBAR_WIDTH, b); printf("%c", CH_TEE_BOT);
    }
}

void drawHeader() {
    const char *logo[] = {
        "   _____ _    _ _____  ______ _____  __  __           _____  _  ________ _________ ",
        "  / ____| |  | |  __ \\|  ____|  __ \\|  \\/  |    /\\    |  __ \\| |/ /  ____|__   __|",
        " | (___ | |  | | |__) | |__  | |__) | \\  / |   /  \\   | |__) | ' /| |__     | |   ",
        "  \\___ \\| |  | |  ___/|  __| |  _  /| |\\/| |  / /\\ \\  |  _  /|  < |  __|    | |   ",
        "  ____) | |__| | |    | |____| | \\ \\| |  | | / ____ \\ | | \\ \\| . \\| |____   | |   ",
        " |_____/ \\____/|_|    |______|_|  \\_\\_|  |_|/_/    \\_\\|_|  \\_\\_|\\_\\______|  |_|   "
    };
    int startX = (screenWidth - 85) / 2;
    for (int i = 0; i < 6; i++) {
        gotoxy(startX > 0 ? startX : 0, 1 + i); printf("%s", logo[i]);
    }
}

// ==============================================================================
// 4. NAVIGATION & CONTENT HELPERS
// ==============================================================================

void updateSidebarTitle(const char* title) {
    gotoxy(2, HEADER_HEIGHT+3); printf("                         ");
    gotoxy(2, HEADER_HEIGHT+3); textHighlightTheme(); printf(" %s ", title); textNormal();
}

void drawBaseLayout(const char* sidebarTitle) {
    isSidebarActive = 1; system("cls"); drawFullFrame(); drawHeader();
    gotoxy(2, HEADER_HEIGHT+2); printf("MENU SYSTEM"); updateSidebarTitle(sidebarTitle);
}

void printMenuItem(int x, int y, char* text, int isSelected) {
    int w = SIDEBAR_WIDTH-4; gotoxy(x, y);
    if(isSelected) {
        textHighlightTheme(); printf(" %-*s ", w-2, text); textNormal();
    } else {
        printf("  %-*s  ", w-4, text);
    }
}

void drawBreadcrumbs(const char* path) {
    gotoxy(SIDEBAR_WIDTH + 2, HEADER_HEIGHT + 1);
    textNormal();
    printf(" Lokasi: ");
    textHighlightTheme();
    printf(" %s ", path);
    textNormal();
}

void drawStatusBar(const char* leftText, const char* rightText) {
    int y = screenHeight - 1;
    textHighlightTheme();
    gotoxy(0, y);
    for(int i = 0; i < screenWidth; i++) printf(" ");
    gotoxy(2, y); printf("%s", leftText);
    int rightX = screenWidth - (int)strlen(rightText) - 2;
    gotoxy(rightX, y); printf("%s", rightText);
    textNormal();
}

void drawNavigationLegend(const char* legend) {
    int y = screenHeight-2;
    gotoxy(1, y); for(int i=0; i<screenWidth-2; i++) printf(" ");
    gotoxy((screenWidth-(int)strlen(legend))/2, y); printf("%s", legend);
}

// ==============================================================================
// 5. DASHBOARD & VISUALS
// ==============================================================================

void drawSummaryCard(int x, int y, const char* title, int value, const char* unit) {
    int w = 22, h = 4;
    drawShadowBox(x, y, w, h);
    gotoxy(x + 2, y + 1); printf("%s", title);
    gotoxy(x + 2, y + 2);
    textHighlightTheme();
    printf(" %d %s ", value, unit);
    textNormal();
}

// --- PERBAIKAN POSISI LOGO DI SINI ---
void drawHomeLogo() {
    const char *art[] = {
        " ############################ ", " ############################ ",
        " ############################ ", "      ######      ######      ",
        "     #######       ######     ", "     ######        ######     ",
        "     ######  ####  ######     ", "    ######  ######  ######    ",
        "    ######  ######  ######    ", "    ######    ##    #######   ",
        "   ######            ######   ", "   ######             ######  ",
        "  ######              ######  ", "  ######              ######  ",
        " #######               ###### ", NULL
    };

    int artWidth = 30;
    int artHeight = 15;

    // Hitung posisi Pojok Kanan Bawah
    // X: Lebar layar - lebar art - padding kanan (2 spasi)
    int startX = screenWidth - artWidth - 2;

    // Y: Tinggi layar - tinggi art - padding bawah (2 baris dari frame bawah)
    int startY = screenHeight - artHeight - 2;

    // Proteksi agar tidak menabrak sidebar atau header jika layar kekecilan
    if (startX <= SIDEBAR_WIDTH) startX = SIDEBAR_WIDTH + 2;
    if (startY <= HEADER_HEIGHT + 5) startY = HEADER_HEIGHT + 6; // +5 estimasi tinggi summary card

    textNormal();
    for(int i=0; art[i] != NULL; i++) {
        // Pastikan tidak menggambar keluar layar bawah
        if (startY + i >= screenHeight - 1) break;

        gotoxy(startX, startY + i);
        printf("%s", art[i]);
    }
}

void showDashboardHome(int role) {
    clearRightContent();
    drawBreadcrumbs(role == 0 ? "ADMIN > DASHBOARD" : "STAFF > HOME");
    int startX = SIDEBAR_WIDTH + 5;
    int startY = HEADER_HEIGHT + 4;
    drawSummaryCard(startX, startY, "TOTAL PRODUK", totalProduk, "Item");
    drawSummaryCard(startX + 26, startY, "PENJUALAN", totalPenjualan, "Nota");
    drawSummaryCard(startX + 52, startY, "SUPPLIER", totalSupplier, "Mitra");
    drawHomeLogo();
}

void drawPerformanceVisual(int x, int y, int percent) {
    int barLen = 20;
    int filled = (percent * barLen) / 100;
    if (percent <= 35)      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xD4); // Merah (D4)
    else if (percent <= 50) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xD5); // Ungu (D5)
    else if (percent <= 80) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xD1); // Biru (D1)
    else                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xDA); // Hijau (DA)
    gotoxy(x, y);
    printf("[");
    for(int i=0; i<barLen; i++) {
        if(i < filled) printf("%c", 219);
        else printf("%c", 176);
    }
    printf("] %d%%", percent);
    textNormal();
}

// ==============================================================================
// 6. FORM & UTILITY
// ==============================================================================

void clearRightContent() {
    textNormal();
    for(int y = HEADER_HEIGHT+1; y < screenHeight-1; y++) {
        gotoxy(SIDEBAR_WIDTH+1, y); for(int x = SIDEBAR_WIDTH+1; x < screenWidth-1; x++) printf(" ");
    }
}

void printCenterRight(int y, const char* text) {
    gotoxy(SIDEBAR_WIDTH+1+((screenWidth-SIDEBAR_WIDTH-(int)strlen(text))/2), y); printf("%s", text);
}

int getCenterXForTable(int tableWidth) {
    return SIDEBAR_WIDTH+1+((screenWidth-SIDEBAR_WIDTH-tableWidth)/2);
}

void drawFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight) {
    clearRightContent();
    *boxWidth = 70; *boxHeight = 16;
    *startX = SIDEBAR_WIDTH+5; *startY = HEADER_HEIGHT+3;
    gotoxy(*startX+2, *startY); printf(" %s ", title);
    drawShadowBox(*startX, *startY, *boxWidth, *boxHeight);
}

void showErrorAndWait(int x, int y, const char* message) {
    Beep(750, 300); gotoxy(x, y); textHighlightTheme(); printf(" %s ", message); textNormal(); getch();
    gotoxy(x, y); for(int i=0; i<(int)strlen(message)+2; i++) printf(" ");
}

// ==============================================================================
// 7. INPUT ENGINE
// ==============================================================================

int isNumeric(const char* str) {
    while (*str) { if (!isdigit(*str)) return 0; str++; } return 1;
}

int getString(char *buffer, int maxLen) {
    setCursorVisible(TRUE); int i = 0; int key;
    while (1) {
        key = getch(); if (key == 27) return 0; if (key == 13) { buffer[i] = '\0'; break; }
        if (key == 8 && i > 0) { i--; printf("\b \b"); }
        else if (i < maxLen - 1 && isprint(key)) { buffer[i] = (char)key; printf("%c", key); i++; }
    }
    setCursorVisible(FALSE); return 1;
}

int getPassword(char *buffer, int maxLen, int inputX, int inputY, int* isVisible) {
    setCursorVisible(TRUE); int i = 0; int key;
    while(1) {
        gotoxy(inputX + i, inputY); key = getch();
        if (key == 27) return 0; if (key == 13) { buffer[i] = '\0'; break; }
        if (key == 9) {
            *isVisible = !*isVisible; gotoxy(inputX, inputY);
            for(int j=0; j<i; j++) printf("%c", *isVisible ? buffer[j] : '*');
        }
        else if (key == 8 && i > 0) { i--; printf("\b \b"); }
        else if (i < maxLen - 1 && isprint(key)) {
            buffer[i] = (char)key;
            printf("%c", *isVisible ? buffer[i] : '*'); i++;
        }
    }
    setCursorVisible(FALSE); return 1;
}

void getValidatedString(char *buffer, int maxLen, int x, int y) {
    while(1) { gotoxy(x, y); if(!getString(buffer, maxLen)) continue; if(strlen(buffer) > 0) break; showErrorAndWait(x, y+1, "Wajib diisi!"); }
}

long getValidatedNumber(int x, int y) {
    char buf[20]; while(1) { gotoxy(x, y); if(!getString(buf, 19)) continue; if(isNumeric(buf) && strlen(buf) > 0) return atol(buf); showErrorAndWait(x, y+1, "Harus angka!"); }
}

void getValidatedPhoneNumber(char *buffer, int maxLen, int x, int y, int checkType, int ignoreID) {
    while(1) { gotoxy(x, y); if(!getString(buffer, maxLen)) continue; if(isNumeric(buffer) && strlen(buffer) >= 10) break; showErrorAndWait(x, y+1, "Min 10 digit!"); }
}

// ==============================================================================
// 8. HIGH-LEVEL SCREENS (LOADING & LOGIN)
// ==============================================================================

void loadingAnimation() {
    system("cls");
    updateScreenSize();
    int midX = screenWidth / 2, midY = screenHeight / 2;
    textNormal();
    gotoxy(midX - 10, midY - 2); printf("MENYIAPKAN SISTEM...");
    int barWidth = 40;
    int startX = midX - (barWidth / 2);
    gotoxy(startX - 1, midY); printf("[");
    gotoxy(startX + barWidth, midY); printf("]");
    for (int i = 0; i <= barWidth; i++) {
        if (i < barWidth/2) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xDF);
        else SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xDB);
        gotoxy(startX + i - 1, midY);
        if(i > 0) printf("%c", 219);
        textNormal();
        gotoxy(midX - 2, midY + 2); printf("%3d%%", (i * 100) / barWidth);
        Sleep(30);
    }
    Sleep(500);
}

int loginScreen(int *loggedIndex) {
    isSidebarActive = 0;
    char user[20] = "", pass[20] = "";
    int bxW = 60, bxH = 10;
    while(1) {
        updateScreenSize();
        int startX = (screenWidth - bxW) / 2, startY = (screenHeight / 2);
        system("cls");
        drawFullFrame();
        drawHeader();

        textNormal();
        gotoxy(startX + (bxW / 2) - 6, startY - 1);
        printf(">> MENU LOGIN <<");

        drawShadowBox(startX, startY, bxW, bxH);
        gotoxy(startX + 10, startY + 3); printf("Username : ");
        gotoxy(startX + 10, startY + 5); printf("Password : ");
        drawNavigationLegend("[TAB] Intip Password | [ENTER] Login | [ESC] Keluar");

        gotoxy(startX + 22, startY + 3);
        if (!getString(user, 19)) return -1;
        int vis = 0;
        if (!getPassword(pass, 19, startX + 22, startY + 5, &vis)) continue;

        if (strcmp(user, "admin") == 0 && strcmp(pass, "admin") == 0) return 0;
        for(int i=0; i<totalKaryawan; i++) {
            if(strcmp(user, dbKaryawan[i].username) == 0 && strcmp(pass, dbKaryawan[i].password) == 0) {
                *loggedIndex = i; return dbKaryawan[i].roleId;
            }
        }
        showErrorAndWait(startX + 20, startY + 8, "Login Gagal!");
    }
}