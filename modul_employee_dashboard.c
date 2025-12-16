#include "controllers.h"
#include "config.h"
#include "structures.h"
#include "globals.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void displayMyProfile(int index) {
    clearRightContent();
    int startX = SIDEBAR_WIDTH + 5;
    int startY = HEADER_HEIGHT + 3;

    textNormal();
    gotoxy(startX, startY);     printf("=== PROFIL SAYA ===");
    gotoxy(startX, startY+2);   printf("ID Karyawan  : %03d", dbKaryawan[index].id);
    gotoxy(startX, startY+3);   printf("Nama Lengkap : %s", dbKaryawan[index].nama);
    gotoxy(startX, startY+4);   printf("Jabatan      : %s", dbKaryawan[index].jabatan);
    gotoxy(startX, startY+5);   printf("Kontak       : %s", dbKaryawan[index].kontak);
    gotoxy(startX, startY+6);   printf("Username     : %s", dbKaryawan[index].username);

    gotoxy(startX, startY+8);   printf("=== PERFORMA ===");
    gotoxy(startX, startY+9);   printf("Skor Kinerja : %d / 100", dbKaryawan[index].performa);

    gotoxy(startX, startY+10); printf("[");
    int bars = dbKaryawan[index].performa / 5;
    for(int i=0; i<20; i++) {
        if(i < bars) printf("%c", 219);
        else printf(".");
    }
    printf("]");

    drawNavigationLegend("Tekan [ESC] untuk kembali ke menu");
}

static void updateEmpMenu(int selected) {
    printMenuItem(2, HEADER_HEIGHT + 5, "1. Profil Saya", selected == 1);
    printMenuItem(2, HEADER_HEIGHT + 7, "2. Lihat Produk", selected == 2);
    printMenuItem(2, HEADER_HEIGHT + 9, "3. Transaksi Jual", selected == 3);
    printMenuItem(2, HEADER_HEIGHT + 11,"0. Logout", selected == 4);
}

void employeeMainMenu(int index) {
    int selected = 1;
    int totalMenu = 4;
    int key;

    drawBaseLayout("DASHBOARD KARYAWAN");
    char upperName[50];
    strcpy(upperName, dbKaryawan[index].nama);
    for(int i = 0; upperName[i]; i++){
      upperName[i] = toupper(upperName[i]);
    }

    char welcomeMsg[100];
    snprintf(welcomeMsg, sizeof(welcomeMsg), "SELAMAT DATANG, %s", upperName); // Gunakan yang sudah kapital

    int msgLen = strlen(welcomeMsg);
    int centerX = (screenWidth - msgLen) / 2;

    textNormal();
    gotoxy(centerX, HEADER_HEIGHT + 2);
    printf("%s", welcomeMsg);

    updateEmpMenu(selected);
    drawNavigationLegend("Gunakan [Panah] pilih menu | [ENTER] Buka Menu");

    while(1) {
        hideCursor();
        key = getch();

        if(key == 224) {
            key = getch();
            if(key == KEY_UP) selected = (selected == 1) ? totalMenu : selected - 1;
            else if(key == KEY_DOWN) selected = (selected == totalMenu) ? 1 : selected + 1;
            updateEmpMenu(selected);
        }
        else if(key == KEY_ENTER) {
            if(selected == 4) return;

            if(selected == 1) {
                displayMyProfile(index);
                while(getch() != KEY_ESC);
                clearRightContent();
                drawBaseLayout("DASHBOARD KARYAWAN");
                gotoxy(centerX, HEADER_HEIGHT + 2); printf("%s", welcomeMsg);
                updateEmpMenu(selected);
                drawNavigationLegend("Gunakan [Panah] pilih menu | [ENTER] Buka Menu");
            }
            else if(selected == 2) {
                crudProduk(0);
                drawBaseLayout("DASHBOARD KARYAWAN");
                gotoxy(centerX, HEADER_HEIGHT + 2); printf("%s", welcomeMsg);
                updateEmpMenu(selected);
                drawNavigationLegend("Gunakan [Panah] pilih menu | [ENTER] Buka Menu");
            }
             else if(selected == 3) {
                crudPenjualan();
                drawBaseLayout("DASHBOARD KARYAWAN");
                gotoxy(centerX, HEADER_HEIGHT + 2); printf("%s", welcomeMsg);
                updateEmpMenu(selected);
                drawNavigationLegend("Gunakan [Panah] pilih menu | [ENTER] Buka Menu");
            }
        }
    }
}