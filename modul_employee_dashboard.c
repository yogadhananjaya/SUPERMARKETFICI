#include "controllers.h"
#include "globals.h"
#include "ui.h"

// --- DETAIL PERFORMA ---
void viewProfile(int idx);
void viewMyTeam(int roleId) {
    int targetRole = -1;
    char teamName[30];

    if(roleId == ROLE_HEAD_CASHIER) {
        targetRole = ROLE_CASHIER;
        strcpy(teamName, "TIM KASIR");
    }
    else if(roleId == ROLE_HEAD_WAREHOUSE) {
        targetRole = ROLE_STAFF_WAREHOUSE;
        strcpy(teamName, "TIM GUDANG");
    }
    else {
        showErrorAndWait(SIDEBAR_WIDTH+5, HEADER_HEIGHT+5, "Akses Ditolak.");
        return;
    }

    clearRightContent();
    drawBreadcrumbs("TIM > ANGGOTA");

    int tx = getCenterXForTable(85);
    drawTableBox(tx - 1, HEADER_HEIGHT + 3, 85+2, 13);

    gotoxy(tx, HEADER_HEIGHT + 4);
    printf(" %-5s %c %-25s %c %-15s %c %-25s ", "ID", 186, "NAMA ANGGOTA", 186, "KONTAK", 186, "PERFORMA");
    gotoxy(tx, HEADER_HEIGHT + 5); for(int i=0; i<85; i++) printf("%c", 205);

    int row = 0;
    for(int i = 0; i < totalKaryawan; i++) {
        if(dbKaryawan[i].roleId == targetRole) {
            gotoxy(tx, HEADER_HEIGHT + 6 + row);
            printf(" %04d  %c %-25s %c %-15s %c ", dbKaryawan[i].id, 186, dbKaryawan[i].nama, 186, dbKaryawan[i].kontak);
            drawPerformanceVisual(tx + 54, HEADER_HEIGHT + 6 + row, dbKaryawan[i].performa);
            row++;
            if(row >= 10) break;
        }
    }

    if(row == 0) {
        gotoxy(tx+2, HEADER_HEIGHT+6); printf("Belum ada anggota tim.");
    }

    drawNavigationLegend("[ESC] Kembali");
    while(getch() != 27);
}
void viewPerformanceDetail(int empIdx) {
    int id = dbKaryawan[empIdx].id;
    int role = dbKaryawan[empIdx].roleId;
    int countTrans = 0;
    long totalValue = 0;

    // Hitung Kontribusi
    if(role == ROLE_CASHIER || role == ROLE_HEAD_CASHIER) {
        for(int i=0; i<totalPenjualan; i++) {
            if(dbPenjualan[i].idKaryawan == id) {
                countTrans++;
                totalValue += dbPenjualan[i].totalHarga;
            }
        }
    } else if (role == ROLE_STAFF_WAREHOUSE || role == ROLE_HEAD_WAREHOUSE) {
         for(int i=0; i<totalPembelian; i++) {
            if(dbPembelian[i].idKaryawan == id) {
                countTrans++;
                totalValue += dbPembelian[i].totalHarga;
            }
        }
    }

    int fx, fy, bw, bh;
    drawFormBox("DETAIL KONTRIBUSI", &fx, &fy, &bw, &bh);

    char rp[50]; formatRupiah(totalValue, rp);

    gotoxy(fx+4, fy+3); printf("Nama Karyawan  : %s", dbKaryawan[empIdx].nama);
    gotoxy(fx+4, fy+5); printf("Total Aktivitas: %d Transaksi", countTrans);
    gotoxy(fx+4, fy+7); printf("Nilai Uang     : Rp %s", rp);

    // Visual Bar sederhana
    gotoxy(fx+4, fy+9); printf("Rating System  :");
    drawPerformanceVisual(fx+20, fy+9, dbKaryawan[empIdx].performa);

    drawNavigationLegend("[ESC] Kembali");
    while(getch() != 27);
}

// --- MENU UTAMA STAFF ---
void employeeMainMenu(int idx) {
    int role = dbKaryawan[idx].roleId;

    // Navigasi Dinamis
    char *menuLabels[10];
    int menuCodes[10];
    int count = 0;

    menuLabels[count]="Dashboard"; menuCodes[count++]=1;
    menuLabels[count]="Profil Saya"; menuCodes[count++]=2;

    if(role == ROLE_CASHIER || role == ROLE_HEAD_CASHIER) {
        menuLabels[count]="Transaksi Baru"; menuCodes[count++]=3;
        menuLabels[count]="Riwayat Jual"; menuCodes[count++]=4;
    }

    // FITUR GUDANG
    if(role == ROLE_STAFF_WAREHOUSE || role == ROLE_HEAD_WAREHOUSE) {
        menuLabels[count]="Restock Barang"; menuCodes[count++]=5; // Input Stok
        menuLabels[count]="Katalog Produk"; menuCodes[count++]=6; // Read Stok
    }

    if(role == ROLE_HEAD_CASHIER || role == ROLE_HEAD_WAREHOUSE) {
        menuLabels[count]="Lihat Tim"; menuCodes[count++]=7;
    }

    menuLabels[count]="Logout"; menuCodes[count++]=0;

    int selected = 0;
    int redraw = 1;

    while(1) {
        if(redraw) {
            drawBaseLayout("DASHBOARD STAFF");
            showDashboardHome(role);
            redraw = 0;
        }

        int startY = HEADER_HEIGHT + 6;
        for(int i=0; i<count; i++) printMenuItem(4, startY+(i*2), menuLabels[i], (i==selected));

        int key = getch();
        if(key == 224) {
            key = getch();
            if(key==KEY_UP) selected=(selected>0)?selected-1:count-1;
            if(key==KEY_DOWN) selected=(selected<count-1)?selected+1:0;
        }
        else if(key == 13) {
            int code = menuCodes[selected];
            if(code == 0) break; // Logout
            if(code == 1) redraw = 1;

            if(code == 2) {
                // View Profile + Button Detail
                viewProfile(idx); // Asumsikan viewProfile ada, kita tambahkan tombol di dalamnya nanti atau panggil detail langsung
                // Untuk "tombol tambahan" seperti request, kita buat menu sub di dalam viewProfile atau shortcut di sini.
                // Sesuai request: "saat masuk menu profil ada tombol tambahan"
                // Kita modifikasi viewProfile sebentar di bawah ini:
            }
            if(code == 3) crudPenjualan(idx);
            if(code == 4) { /* Panggil display penjualan table mode read only */ }
            if(code == 5) crudPembelian(idx); // RESTOCK
            if(code == 6) crudProduk(0); // View Only
            if(code == 7) viewMyTeam(role);

            redraw = 1;
        }
    }
}

// Modifikasi View Profile untuk tombol Detail
void viewProfile(int idx) {
    clearRightContent();
    drawBreadcrumbs("STAFF > PROFIL");
    int fx, fy, bw, bh;
    drawFormBox("DETAIL PROFIL", &fx, &fy, &bw, &bh);

    gotoxy(fx+5, fy+3); printf("Nama      : %s", dbKaryawan[idx].nama);
    gotoxy(fx+5, fy+5); printf("Jabatan   : %s", dbKaryawan[idx].jabatan);
    gotoxy(fx+5, fy+7); printf("Kontak    : %s", dbKaryawan[idx].kontak);

    gotoxy(fx+5, fy+9); printf("Performa  :");
    drawPerformanceVisual(fx+16, fy+9, dbKaryawan[idx].performa);

    // TOMBOL TAMBAHAN
    int btnX = fx+5, btnY = fy+12;
    gotoxy(btnX, btnY); textHighlightTheme(); printf(" [ENTER] LIHAT DETAIL KONTRIBUSI "); textNormal();

    drawNavigationLegend("[ESC] Kembali | [ENTER] Detail Performa");

    while(1) {
        int k = getch();
        if(k == 27) break;
        if(k == 13) {
            viewPerformanceDetail(idx);
            break; // Keluar setelah lihat detail
        }
    }
}