#include "controllers.h"
#include "globals.h"
#include "ui.h"

// Forward Declaration
void viewProfile(int idx);

void viewMyTeam(int roleId) {
    int targetRole = -1;
    char teamName[30];

    if(roleId == ROLE_HEAD_CASHIER) { targetRole = ROLE_CASHIER; strcpy(teamName, "TIM KASIR"); }
    else if(roleId == ROLE_HEAD_WAREHOUSE) { targetRole = ROLE_STAFF_WAREHOUSE; strcpy(teamName, "TIM GUDANG"); }
    else { showErrorAndWait(SIDEBAR_WIDTH+5, HEADER_HEIGHT+5, "Akses Ditolak."); return; }

    clearRightContent();
    char breadcrumb[50]; sprintf(breadcrumb, "DIVISI > %s", teamName);
    drawBreadcrumbs(breadcrumb);
    int tx = getCenterXForTable(85);
    drawTableBox(tx - 1, HEADER_HEIGHT + 3, 85+2, 13);
    gotoxy(tx, HEADER_HEIGHT + 4); printf(" %-5s %c %-25s %c %-15s %c %-25s ", "ID", 186, "NAMA ANGGOTA", 186, "KONTAK", 186, "PERFORMA");
    gotoxy(tx, HEADER_HEIGHT + 5); for(int i=0; i<85; i++) printf("%c", 205);
    int row = 0;
    for(int i = 0; i < totalKaryawan; i++) {
        if(dbKaryawan[i].roleId == targetRole) {
            gotoxy(tx, HEADER_HEIGHT + 6 + row);
            printf(" %04d  %c %-25s %c %-15s %c ", dbKaryawan[i].id, 186, dbKaryawan[i].nama, 186, dbKaryawan[i].kontak);
            drawPerformanceVisual(tx + 54, HEADER_HEIGHT + 6 + row, dbKaryawan[i].performa);
            row++; if(row >= 10) break;
        }
    }
    if(row == 0) { gotoxy(tx+2, HEADER_HEIGHT+6); printf("Belum ada anggota tim."); }
    drawNavigationLegend("[ESC] Kembali"); while(getch() != 27);
}

void employeeMainMenu(int idx) {
    int role = dbKaryawan[idx].roleId;
    char *menuLabels[10]; int menuCodes[10]; int count = 0;
    menuLabels[count]="Dashboard"; menuCodes[count++]=1;
    menuLabels[count]="Profil Saya"; menuCodes[count++]=2;
    if(role == ROLE_CASHIER || role == ROLE_HEAD_CASHIER) {
        menuLabels[count]="Transaksi Baru"; menuCodes[count++]=3;
        if (role == ROLE_HEAD_CASHIER) { menuLabels[count]="Lihat Tim Kasir"; menuCodes[count++]=7; }
    }
    if(role == ROLE_STAFF_WAREHOUSE || role == ROLE_HEAD_WAREHOUSE) {
        menuLabels[count]="Restock Barang"; menuCodes[count++]=5;
        menuLabels[count]="Katalog Produk"; menuCodes[count++]=6;
        if (role == ROLE_HEAD_WAREHOUSE) { menuLabels[count]="Lihat Tim Gudang"; menuCodes[count++]=7; }
    }
    menuLabels[count]="Logout"; menuCodes[count++]=0;

    int selected = 0; int redraw = 1;
    while(1) {
        if(redraw) { drawBaseLayout("DASHBOARD STAFF"); showDashboardHome(role); redraw = 0; }
        int startY = HEADER_HEIGHT + 6;
        for(int i=0; i<count; i++) printMenuItem(4, startY+(i*2), menuLabels[i], (i==selected));
        int key = getch();
        if(key == 224) { key = getch(); if(key==KEY_UP) selected=(selected>0)?selected-1:count-1; if(key==KEY_DOWN) selected=(selected<count-1)?selected+1:0; }
        else if(key == 13) {
            int code = menuCodes[selected];
            if(code == 0) break; if(code == 1) redraw = 1; if(code == 2) viewProfile(idx);
            if(code == 3) crudPenjualan(idx); if(code == 5) crudPembelian(idx);
            if(code == 6) crudProduk(0); if(code == 7) viewMyTeam(role);
            redraw = 1;
        } else if (key == 27) {
            clearRightContent();
            drawHomeLogo(role); // FIX: Redraw ASCII saat ESC
        }
    }
}

void viewProfile(int idx) {
    clearRightContent(); drawBreadcrumbs("STAFF > PROFIL");
    int fx, fy, bw, bh; drawFormBox("DETAIL PROFIL", &fx, &fy, &bw, &bh);
    gotoxy(fx+5, fy+3); printf("Nama      : %s", dbKaryawan[idx].nama);
    gotoxy(fx+5, fy+5); printf("Jabatan   : %s", dbKaryawan[idx].jabatan);
    gotoxy(fx+5, fy+7); printf("Kontak    : %s", dbKaryawan[idx].kontak);
    gotoxy(fx+5, fy+9); printf("Performa  :");
    drawPerformanceVisual(fx+16, fy+9, dbKaryawan[idx].performa);
    drawNavigationLegend("[ESC] Kembali"); while(getch() != 27);
}