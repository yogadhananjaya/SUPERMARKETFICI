#include "controllers.h"
#include "globals.h"
#include "ui.h"
#include <string.h>
#include <stdio.h>
#include <conio.h>

// ==============================================================================
// 1. MENU PROFIL KARYAWAN
// ==============================================================================
void viewProfile(int idx) {
    clearRightContent();
    drawBreadcrumbs("STAFF > PROFIL SAYA");

    int fx, fy, bw, bh;
    drawFormBox("PROFIL KARYAWAN", &fx, &fy, &bw, &bh);

    int startX = fx + 5;
    int startY = fy + 3;

    textNormal();
    gotoxy(startX, startY);     printf("Nama Lengkap   : %s", dbKaryawan[idx].nama);
    gotoxy(startX, startY + 2); printf("Jabatan        : %s", dbKaryawan[idx].jabatan);
    gotoxy(startX, startY + 4); printf("Nomor Kontak   : %s", dbKaryawan[idx].kontak);

    gotoxy(startX, startY + 6); printf("Status Kerja   : ");
    textHighlightTheme();
    printf(" AKTIF ");
    textNormal();

    // Memanggil fungsi dari ui.c
    if (dbKaryawan[idx].roleId != ROLE_ADMIN && dbKaryawan[idx].roleId != ROLE_STORE_MANAGER) {
        gotoxy(startX, startY + 8); printf("Indeks Performa:");
        drawPerformanceVisual(startX, startY + 9, dbKaryawan[idx].performa);
    } else {
        gotoxy(startX, startY + 8); printf("Akses Sistem   : FULL ACCESS (Manajerial)");
    }

    drawNavigationLegend("[ESC] Kembali");
    while(getch() != KEY_ESC);
}

// ==============================================================================
// 2. MENU TIM SAYA
// ==============================================================================
void viewMyTeam(int roleId) {
    int targetRole = -1;
    if(roleId == ROLE_HEAD_CASHIER) targetRole = ROLE_CASHIER;
    else if(roleId == ROLE_HEAD_WAREHOUSE) targetRole = ROLE_STAFF_WAREHOUSE;
    else return;

    clearRightContent();
    drawBreadcrumbs("TIM > DATA ANGGOTA");

    int tx = getCenterXForTable(85);
    drawTableBox(tx - 2, HEADER_HEIGHT + 3, 90, 13);

    gotoxy(tx, HEADER_HEIGHT + 4);
    printf(" %-5s %-25s %-15s %-25s ", "ID", "NAMA KARYAWAN", "KONTAK", "PERFORMA");
    gotoxy(tx, HEADER_HEIGHT + 5); for(int i = 0; i < 86; i++) printf("-");

    int row = 0;
    for(int i = 0; i < totalKaryawan; i++) {
        if(dbKaryawan[i].roleId == targetRole) {
            gotoxy(tx, HEADER_HEIGHT + 6 + row);
            printf(" %03d   %-25s %-15s ", dbKaryawan[i].id, dbKaryawan[i].nama, dbKaryawan[i].kontak);
            drawPerformanceVisual(tx + 50, HEADER_HEIGHT + 6 + row, dbKaryawan[i].performa);
            row++;
            if(row >= 10) break;
        }
    }

    drawNavigationLegend("[ESC] Kembali");
    while(getch() != KEY_ESC);
}

// ==============================================================================
// 3. MAIN DASHBOARD KARYAWAN (ANTI-BLINK)
// ==============================================================================
typedef struct {
    char label[30];
    int actionCode;
} EmployeeMenu;

void employeeMainMenu(int userIdx) {
    int role = dbKaryawan[userIdx].roleId;
    EmployeeMenu menus[6];
    int totalMenu = 0;

    strcpy(menus[0].label, "Profil Saya"); menus[0].actionCode = 99;
    totalMenu = 1;

    if(role == ROLE_HEAD_CASHIER) {
        strcpy(menus[totalMenu].label, "Kelola Penjualan"); menus[totalMenu++].actionCode = 1;
        strcpy(menus[totalMenu].label, "Lihat Tim Saya");   menus[totalMenu++].actionCode = 2;
    }
    else if(role == ROLE_CASHIER) {
        strcpy(menus[totalMenu].label, "Transaksi Baru");   menus[totalMenu++].actionCode = 1;
        strcpy(menus[totalMenu].label, "Katalog Produk");   menus[totalMenu++].actionCode = 4;
    }
    else if(role == ROLE_HEAD_WAREHOUSE) {
        strcpy(menus[totalMenu].label, "Stok Gudang");      menus[totalMenu++].actionCode = 5;
        strcpy(menus[totalMenu].label, "Input Restock");    menus[totalMenu++].actionCode = 6;
        strcpy(menus[totalMenu].label, "Lihat Tim Saya");   menus[totalMenu++].actionCode = 2;
    }
    else if(role == ROLE_STAFF_WAREHOUSE) {
        strcpy(menus[totalMenu].label, "Cek Stok Gudang");  menus[totalMenu++].actionCode = 4;
    }

    strcpy(menus[totalMenu].label, "Logout"); menus[totalMenu++].actionCode = 0;

    int selected = 0;
    int firstRun = 1;

    while(1) {
        if (firstRun) {
            char title[50];
            sprintf(title, "STAFF: %s", getJabatanName(role));
            drawBaseLayout(title);
            showDashboardHome(role);
            firstRun = 0;
        }

        for(int i = 0; i < totalMenu; i++) {
            printMenuItem(4, HEADER_HEIGHT + 6 + (i * 2), menus[i].label, (i == selected));
        }

        int key = getch();
        if(key == 224) {
            key = getch();
            if(key == 72) selected = (selected <= 0) ? totalMenu - 1 : selected - 1;
            else if(key == 80) selected = (selected >= totalMenu - 1) ? 0 : selected + 1;
        }
        else if(key == 13) {
            int code = menus[selected].actionCode;
            if(code == 0) break;
            if(code == 99) viewProfile(userIdx);
            else if(code == 1) crudPenjualan(userIdx);
            else if(code == 2) viewMyTeam(role);
            else if(code == 4) crudProduk(0);
            else if(code == 5) crudProduk(1);
            else if(code == 6) crudPembelian();
            firstRun = 1;
        }
    }
}