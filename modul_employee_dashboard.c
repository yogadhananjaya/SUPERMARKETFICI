#include "controllers.h"
#include "globals.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <conio.h>

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

void employeeMainMenu(int idx) {
    int role = dbKaryawan[idx].roleId;
    char *menuLabels[10]; int menuCodes[10]; int count = 0;

    // 1. Dashboard
    menuLabels[count]="Dashboard"; menuCodes[count++]=1;
    // 2. Profil
    menuLabels[count]="Profil Saya"; menuCodes[count++]=2;

    // MENU KASIR
    if(role == ROLE_CASHIER) {
        menuLabels[count]="Transaksi Kasir"; menuCodes[count++]=3;
    }

    // MENU GUDANG
    if(role == ROLE_WAREHOUSE) {
        menuLabels[count]="Restock Barang"; menuCodes[count++]=5; // Internal
        menuLabels[count]="Transaksi Beli"; menuCodes[count++]=6; // Ke Supplier
    }

    menuLabels[count]="Logout"; menuCodes[count++]=0;

    int selected = 0; int redraw = 1;
    while(1) {
        if(redraw) { drawBaseLayout(role == ROLE_CASHIER ? "DASHBOARD KASIR" : "DASHBOARD GUDANG"); showDashboardHome(role); redraw = 0; }
        int startY = HEADER_HEIGHT + 6;
        for(int i=0; i<count; i++) printMenuItem(4, startY+(i*2), menuLabels[i], (i==selected));

        int key = getch();
        if(key == 224) { key = getch(); if(key==KEY_UP) selected=(selected>0)?selected-1:count-1; if(key==KEY_DOWN) selected=(selected<count-1)?selected+1:0; }
        else if(key == 13) {
            int code = menuCodes[selected];
            if(code == 0) break; // Logout
            if(code == 1) redraw = 1; // Dashboard (Refresh)
            if(code == 2) viewProfile(idx);

            if(code == 3) crudPenjualan(idx);
            if(code == 5) menuRestock(idx);
            if(code == 6) menuTransaksiGudang(idx);

            redraw = 1;
        } else if (key == 27) {
            clearRightContent(); drawHomeLogo(role);
        }
    }
}