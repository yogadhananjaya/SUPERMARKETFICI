#include "controllers.h"
#include "config.h"
#include "structures.h"
#include "globals.h"
#include "ui.h"

static void updateSubMenu(int selected) {
    printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Data", selected == 1);
    printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Data", selected == 2);
    printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Data", selected == 3);
    printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Data", selected == 4);
    printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);
}

void displayGudangTable(int tableX) {
    const int TABLE_WIDTH = 95;
    const int TABLE_HEIGHT = 12;
    int startY = HEADER_HEIGHT + 4;

    int totalPages = (int)ceil((double)totalGudang / ROWS_PER_PAGE);
    if (totalPages == 0) totalPages = 1;
    int currentPage = pageOffset + 1;
    int startIndex = pageOffset * ROWS_PER_PAGE;

    printCenterRight(HEADER_HEIGHT + 2, "=== DATA GUDANG ===");
    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, TABLE_HEIGHT + 1);

    for(int i=0; i<TABLE_HEIGHT; i++) {
        gotoxy(tableX, startY+i);
        for(int x=0; x<TABLE_WIDTH; x++) printf(" ");
    }

    textNormal();
    gotoxy(tableX, startY);
    printf(" %-5s %c %-30s %c %-50s ", "ID", 186, "NAMA GUDANG", 186, "ALAMAT LENGKAP");

    gotoxy(tableX, startY + 1);
    for(int i=0; i<TABLE_WIDTH; i++) printf("%c", 205);
    gotoxy(tableX + 7, startY + 1); printf("%c", 206);
    gotoxy(tableX + 40, startY + 1); printf("%c", 206);

    if (totalGudang == 0) {
        gotoxy(tableX + 30, startY + 3); printf(">> DATA GUDANG KOSONG <<");
    } else {
        int endIndex = startIndex + ROWS_PER_PAGE;
        if (endIndex > totalGudang) endIndex = totalGudang;

        for(int i = startIndex; i < endIndex; i++) {
            int displayRow = startY + 2 + (i - startIndex);
            char n[31], a[51];
            strncpy(n, dbGudang[i].nama, 30); n[30]=0;
            strncpy(a, dbGudang[i].alamat, 50); a[50]=0;

            gotoxy(tableX, displayRow);
            printf(" %03d   %c %-30s %c %-50s ",
                dbGudang[i].id, 186, n, 186, a);
        }
    }
    gotoxy(tableX + 5, startY + TABLE_HEIGHT + 2);
    printf("NAVIGASI: [ ^ ATAS | BAWAH v ] | Hal %d / %d", currentPage, totalPages);
    printf("        TEKAN [ESC] UNTUK KELUAR            ");
}

void crudGudang() {
    int selected = 1, totalMenu = 5, key;
    drawBaseLayout("MANAJEMEN GUDANG");
    const int TABLE_WIDTH = 95;
    int tableX = getCenterXForTable(TABLE_WIDTH);
    int formBoxX, formBoxY, boxW, boxH, formX, formInputX = SIDEBAR_WIDTH + 25, formY;
    int isPagingMode = 0;
    int totalPages;

    pageOffset = 0;
    clearRightContent();
    updateSubMenu(selected);

    while(1) {
        hideCursor();
        key = getch();
        if(key == 224) {
            key = getch();
            totalPages = (int)ceil((double)totalGudang / ROWS_PER_PAGE);
            if(totalPages == 0) totalPages = 1;

            if(isPagingMode) {
                if(key == KEY_UP && pageOffset > 0) { pageOffset--; clearRightContent(); displayGudangTable(tableX); }
                else if(key == KEY_DOWN && pageOffset < totalPages - 1) { pageOffset++; clearRightContent(); displayGudangTable(tableX); }
            } else {
                if(key == KEY_UP) { selected = (selected == 1) ? totalMenu : selected - 1; updateSubMenu(selected); }
                else if(key == KEY_DOWN) { selected = (selected == totalMenu) ? 1 : selected + 1; updateSubMenu(selected); }
            }
        }
        else if(key == KEY_ENTER) {
            formX = SIDEBAR_WIDTH + 7; formY = HEADER_HEIGHT + 5;
            if(selected == 5) break;
            pageOffset = 0; isPagingMode = 0;

            if(selected == 1) {
                clearRightContent(); displayGudangTable(tableX); isPagingMode = 1;
            } else if(selected == 2) {
                if(totalGudang >= MAX_DATA) { printCenterRight(HEADER_HEIGHT+20, "DATABASE PENUH!"); Sleep(1000); continue; }
                drawFormBox("TAMBAH GUDANG", &formBoxX, &formBoxY, &boxW, &boxH);

                dbGudang[totalGudang].id = (totalGudang > 0) ? dbGudang[totalGudang-1].id + 1 : 9000;
                gotoxy(formX, formY+2); printf("ID Otomatis  : %d", dbGudang[totalGudang].id);

                gotoxy(formX, formY+4); printf("Nama Gudang  : "); gotoxy(formInputX, formY+4); getString(dbGudang[totalGudang].nama, 49);
                gotoxy(formX, formY+6); printf("Alamat       : "); gotoxy(formInputX, formY+6); getString(dbGudang[totalGudang].alamat, 99);

                if(strlen(dbGudang[totalGudang].nama) > 0) {
                    totalGudang++;
                    gotoxy(formX, formY+10); printf(">> Sukses Disimpan!");
                }
                Sleep(1000); clearRightContent(); updateSubMenu(selected);
            } else if(selected == 3 || selected == 4) {
                 drawFormBox(selected == 3 ? "EDIT GUDANG" : "HAPUS GUDANG", &formBoxX, &formBoxY, &boxW, &boxH);
                 gotoxy(formX, formY+2); printf("ID Target   : ");
                 int idTarget = (int)getLongInput(formInputX, formY+2);
                 int idx = -1;
                 for(int i=0; i<totalGudang; i++) if(dbGudang[i].id == idTarget) idx = i;

                 if(idx != -1) {
                     if(selected == 3) {
                         gotoxy(formX, formY+4); printf("Nama Baru   : "); gotoxy(formInputX, formY+4); getString(dbGudang[idx].nama, 49);
                         gotoxy(formX, formY+6); printf("Alamat Baru : "); gotoxy(formInputX, formY+6); getString(dbGudang[idx].alamat, 99);
                         gotoxy(formX, formY+10); printf(">> Update Berhasil!");
                     } else {
                         for(int j=idx; j<totalGudang-1; j++) dbGudang[j] = dbGudang[j+1];
                         totalGudang--;
                         gotoxy(formX, formY+6); printf(">> Data Terhapus!");
                     }
                 } else { gotoxy(formX, formY+4); printf(">> ID Tidak Ditemukan!"); }
                 Sleep(1500); clearRightContent(); updateSubMenu(selected);
            }
        } else if (key == KEY_ESC) {
            if (isPagingMode) { clearRightContent(); isPagingMode = 0; }
            else if (selected != 5) { selected = 5; updateSubMenu(selected); }
        }
    }
}