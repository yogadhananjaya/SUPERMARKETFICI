#include "controllers.h"
#include "config.h"
#include "structures.h"
#include "globals.h"
#include "ui.h"
#include <math.h> // Diperlukan untuk ceil()
#include <stdio.h>
#include <string.h>


static void updateSubMenu(int selected) {
    printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Data", selected == 1);
    printMenuItem(2, HEADER_HEIGHT + 7, "2. Tambah Data", selected == 2);
    printMenuItem(2, HEADER_HEIGHT + 9, "3. Edit Data", selected == 3);
    printMenuItem(2, HEADER_HEIGHT + 11,"4. Hapus Data", selected == 4);
    printMenuItem(2, HEADER_HEIGHT + 13,"0. Kembali", selected == 5);
}

void displayKaryawanTable(int tableX) {
    const int TABLE_WIDTH = 110;
    const int TABLE_HEIGHT = 12;
    int startY = HEADER_HEIGHT + 4;

    int totalPages = (int)ceil((double)totalKaryawan / ROWS_PER_PAGE);
    if (totalPages == 0) totalPages = 1;
    int currentPage = pageOffset + 1;
    int startIndex = pageOffset * ROWS_PER_PAGE;

    printCenterRight(HEADER_HEIGHT + 2, "=== DATA KARYAWAN ===");
    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, TABLE_HEIGHT + 1);

    for(int i=0; i<TABLE_HEIGHT; i++) {
        gotoxy(tableX, startY+i);
        for(int x=0; x<TABLE_WIDTH; x++) printf(" ");
    }

    textNormal();
    gotoxy(tableX, startY);
    printf(" %-5s %c %-20s %c %-15s %c %-15s %c %-12s %c %-12s ",
        "ID", 186, "NAMA", 186, "JABATAN", 186, "KONTAK", 186, "USERNAME", 186, "PASSWORD");

    gotoxy(tableX, startY + 1);
    for(int i=0; i<TABLE_WIDTH; i++) printf("%c", 205);

    gotoxy(tableX + 7, startY + 1); printf("%c", 206);
    gotoxy(tableX + 30, startY + 1); printf("%c", 206);
    gotoxy(tableX + 48, startY + 1); printf("%c", 206);
    gotoxy(tableX + 66, startY + 1); printf("%c", 206);
    gotoxy(tableX + 81, startY + 1); printf("%c", 206);

    if (totalKaryawan == 0) {
        gotoxy(tableX + 30, startY + 3); printf(">> DATA KARYAWAN KOSONG <<");
    } else {
        int endIndex = startIndex + ROWS_PER_PAGE;
        if (endIndex > totalKaryawan) endIndex = totalKaryawan;

        for(int i = startIndex; i < endIndex; i++) {
            int displayRow = startY + 2 + (i - startIndex);
            char n[21], j[16], k[16], u[13], p[13];

            strncpy(n, dbKaryawan[i].nama, 20); n[20]=0;
            strncpy(j, dbKaryawan[i].jabatan, 15); j[15]=0;
            strncpy(k, dbKaryawan[i].kontak, 15); k[15]=0;
            strncpy(u, dbKaryawan[i].username, 12); u[12]=0;
            strncpy(p, dbKaryawan[i].password, 12); p[12]=0;

            gotoxy(tableX, displayRow);

            printf(" %03d   %c %-20s %c %-15s %c %-15s %c %-12s %c %-12s ",
                   dbKaryawan[i].id, 186, n, 186, j, 186, k, 186, u, 186, p);
        }
    }
    gotoxy(tableX + 5, startY + TABLE_HEIGHT + 2);
    printf("NAVIGASI: [ ^ ATAS | BAWAH v ] | Hal %d / %d", currentPage, totalPages);
    printf("        TEKAN [ESC] UNTUK KELUAR            ");
}

void crudKaryawan() {
    int selected = 1, totalMenu = 5, key;
    drawBaseLayout("MANAJEMEN KARYAWAN");
    const int TABLE_WIDTH = 110;
    int tableX = getCenterXForTable(TABLE_WIDTH);
    int formBoxX, formBoxY, boxW, boxH, formX, formInputX = SIDEBAR_WIDTH + 25, formY;
    int isPagingMode = 0;
    int totalPages;

    pageOffset = 0;
    clearRightContent();
    updateSubMenu(selected);

    drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Pilih");

    while(1) {
        hideCursor();
        key = getch();
        if(key == 224) {
            key = getch();
            totalPages = (int)ceil((double)totalKaryawan / ROWS_PER_PAGE);
            if(totalPages == 0) totalPages = 1;

            if(isPagingMode) {
                if(key == KEY_UP && pageOffset > 0) { pageOffset--; clearRightContent(); displayKaryawanTable(tableX); }
                else if(key == KEY_DOWN && pageOffset < totalPages - 1) { pageOffset++; clearRightContent(); displayKaryawanTable(tableX); }
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
                clearRightContent(); displayKaryawanTable(tableX); isPagingMode = 1;
                drawNavigationLegend("[Panah Atas/Bawah] Scroll Halaman | [ESC] Kembali ke Menu");
            } else if(selected == 2) {
                if(totalKaryawan >= MAX_DATA) { printCenterRight(HEADER_HEIGHT+20, "DATABASE PENUH!"); Sleep(1000); continue; }
                drawFormBox("TAMBAH KARYAWAN", &formBoxX, &formBoxY, &boxW, &boxH);
                dbKaryawan[totalKaryawan].id = (totalKaryawan > 0) ? dbKaryawan[totalKaryawan-1].id + 1 : 5000;
                gotoxy(formX, formY+1); printf("ID Otomatis : %03d", dbKaryawan[totalKaryawan].id);
                gotoxy(formX, formY+3); printf("Nama        : ");
                gotoxy(formInputX, formY+3); getValidatedString(dbKaryawan[totalKaryawan].nama, 49, formInputX, formY+3);
                gotoxy(formX, formY+5); printf("Jabatan     : ");
                gotoxy(formInputX, formY+5); getValidatedString(dbKaryawan[totalKaryawan].jabatan, 29, formInputX, formY+5);
                gotoxy(formX, formY+7); printf("Kontak      : ");
                gotoxy(formInputX, formY+7); getValidatedPhoneNumber(dbKaryawan[totalKaryawan].kontak, 19, formInputX, formY+7);
                gotoxy(formX, formY+9); printf("Username    : ");
                gotoxy(formInputX, formY+9); getValidatedString(dbKaryawan[totalKaryawan].username, 29, formInputX, formY+9);
                gotoxy(formX, formY+11); printf("Password    : ");
                gotoxy(formInputX, formY+11); getString(dbKaryawan[totalKaryawan].password, 29); // Password boleh karakter apa saja

                dbKaryawan[totalKaryawan].performa = 100;

                if(strlen(dbKaryawan[totalKaryawan].nama) > 0) {
                    totalKaryawan++;
                    gotoxy(formX, formY+13); printf(">> Sukses Disimpan!");
                }
                Sleep(1000); clearRightContent(); updateSubMenu(selected);
                drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Eksekusi");
            } else if(selected == 3 || selected == 4) {
                 drawFormBox(selected == 3 ? "EDIT KARYAWAN" : "HAPUS KARYAWAN", &formBoxX, &formBoxY, &boxW, &boxH);
                 gotoxy(formX, formY+2); printf("ID Target   : ");
                 int idTarget = (int)getLongInput(formInputX, formY+2);
                 int idx = -1;
                 for(int i=0; i<totalKaryawan; i++) if(dbKaryawan[i].id == idTarget) idx = i;

                 if(idx != -1) {
                     if(selected == 3) {
                         gotoxy(formX, formY+4); printf("Nama Baru   : ");
                         gotoxy(formInputX, formY+4); getValidatedString(dbKaryawan[idx].nama, 49, formInputX, formY+4);

                         gotoxy(formX, formY+6); printf("Jabatan     : ");
                         gotoxy(formInputX, formY+6); getValidatedString(dbKaryawan[idx].jabatan, 29, formInputX, formY+6);

                         gotoxy(formX, formY+8); printf("Kontak      : ");
                         gotoxy(formInputX, formY+8); getValidatedPhoneNumber(dbKaryawan[idx].kontak, 19, formInputX, formY+8);

                         gotoxy(formX, formY+10); printf("Pass Baru   : ");
                         gotoxy(formInputX, formY+10); getString(dbKaryawan[idx].password, 29);

                         gotoxy(formX, formY+13); printf(">> Update Berhasil!");
                     } else {
                         for(int j=idx; j<totalKaryawan-1; j++) dbKaryawan[j] = dbKaryawan[j+1];
                         totalKaryawan--;
                         gotoxy(formX, formY+6); printf(">> Data Terhapus!");
                     }
                 } else { gotoxy(formX, formY+4); printf(">> ID Tidak Ditemukan!"); }
                 Sleep(1500); clearRightContent(); updateSubMenu(selected);
                 drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Eksekusi");
            }
        } else if (key == KEY_ESC) {
            if (isPagingMode) {
                clearRightContent();
                isPagingMode = 0;
                drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Eksekusi");
            }
            else if (selected != 5) { selected = 5; updateSubMenu(selected); }
        }
    }
}