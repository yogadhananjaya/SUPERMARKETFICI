#include "controllers.h"
#include "globals.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>

void viewProfile(int idx) {
    clearRightContent(); drawBreadcrumbs("STAFF > PROFIL");
    int fx, fy, bw, bh; drawFormBox("DETAIL PROFIL", &fx, &fy, &bw, &bh);
    gotoxy(fx+5, fy+3); printf("Nama      : %s", dbKaryawan[idx].nama);
    gotoxy(fx+5, fy+5); printf("Jabatan   : %s", dbKaryawan[idx].jabatan);
    gotoxy(fx+5, fy+7); printf("Kontak    : %s", dbKaryawan[idx].kontak);
    gotoxy(fx+5, fy+9); printf("Total Poin: %d", dbKaryawan[idx].performa);
    drawPerformanceVisual(fx+16, fy+9, (dbKaryawan[idx].performa > 100 ? 100 : dbKaryawan[idx].performa));
    drawNavigationLegend("[ESC] Kembali"); while(getch() != 27);
}

void viewJobHistory(int idx) {
    int historyPage = 0; int tableX = getCenterXForTable(85); int startY = HEADER_HEIGHT + 3;
    int myHistoryIndices[MAX_DATA]; int myCount = 0;
    for(int i=0; i<totalRiwayat; i++) { if(dbRiwayat[i].idKaryawan == dbKaryawan[idx].id) { myHistoryIndices[myCount++] = i; } }

    drawBaseLayout("RIWAYAT PEKERJAAN");
    gotoxy(tableX, startY-2); printf("Total Poin Kinerja: %d", dbKaryawan[idx].performa);

    while(1) {
        drawTableBox(tableX - 1, startY - 1, 85 + 2, 13);
        textNormal(); gotoxy(tableX, startY); printf(" %-15s %c %-40s %c %-10s ", "TANGGAL", 186, "PEKERJAAN / AKTIVITAS", 186, "POIN");
        gotoxy(tableX, startY + 1); for(int i=0; i<85; i++) printf("%c", 205);
        int totalPages = (int)ceil((double)myCount / ROWS_PER_PAGE); if(totalPages == 0) totalPages = 1;
        int start = historyPage * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > myCount) ? myCount : start + ROWS_PER_PAGE;

        for(int i = start; i < end; i++) {
            int rIdx = myHistoryIndices[i]; int rowY = startY + 2 + (i - start); gotoxy(tableX, rowY);
            char poinStr[10]; if(dbRiwayat[rIdx].poin > 0) sprintf(poinStr, "+%d", dbRiwayat[rIdx].poin); else sprintf(poinStr, "%d", dbRiwayat[rIdx].poin);
            printf(" %-15s %c %-40s %c %-10s ", dbRiwayat[rIdx].tanggal, 186, dbRiwayat[rIdx].aktivitas, 186, poinStr);
        }
        drawNavigationLegend("[PANAH] Scroll | [ESC] Kembali");
        int key = getch(); if(key == 224) { key = getch(); if(key == KEY_UP && historyPage > 0) historyPage--; if(key == KEY_DOWN && historyPage < totalPages - 1) historyPage++; }
        else if(key == 27) break;
    }
}

void employeeMainMenu(int idx) {
    int role = dbKaryawan[idx].roleId;
    char *menuLabels[10]; int menuCodes[10]; int count = 0;
    menuLabels[count]="Dashboard"; menuCodes[count++]=1;
    menuLabels[count]="Profil Saya"; menuCodes[count++]=2;
    menuLabels[count]="Riwayat & Poin"; menuCodes[count++]=8;

    if(role == ROLE_CASHIER) { menuLabels[count]="Transaksi Kasir"; menuCodes[count++]=3; }
    if(role == ROLE_WAREHOUSE) { menuLabels[count]="Restock Barang"; menuCodes[count++]=5; menuLabels[count]="Transaksi Beli"; menuCodes[count++]=6; }
    menuLabels[count]="Logout"; menuCodes[count++]=0;

    int selected = 0; int redraw = 1;
    while(1) {
        if(redraw) { drawBaseLayout(role == ROLE_CASHIER ? "DASHBOARD KASIR" : "DASHBOARD GUDANG"); showDashboardHome(role); redraw = 0; }
        int startY = HEADER_HEIGHT + 6; for(int i=0; i<count; i++) printMenuItem(4, startY+(i*2), menuLabels[i], (i==selected));
        int key = getch(); if(key == 224) { key = getch(); if(key==KEY_UP) selected=(selected>0)?selected-1:count-1; if(key==KEY_DOWN) selected=(selected<count-1)?selected+1:0; }
        else if(key == 13) {
            int code = menuCodes[selected];
            if(code == 0) break; if(code == 1) redraw = 1;
            if(code == 2) viewProfile(idx);
            if(code == 8) { viewJobHistory(idx); redraw = 1; }
            if(code == 3) crudPenjualan(idx);
            if(code == 5) menuRestock(idx);
            if(code == 6) menuTransaksiGudang(idx);
            redraw = 1;
        } else if (key == 27) { clearRightContent(); drawHomeLogo(role); }
    }
}