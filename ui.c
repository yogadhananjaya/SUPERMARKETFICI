#include "ui.h"
#include "globals.h"
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>

void formatRupiah(long number, char *buffer) {
    char temp[50];
    sprintf(temp, "%ld", number);
    int len = strlen(temp);
    int i, j = 0;
    for (i = 0; i < len; i++) {
        buffer[j++] = temp[i];
        if ((len - i - 1) % 3 == 0 && (len - i - 1) > 0) buffer[j++] = '.';
    }
    buffer[j] = '\0';
}

int getConfirmation(int x, int y, const char *msg) {
    gotoxy(x, y); textHighlightTheme(); printf(" %s (Y/N)? ", msg); textNormal();
    while(1) {
        char key = getch();
        if (tolower(key) == 'y') return 1;
        if (tolower(key) == 'n' || key == 27) {
            gotoxy(x, y); for(int i=0; i<strlen(msg)+10; i++) printf(" ");
            return 0;
        }
    }
}

void setPinkTheme() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hOut, &csbi);
    csbi.ColorTable[13] = RGB(255, 20, 147);
    csbi.ColorTable[15] = RGB(255, 255, 255);
    SetConsoleScreenBufferInfoEx(hOut, &csbi);
}

void textNormal() { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), THEME_PRIMARY_COLOR); }
void textHighlightTheme() { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), THEME_HIGHLIGHT_COLOR); }
void gotoxy(int x, int y) { COORD coord = {(short)x, (short)y}; SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); }
void setCursorVisible(BOOL visible) { CONSOLE_CURSOR_INFO ci; GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci); ci.bVisible = visible; SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci); }
void hideCursor() { setCursorVisible(FALSE); }
void maximizeConsole() { ShowWindow(GetConsoleWindow(), SW_MAXIMIZE); Sleep(100); }
void goFullscreen() { keybd_event(VK_MENU, 0x38, 0, 0); keybd_event(VK_RETURN, 0x1C, 0, 0); keybd_event(VK_RETURN, 0x1C, KEYEVENTF_KEYUP, 0); keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0); }

void updateScreenSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    screenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

// --- BOX SOLID ---
void drawTableBox(int startX, int startY, int width, int height) {
    textNormal();
    gotoxy(startX, startY); printf("%c", CH_TOP_LEFT);
    for(int i=0; i<width-1; i++) printf("%c", CH_HLINE);
    printf("%c", CH_TOP_RIGHT);
    for(int y = startY+1; y < startY+height; y++) {
        gotoxy(startX, y); printf("%c", CH_VLINE);
        for(int x = 0; x < width-1; x++) printf(" ");
        gotoxy(startX+width, y); printf("%c", CH_VLINE);
    }
    gotoxy(startX, startY+height); printf("%c", CH_BOT_LEFT);
    for(int i=0; i<width-1; i++) printf("%c", CH_HLINE);
    printf("%c", CH_BOT_RIGHT);
}

void drawShadowBox(int x, int y, int w, int h) {
    drawTableBox(x, y, w, h);
    textNormal();
    for(int i = 1; i <= h; i++) { gotoxy(x + w + 1, y + i); printf("%c", 176); }
    for(int i = 1; i <= w + 1; i++) { gotoxy(x + i, y + h + 1); printf("%c", 176); }
}

void drawFullFrame() {
    updateScreenSize(); int r = screenWidth-1, b = screenHeight-1; textNormal();
    for (int x = 0; x <= r; x++) { gotoxy(x, 0); printf("%c", CH_HLINE); gotoxy(x, b); printf("%c", CH_HLINE); gotoxy(x, HEADER_HEIGHT); printf("%c", CH_HLINE); }
    for (int y = 0; y <= b; y++) { gotoxy(0, y); printf("%c", CH_VLINE); gotoxy(r, y); printf("%c", CH_VLINE); if(isSidebarActive && y > HEADER_HEIGHT) { gotoxy(SIDEBAR_WIDTH, y); printf("%c", CH_VLINE); } }
    gotoxy(0, 0); printf("%c", CH_TOP_LEFT); gotoxy(r, 0); printf("%c", CH_TOP_RIGHT); gotoxy(0, b); printf("%c", CH_BOT_LEFT); gotoxy(r, b); printf("%c", CH_BOT_RIGHT);
    if(isSidebarActive) { gotoxy(SIDEBAR_WIDTH, HEADER_HEIGHT); printf("%c", CH_TEE_TOP); gotoxy(SIDEBAR_WIDTH, b); printf("%c", CH_TEE_BOT); }
}

void drawHeader() {
    const char *logo[] = {
        "  ____  _   _ ____  _____ ____  __  __    _    ____  _  _______ _____ ",
        " / ___|| | | |  _ \\| ____|  _ \\|  \\/  |  / \\  |  _ \\| |/ / ____|_   _|",
        " \\___ \\| | | | |_) |  _| | |_) | |\\/| | / _ \\ | |_) | ' /|  _|   | |  ",
        "  ___) | |_| |  __/| |___|  _ <| |  | |/ ___ \\|  _ <| . \\| |___  | |  ",
        " |____/ \\___/|_|  |_____|_| \\_\\_|  |_/_/   \\_\\_|_| \\_\\_|\\_\\_____| |_|  ",
        NULL
    };
    int logoWidth = 73; int startX = (screenWidth - logoWidth) / 2; if (startX < 0) startX = 0;
    textNormal(); for (int i = 0; logo[i] != NULL; i++) { gotoxy(startX, 1 + i); printf("%s", logo[i]); }
}

void drawHomeLogo(int role) {
    const char **art = NULL;
    int artWidth = 0; int artHeight = 0;
    const char *artAdmin[] = { "   _________", "   | _______ |", "  / \\         \\", " /___\\_________\\", " |   | \\       |", " |   |  \\      |", " |   |   \\     |", " |   | F  \\    |", " |   |     \\   |", " |   |\\  I  \\  |", " |   | \\      \\ |", " |   |  \\  C  \\|", " |   |   \\      |", " |   |    \\  I |", " |   |     \\   |", " |   |      \\  |", " |___|_______\\_|", NULL };
    const char *artHeadCashier[] = { "  (  )    (    )  )", "     ) (    )   (  (", "     ( )  (    ) )", "     _____________", "    <_____________> ___", "    |             |/ _ \\", "    |               | | |", "    |               |_| |", " ___|             |\\___/", "/    \\___________/    \\", "\\_____________________/", NULL };
    const char *artHeadWarehouse[] = { " ,--./,-.", " / #      \\", "|          |", " \\        /    ", "  `._,._,'     ", " ,--./,-.", " / #      \\", "|          |", " \\        /    ", "  `._,._,'", " ,--./,-.", " / #      \\", "|          |", " \\        /    ", "  `._,._,'", NULL };
    const char *artStaff[] = { " _", "//\\", "V  \\", " \\  \\_", "  \\,'.`-.", "   |\\ `. `.        ", "   ( \\  `. `-.                         _,.-:\\", "    \\ \\    `.  `-._             __..--' ,-';/", "     \\ `.    `-.    `-..___..---'    _.--' ,'/ ", "      `. `.     `-._         __..--'    ,' /", "        `. `-_      ``--..''        _.-' ,' ", "          `-_ `-.___         __,--'    ,'", "              `-.__  `----\"\"\"    __.-'", "                   `--..____..--'", NULL };

    if (role == ROLE_ADMIN) { art = (const char **)artAdmin; artWidth = 20; artHeight = 17; }
    else if (role == ROLE_HEAD_CASHIER) { art = (const char **)artHeadCashier; artWidth = 28; artHeight = 11; }
    else if (role == ROLE_HEAD_WAREHOUSE) { art = (const char **)artHeadWarehouse; artWidth = 16; artHeight = 15; }
    else { art = (const char **)artStaff; artWidth = 50; artHeight = 14; }

    int startX = screenWidth - artWidth - 3;
    int startY = screenHeight - artHeight - 2;
    if (startX > SIDEBAR_WIDTH + 2 && startY > HEADER_HEIGHT) { textNormal(); for(int i=0; art[i] != NULL; i++) { if (startY + i >= screenHeight - 1) break; gotoxy(startX, startY + i); printf("%s", art[i]); } }
}

void showDashboardHome(int role) {
    clearRightContent(); drawBreadcrumbs(role == ROLE_ADMIN ? "ADMIN > DASHBOARD" : "STAFF > HOME");
    int startX = SIDEBAR_WIDTH + 5, startY = HEADER_HEIGHT + 4;
    drawSummaryCard(startX, startY, "TOTAL PRODUK", totalProduk, "Item");
    drawSummaryCard(startX + 26, startY, "PENJUALAN", totalPenjualan, "Nota");
    drawSummaryCard(startX + 52, startY, "SUPPLIER", totalSupplier, "Mitra");
    drawHomeLogo(role);
}

void drawBaseLayout(const char* sidebarTitle) {
    isSidebarActive = 1; system("cls"); drawFullFrame(); drawHeader();
    gotoxy(2, HEADER_HEIGHT+2); printf("MENU SYSTEM");
    gotoxy(2, HEADER_HEIGHT+3); printf("                          ");
    gotoxy(2, HEADER_HEIGHT+3); textHighlightTheme(); printf(" %s ", sidebarTitle); textNormal();
}

void printMenuItem(int x, int y, char* text, int isSelected) {
    int w = SIDEBAR_WIDTH-4; gotoxy(x, y);
    if(isSelected) { textHighlightTheme(); printf(" %-*s ", w-2, text); textNormal(); }
    else printf("  %-*s  ", w-4, text);
}

void drawBreadcrumbs(const char* path) { gotoxy(SIDEBAR_WIDTH + 2, HEADER_HEIGHT + 1); textNormal(); printf(" Lokasi: "); textHighlightTheme(); printf(" %s ", path); textNormal(); }
void drawNavigationLegend(const char* legend) { int y = screenHeight-2; gotoxy(1, y); for(int i=0; i<screenWidth-2; i++) printf(" "); gotoxy((screenWidth-(int)strlen(legend))/2, y); printf("%s", legend); }
void clearRightContent() { textNormal(); for(int y = HEADER_HEIGHT + 1; y < screenHeight - 1; y++) { gotoxy(SIDEBAR_WIDTH + 1, y); for(int x = SIDEBAR_WIDTH + 1; x < screenWidth - 1; x++) printf(" "); } }
void printCenterRight(int y, const char* text) { gotoxy(SIDEBAR_WIDTH+1+((screenWidth-SIDEBAR_WIDTH-(int)strlen(text))/2), y); printf("%s", text); }
int getCenterXForTable(int tableWidth) { return SIDEBAR_WIDTH+1+((screenWidth-SIDEBAR_WIDTH-tableWidth)/2); }
void drawFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight) {
    *boxWidth = 70; *boxHeight = 16; *startX = SIDEBAR_WIDTH+5; *startY = HEADER_HEIGHT+3;
    drawShadowBox(*startX, *startY, *boxWidth, *boxHeight);
    gotoxy(*startX+2, *startY); printf(" %s ", title);
}
void showErrorAndWait(int x, int y, const char* message) { Beep(750, 300); gotoxy(x, y); textHighlightTheme(); printf(" %s ", message); textNormal(); getch(); gotoxy(x, y); for(int i=0; i<(int)strlen(message)+2; i++) printf(" "); }
void drawSummaryCard(int x, int y, const char* title, int value, const char* unit) { int w = 22, h = 4; drawShadowBox(x, y, w, h); gotoxy(x + 2, y + 1); printf("%s", title); gotoxy(x + 2, y + 2); textHighlightTheme(); printf(" %d %s ", value, unit); textNormal(); }

void drawPerformanceVisual(int x, int y, int percent) {
    int barLen = 20; int filled = (percent * barLen) / 100; int colorCode;
    if (percent >= 80) colorCode = 0x0A; else if (percent >= 50) colorCode = 0x09; else colorCode = 0x0C;
    gotoxy(x, y); printf("["); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
    for(int i=0; i<barLen; i++) { if(i < filled) printf("%c", 219); else printf("%c", 176); }
    textNormal(); printf("] %d%%", percent);
}

// --- INPUTS & VALIDATION ---

int isNumeric(const char* str) {
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

int getString(char *buffer, int maxLen) {
    setCursorVisible(TRUE); int i = 0; int key;
    while (1) {
        key = getch();
        if (key == 27) return 0;
        if (key == 13) { buffer[i] = '\0'; break; }
        if (key == 8 && i > 0) { i--; printf("\b \b"); }
        else if (i < maxLen - 1 && isprint(key)) { buffer[i] = (char)key; printf("%c", key); i++; }
    }
    setCursorVisible(FALSE); return 1;
}

void getValidatedString(char *buffer, int maxLen, int x, int y) {
    while(1) {
        gotoxy(x, y);
        if(!getString(buffer, maxLen)) continue;
        if(strlen(buffer) > 0) break;
        showErrorAndWait(x, y+1, "Wajib diisi!");
    }
}

// --- UPDATE: VALIDASI ANGKA (INPUT INVALID!) ---
long getValidatedNumber(int x, int y) {
    char buf[20];
    while(1) {
        gotoxy(x, y);
        // Bersihkan area input visual jika retry
        printf("                   ");
        gotoxy(x, y);

        if(!getString(buf, 19)) continue;

        // Cek apakah angka
        if(isNumeric(buf) && strlen(buf) > 0) {
            return atol(buf);
        }

        // Tampilkan pesan error custom
        showErrorAndWait(x, y+1, "Input Invalid!");

        // Bersihkan error message setelah user tekan enter
        gotoxy(x, y+1); printf("                  ");
    }
}

// --- UPDATE: VALIDASI NO TELP (INPUT INVALID!) ---
void getValidatedPhoneNumber(char *buffer, int maxLen, int x, int y, int checkType, int ignoreID) {
    while(1) {
        gotoxy(x, y);
        // Bersihkan area
        printf("                   ");
        gotoxy(x, y);

        if(!getString(buffer, maxLen)) continue;

        if(isNumeric(buffer) && strlen(buffer) >= 10) {
            break;
        }

        showErrorAndWait(x, y+1, "Input Invalid!");
        gotoxy(x, y+1); printf("                  ");
    }
}

int getPassword(char *buffer, int maxLen, int inputX, int inputY, int* isVisible) {
    setCursorVisible(TRUE); int i = 0; int key;
    while(1) {
        gotoxy(inputX + i, inputY); key = getch();
        if (key == 27) return 0;
        if (key == 13) { buffer[i] = '\0'; break; }
        if (key == 9) { *isVisible = !*isVisible; gotoxy(inputX, inputY); for(int j=0; j<i; j++) printf("%c", *isVisible ? buffer[j] : '*'); }
        else if (key == 8 && i > 0) { i--; printf("\b \b"); }
        else if (i < maxLen - 1 && isprint(key)) { buffer[i] = (char)key; printf("%c", *isVisible ? buffer[i] : '*'); i++; }
    }
    setCursorVisible(FALSE); return 1;
}

void loadingAnimation() { system("cls"); updateScreenSize(); int midX = screenWidth / 2, midY = screenHeight / 2; textNormal(); gotoxy(midX - 10, midY - 2); printf("MENYIAPKAN SISTEM..."); int barWidth = 40; int startX = midX - (barWidth / 2); gotoxy(startX - 1, midY); printf("["); gotoxy(startX + barWidth, midY); printf("]"); for (int i = 0; i <= barWidth; i++) { gotoxy(startX + i - 1, midY); if(i > 0) printf("%c", 219); gotoxy(midX - 2, midY + 2); printf("%3d%%", (i * 100) / barWidth); Sleep(30); } Sleep(500); }

int loginScreen(int *loggedIndex) {
    isSidebarActive = 0; char user[20] = "", pass[20] = ""; int bxW = 60, bxH = 10;
    const char *loginArt[] = { "░█░░░█▀█░█▀▀░▀█▀░█▀█", "░█░░░█░█░█░█░░█░░█░█", "░▀▀▀░▀▀▀░▀▀▀░▀▀▀░▀░▀", NULL };

    while(1) {
        updateScreenSize(); int startX = (screenWidth - bxW) / 2, startY = (screenHeight / 2);
        system("cls"); drawFullFrame(); drawHeader();
        textNormal();

        int artW = 20; int artX = startX + (bxW - artW) / 2; int artY = startY - 4;
        for(int i=0; loginArt[i]!=NULL; i++) { gotoxy(artX, artY + i); printf("%s", loginArt[i]); }

        drawShadowBox(startX, startY, bxW, bxH);
        gotoxy(startX + 10, startY + 3); printf("Username : "); gotoxy(startX + 10, startY + 5); printf("Password : ");
        drawNavigationLegend("[TAB] Intip Password | [ENTER] Login | [ESC] Keluar");
        gotoxy(startX + 22, startY + 3); if (!getString(user, 19)) return -1;
        int vis = 0; if (!getPassword(pass, 19, startX + 22, startY + 5, &vis)) continue;
        if (strcmp(user, "admin") == 0 && strcmp(pass, "admin") == 0) return 0;
        for(int i=0; i<totalKaryawan; i++) { if(strcmp(user, dbKaryawan[i].username) == 0 && strcmp(pass, dbKaryawan[i].password) == 0) { *loggedIndex = i; return dbKaryawan[i].roleId; } }
        showErrorAndWait(startX + 20, startY + 8, "Login Gagal!");
    }
}