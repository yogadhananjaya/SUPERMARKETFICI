#include "ui.h"
#include "globals.h"

// ================= THEMES & CONSOLE UTILS =================

void setPinkTheme() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    GetConsoleScreenBufferInfoEx(hOut, &csbi);
    csbi.ColorTable[COLOR_HOT_PINK] = RGB(255, 105, 180);
    csbi.ColorTable[COLOR_WHITE] = RGB(255, 255, 255);
    SetConsoleScreenBufferInfoEx(hOut, &csbi);
}

void textNormal() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xDF);
}

void goFullscreen() {
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1C, 0, 0);
    keybd_event(VK_RETURN, 0x1C, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    Sleep(150);
}

void maximizeConsole() {
    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_MAXIMIZE);
    Sleep(150);
}

void removeScrollbar() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);

    COORD newSize;
    newSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    newSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    SetConsoleScreenBufferSize(hOut, newSize);
}

void gotoxy(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setCursorVisible(BOOL visible) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void hideCursor() {
    setCursorVisible(FALSE);
}

// ================= DRAWING FUNCTIONS =================

void drawFullFrame() {
    updateScreenSize();
    int right = screenWidth - 1;
    int bottom = screenHeight - 1;
    textNormal();

    for (int x = 0; x <= right; x++) {
        gotoxy(x, 0); printf("%c", 205);
        gotoxy(x, bottom); printf("%c", 205);
        gotoxy(x, HEADER_HEIGHT); printf("%c", 205);
    }
    for (int y = 0; y <= bottom; y++) {
        gotoxy(0, y); printf("%c", 186);
        gotoxy(right, y); printf("%c", 186);
        if (y > HEADER_HEIGHT) {
            gotoxy(SIDEBAR_WIDTH, y); printf("%c", 186);
        }
    }
    gotoxy(0, 0); printf("%c", 201);
    gotoxy(right, 0); printf("%c", 187);
    gotoxy(right, bottom); printf("%c", 188);
    gotoxy(0, bottom); printf("%c", 200);

    gotoxy(0, HEADER_HEIGHT); printf("%c", 204);
    gotoxy(right, HEADER_HEIGHT); printf("%c", 185);
    gotoxy(SIDEBAR_WIDTH, HEADER_HEIGHT); printf("%c", 203);
    gotoxy(SIDEBAR_WIDTH, bottom); printf("%c", 202);
}

void drawLoginFrame() {
    updateScreenSize();
    int right = screenWidth - 1;
    int bottom = screenHeight - 1;
    textNormal();
    for (int x = 0; x <= right; x++) {
        gotoxy(x, 0); printf("%c", 205);
        gotoxy(x, bottom); printf("%c", 205);
        gotoxy(x, HEADER_HEIGHT); printf("%c", 205);
    }
    for (int y = 0; y <= bottom; y++) {
        gotoxy(0, y); printf("%c", 186);
        gotoxy(right, y); printf("%c", 186);
    }
    gotoxy(0, 0); printf("%c", 201);
    gotoxy(right, 0); printf("%c", 187);
    gotoxy(right, bottom); printf("%c", 188);
    gotoxy(0, bottom); printf("%c", 200);
    gotoxy(0, HEADER_HEIGHT); printf("%c", 204);
    gotoxy(right, HEADER_HEIGHT); printf("%c", 185);
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

    int logoHeight = 6;     // Jumlah baris logo di atas
    int logoWidth = 85;     // Panjang karakter logo (harus dihitung manual dari 1 baris terpanjang)

    // Hitung posisi tengah otomatis
    // (Lebar Layar - Lebar Logo) / 2
    int startX = (screenWidth - logoWidth) / 2;
    if (startX < 0) startX = 0;

    int startY = 1; // Mulai dari baris ke-1

    textNormal(); // Pastikan warna normal

    // Loop untuk print baris demi baris
    for (int i = 0; i < logoHeight; i++) {
        gotoxy(startX, startY + i);
        printf("%s", logo[i]);
    }
}

void clearRightContent() {
    updateScreenSize();
    textNormal();
    for(int y = HEADER_HEIGHT + 1; y < screenHeight - 1; y++) {
        gotoxy(SIDEBAR_WIDTH + 1, y);
        for(int x = SIDEBAR_WIDTH + 1; x < screenWidth - 1; x++) printf(" ");
    }
}

void printCenterRight(int y, const char* text) {
    int contentWidth = (screenWidth - 1) - (SIDEBAR_WIDTH + 1);
    int textLen = strlen(text);
    int x = SIDEBAR_WIDTH + 1 + (contentWidth - textLen) / 2;
    gotoxy(x, y); printf("%s", text);
}

int getCenterXForTable(int tableWidth) {
    int contentWidth = (screenWidth - 1) - (SIDEBAR_WIDTH + 1);
    int x = SIDEBAR_WIDTH + 1 + (contentWidth - tableWidth) / 2;
    if (x <= SIDEBAR_WIDTH) x = SIDEBAR_WIDTH + 2;
    return x;
}

void drawBaseLayout(const char* sidebarTitle) {
    system("cls");
    drawFullFrame();
    drawHeader();
    textNormal();
    for(int y = HEADER_HEIGHT + 1; y < screenHeight - 1; y++) {
        gotoxy(1, y);
        for(int x = 1; x < SIDEBAR_WIDTH; x++) printf(" ");
    }
    gotoxy(2, HEADER_HEIGHT + 2);
    printf("     MENU");
    gotoxy(2, HEADER_HEIGHT + 3);
    printf("   %-20s", sidebarTitle);
}

void printMenuItem(int x, int y, char* text, int isSelected) {
    gotoxy(x, y);
    if(isSelected) {
        textNormal();
        printf(" >> %-20s << ", text);
    } else {
        textNormal();
        printf("    %-20s    ", text);
    }
}

void drawTableBox(int startX, int startY, int width, int height) {
    textNormal();
    for(int x = startX + 1; x < startX + width; x++) {
        gotoxy(x, startY); printf("%c", 205);
        gotoxy(x, startY + height); printf("%c", 205);
    }
    for(int y = startY + 1; y < startY + height; y++) {
        gotoxy(startX, y); printf("%c", 186);
        gotoxy(startX + width, y); printf("%c", 186);
    }
    gotoxy(startX, startY); printf("%c", 201);
    gotoxy(startX + width, startY); printf("%c", 187);
    gotoxy(startX, startY + height); printf("%c", 200);
    gotoxy(startX + width, startY + height); printf("%c", 188);
}

void drawFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight) {
    clearRightContent();
    *boxWidth = 70;
    *boxHeight = 16;
    *startX = SIDEBAR_WIDTH + 5;
    *startY = HEADER_HEIGHT + 3;

    textNormal();
    gotoxy(*startX + 2, *startY); printf(" %s ", title);

    for(int x=*startX; x<*startX+*boxWidth; x++) {
        gotoxy(x, *startY); printf("%c", 205);
        gotoxy(x, *startY+*boxHeight); printf("%c", 205);
    }
    for(int y=*startY+1; y<*startY+*boxHeight; y++) {
        gotoxy(*startX, y); printf("%c", 186);
        gotoxy(*startX+*boxWidth, y); printf("%c", 186);
    }
    gotoxy(*startX, *startY); printf("%c", 201);
    gotoxy(*startX+*boxWidth, *startY); printf("%c", 187);
    gotoxy(*startX, *startY+*boxHeight); printf("%c", 200);
    gotoxy(*startX+*boxWidth, *startY+*boxHeight); printf("%c", 188);
}

void loadingAnimation() {
    system("cls");
    updateScreenSize();
    int midY = screenHeight / 2;
    int midX = screenWidth / 2;
    gotoxy(midX - 5, midY); printf("Loading System...");
    gotoxy(midX - 20, midY+2);
    for(int i=0; i<40; i++) {
        printf("%c", 219);
        Sleep(5);
    }
    Sleep(200);
}

// ================= INPUT FUNCTIONS =================

// ================= INPUT FUNCTIONS =================

// Fungsi helper tambahan (Salin ini agar error hilang)
COORD getCurrentCursorPosition() {
    COORD coord = {0, 0};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        coord = csbi.dwCursorPosition;
    }
    return coord;
}

void getString(char *buffer, int maxLen) {
    setCursorVisible(TRUE);

    // Sekarang fungsi ini sudah dikenali karena ditulis di atasnya
    COORD currentPos = getCurrentCursorPosition();
    int currentX = currentPos.X;
    int currentY = currentPos.Y;

    // Clear area input visual (biar rapi)
    gotoxy(currentX, currentY);
    for (int i = 0; i < maxLen; i++) printf(" ");
    gotoxy(currentX, currentY);

    fflush(stdin);
    fgets(buffer, maxLen, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    setCursorVisible(FALSE);
}

long getLongInput(int inputX, int inputY) {
    char buffer[20];
    long result = -1;

    setCursorVisible(TRUE);
    gotoxy(inputX, inputY);
    for (int i = 0; i < 19; i++) printf(" ");
    gotoxy(inputX, inputY);

    fflush(stdin);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        result = atol(buffer);
    }
    setCursorVisible(FALSE);
    return result;
}

void getPassword(char *buffer, int maxLen, int inputX, int inputY, int* isVisible) {
    int i = 0;
    int key;
    setCursorVisible(TRUE);

    while(1) {
        if(i < maxLen - 1) key = getch();
        else {
            key = getch();
            if(key != 8 && key != KEY_ENTER && key != KEY_TAB) continue;
        }

        if (key == KEY_ENTER || key == 0) {
            buffer[i] = '\0';
            break;
        } else if (key == 8) { // Backspace
            if (i > 0) {
                i--;
                buffer[i] = '\0';
                gotoxy(inputX + i, inputY); printf(" ");
                gotoxy(inputX + i, inputY);
            }
        } else if (key == KEY_TAB) {
            *isVisible = !(*isVisible);
            gotoxy(inputX, inputY);
            for(int j=0; j<maxLen-1; j++) printf(" ");
            gotoxy(inputX, inputY);
            for(int j=0; j<i; j++) printf("%c", (*isVisible) ? buffer[j] : '*');
        }
        else if (i < maxLen - 1) {
            buffer[i] = (char)key;
            printf("%c", (*isVisible) ? buffer[i] : '*');
            i++;
        }
    }
    setCursorVisible(FALSE);
}

int loginScreen() {
    char username[20] = "";
    char password[20] = "";
    int loginX, loginY, boxW, boxH;

    while(1) {
        system("cls");
        drawLoginFrame();
        drawHeader();
        updateScreenSize();
        boxW = 50; boxH = 10;
        loginX = (screenWidth - boxW) / 2;
        loginY = (screenHeight - boxH) / 2;
        if(loginY <= HEADER_HEIGHT) loginY = HEADER_HEIGHT + 4;

        textNormal();
        for(int x=loginX; x<loginX+boxW; x++) {
            gotoxy(x, loginY); printf("%c", 205);
            gotoxy(x, loginY+boxH); printf("%c", 205);
        }
        for(int y=loginY+1; y<loginY+boxH; y++) {
            gotoxy(loginX, y); printf("%c", 186);
            gotoxy(loginX+boxW, y); printf("%c", 186);
        }
        gotoxy(loginX, loginY); printf("%c", 201);
        gotoxy(loginX+boxW, loginY); printf("%c", 187);
        gotoxy(loginX, loginY+boxH); printf("%c", 200);
        gotoxy(loginX+boxW, loginY+boxH); printf("%c", 188);

        gotoxy(loginX + (boxW/2) - 2, loginY - 2); printf("LOGIN");

        int labelX = loginX + 5;
        int inputX = loginX + 17;
        int labelY = loginY + 3;

        gotoxy(labelX, labelY);    printf("Username :");
        gotoxy(labelX, labelY+1); printf("Password :");
        gotoxy(labelX, labelY+2); printf("Tekan Tab lihat password");

        gotoxy(inputX, labelY);   printf("                   ");
        gotoxy(inputX, labelY+1); printf("                   ");

        setCursorVisible(TRUE);
        gotoxy(inputX, labelY); getString(username, 19);
        int isVisible = 0;
        gotoxy(inputX, labelY+1); getPassword(password, 19, inputX, labelY+1, &isVisible);

        if (strcmp(username, "123") == 0 && strcmp(password, "123") == 0) {
            gotoxy(loginX + 5, loginY + 8);
            printf("Login Berhasil! Memuat Menu...");
            Sleep(800);
            setCursorVisible(FALSE);
            return 1;
        } else {
            gotoxy(loginX + 5, loginY + 8);
            printf("Salah! (ESC Keluar)");
            setCursorVisible(FALSE);
            while(1) {
                int key = getch();
                if (key == KEY_ESC) return 0;
                if (key == KEY_ENTER) break;
            }
        }
    }
}

// ================= MAIN MENU HELPERS (Ini yang tadi Missing!) =================

void drawMainMenuStatic() {
    system("cls");
    drawLoginFrame();
    drawHeader();
    updateScreenSize();
    int boxW = 50, boxH = 16;
    int menuX = (screenWidth - boxW) / 2;
    int menuY = (screenHeight - boxH) / 2;
    if (menuY <= HEADER_HEIGHT) menuY = HEADER_HEIGHT + 4;

    textNormal();
    for(int x=menuX; x<menuX+boxW; x++) {
        gotoxy(x, menuY); printf("%c", 205); gotoxy(x, menuY+boxH); printf("%c", 205);
    }
    for(int y=menuY+1; y<menuY+boxH; y++) {
        gotoxy(menuX, y); printf("%c", 186); gotoxy(menuX+boxW, y); printf("%c", 186);
    }
    gotoxy(menuX, menuY); printf("%c", 201); gotoxy(menuX+boxW, menuY); printf("%c", 187);
    gotoxy(menuX, menuY+boxH); printf("%c", 200); gotoxy(menuX+boxW, menuY+boxH); printf("%c", 188);

    char* title = "=== MENU UTAMA SYSTEM ===";
    gotoxy(menuX + (boxW - strlen(title))/2, menuY + 2); printf("%s", title);
    char* footer = "Gunakan Panah Atas/Bawah";
    gotoxy(menuX + (boxW - strlen(footer))/2, menuY + boxH - 2); printf("%s", footer);
}

void updateMainMenuOptions(int selected) {
    updateScreenSize();
    int boxW = 50, boxH = 16;
    int menuX = (screenWidth - boxW) / 2;
    int menuY = (screenHeight - boxH) / 2;
    if (menuY <= HEADER_HEIGHT) menuY = HEADER_HEIGHT + 4;

    int startY = menuY + 5;
    int itemX = menuX + (boxW/2) - 12;

    printMenuItem(itemX, startY,     "1. KARYAWAN", selected == 1);
    printMenuItem(itemX, startY + 1, "2. SUPPLIER", selected == 2);
    printMenuItem(itemX, startY + 2, "3. GUDANG",   selected == 3);
    printMenuItem(itemX, startY + 3, "4. PRODUK",   selected == 4);
    printMenuItem(itemX, startY + 4, "0. KELUAR",   selected == 5);
}