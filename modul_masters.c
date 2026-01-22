#include "controllers.h"
#include "config.h"
#include "globals.h"
#include "ui.h"
#include <math.h>

void refreshRightArea(int isTableMode, void (*displayFunc)(int), int tableX) {
    if (isTableMode) {
        clearRightContent();
        displayFunc(tableX);
    } else {
        drawHomeLogo();
    }
}

void displayKaryawanTable(int tableX, int hideId) {
    int startY = HEADER_HEIGHT + 4;
    int tableWidth = 100;

    drawTableBox(tableX - 1, startY - 1, tableWidth + 2, 13);
    textNormal();
    gotoxy(tableX, startY);
    printf(" %-5s %c %-20s %c %-15s %c %-15s %c %-8s ",
           "ID", 186, "NAMA", 186, "JABATAN", 186, "USERNAME", 186, "PERF");
    gotoxy(tableX, startY + 1); for(int i=0; i<tableWidth; i++) printf("%c", 205);

    int start = pageOffset * ROWS_PER_PAGE;
    int end = (start + ROWS_PER_PAGE > totalKaryawan) ? totalKaryawan : start + ROWS_PER_PAGE;

    for(int i = start; i < end; i++) {
        gotoxy(tableX, startY + 2 + (i - start));

        char idStr[10];
        if (hideId) strcpy(idStr, "***");
        else sprintf(idStr, "%04d", dbKaryawan[i].id);

        printf(" %-5s %c %-20s %c %-15s %c %-15s %c %-8d ",
               idStr, 186, dbKaryawan[i].nama, 186, dbKaryawan[i].jabatan,
               186, dbKaryawan[i].username, 186, dbKaryawan[i].performa);
    }
}

void crudKaryawan() {
    int selected = 0;
    const char *menuItems[] = {"Lihat Data", "Input Data", "Hapus Data", "Kembali"};
    int hideId = 1; // Default Hidden
    int tableX = getCenterXForTable(100);
    int isTableMode = 0;
    pageOffset = 0;

    drawBaseLayout("MENU KARYAWAN"); drawHomeLogo();

    while(1) {
        int startY = HEADER_HEIGHT + 6;
        for(int i=0; i<4; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));

        if(isTableMode) {
            clearRightContent();
            displayKaryawanTable(tableX, hideId);
            drawNavigationLegend("[TAB] Show/Hide ID | [PANAH] Scroll | [ESC] Menu");
        } else {
            drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");
        }

        int key = getch();

        // Fitur Hide/Unhide ID dengan TAB
        if (key == KEY_TAB && isTableMode) {
            hideId = !hideId;
            continue;
        }

        if(key == 224) {
            key = getch();
            if(isTableMode) {
               // Logic Pagination scroll...
            } else {
               if(key==KEY_UP) selected = (selected > 0) ? selected-1 : 3;
               if(key==KEY_DOWN) selected = (selected < 3) ? selected+1 : 0;
            }
        } else if(key == 13 && !isTableMode) {
            if(selected == 3) break;
            if(selected == 0) { isTableMode = 1; }
            if(selected == 1) { /* Logic Input Data (Sama seperti sebelumnya) */ }
            if(selected == 2) {
                // Logic Hapus dengan Validasi
                int fx, fy, bw, bh;
                drawFormBox("HAPUS KARYAWAN", &fx, &fy, &bw, &bh);
                gotoxy(fx+2, fy+2); printf("ID Target: ");
                int tid = (int)getValidatedNumber(fx+13, fy+2);

                // Cari Index
                int idx = -1;
                for(int i=0; i<totalKaryawan; i++) if(dbKaryawan[i].id == tid) idx=i;

                if(idx != -1) {
                    gotoxy(fx+2, fy+4); printf("Nama: %s", dbKaryawan[idx].nama);
                    // VALIDASI
                    if(getConfirmation(fx+2, fy+6, "Yakin hapus data ini")) {
                        for(int j=idx; j<totalKaryawan-1; j++) dbKaryawan[j] = dbKaryawan[j+1];
                        totalKaryawan--;
                        showErrorAndWait(fx+2, fy+8, "Data Terhapus!");
                    } else {
                        showErrorAndWait(fx+2, fy+8, "Penghapusan Dibatalkan.");
                    }
                } else showErrorAndWait(fx+2, fy+4, "ID Tidak Ditemukan!");
                drawBaseLayout("MENU KARYAWAN"); drawHomeLogo();
            }
        } else if(key == 27 && isTableMode) {
            isTableMode = 0; clearRightContent(); drawHomeLogo();
        }
    }
}

// --- MODUL PRODUK (Format Rupiah) ---

void displayProdukTable(int tableX) {
    int startY = HEADER_HEIGHT + 4;
    drawTableBox(tableX - 1, startY - 1, 95 + 2, 13);
    textNormal();
    gotoxy(tableX, startY);
    printf(" %-5s %c %-40s %c %-10s %c %-20s ", "ID", 186, "NAMA PRODUK", 186, "STOK", 186, "HARGA (Rp)");
    gotoxy(tableX, startY + 1); for(int i=0; i<95; i++) printf("%c", 205);

    int start = pageOffset * ROWS_PER_PAGE;
    int end = (start + ROWS_PER_PAGE > totalProduk) ? totalProduk : start + ROWS_PER_PAGE;

    for(int i = start; i < end; i++) {
        char rp[30];
        formatRupiah(dbProduk[i].harga, rp); // Format Harga

        gotoxy(tableX, startY + 2 + (i - start));
        printf(" %03d   %c %-40s %c %-10d %c Rp %-17s ",
            dbProduk[i].id, 186, dbProduk[i].nama, 186, dbProduk[i].stok, 186, rp);
    }
}

// Fungsi crudProduk() perlu dipanggil dengan validasi saat Hapus (implementasi sama dengan crudKaryawan di atas)
// ... (Logic crudProduk existing, tambahkan getConfirmation di bagian Delete) ...

void crudProduk(int isAdmin) {
    int selected = 0;
    int totalMenu = isAdmin ? 5 : 2;
    const char *menuItems[] = {"Lihat Data", "Tambah Data", "Edit Data", "Hapus Data", "Kembali"};
    const char *menuItemsStaff[] = {"Lihat Data", "Kembali"};

    int tableX = getCenterXForTable(95);
    int formBoxX, formBoxY, boxW, boxH, formX, formInputX = SIDEBAR_WIDTH + 25, formY;
    int isPagingMode = 0; pageOffset = 0;

    // INIT LAYOUT
    drawBaseLayout(isAdmin ? "MANAJEMEN PRODUK" : "KATALOG GUDANG");
    drawHomeLogo();
    int needContentRedraw = 0;

    while(1) {
        int startY = HEADER_HEIGHT + 6;
        if(isAdmin) {
             for(int i=0; i<totalMenu; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
        } else {
             for(int i=0; i<totalMenu; i++) printMenuItem(4, startY + (i*2), (char*)menuItemsStaff[i], (i == selected));
        }

        if(needContentRedraw) {
            refreshRightArea(isPagingMode, displayProdukTable, tableX);
            needContentRedraw = 0;
        }

        if(!isPagingMode) drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");
        else drawNavigationLegend("[PANAH] Scroll | [ESC] Menu");

        int key = getch();
        if(key == 224) {
            key = getch();
            if(isPagingMode) {
                 int totalPages = (int)ceil((double)totalProduk / ROWS_PER_PAGE);
                 if(key == KEY_UP && pageOffset > 0) { pageOffset--; needContentRedraw = 1; }
                 else if(key == KEY_DOWN && pageOffset < totalPages - 1) { pageOffset++; needContentRedraw = 1; }
            } else {
                 if(key == KEY_UP) { selected--; if(selected < 0) selected = totalMenu - 1; }
                 else if(key == KEY_DOWN) { selected++; if(selected >= totalMenu) selected = 0; }
            }
        }
        else if(key == 13 && !isPagingMode) {
            if(isAdmin && selected == 4) break;
            if(!isAdmin && selected == 1) break;

            if(selected == 0) { // Lihat Data
                isPagingMode = 1; pageOffset = 0; needContentRedraw = 1;
            }
            else if(isAdmin && selected == 1) { // Tambah
                formX = SIDEBAR_WIDTH + 7; formY = HEADER_HEIGHT + 5;
                if(totalProduk >= MAX_DATA) { showErrorAndWait(formX, formY, "Penuh!"); continue; }
                drawFormBox("TAMBAH PRODUK", &formBoxX, &formBoxY, &boxW, &boxH);

                int newId = (totalProduk > 0) ? dbProduk[totalProduk-1].id + 1 : 1;
                gotoxy(formX, formY+2); printf("ID Otomatis : %d", newId);
                gotoxy(formX, formY+4); printf("Nama Produk : "); getValidatedString(dbProduk[totalProduk].nama, 49, formInputX, formY+4);
                gotoxy(formX, formY+6); printf("Stok Awal   : "); dbProduk[totalProduk].stok = (int)getValidatedNumber(formInputX, formY+6);
                gotoxy(formX, formY+8); printf("Harga (Rp)  : "); dbProduk[totalProduk].harga = getValidatedNumber(formInputX, formY+8);

                dbProduk[totalProduk].id = newId; totalProduk++;
                showErrorAndWait(formX, formY+10, "Produk Disimpan!");
                drawBaseLayout(isAdmin ? "MANAJEMEN PRODUK" : "KATALOG GUDANG"); drawHomeLogo();
            }
            else if(isAdmin && (selected == 2 || selected == 3)) { // Edit/Hapus
                formX = SIDEBAR_WIDTH + 7; formY = HEADER_HEIGHT + 5;
                drawFormBox(selected == 2 ? "EDIT PRODUK" : "HAPUS PRODUK", &formBoxX, &formBoxY, &boxW, &boxH);
                gotoxy(formX, formY+2); printf("ID Target   : ");
                int tid = (int)getValidatedNumber(formInputX, formY+2);
                int idx = -1;
                for(int i=0; i<totalProduk; i++) if(dbProduk[i].id==tid) idx=i;

                if(idx!=-1) {
                    if(selected==2) {
                        gotoxy(formX, formY+4); printf("Nama Baru   : "); getValidatedString(dbProduk[idx].nama, 49, formInputX, formY+4);
                        gotoxy(formX, formY+6); printf("Stok Baru   : "); dbProduk[idx].stok = (int)getValidatedNumber(formInputX, formY+6);
                        gotoxy(formX, formY+8); printf("Harga Baru  : "); dbProduk[idx].harga = getValidatedNumber(formInputX, formY+8);
                        showErrorAndWait(formX, formY+10, "Update Berhasil!");
                    } else {
                         for(int j=idx; j<totalProduk-1; j++) dbProduk[j]=dbProduk[j+1];
                         totalProduk--;
                         showErrorAndWait(formX, formY+4, "Produk Dihapus!");
                    }
                } else showErrorAndWait(formX, formY+4, "ID Tidak Ditemukan!");
                drawBaseLayout(isAdmin ? "MANAJEMEN PRODUK" : "KATALOG GUDANG"); drawHomeLogo();
            }
        }
        else if(key == 27) {
            if(isPagingMode) { isPagingMode = 0; needContentRedraw = 1; }
            else break;
        }
    }
}

// ---------------------------------------------------------
// 3. CRUD SUPPLIER
// ---------------------------------------------------------
void displaySupplierTable(int tableX) {
    const int TABLE_WIDTH = 105;
    const int TABLE_HEIGHT = 12;
    int startY = HEADER_HEIGHT + 4;
    int totalPages = (int)ceil((double)totalSupplier / ROWS_PER_PAGE);
    if (totalPages == 0) totalPages = 1;
    int startIndex = pageOffset * ROWS_PER_PAGE;
    int endIndex = startIndex + ROWS_PER_PAGE;
    if (endIndex > totalSupplier) endIndex = totalSupplier;

    printCenterRight(HEADER_HEIGHT + 2, "=== DATA SUPPLIER ===");
    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, TABLE_HEIGHT + 1);

    textNormal();
    gotoxy(tableX, startY);
    printf(" %-5s %c %-30s %c %-40s %c %-18s ", "ID", 186, "NAMA SUPPLIER", 186, "ALAMAT", 186, "KONTAK");
    gotoxy(tableX, startY + 1); for(int i=0; i<TABLE_WIDTH; i++) printf("%c", 205);

    for(int i = startIndex; i < endIndex; i++) {
        int r = startY + 2 + (i - startIndex);
        char n[31], a[41], k[21];
        strncpy(n, dbSupplier[i].nama, 30); n[30]=0;
        strncpy(a, dbSupplier[i].alamat, 40); a[40]=0;
        strncpy(k, dbSupplier[i].kontak, 18); k[18]=0;
        gotoxy(tableX, r);
        printf(" %03d   %c %-30s %c %-40s %c %-18s ", dbSupplier[i].id, 186, n, 186, a, 186, k);
    }
    gotoxy(tableX + 5, startY + TABLE_HEIGHT + 2);
    printf("Hal %d / %d | Total Data: %d", pageOffset + 1, totalPages, totalSupplier);
}

void crudSupplier() {
    int selected = 0;
    const int totalMenu = 5;
    const char *menuItems[] = {"Lihat Data", "Tambah Data", "Edit Data", "Hapus Data", "Kembali"};
    int tableX = getCenterXForTable(105);
    int formBoxX, formBoxY, boxW, boxH, formX, formInputX = SIDEBAR_WIDTH + 25, formY;
    int isPagingMode = 0; pageOffset = 0;

    drawBaseLayout("MANAJEMEN SUPPLIER");
    drawHomeLogo();
    int needContentRedraw = 0;

    while(1) {
        int startY = HEADER_HEIGHT + 6;
        for(int i=0; i<totalMenu; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));

        if(needContentRedraw) {
            refreshRightArea(isPagingMode, displaySupplierTable, tableX);
            needContentRedraw = 0;
        }

        if(!isPagingMode) drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");
        else drawNavigationLegend("[PANAH] Scroll | [ESC] Menu");

        int key = getch();
        if(key == 224) {
            key = getch();
            if(isPagingMode) {
                 int totalPages = (int)ceil((double)totalSupplier / ROWS_PER_PAGE);
                 if(key == KEY_UP && pageOffset > 0) { pageOffset--; needContentRedraw = 1; }
                 else if(key == KEY_DOWN && pageOffset < totalPages - 1) { pageOffset++; needContentRedraw = 1; }
            } else {
                 if(key == KEY_UP) { selected--; if(selected < 0) selected = totalMenu - 1; }
                 else if(key == KEY_DOWN) { selected++; if(selected >= totalMenu) selected = 0; }
            }
        }
        else if(key == 13 && !isPagingMode) {
            if(selected == 4) break;
            if(selected == 0) { isPagingMode = 1; pageOffset = 0; needContentRedraw = 1; }
            else {
                formX = SIDEBAR_WIDTH + 7; formY = HEADER_HEIGHT + 5;
                if(selected == 1) { // Tambah
                    drawFormBox("TAMBAH SUPPLIER", &formBoxX, &formBoxY, &boxW, &boxH);
                    int nid = (totalSupplier > 0) ? dbSupplier[totalSupplier-1].id + 1 : 1000;
                    gotoxy(formX, formY+2); printf("ID Otomatis : %d", nid);
                    gotoxy(formX, formY+4); printf("Nama Supplier:"); getValidatedString(dbSupplier[totalSupplier].nama, 49, formInputX, formY+4);
                    gotoxy(formX, formY+6); printf("Alamat       :"); getValidatedString(dbSupplier[totalSupplier].alamat, 99, formInputX, formY+6);
                    gotoxy(formX, formY+8); printf("Kontak       :"); getValidatedPhoneNumber(dbSupplier[totalSupplier].kontak, 19, formInputX, formY+8, 2, -1);
                    dbSupplier[totalSupplier].id = nid; totalSupplier++;
                    showErrorAndWait(formX, formY+10, "Disimpan!");
                }
                else { // Edit / Hapus
                    drawFormBox(selected == 2 ? "EDIT" : "HAPUS", &formBoxX, &formBoxY, &boxW, &boxH);
                    gotoxy(formX, formY+2); printf("ID Target   : ");
                    int tid = (int)getValidatedNumber(formInputX, formY+2);
                    int idx = -1; for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].id == tid) idx = i;
                    if(idx != -1) {
                        if(selected == 2) {
                            gotoxy(formX, formY+4); printf("Nama Baru   : "); getValidatedString(dbSupplier[idx].nama, 49, formInputX, formY+4);
                            gotoxy(formX, formY+6); printf("Alamat Baru : "); getValidatedString(dbSupplier[idx].alamat, 99, formInputX, formY+6);
                            gotoxy(formX, formY+8); printf("Kontak Baru : "); getValidatedPhoneNumber(dbSupplier[idx].kontak, 19, formInputX, formY+8, 2, dbSupplier[idx].id);
                            showErrorAndWait(formX, formY+10, "Diupdate!");
                        } else {
                            for(int j=idx; j<totalSupplier-1; j++) dbSupplier[j] = dbSupplier[j+1];
                            totalSupplier--;
                            showErrorAndWait(formX, formY+4, "Dihapus!");
                        }
                    } else showErrorAndWait(formX, formY+4, "Tidak Ditemukan!");
                }
                drawBaseLayout("MANAJEMEN SUPPLIER"); drawHomeLogo();
            }
        }
        else if (key == 27) {
            if(isPagingMode) { isPagingMode = 0; needContentRedraw = 1; }
            else break;
        }
    }
}

// ---------------------------------------------------------
// 4. CRUD GUDANG
// ---------------------------------------------------------

void displayGudangTable(int tableX) {
    const int TABLE_WIDTH = 95;
    const int TABLE_HEIGHT = 12;
    int startY = HEADER_HEIGHT + 4;
    int totalPages = (int)ceil((double)totalGudang / ROWS_PER_PAGE);
    if (totalPages == 0) totalPages = 1;
    int startIndex = pageOffset * ROWS_PER_PAGE;
    int endIndex = startIndex + ROWS_PER_PAGE;
    if (endIndex > totalGudang) endIndex = totalGudang;

    printCenterRight(HEADER_HEIGHT + 2, "=== DATA GUDANG ===");
    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, TABLE_HEIGHT + 1);

    textNormal();
    gotoxy(tableX, startY);
    printf(" %-5s %c %-30s %c %-50s ", "ID", 186, "NAMA GUDANG", 186, "ALAMAT LENGKAP");
    gotoxy(tableX, startY + 1); for(int i=0; i<TABLE_WIDTH; i++) printf("%c", 205);

    for(int i = startIndex; i < endIndex; i++) {
        int r = startY + 2 + (i - startIndex);
        char n[31], a[51];
        strncpy(n, dbGudang[i].nama, 30); n[30]=0;
        strncpy(a, dbGudang[i].alamat, 50); a[50]=0;
        gotoxy(tableX, r);
        printf(" %03d   %c %-30s %c %-50s ", dbGudang[i].id, 186, n, 186, a);
    }
    gotoxy(tableX + 5, startY + TABLE_HEIGHT + 2);
    printf("Hal %d / %d | Total Data: %d", pageOffset + 1, totalPages, totalGudang);
}

void crudGudang() {
    int selected = 0;
    const int totalMenu = 5;
    const char *menuItems[] = {"Lihat Data", "Tambah Data", "Edit Data", "Hapus Data", "Kembali"};
    int tableX = getCenterXForTable(95);
    int formBoxX, formBoxY, boxW, boxH, formX, formInputX = SIDEBAR_WIDTH + 25, formY;
    int isPagingMode = 0; pageOffset = 0;

    drawBaseLayout("MANAJEMEN GUDANG");
    drawHomeLogo();
    int needContentRedraw = 0;

    while(1) {
        int startY = HEADER_HEIGHT + 6;
        for(int i=0; i<totalMenu; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));

        if(needContentRedraw) {
            refreshRightArea(isPagingMode, displayGudangTable, tableX);
            needContentRedraw = 0;
        }

        if(!isPagingMode) drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");
        else drawNavigationLegend("[PANAH] Scroll | [ESC] Menu");

        int key = getch();
        if(key == 224) {
            key = getch();
            if(isPagingMode) {
                 int totalPages = (int)ceil((double)totalGudang / ROWS_PER_PAGE);
                 if(key == KEY_UP && pageOffset > 0) { pageOffset--; needContentRedraw = 1; }
                 else if(key == KEY_DOWN && pageOffset < totalPages - 1) { pageOffset++; needContentRedraw = 1; }
            } else {
                 if(key == KEY_UP) { selected--; if(selected < 0) selected = totalMenu - 1; }
                 else if(key == KEY_DOWN) { selected++; if(selected >= totalMenu) selected = 0; }
            }
        }
        else if(key == 13 && !isPagingMode) {
            if(selected == 4) break;
            if(selected == 0) { isPagingMode = 1; pageOffset = 0; needContentRedraw = 1; }
            else {
                formX = SIDEBAR_WIDTH + 7; formY = HEADER_HEIGHT + 5;
                if(selected == 1) {
                    drawFormBox("TAMBAH GUDANG", &formBoxX, &formBoxY, &boxW, &boxH);
                    int nid = (totalGudang > 0) ? dbGudang[totalGudang-1].id + 1 : 9000;
                    gotoxy(formX, formY+2); printf("ID Otomatis : %d", nid);
                    gotoxy(formX, formY+4); printf("Nama Gudang :"); getValidatedString(dbGudang[totalGudang].nama, 49, formInputX, formY+4);
                    gotoxy(formX, formY+6); printf("Alamat      :"); getValidatedString(dbGudang[totalGudang].alamat, 99, formInputX, formY+6);
                    dbGudang[totalGudang].id = nid; totalGudang++;
                    showErrorAndWait(formX, formY+10, "Disimpan!");
                } else {
                    drawFormBox(selected == 2 ? "EDIT" : "HAPUS", &formBoxX, &formBoxY, &boxW, &boxH);
                    gotoxy(formX, formY+2); printf("ID Target   : ");
                    int tid = (int)getValidatedNumber(formInputX, formY+2);
                    int idx = -1; for(int i=0; i<totalGudang; i++) if(dbGudang[i].id == tid) idx = i;
                    if(idx != -1) {
                        if(selected == 2) {
                             gotoxy(formX, formY+4); printf("Nama Baru   : "); getValidatedString(dbGudang[idx].nama, 49, formInputX, formY+4);
                             gotoxy(formX, formY+6); printf("Alamat Baru : "); getValidatedString(dbGudang[idx].alamat, 99, formInputX, formY+6);
                             showErrorAndWait(formX, formY+10, "Diupdate!");
                        } else {
                             for(int j=idx; j<totalGudang-1; j++) dbGudang[j] = dbGudang[j+1];
                             totalGudang--;
                             showErrorAndWait(formX, formY+4, "Dihapus!");
                        }
                    } else showErrorAndWait(formX, formY+4, "Tidak Ditemukan!");
                }
                drawBaseLayout("MANAJEMEN GUDANG"); drawHomeLogo();
            }
        }
        else if (key == 27) {
            if(isPagingMode) { isPagingMode = 0; needContentRedraw = 1; }
            else break;
        }
    }
}