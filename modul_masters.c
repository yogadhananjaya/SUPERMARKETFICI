#include "controllers.h"
#include "config.h"
#include "globals.h"
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void refreshRightArea(int isTableMode, void (*displayFunc)(int, int), int tableX, int hideId) {
    if (isTableMode) {
        clearRightContent();
        displayFunc(tableX, hideId);
        drawNavigationLegend("[TAB] Show/Hide No | [PANAH] Scroll | [ESC] Menu");
    } else { drawHomeLogo(ROLE_ADMIN); }
}

int isUsernameExists(char* username, int ignoreId) {
    for(int i=0; i<totalKaryawan; i++) {
        if(dbKaryawan[i].isActive && dbKaryawan[i].id != ignoreId) {
            if(strcmp(dbKaryawan[i].username, username) == 0) return 1;
        }
    }
    return 0;
}

// 1. KARYAWAN
void displayKaryawanTable(int tableX, int hideId) {
    sortKaryawanByID(); // Ensure sorted
    int startY = HEADER_HEIGHT + 4; int tableWidth = 100;
    drawTableBox(tableX - 1, startY - 1, tableWidth + 2, 13);
    textNormal();
    gotoxy(tableX, startY); printf(" %-5s %c %-5s %c %-20s %c %-15s %c %-15s %c %-8s ", "NO", 186, "ID", 186, "NAMA", 186, "JABATAN", 186, "USERNAME", 186, "POIN");
    gotoxy(tableX, startY + 1); for(int i=0; i<tableWidth; i++) printf("%c", 205);

    // Filter Active Only for Paging
    int activeIndices[MAX_DATA]; int activeCount = 0;
    for(int i=0; i<totalKaryawan; i++) if(dbKaryawan[i].isActive) activeIndices[activeCount++] = i;

    int start = pageOffset * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > activeCount) ? activeCount : start + ROWS_PER_PAGE;
    for(int i = start; i < end; i++) {
        int idx = activeIndices[i];
        gotoxy(tableX, startY + 2 + (i - start));
        char noStr[10]; if (hideId) strcpy(noStr, "***"); else sprintf(noStr, "%03d", i+1);
        char perfStr[10]; if(dbKaryawan[idx].roleId == ROLE_ADMIN) strcpy(perfStr, "-"); else sprintf(perfStr, "%d", dbKaryawan[idx].performa);
        printf(" %-5s %c %04d  %c %-20s %c %-15s %c %-15s %c %-8s ", noStr, 186, dbKaryawan[idx].id, 186, dbKaryawan[idx].nama, 186, dbKaryawan[idx].jabatan, 186, dbKaryawan[idx].username, 186, perfStr);
    }
}

void crudKaryawan() {
    int selected = 0; const char *menuItems[] = {"Lihat Data", "Input Data", "Update Data", "Hapus Data", "Kembali"};
    int hideId = 0; int tableX = getCenterXForTable(100); int isTableMode = 0; pageOffset = 0;
    drawBaseLayout("MENU KARYAWAN"); drawHomeLogo(ROLE_ADMIN);
    while(1) {
        int startY = HEADER_HEIGHT + 6; for(int i=0; i<5; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
        if(isTableMode) refreshRightArea(1, displayKaryawanTable, tableX, hideId); else drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");
        int key = getch(); if (key == KEY_TAB && isTableMode) { hideId = !hideId; continue; }

        // Recount active for paging
        int activeCount = 0; for(int i=0; i<totalKaryawan; i++) if(dbKaryawan[i].isActive) activeCount++;

        if(key == 224) { key = getch(); if(isTableMode) { int totalPages = (int)ceil((double)activeCount / ROWS_PER_PAGE); if(key == KEY_UP && pageOffset > 0) pageOffset--; else if(key == KEY_DOWN && pageOffset < totalPages - 1) pageOffset++; } else { if(key==KEY_UP) selected = (selected > 0) ? selected-1 : 4; if(key==KEY_DOWN) selected = (selected < 4) ? selected+1 : 0; } }
        else if(key == 13 && !isTableMode) {
            if(selected == 4) break;
            if(selected == 0) { isTableMode = 1; pageOffset = 0; }
            if(selected == 1) { // INPUT
                int fx, fy, bw, bh; int formInputX = SIDEBAR_WIDTH + 25; drawFormBox("TAMBAH KARYAWAN", &fx, &fy, &bw, &bh);
                int newId = totalKaryawan + 1; gotoxy(fx+2, fy+2); printf("ID Baru : %d", newId);
                gotoxy(fx+2, fy+4); printf("Nama    : "); getValidatedString(dbKaryawan[totalKaryawan].nama, 49, formInputX, fy+4);
                gotoxy(fx+2, fy+6); printf("Role ID : "); gotoxy(fx+2, fy+7); printf("(0:Adm, 1:Ksr, 2:Gdg)"); int rid = (int)getValidatedNumber(formInputX, fy+6);

                // VALIDASI USERNAME
                char tempUser[30];
                while(1) {
                    gotoxy(fx+2, fy+8); printf("Username:                    ");
                    gotoxy(fx+12, fy+8); getValidatedString(tempUser, 29, formInputX, fy+8);
                    if(!isUsernameExists(tempUser, -1)) break;
                    showErrorAndWait(fx+2, fy+9, "Username Sudah Ada!"); gotoxy(fx+2, fy+9); printf("                   ");
                }
                strcpy(dbKaryawan[totalKaryawan].username, tempUser);

                dbKaryawan[totalKaryawan].roleId = rid; strcpy(dbKaryawan[totalKaryawan].jabatan, getJabatanName(rid));
                gotoxy(fx+2, fy+10); printf("Password: "); getValidatedString(dbKaryawan[totalKaryawan].password, 29, formInputX, fy+10);
                sprintf(dbKaryawan[totalKaryawan].kontak, "081%08d", rand()); dbKaryawan[totalKaryawan].id = newId;
                dbKaryawan[totalKaryawan].performa = 0; dbKaryawan[totalKaryawan].isActive = 1;
                totalKaryawan++; saveAllData(); showErrorAndWait(fx+2, fy+12, "Data Tersimpan!"); drawBaseLayout("MENU KARYAWAN"); drawHomeLogo(ROLE_ADMIN);
            }
            if(selected == 2) { // UPDATE
                clearRightContent(); int fx, fy, bw, bh; drawFormBox("PILIH KARYAWAN", &fx, &fy, &bw, &bh);
                gotoxy(fx+2, fy+2); printf("Masukkan ID: "); int targetID = (int)getValidatedNumber(fx+24, fy+2);
                int idx = -1; for(int i=0; i<totalKaryawan; i++) { if(dbKaryawan[i].isActive && dbKaryawan[i].id == targetID) { idx = i; break; } }
                if(idx != -1) {
                    clearRightContent(); drawSimpleFormBox("OPSI UPDATE", &fx, &fy, &bw, &bh);
                    gotoxy(fx+2, fy+2); printf("Nama   : %s", dbKaryawan[idx].nama);
                    gotoxy(fx+2, fy+3); printf("Jabatan: %s", dbKaryawan[idx].jabatan);
                    gotoxy(fx+2, fy+4); printf("Status : %s", dbKaryawan[idx].isActive ? "AKTIF" : "NON-AKTIF");
                    gotoxy(fx+2, fy+6); printf("[1] Username | [2] Password | [3] Kontak | [0] Batal");
                    gotoxy(fx+2, fy+8); printf("Pilihan: "); int choice = (int)getValidatedNumber(fx+11, fy+8);

                    if(choice == 1) {
                        char tempUser[30];
                        while(1) {
                            gotoxy(fx+2, fy+10); printf("Username Baru:               ");
                            gotoxy(fx+17, fy+10); getValidatedString(tempUser, 29, fx+17, fy+10);
                            if(!isUsernameExists(tempUser, dbKaryawan[idx].id)) break;
                            showErrorAndWait(fx+2, fy+11, "Username Sudah Ada!");
                        }
                        strcpy(dbKaryawan[idx].username, tempUser); saveAllData(); showErrorAndWait(fx+2, fy+12, "Sukses!");
                    }
                    else if(choice == 2) { gotoxy(fx+2, fy+10); printf("Password Baru: "); getValidatedString(dbKaryawan[idx].password, 29, fx+17, fy+10); saveAllData(); showErrorAndWait(fx+2, fy+12, "Sukses!"); }
                    else if(choice == 3) { gotoxy(fx+2, fy+10); printf("Kontak Baru  : "); getValidatedPhoneNumber(dbKaryawan[idx].kontak, 19, fx+17, fy+10, 0, 0); saveAllData(); showErrorAndWait(fx+2, fy+12, "Sukses!"); }
                    else showErrorAndWait(fx+2, fy+10, "Batal");
                } else showErrorAndWait(fx+2, fy+4, "Tidak Ditemukan!"); drawBaseLayout("MENU KARYAWAN"); drawHomeLogo(ROLE_ADMIN);
            }
            if(selected == 3) { // DELETE (SOFT)
                int fx, fy, bw, bh; drawFormBox("HAPUS KARYAWAN", &fx, &fy, &bw, &bh);
                gotoxy(fx+2, fy+2); printf("ID Hapus: "); int tid = (int)getValidatedNumber(fx+12, fy+2);
                int idx = -1; for(int i=0; i<totalKaryawan; i++) if(dbKaryawan[i].isActive && dbKaryawan[i].id==tid) idx=i;
                if(idx!=-1 && getConfirmation(fx+2, fy+4, "Hapus (Soft)?")) {
                    dbKaryawan[idx].isActive = 0; // SOFT DELETE
                    saveAllData(); showErrorAndWait(fx+2, fy+6, "Terhapus!");
                }
                else showErrorAndWait(fx+2, fy+6, "Batal/Gagal!"); drawBaseLayout("MENU KARYAWAN"); drawHomeLogo(ROLE_ADMIN);
            }
        } else if(key == 27 && isTableMode) { isTableMode = 0; clearRightContent(); drawHomeLogo(ROLE_ADMIN); }
    }
}

// 2. PRODUK
void displayProdukTable(int tableX, int hideId) {
    sortProdukByID();
    int startY = HEADER_HEIGHT + 4; drawTableBox(tableX - 1, startY - 1, 95 + 2, 13); textNormal();
    gotoxy(tableX, startY); printf(" %-5s %c %-5s %c %-40s %c %-10s %c %-20s ", "NO", 186, "ID", 186, "NAMA PRODUK", 186, "STOK", 186, "HARGA (Rp)");
    gotoxy(tableX, startY + 1); for(int i=0; i<95; i++) printf("%c", 205);

    int activeIndices[MAX_DATA]; int activeCount = 0;
    for(int i=0; i<totalProduk; i++) if(dbProduk[i].isActive) activeIndices[activeCount++] = i;

    int start = pageOffset * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > activeCount) ? activeCount : start + ROWS_PER_PAGE;
    for(int i = start; i < end; i++) {
        int idx = activeIndices[i];
        char rp[30], noStr[10]; formatRupiah(dbProduk[idx].harga, rp);
        if (hideId) strcpy(noStr, "***"); else sprintf(noStr, "%03d", i+1);
        gotoxy(tableX, startY + 2 + (i - start));
        // Rata kanan untuk harga & stok: %10d, %17s
        printf(" %-5s %c %03d   %c %-40s %c %10d %c %19s ", noStr, 186, dbProduk[idx].id, 186, dbProduk[idx].nama, 186, dbProduk[idx].stok, 186, rp);
    }
}
void crudProduk(int isAdmin) {
    int selected = 0; const char *menuItems[] = {"Lihat Data", "Input Data", "Update Data", "Hapus Data", "Kembali"};
    int hideId = 0; int tableX = getCenterXForTable(95); int isPagingMode = 0; pageOffset = 0;
    drawBaseLayout(isAdmin ? "MANAJEMEN PRODUK" : "KATALOG GUDANG"); drawHomeLogo(isAdmin ? ROLE_ADMIN : ROLE_WAREHOUSE);
    while(1) {
        int startY = HEADER_HEIGHT + 6; int limit = isAdmin ? 5 : 2;
        for(int i=0; i<limit; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
        if(isPagingMode) refreshRightArea(1, displayProdukTable, tableX, hideId); else drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");
        int key = getch(); if (key == KEY_TAB && isPagingMode) { hideId = !hideId; continue; }

        int activeCount = 0; for(int i=0; i<totalProduk; i++) if(dbProduk[i].isActive) activeCount++;

        if(key == 224) { key = getch(); if(isPagingMode) { int totalPages = (int)ceil((double)activeCount / ROWS_PER_PAGE); if(key == KEY_UP && pageOffset > 0) pageOffset--; if(key == KEY_DOWN && pageOffset < totalPages - 1) pageOffset++; } else { if(key == KEY_UP) selected = (selected > 0) ? selected - 1 : limit - 1; else if(key == KEY_DOWN) selected = (selected < limit - 1) ? selected + 1 : 0; } }
        else if(key == 13 && !isPagingMode) {
            if(isAdmin && selected == 4) break; if(!isAdmin && selected == 1) break;
            if(selected == 0) { isPagingMode = 1; pageOffset = 0; }
            if(isAdmin && selected == 1) {
                clearRightContent(); int fx, fy, bw, bh; drawFormBox("TAMBAH PRODUK", &fx, &fy, &bw, &bh);
                if(totalProduk >= MAX_DATA) { showErrorAndWait(fx, fy, "Penuh!"); continue; }
                int newId = totalProduk + 1; gotoxy(fx+2, fy+2); printf("ID Baru    : %d", newId);
                gotoxy(fx+2, fy+4); printf("Nama Prod : "); getValidatedString(dbProduk[totalProduk].nama, 39, fx+14, fy+4);
                gotoxy(fx+2, fy+6); printf("Stok Awal : "); dbProduk[totalProduk].stok = (int)getValidatedNumber(fx+14, fy+6);
                gotoxy(fx+2, fy+8); printf("Harga (Rp): "); dbProduk[totalProduk].harga = getValidatedNumber(fx+14, fy+8);
                dbProduk[totalProduk].id = newId; dbProduk[totalProduk].isActive = 1;
                totalProduk++; saveAllData(); showErrorAndWait(fx+2, fy+10, "Tersimpan!"); drawBaseLayout("MANAJEMEN PRODUK"); drawHomeLogo(ROLE_ADMIN);
            }
            if(isAdmin && selected == 2) {
                clearRightContent(); int fx, fy, bw, bh; drawFormBox("PILIH PRODUK", &fx, &fy, &bw, &bh);
                gotoxy(fx+2, fy+2); printf("ID Produk : "); int tid = (int)getValidatedNumber(fx+14, fy+2);
                int idx = -1; for(int i=0; i<totalProduk; i++) if(dbProduk[i].isActive && dbProduk[i].id == tid) idx=i;
                if(idx != -1) {
                    clearRightContent(); drawSimpleFormBox("UPDATE PRODUK", &fx, &fy, &bw, &bh);
                    gotoxy(fx+2, fy+2); printf("Produk: %s", dbProduk[idx].nama);
                    char rp[30]; formatRupiah(dbProduk[idx].harga, rp);
                    gotoxy(fx+2, fy+3); printf("Harga: Rp %s", rp);
                    gotoxy(fx+2, fy+5); printf("[1] Ubah Harga | [0] Batal");
                    gotoxy(fx+2, fy+7); printf("Pilihan: "); int choice = (int)getValidatedNumber(fx+11, fy+7);
                    if(choice == 1) { gotoxy(fx+2, fy+9); printf("Harga Baru: Rp "); dbProduk[idx].harga = getValidatedNumber(fx+17, fy+9); saveAllData(); showErrorAndWait(fx+2, fy+11, "Updated!"); }
                    else showErrorAndWait(fx+2, fy+9, "Batal");
                } else showErrorAndWait(fx+2, fy+4, "Tidak Ada!"); drawBaseLayout("MANAJEMEN PRODUK"); drawHomeLogo(ROLE_ADMIN);
            }
            if(isAdmin && selected == 3) {
                 clearRightContent(); int fx, fy, bw, bh; drawFormBox("HAPUS PRODUK", &fx, &fy, &bw, &bh);
                 gotoxy(fx+2, fy+2); printf("ID Hapus : "); int tid = (int)getValidatedNumber(fx+13, fy+2);
                 int idx = -1; for(int i=0; i<totalProduk; i++) if(dbProduk[i].isActive && dbProduk[i].id == tid) idx=i;
                 if(idx != -1 && getConfirmation(fx+2, fy+4, "Hapus?")) {
                     dbProduk[idx].isActive = 0; // Soft Delete
                     saveAllData(); showErrorAndWait(fx+2, fy+6, "Terhapus!");
                 }
                 else showErrorAndWait(fx+2, fy+6, "Batal."); drawBaseLayout("MANAJEMEN PRODUK"); drawHomeLogo(ROLE_ADMIN);
            }
        } else if(key == 27 && isPagingMode) { isPagingMode = 0; clearRightContent(); drawHomeLogo(isAdmin ? ROLE_ADMIN : ROLE_WAREHOUSE); }
    }
}

// 3. SUPPLIER
void displaySupplierTable(int tableX, int hideId) {
    const int TABLE_WIDTH = 105; int startY = HEADER_HEIGHT + 4;
    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, 13); textNormal();
    gotoxy(tableX, startY); printf(" %-5s %c %-5s %c %-30s %c %-40s %c %-18s ", "NO", 186, "ID", 186, "NAMA SUPPLIER", 186, "ALAMAT", 186, "KONTAK");
    gotoxy(tableX, startY + 1); for(int i=0; i<TABLE_WIDTH; i++) printf("%c", 205);

    int activeIndices[MAX_DATA]; int activeCount = 0;
    for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].isActive) activeIndices[activeCount++] = i;

    int start = pageOffset * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > activeCount) ? activeCount : start + ROWS_PER_PAGE;
    for(int i = start; i < end; i++) {
        int idx = activeIndices[i];
        char noStr[10], n[31], a[41];
        if (hideId) strcpy(noStr, "***"); else sprintf(noStr, "%03d", i+1);
        strncpy(n, dbSupplier[idx].nama, 30); n[30]=0; strncpy(a, dbSupplier[idx].alamat, 40); a[40]=0;
        gotoxy(tableX, startY + 2 + (i - start)); printf(" %-5s %c %03d   %c %-30s %c %-40s %c %-18s ", noStr, 186, dbSupplier[idx].id, 186, n, 186, a, 186, dbSupplier[idx].kontak);
    }
}
void crudSupplier() {
    int selected = 0; const char *menuItems[] = {"Lihat Data", "Input Data", "Update Data", "Hapus Data", "Kembali"};
    int hideId = 0; int tableX = getCenterXForTable(105); int isPagingMode = 0; pageOffset = 0;
    drawBaseLayout("MANAJEMEN SUPPLIER"); drawHomeLogo(ROLE_ADMIN);
    while(1) {
        int startY = HEADER_HEIGHT + 6; for(int i=0; i<5; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
        if(isPagingMode) refreshRightArea(1, displaySupplierTable, tableX, hideId); else drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");
        int key = getch(); if (key == KEY_TAB && isPagingMode) { hideId = !hideId; continue; }

        int activeCount = 0; for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].isActive) activeCount++;

        if(key == 224) { key = getch(); if(isPagingMode) { int totalPages = (int)ceil((double)activeCount / ROWS_PER_PAGE); if(key == KEY_UP && pageOffset > 0) pageOffset--; if(key == KEY_DOWN && pageOffset < totalPages - 1) pageOffset++; } else { if(key == KEY_UP) selected = (selected > 0) ? selected - 1 : 4; else if(key == KEY_DOWN) selected = (selected < 4) ? selected + 1 : 0; } }
        else if(key == 13 && !isPagingMode) {
            if(selected == 4) break;
            if(selected == 0) { isPagingMode = 1; pageOffset = 0; }
            if(selected == 1) {
                clearRightContent(); int fx, fy, bw, bh; drawFormBox("TAMBAH SUPPLIER", &fx, &fy, &bw, &bh);
                int newId = (totalSupplier > 0) ? dbSupplier[totalSupplier-1].id + 1 : 101;
                gotoxy(fx+2, fy+2); printf("ID Baru: %d", newId);
                gotoxy(fx+2, fy+4); printf("Nama   : "); getValidatedString(dbSupplier[totalSupplier].nama, 49, fx+11, fy+4);
                gotoxy(fx+2, fy+6); printf("Alamat : "); getValidatedString(dbSupplier[totalSupplier].alamat, 99, fx+11, fy+6);
                gotoxy(fx+2, fy+8); printf("Kontak : "); getValidatedString(dbSupplier[totalSupplier].kontak, 19, fx+11, fy+8);
                dbSupplier[totalSupplier].id = newId; dbSupplier[totalSupplier].isActive = 1;
                totalSupplier++; saveAllData(); showErrorAndWait(fx+2, fy+10, "Tersimpan!"); drawBaseLayout("MANAJEMEN SUPPLIER"); drawHomeLogo(ROLE_ADMIN);
            }
            if(selected == 2) {
                clearRightContent(); int fx, fy, bw, bh; drawFormBox("PILIH SUPPLIER", &fx, &fy, &bw, &bh);
                gotoxy(fx+2, fy+2); printf("ID Supplier: "); int tid = (int)getValidatedNumber(fx+15, fy+2);
                int idx = -1; for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].isActive && dbSupplier[i].id == tid) idx=i;
                if(idx != -1) {
                    clearRightContent(); drawSimpleFormBox("UPDATE SUPPLIER", &fx, &fy, &bw, &bh);
                    gotoxy(fx+2, fy+2); printf("Supplier: %s", dbSupplier[idx].nama);
                    gotoxy(fx+2, fy+4); printf("[1] Nama | [2] Alamat | [3] Kontak | [0] Batal");
                    gotoxy(fx+2, fy+6); printf("Pilihan: "); int choice = (int)getValidatedNumber(fx+11, fy+6);
                    if(choice==1) { gotoxy(fx+2, fy+8); printf("Nama Baru: "); getValidatedString(dbSupplier[idx].nama, 49, fx+13, fy+8); saveAllData(); showErrorAndWait(fx+2, fy+10, "Updated!"); }
                    else if(choice==2) { gotoxy(fx+2, fy+8); printf("Alamat Baru: "); getValidatedString(dbSupplier[idx].alamat, 99, fx+15, fy+8); saveAllData(); showErrorAndWait(fx+2, fy+10, "Updated!"); }
                    else if(choice==3) { gotoxy(fx+2, fy+8); printf("Kontak Baru: "); getValidatedString(dbSupplier[idx].kontak, 19, fx+15, fy+8); saveAllData(); showErrorAndWait(fx+2, fy+10, "Updated!"); }
                    else showErrorAndWait(fx+2, fy+8, "Batal");
                } else showErrorAndWait(fx+2, fy+4, "Tidak Ada!"); drawBaseLayout("MANAJEMEN SUPPLIER"); drawHomeLogo(ROLE_ADMIN);
            }
            if(selected == 3) {
                clearRightContent(); int fx, fy, bw, bh; drawFormBox("HAPUS SUPPLIER", &fx, &fy, &bw, &bh);
                gotoxy(fx+2, fy+2); printf("ID Hapus : "); int tid = (int)getValidatedNumber(fx+13, fy+2);
                int idx = -1; for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].isActive && dbSupplier[i].id == tid) idx=i;
                if(idx != -1 && getConfirmation(fx+2, fy+4, "Hapus?")) { dbSupplier[idx].isActive = 0; saveAllData(); showErrorAndWait(fx+2, fy+6, "Terhapus!"); }
                else showErrorAndWait(fx+2, fy+6, "Batal."); drawBaseLayout("MANAJEMEN SUPPLIER"); drawHomeLogo(ROLE_ADMIN);
            }
        } else if(key == 27 && isPagingMode) { isPagingMode = 0; clearRightContent(); drawHomeLogo(ROLE_ADMIN); }
    }
}

// 4. GUDANG
void displayGudangTable(int tableX, int hideId) {
    const int TABLE_WIDTH = 95; int startY = HEADER_HEIGHT + 4;
    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, 13); textNormal();
    gotoxy(tableX, startY); printf(" %-5s %c %-5s %c %-30s %c %-50s ", "NO", 186, "ID", 186, "NAMA GUDANG", 186, "ALAMAT LENGKAP");
    gotoxy(tableX, startY + 1); for(int i=0; i<TABLE_WIDTH; i++) printf("%c", 205);

    int activeIndices[MAX_DATA]; int activeCount = 0;
    for(int i=0; i<totalGudang; i++) if(dbGudang[i].isActive) activeIndices[activeCount++] = i;

    int start = pageOffset * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > activeCount) ? activeCount : start + ROWS_PER_PAGE;
    for(int i = start; i < end; i++) {
        int idx = activeIndices[i];
        char noStr[10]; if (hideId) strcpy(noStr, "***"); else sprintf(noStr, "%03d", i+1);
        gotoxy(tableX, startY + 2 + (i - start)); printf(" %-5s %c %03d   %c %-30s %c %-50s ", noStr, 186, dbGudang[idx].id, 186, dbGudang[idx].nama, 186, dbGudang[idx].alamat);
    }
}
void crudGudang() {
    int selected = 0; const char *menuItems[] = {"Lihat Data", "Input Data", "Update Data", "Hapus Data", "Kembali"};
    int hideId = 0; int tableX = getCenterXForTable(95); int isPagingMode = 0; pageOffset = 0;
    drawBaseLayout("MANAJEMEN GUDANG"); drawHomeLogo(ROLE_ADMIN);
    while(1) {
        int startY = HEADER_HEIGHT + 6; for(int i=0; i<5; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
        if(isPagingMode) refreshRightArea(1, displayGudangTable, tableX, hideId); else drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");
        int key = getch(); if (key == KEY_TAB && isPagingMode) { hideId = !hideId; continue; }

        int activeCount = 0; for(int i=0; i<totalGudang; i++) if(dbGudang[i].isActive) activeCount++;

        if(key == 224) { key = getch(); if(isPagingMode) { int totalPages = (int)ceil((double)activeCount / ROWS_PER_PAGE); if(key == KEY_UP && pageOffset > 0) pageOffset--; if(key == KEY_DOWN && pageOffset < totalPages - 1) pageOffset++; } else { if(key == KEY_UP) selected = (selected > 0) ? selected - 1 : 4; else if(key == KEY_DOWN) selected = (selected < 4) ? selected + 1 : 0; } }
        else if(key == 13 && !isPagingMode) {
            if(selected == 4) break;
            if(selected == 0) { isPagingMode = 1; pageOffset = 0; }
            if(selected == 1) {
                clearRightContent(); int fx, fy, bw, bh; drawFormBox("TAMBAH GUDANG", &fx, &fy, &bw, &bh);
                int newId = (totalGudang > 0) ? dbGudang[totalGudang-1].id + 1 : 9001;
                gotoxy(fx+2, fy+2); printf("ID Baru : %d", newId);
                gotoxy(fx+2, fy+4); printf("Nama    : "); getValidatedString(dbGudang[totalGudang].nama, 29, fx+12, fy+4);
                gotoxy(fx+2, fy+6); printf("Alamat  : "); getValidatedString(dbGudang[totalGudang].alamat, 99, fx+12, fy+6);
                dbGudang[totalGudang].id = newId; dbGudang[totalGudang].isActive = 1;
                totalGudang++; saveAllData(); showErrorAndWait(fx+2, fy+8, "Tersimpan!"); drawBaseLayout("MANAJEMEN GUDANG"); drawHomeLogo(ROLE_ADMIN);
            }
            if(selected == 2) {
                clearRightContent(); int fx, fy, bw, bh; drawFormBox("PILIH GUDANG", &fx, &fy, &bw, &bh);
                gotoxy(fx+2, fy+2); printf("ID Gudang: "); int tid = (int)getValidatedNumber(fx+13, fy+2);
                int idx = -1; for(int i=0; i<totalGudang; i++) if(dbGudang[i].isActive && dbGudang[i].id == tid) idx=i;
                if(idx != -1) {
                    clearRightContent(); drawSimpleFormBox("UPDATE GUDANG", &fx, &fy, &bw, &bh);
                    gotoxy(fx+2, fy+2); printf("Gudang: %s", dbGudang[idx].nama);
                    gotoxy(fx+2, fy+4); printf("[1] Nama | [2] Alamat | [0] Batal");
                    gotoxy(fx+2, fy+6); printf("Pilihan: "); int choice = (int)getValidatedNumber(fx+11, fy+6);
                    if(choice==1) { gotoxy(fx+2, fy+8); printf("Nama Baru: "); getValidatedString(dbGudang[idx].nama, 29, fx+13, fy+8); saveAllData(); showErrorAndWait(fx+2, fy+10, "Updated!"); }
                    else if(choice==2) { gotoxy(fx+2, fy+8); printf("Alamat Baru: "); getValidatedString(dbGudang[idx].alamat, 99, fx+15, fy+8); saveAllData(); showErrorAndWait(fx+2, fy+10, "Updated!"); }
                    else showErrorAndWait(fx+2, fy+8, "Batal");
                } else showErrorAndWait(fx+2, fy+4, "Tidak Ada!"); drawBaseLayout("MANAJEMEN GUDANG"); drawHomeLogo(ROLE_ADMIN);
            }
            if(selected == 3) {
                 clearRightContent(); int fx, fy, bw, bh; drawFormBox("HAPUS GUDANG", &fx, &fy, &bw, &bh);
                 gotoxy(fx+2, fy+2); printf("ID Hapus : "); int tid = (int)getValidatedNumber(fx+13, fy+2);
                 int idx = -1; for(int i=0; i<totalGudang; i++) if(dbGudang[i].isActive && dbGudang[i].id == tid) idx=i;
                 if(idx != -1 && getConfirmation(fx+2, fy+4, "Hapus?")) { dbGudang[idx].isActive = 0; saveAllData(); showErrorAndWait(fx+2, fy+6, "Terhapus!"); }
                 else showErrorAndWait(fx+2, fy+6, "Batal."); drawBaseLayout("MANAJEMEN GUDANG"); drawHomeLogo(ROLE_ADMIN);
            }
        } else if(key == 27 && isPagingMode) { isPagingMode = 0; clearRightContent(); drawHomeLogo(ROLE_ADMIN); }
    }
}