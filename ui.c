#include "ui.h"
#include "globals.h"


// Fungsi untuk mengatur tema warna konsol menjadi warna pink.
void setPinkTheme() {
    // Mendapatkan handle (pegangan) ke output standar konsol.
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    // Mendeklarasikan struct untuk informasi buffer layar konsol yang diperluas.
    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    // Mengatur ukuran struct yang diperlukan sebelum memanggil fungsi Get.
    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    // Mengambil informasi buffer layar konsol saat ini.
    GetConsoleScreenBufferInfoEx(hOut, &csbi);
    // Mengatur warna Hot Pink kustom dalam ColorTable.
    csbi.ColorTable[COLOR_HOT_PINK] = RGB(255, 105, 180);
    // Mengatur warna Putih kustom.
    csbi.ColorTable[COLOR_WHITE] = RGB(255, 255, 255);
    // Menerapkan perubahan informasi buffer layar konsol.
    SetConsoleScreenBufferInfoEx(hOut, &csbi);
}

// Fungsi untuk mengatur atribut teks ke warna/latar belakang normal (0xDF).
void textNormal() {
    // Mengatur atribut teks konsol. Nilai 0xDF (putih terang di latar belakang hitam) mungkin didefinisikan di ui.h.
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xDF);
}

// Fungsi untuk memaksa konsol masuk ke mode layar penuh (biasanya dengan menekan ALT+ENTER).
void goFullscreen() {
    // Menekan tombol ALT.
    keybd_event(VK_MENU, 0x38, 0, 0);
    // Menekan tombol ENTER.
    keybd_event(VK_RETURN, 0x1C, 0, 0);
    // Melepas tombol ENTER.
    keybd_event(VK_RETURN, 0x1C, KEYEVENTF_KEYUP, 0);
    // Melepas tombol ALT.
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    // Menunda eksekusi sebentar (150ms).
    Sleep(150);
}

// Fungsi untuk memaksimalkan jendela konsol.
void maximizeConsole() {
    // Mendapatkan handle ke jendela konsol.
    HWND consoleWindow = GetConsoleWindow();
    // Mengatur status jendela menjadi maksimal.
    ShowWindow(consoleWindow, SW_MAXIMIZE);
    // Menunda eksekusi sebentar (150ms).
    Sleep(150);
}

// Fungsi untuk menghilangkan scrollbar dengan mengatur ukuran buffer sama dengan ukuran jendela.
void removeScrollbar() {
    // Mendapatkan handle ke output standar.
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    // Mendeklarasikan struct untuk info buffer konsol.
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    // Mendapatkan info buffer konsol.
    GetConsoleScreenBufferInfo(hOut, &csbi);

    // Mendeklarasikan struct COORD untuk ukuran baru.
    COORD newSize;
    // Menghitung lebar jendela saat ini.
    newSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    // Menghitung tinggi jendela saat ini.
    newSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // Mengatur ukuran buffer layar konsol agar sesuai dengan ukuran jendela (menghilangkan scrollbar).
    SetConsoleScreenBufferSize(hOut, newSize);
}

// Fungsi untuk memindahkan kursor ke posisi (x, y) tertentu.
void gotoxy(int x, int y) {
    // Mendeklarasikan struct COORD.
    COORD coord = {x, y};
    // Mengatur posisi kursor konsol.
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// Fungsi untuk mengatur apakah kursor terlihat atau tidak.
void setCursorVisible(BOOL visible) {
    // Mendeklarasikan struct untuk informasi kursor.
    CONSOLE_CURSOR_INFO cursorInfo;
    // Mendapatkan informasi kursor saat ini.
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    // Mengatur visibilitas kursor.
    cursorInfo.bVisible = visible;
    // Menerapkan perubahan informasi kursor.
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// Fungsi wrapper untuk menyembunyikan kursor.
void hideCursor() {
    setCursorVisible(FALSE);
}


// Fungsi untuk menggambar bingkai penuh dengan header dan sidebar.
void drawFullFrame() {
    // Memastikan variabel global screenWidth dan screenHeight sudah diperbarui.
    updateScreenSize();
    // Koordinat batas kanan dan bawah.
    int right = screenWidth - 1;
    int bottom = screenHeight - 1;
    // Mengatur warna teks normal.
    textNormal();

    // Menggambar garis horizontal (atas, bawah, dan batas header).
    for (int x = 0; x <= right; x++) {
        gotoxy(x, 0); printf("%c", 205); // Garis atas
        gotoxy(x, bottom); printf("%c", 205); // Garis bawah
        gotoxy(x, HEADER_HEIGHT); printf("%c", 205); // Garis pemisah header
    }
    // Menggambar garis vertikal (kiri, kanan, dan batas sidebar).
    for (int y = 0; y <= bottom; y++) {
        gotoxy(0, y); printf("%c", 186); // Garis kiri
        gotoxy(right, y); printf("%c", 186); // Garis kanan
        // Garis pemisah sidebar (hanya di bawah header)
        if (y > HEADER_HEIGHT) {
            gotoxy(SIDEBAR_WIDTH, y); printf("%c", 186);
        }
    }
    // Menggambar sudut-sudut bingkai utama.
    gotoxy(0, 0); printf("%c", 201); // Sudut kiri atas
    gotoxy(right, 0); printf("%c", 187); // Sudut kanan atas
    gotoxy(right, bottom); printf("%c", 188); // Sudut kanan bawah
    gotoxy(0, bottom); printf("%c", 200); // Sudut kiri bawah

    // Menggambar sambungan di batas header dan sidebar.
    gotoxy(0, HEADER_HEIGHT); printf("%c", 204); // Sambungan kiri di header
    gotoxy(right, HEADER_HEIGHT); printf("%c", 185); // Sambungan kanan di header
    gotoxy(SIDEBAR_WIDTH, HEADER_HEIGHT); printf("%c", 203); // Sambungan tengah (header-sidebar)
    gotoxy(SIDEBAR_WIDTH, bottom); printf("%c", 202); // Sambungan bawah (sidebar-content)
}

// Fungsi untuk menggambar bingkai yang digunakan pada layar login (tanpa sidebar).
void drawLoginFrame() {
    // Memastikan ukuran layar terbaru.
    updateScreenSize();
    int right = screenWidth - 1;
    int bottom = screenHeight - 1;
    textNormal();
    // Menggambar garis horizontal (atas, bawah, batas header).
    for (int x = 0; x <= right; x++) {
        gotoxy(x, 0); printf("%c", 205);
        gotoxy(x, bottom); printf("%c", 205);
        gotoxy(x, HEADER_HEIGHT); printf("%c", 205);
    }
    // Menggambar garis vertikal (kiri, kanan).
    for (int y = 0; y <= bottom; y++) {
        gotoxy(0, y); printf("%c", 186);
        gotoxy(right, y); printf("%c", 186);
    }
    // Menggambar sudut-sudut bingkai.
    gotoxy(0, 0); printf("%c", 201);
    gotoxy(right, 0); printf("%c", 187);
    gotoxy(right, bottom); printf("%c", 188);
    gotoxy(0, bottom); printf("%c", 200);
    // Menggambar sambungan batas header.
    gotoxy(0, HEADER_HEIGHT); printf("%c", 204);
    gotoxy(right, HEADER_HEIGHT); printf("%c", 185);
}

// Fungsi untuk menggambar logo/judul di area header.
void drawHeader() {
    // Array string yang menyimpan ASCII Art logo.
    const char *logo[] = {
        "   _____ _    _ _____  ______ _____  __  __           _____  _  ________ _________ ",
        "  / ____| |  | |  __ \\|  ____|  __ \\|  \\/  |    /\\    |  __ \\| |/ /  ____|__   __|",
        " | (___ | |  | | |__) | |__  | |__) | \\  / |   /  \\   | |__) | ' /| |__     | |   ",
        "  \\___ \\| |  | |  ___/|  __| |  _  /| |\\/| |  / /\\ \\  |  _  /|  < |  __|    | |   ",
        "  ____) | |__| | |    | |____| | \\ \\| |  | | / ____ \\ | | \\ \\| . \\| |____   | |   ",
        " |_____/ \\____/|_|    |______|_|  \\_\\_|  |_|/_/    \\_\\|_|  \\_\\_|\\_\\______|  |_|   "
    };

    int logoHeight = 6;     // Jumlah baris logo.
    int logoWidth = 85;     // Panjang karakter logo.

    // Menghitung posisi X awal agar logo berada di tengah layar.
    int startX = (screenWidth - logoWidth) / 2;
    // Memastikan startX tidak negatif.
    if (startX < 0) startX = 0;

    // Posisi Y awal untuk logo (di bawah garis atas).
    int startY = 1;

    textNormal();

    // Loop untuk print baris demi baris logo.
    for (int i = 0; i < logoHeight; i++) {
        gotoxy(startX, startY + i);
        printf("%s", logo[i]);
    }
}

// Fungsi untuk membersihkan area konten utama (sebelah kanan sidebar).
void clearRightContent() {
    updateScreenSize();
    textNormal();
    // Loop melalui setiap baris di area konten.
    for(int y = HEADER_HEIGHT + 1; y < screenHeight - 1; y++) {
        // Pindah kursor ke awal area konten.
        gotoxy(SIDEBAR_WIDTH + 1, y);
        // Loop melalui setiap kolom di area konten untuk mencetak spasi.
        for(int x = SIDEBAR_WIDTH + 1; x < screenWidth - 1; x++) printf(" ");
    }
}

// Fungsi untuk mencetak teks tepat di tengah area konten kanan.
void printCenterRight(int y, const char* text) {
    // Menghitung lebar area konten (total lebar - posisi sidebar - batas kanan).
    int contentWidth = (screenWidth - 1) - (SIDEBAR_WIDTH + 1);
    int textLen = strlen(text);
    // Menghitung posisi X awal agar teks di tengah.
    int x = SIDEBAR_WIDTH + 1 + (contentWidth - textLen) / 2;
    gotoxy(x, y); printf("%s", text);
}

// Fungsi untuk mendapatkan posisi X awal agar tabel berada di tengah area konten kanan.
int getCenterXForTable(int tableWidth) {
    int contentWidth = (screenWidth - 1) - (SIDEBAR_WIDTH + 1);
    // Menghitung posisi X agar tabel di tengah.
    int x = SIDEBAR_WIDTH + 1 + (contentWidth - tableWidth) / 2;
    // Memastikan tabel tidak tumpang tindih dengan sidebar.
    if (x <= SIDEBAR_WIDTH) x = SIDEBAR_WIDTH + 2;
    return x;
}

// Fungsi untuk menggambar tata letak dasar aplikasi (Full Frame, Header, Sidebar Title).
void drawBaseLayout(const char* sidebarTitle) {
    // Membersihkan layar konsol.
    system("cls");
    // Menggambar bingkai penuh.
    drawFullFrame();
    // Menggambar header (logo).
    drawHeader();
    textNormal();
    // Membersihkan area sidebar.
    for(int y = HEADER_HEIGHT + 1; y < screenHeight - 1; y++) {
        gotoxy(1, y);
        for(int x = 1; x < SIDEBAR_WIDTH; x++) printf(" ");
    }
    // Mencetak judul 'MENU' di sidebar.
    gotoxy(2, HEADER_HEIGHT + 2);
    printf("     MENU");
    // Mencetak judul spesifik sidebar (e.g., "KELOLA KARYAWAN").
    gotoxy(2, HEADER_HEIGHT + 3);
    printf("   %-20s", sidebarTitle);
}

// Fungsi untuk mencetak satu item menu, dengan penanda jika item tersebut terpilih (isSelected).
void printMenuItem(int x, int y, char* text, int isSelected) {
    gotoxy(x, y);
    if(isSelected) {
        textNormal(); // Menggunakan warna normal untuk item terpilih
        printf(" >> %-20s << ", text); // Menambahkan penanda >> dan <<
    } else {
        textNormal();
        printf("    %-20s    ", text); // Padding spasi untuk item tidak terpilih
    }
}

// Fungsi untuk menggambar kotak bingkai tabel sederhana.
void drawTableBox(int startX, int startY, int width, int height) {
    textNormal();
    // Garis horizontal atas dan bawah.
    for(int x = startX + 1; x < startX + width; x++) {
        gotoxy(x, startY); printf("%c", 205);
        gotoxy(x, startY + height); printf("%c", 205);
    }
    // Garis vertikal kiri dan kanan.
    for(int y = startY + 1; y < startY + height; y++) {
        gotoxy(startX, y); printf("%c", 186);
        gotoxy(startX + width, y); printf("%c", 186);
    }
    // Sudut-sudut.
    gotoxy(startX, startY); printf("%c", 201);
    gotoxy(startX + width, startY); printf("%c", 187);
    gotoxy(startX, startY + height); printf("%c", 200);
    gotoxy(startX + width, startY + height); printf("%c", 188);
}

// Fungsi untuk menggambar kotak formulir (digunakan untuk input data), juga membersihkan area konten.
void drawFormBox(char* title, int* startX, int* startY, int* boxWidth, int* boxHeight) {
    // Membersihkan area konten kanan.
    clearRightContent();
    // Mengatur dimensi default kotak.
    *boxWidth = 70;
    *boxHeight = 16;
    // Mengatur posisi awal kotak (sedikit menjorok dari sidebar).
    *startX = SIDEBAR_WIDTH + 5;
    *startY = HEADER_HEIGHT + 3;

    textNormal();
    // Mencetak judul form di atas garis atas.
    gotoxy(*startX + 2, *startY); printf(" %s ", title);

    // Menggambar garis horizontal (atas dan bawah).
    for(int x=*startX; x<*startX+*boxWidth; x++) {
        gotoxy(x, *startY); printf("%c", 205);
        gotoxy(x, *startY+*boxHeight); printf("%c", 205);
    }
    // Menggambar garis vertikal (kiri dan kanan).
    for(int y=*startY+1; y<*startY+*boxHeight; y++) {
        gotoxy(*startX, y); printf("%c", 186);
        gotoxy(*startX+*boxWidth, y); printf("%c", 186);
    }
    // Menggambar sudut-sudut.
    gotoxy(*startX, *startY); printf("%c", 201);
    gotoxy(*startX+*boxWidth, *startY); printf("%c", 187);
    gotoxy(*startX, *startY+*boxHeight); printf("%c", 200);
    gotoxy(*startX+*boxWidth, *startY+*boxHeight); printf("%c", 188);
}

// Fungsi untuk menampilkan animasi loading sederhana (progress bar).
void loadingAnimation() {
    system("cls");
    updateScreenSize();
    // Menghitung posisi tengah layar.
    int midY = screenHeight / 2;
    int midX = screenWidth / 2;
    // Mencetak teks "Memuat...".
    gotoxy(midX - 5, midY); printf("Memuat...");
    // Posisi awal progress bar.
    gotoxy(midX - 20, midY+2);
    // Loop untuk menggambar progress bar.
    for(int i=0; i<40; i++) {
        printf("%c", 219); // Mencetak karakter blok penuh.
        Sleep(5); // Menunda sebentar untuk efek animasi.
    }
    Sleep(200);
}


// Fungsi untuk mendapatkan posisi kursor konsol saat ini.
COORD getCurrentCursorPosition() {
    COORD coord = {0, 0};
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    // Mengambil info buffer, jika berhasil, atur koordinat.
    if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
        coord = csbi.dwCursorPosition;
    }
    return coord;
}

// Fungsi untuk mendapatkan input string dari user.
void getString(char *buffer, int maxLen) {
    // Membuat kursor terlihat selama input.
    setCursorVisible(TRUE);

    // Menyimpan posisi kursor sebelum input.
    COORD currentPos = getCurrentCursorPosition();
    int currentX = currentPos.X;
    int currentY = currentPos.Y;

    // Bersihkan area input visual (opsional, untuk memastikan input bersih).
    gotoxy(currentX, currentY);
    for (int i = 0; i < maxLen; i++) printf(" ");
    gotoxy(currentX, currentY);

    // Membersihkan buffer input standar.
    fflush(stdin);
    // Menggunakan fgets untuk membaca string, lebih aman daripada gets.
    fgets(buffer, maxLen, stdin);
    // Menghapus karakter newline ('\n') dari buffer jika ada.
    buffer[strcspn(buffer, "\n")] = 0;
    // Menyembunyikan kursor setelah input selesai.
    setCursorVisible(FALSE);
}

// Fungsi untuk mendapatkan input numerik bertipe long dari user.
long getLongInput(int inputX, int inputY) {
    char buffer[20];
    long result = -1;

    setCursorVisible(TRUE);
    // Bersihkan area input.
    gotoxy(inputX, inputY);
    for (int i = 0; i < 19; i++) printf(" ");
    gotoxy(inputX, inputY);

    fflush(stdin);
    // Baca input string.
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        // Hapus newline.
        buffer[strcspn(buffer, "\n")] = 0;
        // Konversi string ke long.
        result = atol(buffer);
    }
    setCursorVisible(FALSE);
    return result;
}

// Fungsi untuk mendapatkan input password dengan masking karakter '*' dan fitur toggle visibility (Tab).
void getPassword(char *buffer, int maxLen, int inputX, int inputY, int* isVisible) {
    int i = 0; // Index karakter yang sudah dimasukkan.
    int key; // Variabel untuk menyimpan input karakter.
    setCursorVisible(TRUE);

    // Loop tak terbatas sampai ENTER ditekan atau ESC/lainnya.
    while(1) {
        // Membaca input karakter, tetapi membatasi hanya untuk backspace/enter/tab jika sudah mencapai maxLen.
        if(i < maxLen - 1) key = getch();
        else {
            key = getch();
            if(key != 8 && key != KEY_ENTER && key != KEY_TAB) continue; // Abaikan karakter lain jika sudah penuh.
        }

        if (key == KEY_ENTER || key == 0) { // Cek jika ENTER ditekan.
            buffer[i] = '\0'; // Menambahkan null-terminator.
            break; // Keluar dari loop.
        } else if (key == 8) { // Cek jika BACKSPACE ditekan.
            if (i > 0) {
                i--;
                buffer[i] = '\0';
                gotoxy(inputX + i, inputY); printf(" "); // Menghapus karakter di layar.
                gotoxy(inputX + i, inputY);
            }
        } else if (key == KEY_TAB) { // Cek jika TAB ditekan (toggle visibility).
            *isVisible = !(*isVisible); // Toggle status visibility.
            gotoxy(inputX, inputY);
            // Membersihkan tampilan password lama.
            for(int j=0; j<maxLen-1; j++) printf(" ");
            gotoxy(inputX, inputY);
            // Mencetak ulang password sesuai status visibility.
            for(int j=0; j<i; j++) printf("%c", (*isVisible) ? buffer[j] : '*');
        }
        else if (i < maxLen - 1) { // Jika karakter normal dan belum penuh.
            buffer[i] = (char)key; // Simpan karakter.
            printf("%c", (*isVisible) ? buffer[i] : '*'); // Tampilkan karakter (atau '*').
            i++; // Maju ke index berikutnya.
        }
    }
    setCursorVisible(FALSE);
}

// Fungsi utama untuk menampilkan layar login.
int loginScreen() {
    char username[20] = "";
    char password[20] = "";
    int loginX, loginY, boxW, boxH;

    while(1) { // Loop login sampai berhasil atau keluar.
        system("cls");
        drawLoginFrame(); // Menggambar bingkai login.
        drawHeader(); // Menggambar header.
        updateScreenSize();

        // Menentukan ukuran dan posisi kotak login agar di tengah layar.
        boxW = 50; boxH = 10;
        loginX = (screenWidth - boxW) / 2;
        loginY = (screenHeight - boxH) / 2;
        if(loginY <= HEADER_HEIGHT) loginY = HEADER_HEIGHT + 4; // Pastikan tidak tumpang tindih dengan header.

        textNormal();
        // Menggambar garis dan sudut kotak login. (Kode pengulangan drawTableBox/drawFormBox)
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

        // Mencetak judul "LOGIN".
        gotoxy(loginX + (boxW/2) - 2, loginY - 2); printf("LOGIN");

        // Menentukan posisi label dan input field.
        int labelX = loginX + 5;
        int inputX = loginX + 17;
        int labelY = loginY + 3;

        gotoxy(labelX, labelY);    printf("Username :");
        gotoxy(labelX, labelY+1); printf("Password :");
        gotoxy(labelX, labelY+2); printf("Tekan Tab lihat password"); // Instruksi untuk toggle password.

        // Membersihkan visual area input.
        gotoxy(inputX, labelY);   printf("                   ");
        gotoxy(inputX, labelY+1); printf("                   ");

        // Proses input username.
        setCursorVisible(TRUE);
        gotoxy(inputX, labelY); getString(username, 19);
        // Proses input password (menggunakan masking).
        int isVisible = 0;
        gotoxy(inputX, labelY+1); getPassword(password, 19, inputX, labelY+1, &isVisible);

        // Pengecekan kredensial (hardcoded: admin/admin).
        if (strcmp(username, "admin") == 0 && strcmp(password, "admin") == 0) {
            gotoxy(loginX + 5, loginY + 8);
            printf("Login Berhasil! Memuat Menu...");
            Sleep(800);
            setCursorVisible(FALSE);
            return 1; // Login berhasil
        } else {
            gotoxy(loginX + 5, loginY + 8);
            printf("Username/Password Salah! | [ESC] Keluar");
            setCursorVisible(FALSE);
            // Loop untuk menunggu user menekan ENTER (coba lagi) atau ESC (keluar).
            while(1) {
                int key = getch();
                if (key == KEY_ESC) return 0; // Keluar dari program.
                if (key == KEY_ENTER) break; // Coba login lagi (mengulang loop while(1) di luar).
            }
        }
    }
}


// Fungsi untuk menggambar bingkai statis menu utama.
void drawMainMenuStatic() {
    system("cls");
    drawLoginFrame(); // Menggunakan bingkai yang sama dengan login.
    drawHeader();
    updateScreenSize();

    // Menentukan ukuran dan posisi kotak menu (di tengah).
    int boxW = 50, boxH = 16;
    int menuX = (screenWidth - boxW) / 2;
    int menuY = (screenHeight - boxH) / 2;
    if (menuY <= HEADER_HEIGHT) menuY = HEADER_HEIGHT + 4;

    textNormal();
    // Menggambar kotak menu (garis, sudut).
    for(int x=menuX; x<menuX+boxW; x++) {
        gotoxy(x, menuY); printf("%c", 205); gotoxy(x, menuY+boxH); printf("%c", 205);
    }
    for(int y=menuY+1; y<menuY+boxH; y++) {
        gotoxy(menuX, y); printf("%c", 186); gotoxy(menuX+boxW, y); printf("%c", 186);
    }
    gotoxy(menuX, menuY); printf("%c", 201); gotoxy(menuX+boxW, menuY); printf("%c", 187);
    gotoxy(menuX, menuY+boxH); printf("%c", 200); gotoxy(menuX+boxW, menuY+boxH); printf("%c", 188);

    // Mencetak judul menu.
    char* title = "=== MENU UTAMA SYSTEM ===";
    gotoxy(menuX + (boxW - strlen(title))/2, menuY + 2); printf("%s", title);
    // Mencetak footer instruksi.
    char* footer = "Gunakan Panah Atas/Bawah";
    gotoxy(menuX + (boxW - strlen(footer))/2, menuY + boxH - 2); printf("%s", footer);
}

// Fungsi untuk memperbarui opsi menu utama berdasarkan item yang sedang dipilih.
void updateMainMenuOptions(int selected) {
    updateScreenSize();
    // Mengambil ulang posisi kotak menu.
    int boxW = 50, boxH = 16;
    int menuX = (screenWidth - boxW) / 2;
    int menuY = (screenHeight - boxH) / 2;
    if (menuY <= HEADER_HEIGHT) menuY = HEADER_HEIGHT + 4;

    int startY = menuY + 5;
    // Posisi X untuk item menu agar di tengah kotak.
    int itemX = menuX + (boxW/2) - 12;

    // Mencetak setiap item menu, memanggil printMenuItem untuk menangani penandaan item terpilih.
    printMenuItem(itemX, startY,     "1. KARYAWAN", selected == 1);
    printMenuItem(itemX, startY + 1, "2. SUPPLIER", selected == 2);
    printMenuItem(itemX, startY + 2, "3. GUDANG",   selected == 3);
    printMenuItem(itemX, startY + 3, "4. PRODUK",   selected == 4);
    printMenuItem(itemX, startY + 4, "0. KELUAR",   selected == 5);
}