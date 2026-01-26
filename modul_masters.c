#include "controllers.h"
#include "config.h"
#include "globals.h"
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

// Helper untuk refresh tampilan tabel/logo kanan
void refreshRightArea(int isTableMode, void (*displayFunc)(int, int), int tableX, int hideId) {
    if (isTableMode) {
        clearRightContent();
        displayFunc(tableX, hideId);
        drawNavigationLegend("[TAB] Show/Hide ID | [PANAH] Scroll | [ESC] Menu");
    } else {
        drawHomeLogo(ROLE_ADMIN);
    }
}

// ============================================================================
// 1. MANAJEMEN KARYAWAN
// ============================================================================
void displayKaryawanTable(int tableX, int hideId) {
    int startY = HEADER_HEIGHT + 4; int tableWidth = 100;
    drawTableBox(tableX - 1, startY - 1, tableWidth + 2, 13);
    textNormal();
    gotoxy(tableX, startY); printf(" %-5s %c %-20s %c %-15s %c %-15s %c %-8s ", "ID", 186, "NAMA", 186, "JABATAN", 186, "USERNAME", 186, "PERF");
    gotoxy(tableX, startY + 1); for(int i=0; i<tableWidth; i++) printf("%c", 205);

    int start = pageOffset * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > totalKaryawan) ? totalKaryawan : start + ROWS_PER_PAGE;
    for(int i = start; i < end; i++) {
        gotoxy(tableX, startY + 2 + (i - start));
        char idStr[10]; if (hideId) strcpy(idStr, "***"); else sprintf(idStr, "%04d", dbKaryawan[i].id);
        char perfStr[10]; if(dbKaryawan[i].roleId == ROLE_ADMIN) strcpy(perfStr, "-"); else sprintf(perfStr, "%d", dbKaryawan[i].performa);
        printf(" %-5s %c %-20s %c %-15s %c %-15s %c %-8s ", idStr, 186, dbKaryawan[i].nama, 186, dbKaryawan[i].jabatan, 186, dbKaryawan[i].username, 186, perfStr);
    }
}

void crudKaryawan() {
    int selected = 0; const char *menuItems[] = {"Lihat Data", "Input Data", "Update Data", "Hapus Data", "Kembali"};
    int hideId = 1; int tableX = getCenterXForTable(100); int isTableMode = 0; pageOffset = 0;

    drawBaseLayout("MENU KARYAWAN"); drawHomeLogo(ROLE_ADMIN);

    while(1) {
        int startY = HEADER_HEIGHT + 6; for(int i=0; i<5; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
        if(isTableMode) refreshRightArea(1, displayKaryawanTable, tableX, hideId); else drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");

        int key = getch();
        if (key == KEY_TAB && isTableMode) { hideId = !hideId; continue; }

        if(key == 224) {
            key = getch();
            if(isTableMode) { int totalPages = (int)ceil((double)totalKaryawan / ROWS_PER_PAGE); if(key == KEY_UP && pageOffset > 0) pageOffset--; else if(key == KEY_DOWN && pageOffset < totalPages - 1) pageOffset++; }
            else { if(key==KEY_UP) selected = (selected > 0) ? selected-1 : 4; if(key==KEY_DOWN) selected = (selected < 4) ? selected+1 : 0; }
        }
        else if(key == 13 && !isTableMode) {
            if(selected == 4) break;
            if(selected == 0) { isTableMode = 1; pageOffset = 0; }
            if(selected == 1) {
                int fx, fy, bw, bh; int formInputX = SIDEBAR_WIDTH + 25; drawFormBox("TAMBAH KARYAWAN", &fx, &fy, &bw, &bh);
                if(totalKaryawan >= MAX_DATA) { showErrorAndWait(fx, fy, "Database Penuh!"); continue; }
                int newId = totalKaryawan + 1; gotoxy(fx+2, fy+2); printf("ID Baru : %d", newId);
                gotoxy(fx+2, fy+4); printf("Nama    : "); getValidatedString(dbKaryawan[totalKaryawan].nama, 49, formInputX, fy+4);

                // UPDATE: Input Role ID dengan panduan
                gotoxy(fx+2, fy+6); printf("Role ID : ");
                gotoxy(fx+2, fy+7); printf("(0:Admin, 1:Kasir, 2:Gudang)");
                int rid = (int)getValidatedNumber(formInputX, fy+6);

                dbKaryawan[totalKaryawan].roleId = rid; strcpy(dbKaryawan[totalKaryawan].jabatan, getJabatanName(rid));
                gotoxy(fx+2, fy+8); printf("Username: "); getValidatedString(dbKaryawan[totalKaryawan].username, 29, formInputX, fy+8);
                gotoxy(fx+2, fy+10); printf("Password: "); getValidatedString(dbKaryawan[totalKaryawan].password, 29, formInputX, fy+10);
                sprintf(dbKaryawan[totalKaryawan].kontak, "081%08d", rand()); dbKaryawan[totalKaryawan].id = newId; dbKaryawan[totalKaryawan].performa = 80;
                totalKaryawan++; saveAllData(); showErrorAndWait(fx+2, fy+12, "Data Tersimpan!"); drawBaseLayout("MENU KARYAWAN"); drawHomeLogo(ROLE_ADMIN);
            }
            if(selected == 2) {
                clearRightContent(); int fx, fy, bw, bh; drawFormBox("UPDATE KARYAWAN", &fx, &fy, &bw, &bh);
                gotoxy(fx+2, fy+2); printf("Masukkan ID Karyawan: "); int targetID = (int)getValidatedNumber(fx+24, fy+2);
                int idx = -1; for(int i=0; i<totalKaryawan; i++) { if(dbKaryawan[i].id == targetID) { idx = i; break; } }
                if(idx != -1) {
                    gotoxy(fx+2, fy+4); printf("Nama: %s", dbKaryawan[idx].nama);
                    drawShadowBox(fx, fy+6, bw, 8);
                    gotoxy(fx+2, fy+7); printf("Pilih Data yang Diubah:");
                    gotoxy(fx+4, fy+8); printf("[1] Username"); gotoxy(fx+4, fy+9); printf("[2] Password");
                    gotoxy(fx+4, fy+10); printf("[3] No Telepon"); gotoxy(fx+4, fy+11); printf("[0] Batal");
                    gotoxy(fx+2, fy+13); printf("Pilihan Anda: "); int choice = (int)getValidatedNumber(fx+16, fy+13);
                    if(choice == 1) { gotoxy(fx+2, fy+15); printf("Username Baru: "); getValidatedString(dbKaryawan[idx].username, 29, fx+17, fy+15); saveAllData(); showErrorAndWait(fx+2, fy+17, "Username Diupdate!"); }
                    else if(choice == 2) { gotoxy(fx+2, fy+15); printf("Password Baru: "); getValidatedString(dbKaryawan[idx].password, 29, fx+17, fy+15); saveAllData(); showErrorAndWait(fx+2, fy+17, "Password Diupdate!"); }
                    else if(choice == 3) { gotoxy(fx+2, fy+15); printf("No Telp Baru : "); getValidatedPhoneNumber(dbKaryawan[idx].kontak, 19, fx+17, fy+15, 0, 0); saveAllData(); showErrorAndWait(fx+2, fy+17, "Kontak Diupdate!"); }
                    else { showErrorAndWait(fx+2, fy+15, "Update Dibatalkan."); }
                } else { showErrorAndWait(fx+2, fy+4, "ID Tidak Ditemukan!"); }
                drawBaseLayout("MENU KARYAWAN"); drawHomeLogo(ROLE_ADMIN);
            }
            if(selected == 3) {
                int fx, fy, bw, bh; drawFormBox("HAPUS KARYAWAN", &fx, &fy, &bw, &bh);
                gotoxy(fx+2, fy+2); printf("ID Hapus: "); int tid = (int)getValidatedNumber(fx+12, fy+2);
                int idx = -1; for(int i=0; i<totalKaryawan; i++) if(dbKaryawan[i].id==tid) idx=i;
                if(idx!=-1 && getConfirmation(fx+2, fy+4, "Yakin Hapus")) { for(int j=idx; j<totalKaryawan-1; j++) dbKaryawan[j]=dbKaryawan[j+1]; totalKaryawan--; saveAllData(); showErrorAndWait(fx+2, fy+6, "Terhapus!"); }
                else showErrorAndWait(fx+2, fy+6, "Batal/Gagal!"); drawBaseLayout("MENU KARYAWAN"); drawHomeLogo(ROLE_ADMIN);
            }
        }
        else if(key == 27 && isTableMode) { isTableMode = 0; clearRightContent(); drawHomeLogo(ROLE_ADMIN); }
    }
}

// ============================================================================
// 2. CRUD PRODUK (TAMBAH, EDIT HARGA, HAPUS)
// ============================================================================
void displayProdukTable(int tableX, int hideId) {
    int startY = HEADER_HEIGHT + 4; drawTableBox(tableX - 1, startY - 1, 95 + 2, 13); textNormal();
    gotoxy(tableX, startY); printf(" %-5s %c %-40s %c %-10s %c %-20s ", "ID", 186, "NAMA PRODUK", 186, "STOK", 186, "HARGA (Rp)");
    gotoxy(tableX, startY + 1); for(int i=0; i<95; i++) printf("%c", 205);
    int start = pageOffset * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > totalProduk) ? totalProduk : start + ROWS_PER_PAGE;
    for(int i = start; i < end; i++) {
        char rp[30], idStr[10]; formatRupiah(dbProduk[i].harga, rp);
        if (hideId) strcpy(idStr, "***"); else sprintf(idStr, "%03d", dbProduk[i].id);
        gotoxy(tableX, startY + 2 + (i - start)); printf(" %-5s %c %-40s %c %-10d %c Rp %-17s ", idStr, 186, dbProduk[i].nama, 186, dbProduk[i].stok, 186, rp);
    }
}

void crudProduk(int isAdmin) {
    int selected = 0;
    const char *menuItems[] = {"Lihat Data", "Input Data", "Update Data", "Hapus Data", "Kembali"};
    int hideId = 1; int tableX = getCenterXForTable(95); int isPagingMode = 0; pageOffset = 0;

    // FIX: Menggunakan ROLE_WAREHOUSE sebagai pengganti ROLE_STAFF_WAREHOUSE
    drawBaseLayout(isAdmin ? "MANAJEMEN PRODUK" : "KATALOG GUDANG");
    drawHomeLogo(isAdmin ? ROLE_ADMIN : ROLE_WAREHOUSE);

    while(1) {
        int startY = HEADER_HEIGHT + 6;
        int limit = isAdmin ? 5 : 2; // Staff Gudang cuma bisa Lihat Data
        for(int i=0; i<limit; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));

        if(isPagingMode) refreshRightArea(1, displayProdukTable, tableX, hideId);
        else drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");

        int key = getch();
        if (key == KEY_TAB && isPagingMode) { hideId = !hideId; continue; }

        if(key == 224) {
            key = getch();
            if(isPagingMode) {
                int totalPages = (int)ceil((double)totalProduk / ROWS_PER_PAGE);
                if(key == KEY_UP && pageOffset > 0) pageOffset--;
                if(key == KEY_DOWN && pageOffset < totalPages - 1) pageOffset++;
            }
            else {
                if(key == KEY_UP) selected = (selected > 0) ? selected - 1 : limit - 1;
                else if(key == KEY_DOWN) selected = (selected < limit - 1) ? selected + 1 : 0;
            }
        }
        else if(key == 13 && !isPagingMode) {
            if(isAdmin && selected == 4) break; if(!isAdmin && selected == 1) break; // Kembali

            // 1. LIHAT DATA
            if(selected == 0) { isPagingMode = 1; pageOffset = 0; }

            // 2. INPUT DATA (TAMBAH PRODUK)
            if(isAdmin && selected == 1) {
                clearRightContent(); int fx, fy, bw, bh;
                drawFormBox("TAMBAH PRODUK", &fx, &fy, &bw, &bh);
                if(totalProduk >= MAX_DATA) { showErrorAndWait(fx, fy, "Database Penuh!"); continue; }

                int newId = totalProduk + 1;
                gotoxy(fx+2, fy+2); printf("ID Baru    : %d", newId);
                gotoxy(fx+2, fy+4); printf("Nama Prod : "); getValidatedString(dbProduk[totalProduk].nama, 39, fx+14, fy+4);
                gotoxy(fx+2, fy+6); printf("Stok Awal : "); dbProduk[totalProduk].stok = (int)getValidatedNumber(fx+14, fy+6);
                gotoxy(fx+2, fy+8); printf("Harga (Rp): "); dbProduk[totalProduk].harga = getValidatedNumber(fx+14, fy+8);
                dbProduk[totalProduk].id = newId;

                totalProduk++; saveAllData();
                showErrorAndWait(fx+2, fy+10, "Produk Tersimpan!");
                drawBaseLayout("MANAJEMEN PRODUK"); drawHomeLogo(ROLE_ADMIN);
            }

            // 3. UPDATE DATA (HANYA HARGA)
            if(isAdmin && selected == 2) {
                clearRightContent(); int fx, fy, bw, bh;
                drawFormBox("UPDATE HARGA", &fx, &fy, &bw, &bh);

                gotoxy(fx+2, fy+2); printf("ID Produk : ");
                int tid = (int)getValidatedNumber(fx+14, fy+2);

                int idx = -1;
                for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == tid) idx=i;

                if(idx != -1) {
                    gotoxy(fx+2, fy+4); printf("Produk    : %s", dbProduk[idx].nama);
                    char rp[30]; formatRupiah(dbProduk[idx].harga, rp);
                    gotoxy(fx+2, fy+6); printf("Harga Lama: Rp %s", rp);

                    // Input Harga Baru
                    drawShadowBox(fx, fy+8, bw, 4);
                    gotoxy(fx+2, fy+9); printf("Harga Baru: Rp ");
                    long newPrice = getValidatedNumber(fx+17, fy+9);

                    dbProduk[idx].harga = newPrice;
                    saveAllData();
                    showErrorAndWait(fx+2, fy+11, "Harga Diupdate!");
                } else {
                    showErrorAndWait(fx+2, fy+4, "ID Tidak Ada!");
                }
                drawBaseLayout("MANAJEMEN PRODUK"); drawHomeLogo(ROLE_ADMIN);
            }

            // 4. HAPUS DATA
            if(isAdmin && selected == 3) {
                clearRightContent(); int fx, fy, bw, bh;
                drawFormBox("HAPUS PRODUK", &fx, &fy, &bw, &bh);

                gotoxy(fx+2, fy+2); printf("ID Hapus  : ");
                int tid = (int)getValidatedNumber(fx+14, fy+2);

                int idx = -1;
                for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == tid) idx=i;

                if(idx != -1) {
                    gotoxy(fx+2, fy+4); printf("Menghapus : %s", dbProduk[idx].nama);
                    if(getConfirmation(fx+2, fy+6, "Yakin Hapus")) {
                        for(int j=idx; j<totalProduk-1; j++) dbProduk[j] = dbProduk[j+1];
                        totalProduk--; saveAllData();
                        showErrorAndWait(fx+2, fy+8, "Terhapus!");
                    } else {
                        showErrorAndWait(fx+2, fy+8, "Dibatalkan.");
                    }
                } else {
                    showErrorAndWait(fx+2, fy+4, "ID Tidak Ada!");
                }
                drawBaseLayout("MANAJEMEN PRODUK"); drawHomeLogo(ROLE_ADMIN);
            }

        } else if(key == 27 && isPagingMode) {
            isPagingMode = 0;
            clearRightContent();
            // FIX: Menggunakan ROLE_WAREHOUSE
            drawHomeLogo(isAdmin ? ROLE_ADMIN : ROLE_WAREHOUSE);
        }
    }
}

// ============================================================================
// 3. CRUD SUPPLIER (TAMBAH, EDIT LENGKAP, HAPUS)
// ============================================================================
void displaySupplierTable(int tableX, int hideId) {
    const int TABLE_WIDTH = 105; int startY = HEADER_HEIGHT + 4;
    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, 13); textNormal();
    gotoxy(tableX, startY); printf(" %-5s %c %-30s %c %-40s %c %-18s ", "ID", 186, "NAMA SUPPLIER", 186, "ALAMAT", 186, "KONTAK");
    gotoxy(tableX, startY + 1); for(int i=0; i<TABLE_WIDTH; i++) printf("%c", 205);
    int start = pageOffset * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > totalSupplier) ? totalSupplier : start + ROWS_PER_PAGE;
    for(int i = start; i < end; i++) {
        char idStr[10], n[31], a[41]; if (hideId) strcpy(idStr, "***"); else sprintf(idStr, "%03d", dbSupplier[i].id);
        strncpy(n, dbSupplier[i].nama, 30); n[30]=0; strncpy(a, dbSupplier[i].alamat, 40); a[40]=0;
        gotoxy(tableX, startY + 2 + (i - start)); printf(" %-5s %c %-30s %c %-40s %c %-18s ", idStr, 186, n, 186, a, 186, dbSupplier[i].kontak);
    }
}

void crudSupplier() {
    int selected = 0; const char *menuItems[] = {"Lihat Data", "Input Data", "Update Data", "Hapus Data", "Kembali"};
    int hideId = 1; int tableX = getCenterXForTable(105); int isPagingMode = 0; pageOffset = 0;

    drawBaseLayout("MANAJEMEN SUPPLIER"); drawHomeLogo(ROLE_ADMIN);

    while(1) {
        int startY = HEADER_HEIGHT + 6; for(int i=0; i<5; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
        if(isPagingMode) refreshRightArea(1, displaySupplierTable, tableX, hideId); else drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");

        int key = getch(); if (key == KEY_TAB && isPagingMode) { hideId = !hideId; continue; }

        if(key == 224) { key = getch(); if(isPagingMode) { int totalPages = (int)ceil((double)totalSupplier / ROWS_PER_PAGE); if(key == KEY_UP && pageOffset > 0) pageOffset--; if(key == KEY_DOWN && pageOffset < totalPages - 1) pageOffset++; } else { if(key == KEY_UP) selected = (selected > 0) ? selected - 1 : 4; else if(key == KEY_DOWN) selected = (selected < 4) ? selected + 1 : 0; } }
        else if(key == 13 && !isPagingMode) {
            if(selected == 4) break;

            // 1. LIHAT
            if(selected == 0) { isPagingMode = 1; pageOffset = 0; }

            // 2. INPUT DATA
            if(selected == 1) {
                clearRightContent(); int fx, fy, bw, bh;
                drawFormBox("TAMBAH SUPPLIER", &fx, &fy, &bw, &bh);
                if(totalSupplier >= MAX_DATA) { showErrorAndWait(fx, fy, "Database Penuh!"); continue; }

                int newId = (totalSupplier > 0) ? dbSupplier[totalSupplier-1].id + 1 : 101;
                gotoxy(fx+2, fy+2); printf("ID Baru    : %d", newId);
                gotoxy(fx+2, fy+4); printf("Nama PT    : "); getValidatedString(dbSupplier[totalSupplier].nama, 49, fx+14, fy+4);
                gotoxy(fx+2, fy+6); printf("Alamat     : "); getValidatedString(dbSupplier[totalSupplier].alamat, 99, fx+14, fy+6);
                gotoxy(fx+2, fy+8); printf("Kontak     : "); getValidatedString(dbSupplier[totalSupplier].kontak, 19, fx+14, fy+8);
                dbSupplier[totalSupplier].id = newId;

                totalSupplier++; saveAllData();
                showErrorAndWait(fx+2, fy+10, "Supplier Tersimpan!");
                drawBaseLayout("MANAJEMEN SUPPLIER"); drawHomeLogo(ROLE_ADMIN);
            }

            // 3. UPDATE DATA
            if(selected == 2) {
                clearRightContent(); int fx, fy, bw, bh;
                drawFormBox("UPDATE SUPPLIER", &fx, &fy, &bw, &bh);

                gotoxy(fx+2, fy+2); printf("ID Supplier: ");
                int tid = (int)getValidatedNumber(fx+15, fy+2);

                int idx = -1;
                for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].id == tid) idx=i;

                if(idx != -1) {
                    gotoxy(fx+2, fy+4); printf("Nama: %s", dbSupplier[idx].nama);

                    drawShadowBox(fx, fy+6, bw, 8);
                    gotoxy(fx+2, fy+7); printf("Pilih Data yang Diubah:");
                    gotoxy(fx+4, fy+8); printf("[1] Nama Supplier");
                    gotoxy(fx+4, fy+9); printf("[2] Alamat");
                    gotoxy(fx+4, fy+10); printf("[3] Kontak");
                    gotoxy(fx+4, fy+11); printf("[0] Batal");
                    gotoxy(fx+2, fy+13); printf("Pilihan Anda: ");
                    int choice = (int)getValidatedNumber(fx+16, fy+13);

                    if(choice == 1) { gotoxy(fx+2, fy+15); printf("Nama Baru: "); getValidatedString(dbSupplier[idx].nama, 49, fx+13, fy+15); saveAllData(); showErrorAndWait(fx+2, fy+17, "Nama Diupdate!"); }
                    else if(choice == 2) { gotoxy(fx+2, fy+15); printf("Alamat Baru: "); getValidatedString(dbSupplier[idx].alamat, 99, fx+15, fy+15); saveAllData(); showErrorAndWait(fx+2, fy+17, "Alamat Diupdate!"); }
                    else if(choice == 3) { gotoxy(fx+2, fy+15); printf("Kontak Baru: "); getValidatedString(dbSupplier[idx].kontak, 19, fx+15, fy+15); saveAllData(); showErrorAndWait(fx+2, fy+17, "Kontak Diupdate!"); }
                    else { showErrorAndWait(fx+2, fy+15, "Batal Update."); }
                } else {
                    showErrorAndWait(fx+2, fy+4, "ID Tidak Ditemukan!");
                }
                drawBaseLayout("MANAJEMEN SUPPLIER"); drawHomeLogo(ROLE_ADMIN);
            }

            // 4. HAPUS DATA
            if(selected == 3) {
                clearRightContent(); int fx, fy, bw, bh;
                drawFormBox("HAPUS SUPPLIER", &fx, &fy, &bw, &bh);

                gotoxy(fx+2, fy+2); printf("ID Hapus  : ");
                int tid = (int)getValidatedNumber(fx+14, fy+2);

                int idx = -1;
                for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].id == tid) idx=i;

                if(idx != -1) {
                    gotoxy(fx+2, fy+4); printf("Menghapus : %s", dbSupplier[idx].nama);
                    if(getConfirmation(fx+2, fy+6, "Yakin Hapus")) {
                        for(int j=idx; j<totalSupplier-1; j++) dbSupplier[j] = dbSupplier[j+1];
                        totalSupplier--; saveAllData();
                        showErrorAndWait(fx+2, fy+8, "Terhapus!");
                    } else {
                        showErrorAndWait(fx+2, fy+8, "Dibatalkan.");
                    }
                } else {
                    showErrorAndWait(fx+2, fy+4, "ID Tidak Ada!");
                }
                drawBaseLayout("MANAJEMEN SUPPLIER"); drawHomeLogo(ROLE_ADMIN);
            }
        }
        else if(key == 27 && isPagingMode) { isPagingMode = 0; clearRightContent(); drawHomeLogo(ROLE_ADMIN); }
    }
}

// ============================================================================
// 4. CRUD GUDANG (TAMBAH, EDIT NAMA/ALAMAT, HAPUS)
// ============================================================================
void displayGudangTable(int tableX, int hideId) {
    const int TABLE_WIDTH = 95; int startY = HEADER_HEIGHT + 4;
    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, 13); textNormal();
    gotoxy(tableX, startY); printf(" %-5s %c %-30s %c %-50s ", "ID", 186, "NAMA GUDANG", 186, "ALAMAT LENGKAP");
    gotoxy(tableX, startY + 1); for(int i=0; i<TABLE_WIDTH; i++) printf("%c", 205);
    int start = pageOffset * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > totalGudang) ? totalGudang : start + ROWS_PER_PAGE;
    for(int i = start; i < end; i++) {
        char idStr[10]; if (hideId) strcpy(idStr, "***"); else sprintf(idStr, "%03d", dbGudang[i].id);
        gotoxy(tableX, startY + 2 + (i - start)); printf(" %-5s %c %-30s %c %-50s ", idStr, 186, dbGudang[i].nama, 186, dbGudang[i].alamat);
    }
}

void crudGudang() {
    int selected = 0; const char *menuItems[] = {"Lihat Data", "Input Data", "Update Data", "Hapus Data", "Kembali"};
    int hideId = 1; int tableX = getCenterXForTable(95); int isPagingMode = 0; pageOffset = 0;

    drawBaseLayout("MANAJEMEN GUDANG"); drawHomeLogo(ROLE_ADMIN);

    while(1) {
        int startY = HEADER_HEIGHT + 6; for(int i=0; i<5; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
        if(isPagingMode) refreshRightArea(1, displayGudangTable, tableX, hideId); else drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");

        int key = getch(); if (key == KEY_TAB && isPagingMode) { hideId = !hideId; continue; }

        if(key == 224) { key = getch(); if(isPagingMode) { int totalPages = (int)ceil((double)totalGudang / ROWS_PER_PAGE); if(key == KEY_UP && pageOffset > 0) pageOffset--; if(key == KEY_DOWN && pageOffset < totalPages - 1) pageOffset++; } else { if(key == KEY_UP) selected = (selected > 0) ? selected - 1 : 4; else if(key == KEY_DOWN) selected = (selected < 4) ? selected + 1 : 0; } }
        else if(key == 13 && !isPagingMode) {
            if(selected == 4) break;

            // 1. LIHAT
            if(selected == 0) { isPagingMode = 1; pageOffset = 0; }

            // 2. INPUT DATA
            if(selected == 1) {
                clearRightContent(); int fx, fy, bw, bh;
                drawFormBox("TAMBAH GUDANG", &fx, &fy, &bw, &bh);
                if(totalGudang >= MAX_DATA) { showErrorAndWait(fx, fy, "Database Penuh!"); continue; }

                int newId = (totalGudang > 0) ? dbGudang[totalGudang-1].id + 1 : 9001;
                gotoxy(fx+2, fy+2); printf("ID Baru    : %d", newId);
                gotoxy(fx+2, fy+4); printf("Nama Gdng : "); getValidatedString(dbGudang[totalGudang].nama, 29, fx+14, fy+4);
                gotoxy(fx+2, fy+6); printf("Alamat     : "); getValidatedString(dbGudang[totalGudang].alamat, 99, fx+14, fy+6);
                dbGudang[totalGudang].id = newId;

                totalGudang++; saveAllData();
                showErrorAndWait(fx+2, fy+8, "Gudang Tersimpan!");
                drawBaseLayout("MANAJEMEN GUDANG"); drawHomeLogo(ROLE_ADMIN);
            }

            // 3. UPDATE DATA
            if(selected == 2) {
                clearRightContent(); int fx, fy, bw, bh;
                drawFormBox("UPDATE GUDANG", &fx, &fy, &bw, &bh);

                gotoxy(fx+2, fy+2); printf("ID Gudang: ");
                int tid = (int)getValidatedNumber(fx+13, fy+2);

                int idx = -1;
                for(int i=0; i<totalGudang; i++) if(dbGudang[i].id == tid) idx=i;

                if(idx != -1) {
                    gotoxy(fx+2, fy+4); printf("Nama: %s", dbGudang[idx].nama);

                    drawShadowBox(fx, fy+6, bw, 7);
                    gotoxy(fx+2, fy+7); printf("Pilih Data yang Diubah:");
                    gotoxy(fx+4, fy+8); printf("[1] Nama Gudang");
                    gotoxy(fx+4, fy+9); printf("[2] Alamat");
                    gotoxy(fx+4, fy+10); printf("[0] Batal");
                    gotoxy(fx+2, fy+12); printf("Pilihan Anda: ");
                    int choice = (int)getValidatedNumber(fx+16, fy+12);

                    if(choice == 1) { gotoxy(fx+2, fy+14); printf("Nama Baru: "); getValidatedString(dbGudang[idx].nama, 29, fx+13, fy+14); saveAllData(); showErrorAndWait(fx+2, fy+16, "Nama Diupdate!"); }
                    else if(choice == 2) { gotoxy(fx+2, fy+14); printf("Alamat Baru: "); getValidatedString(dbGudang[idx].alamat, 99, fx+15, fy+14); saveAllData(); showErrorAndWait(fx+2, fy+16, "Alamat Diupdate!"); }
                    else { showErrorAndWait(fx+2, fy+14, "Batal Update."); }
                } else {
                    showErrorAndWait(fx+2, fy+4, "ID Tidak Ditemukan!");
                }
                drawBaseLayout("MANAJEMEN GUDANG"); drawHomeLogo(ROLE_ADMIN);
            }

            // 4. HAPUS DATA
            if(selected == 3) {
                clearRightContent(); int fx, fy, bw, bh;
                drawFormBox("HAPUS GUDANG", &fx, &fy, &bw, &bh);

                gotoxy(fx+2, fy+2); printf("ID Hapus  : ");
                int tid = (int)getValidatedNumber(fx+14, fy+2);

                int idx = -1;
                for(int i=0; i<totalGudang; i++) if(dbGudang[i].id == tid) idx=i;

                if(idx != -1) {
                    gotoxy(fx+2, fy+4); printf("Menghapus : %s", dbGudang[idx].nama);
                    if(getConfirmation(fx+2, fy+6, "Yakin Hapus")) {
                        for(int j=idx; j<totalGudang-1; j++) dbGudang[j] = dbGudang[j+1];
                        totalGudang--; saveAllData();
                        showErrorAndWait(fx+2, fy+8, "Terhapus!");
                    } else {
                        showErrorAndWait(fx+2, fy+8, "Dibatalkan.");
                    }
                } else {
                    showErrorAndWait(fx+2, fy+4, "ID Tidak Ada!");
                }
                drawBaseLayout("MANAJEMEN GUDANG"); drawHomeLogo(ROLE_ADMIN);
            }
        }
        else if(key == 27 && isPagingMode) { isPagingMode = 0; clearRightContent(); drawHomeLogo(ROLE_ADMIN); }
    }
}