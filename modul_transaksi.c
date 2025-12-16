#include "controllers.h"
#include "config.h"
#include "structures.h"
#include "globals.h"
#include "ui.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

static void updateSubMenu(int selected) {
    printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Riwayat", selected == 1);
    printMenuItem(2, HEADER_HEIGHT + 7, "2. Input Baru", selected == 2);
    printMenuItem(2, HEADER_HEIGHT + 9, "0. Kembali", selected == 3);
}


void displayPenjualanTable(int tableX) {
    const int TABLE_WIDTH = 100;
    const int TABLE_HEIGHT = 12;
    int startY = HEADER_HEIGHT + 4;

    int totalPages = (int)ceil((double)totalPenjualan / ROWS_PER_PAGE);
    if (totalPages == 0) totalPages = 1;
    int startIndex = pageOffset * ROWS_PER_PAGE;
    int endIndex = startIndex + ROWS_PER_PAGE;
    if (endIndex > totalPenjualan) endIndex = totalPenjualan;

    printCenterRight(HEADER_HEIGHT + 2, "=== RIWAYAT PENJUALAN ===");
    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, TABLE_HEIGHT + 1);

    textNormal();
    gotoxy(tableX, startY);
    printf(" %-5s %c %-12s %c %-25s %c %-8s %c %-15s ",
           "ID", 186, "TANGGAL", 186, "PELANGGAN", 186, "QTY", 186, "TOTAL (Rp)");

    gotoxy(tableX, startY + 1);
    for(int i=0; i<TABLE_WIDTH; i++) printf("%c", 205);

    for(int i = startIndex; i < endIndex; i++) {
        int displayRow = startY + 2 + (i - startIndex);
        gotoxy(tableX, displayRow);
        printf(" %04d  %c %-12s %c %-25s %c %-8d %c Rp. %-11ld ",
            dbPenjualan[i].id, 186, dbPenjualan[i].tanggal, 186,
            dbPenjualan[i].namaPelanggan, 186, dbPenjualan[i].jumlah, 186, dbPenjualan[i].totalHarga);
    }

    gotoxy(tableX + 5, startY + TABLE_HEIGHT + 2);
    printf("Hal %d / %d | Total Data: %d", pageOffset + 1, totalPages, totalPenjualan);
}

void crudPenjualan() {
    int selected = 1, totalMenu = 3, key;
    drawBaseLayout("TRANSAKSI PENJUALAN");
    int tableX = getCenterXForTable(100);
    int formBoxX, formBoxY, boxW, boxH, formX, formInputX = SIDEBAR_WIDTH + 25, formY;
    int isPagingMode = 0;
    int totalPages;

    pageOffset = 0;
    clearRightContent();
    updateSubMenu(selected);
    drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Eksekusi");

    while(1) {
        hideCursor();
        key = getch();
        if(key == 224) {
            key = getch();
            totalPages = (int)ceil((double)totalPenjualan / ROWS_PER_PAGE);
            if(totalPages == 0) totalPages = 1;

            if(isPagingMode) {
                if(key == KEY_UP && pageOffset > 0) { pageOffset--; clearRightContent(); displayPenjualanTable(tableX); }
                else if(key == KEY_DOWN && pageOffset < totalPages - 1) { pageOffset++; clearRightContent(); displayPenjualanTable(tableX); }
            } else {
                if(key == KEY_UP) { selected = (selected == 1) ? totalMenu : selected - 1; updateSubMenu(selected); }
                else if(key == KEY_DOWN) { selected = (selected == totalMenu) ? 1 : selected + 1; updateSubMenu(selected); }
            }
        }
        else if(key == KEY_ENTER) {
            formX = SIDEBAR_WIDTH + 7; formY = HEADER_HEIGHT + 5;
            if(selected == 3) break;
            pageOffset = 0; isPagingMode = 0;

            if(selected == 1) {
                clearRightContent(); displayPenjualanTable(tableX); isPagingMode = 1;
                drawNavigationLegend("[Atas/Bawah] Scroll Halaman | [ESC] Kembali");
            }
            else if(selected == 2) {
                if(totalPenjualan >= MAX_DATA) { /* Error Handling */ continue; }
                drawFormBox("INPUT PENJUALAN", &formBoxX, &formBoxY, &boxW, &boxH);

                dbPenjualan[totalPenjualan].id = (totalPenjualan > 0) ? dbPenjualan[totalPenjualan-1].id + 1 : 1000;
                gotoxy(formX, formY+1); printf("ID Transaksi : %d", dbPenjualan[totalPenjualan].id);

                gotoxy(formX, formY+3); printf("Tanggal (DD/MM): ");
                gotoxy(formInputX, formY+3); getValidatedString(dbPenjualan[totalPenjualan].tanggal, 19, formInputX, formY+3);

                gotoxy(formX, formY+5); printf("Nama Pelanggan : ");
                gotoxy(formInputX, formY+5); getValidatedString(dbPenjualan[totalPenjualan].namaPelanggan, 49, formInputX, formY+5);

                gotoxy(formX, formY+7); printf("ID Produk      : ");
                dbPenjualan[totalPenjualan].idProduk = (int)getValidatedNumber(formInputX, formY+7);

                // Cari harga produk (Simulasi sederhana)
                long hargaSatuan = 0;
                for(int i=0; i<totalProduk; i++) {
                    if(dbProduk[i].id == dbPenjualan[totalPenjualan].idProduk) {
                        hargaSatuan = dbProduk[i].harga;
                        gotoxy(formX, formY+8); printf("Info: %s (@ Rp %ld)", dbProduk[i].nama, hargaSatuan);
                        break;
                    }
                }
                if(hargaSatuan == 0) hargaSatuan = 10000; // Default jika ID dummy tidak ketemu

                gotoxy(formX, formY+9); printf("Jumlah Beli    : ");
                dbPenjualan[totalPenjualan].jumlah = (int)getValidatedNumber(formInputX, formY+9);

                dbPenjualan[totalPenjualan].totalHarga = dbPenjualan[totalPenjualan].jumlah * hargaSatuan;
                gotoxy(formX, formY+11); printf("Total Bayar    : Rp. %ld", dbPenjualan[totalPenjualan].totalHarga);

                totalPenjualan++;
                gotoxy(formX, formY+13); printf(">> Transaksi Disimpan!");
                Sleep(1000); clearRightContent(); updateSubMenu(selected);
                drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Eksekusi");
            }
        } else if (key == KEY_ESC) {
            if (isPagingMode) { clearRightContent(); isPagingMode = 0; drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Eksekusi"); }
            else if (selected != 3) { selected = 3; updateSubMenu(selected); }
        }
    }
}

void displayPembelianTable(int tableX) {
    const int TABLE_WIDTH = 100;
    const int TABLE_HEIGHT = 12;
    int startY = HEADER_HEIGHT + 4;

    int totalPages = (int)ceil((double)totalPembelian / ROWS_PER_PAGE);
    if (totalPages == 0) totalPages = 1;
    int startIndex = pageOffset * ROWS_PER_PAGE;
    int endIndex = startIndex + ROWS_PER_PAGE;
    if (endIndex > totalPembelian) endIndex = totalPembelian;

    printCenterRight(HEADER_HEIGHT + 2, "=== RIWAYAT PEMBELIAN (RESTOCK) ===");
    drawTableBox(tableX - 1, startY - 1, TABLE_WIDTH + 2, TABLE_HEIGHT + 1);

    textNormal();
    gotoxy(tableX, startY);
    printf(" %-5s %c %-12s %c %-10s %c %-10s %c %-15s ",
           "ID", 186, "TANGGAL", 186, "ID SUPP", 186, "QTY MASUK", 186, "TOTAL BIAYA");

    gotoxy(tableX, startY + 1);
    for(int i=0; i<TABLE_WIDTH; i++) printf("%c", 205);

    for(int i = startIndex; i < endIndex; i++) {
        int displayRow = startY + 2 + (i - startIndex);
        gotoxy(tableX, displayRow);
        printf(" %04d  %c %-12s %c %-10d %c %-10d %c Rp. %-11ld ",
            dbPembelian[i].id, 186, dbPembelian[i].tanggal, 186,
            dbPembelian[i].idSupplier, 186, dbPembelian[i].jumlah, 186, dbPembelian[i].totalHarga);
    }

    gotoxy(tableX + 5, startY + TABLE_HEIGHT + 2);
    printf("Hal %d / %d | Total Data: %d", pageOffset + 1, totalPages, totalPembelian);
}

void crudPembelian() {
    int selected = 1, totalMenu = 3, key;
    drawBaseLayout("TRANSAKSI PEMBELIAN");
    int tableX = getCenterXForTable(100);
    int formBoxX, formBoxY, boxW, boxH, formX, formInputX = SIDEBAR_WIDTH + 25, formY;
    int isPagingMode = 0;
    int totalPages;

    pageOffset = 0;
    clearRightContent();
    updateSubMenu(selected);
    drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Eksekusi");

    while(1) {
        hideCursor();
        key = getch();
        if(key == 224) {
            key = getch();
            totalPages = (int)ceil((double)totalPembelian / ROWS_PER_PAGE);
            if(totalPages == 0) totalPages = 1;

            if(isPagingMode) {
                if(key == KEY_UP && pageOffset > 0) { pageOffset--; clearRightContent(); displayPembelianTable(tableX); }
                else if(key == KEY_DOWN && pageOffset < totalPages - 1) { pageOffset++; clearRightContent(); displayPembelianTable(tableX); }
            } else {
                if(key == KEY_UP) { selected = (selected == 1) ? totalMenu : selected - 1; updateSubMenu(selected); }
                else if(key == KEY_DOWN) { selected = (selected == totalMenu) ? 1 : selected + 1; updateSubMenu(selected); }
            }
        }
        else if(key == KEY_ENTER) {
            formX = SIDEBAR_WIDTH + 7; formY = HEADER_HEIGHT + 5;
            if(selected == 3) break;
            pageOffset = 0; isPagingMode = 0;

            if(selected == 1) {
                clearRightContent(); displayPembelianTable(tableX); isPagingMode = 1;
                drawNavigationLegend("[Atas/Bawah] Scroll Halaman | [ESC] Kembali");
            }
            else if(selected == 2) {
                drawFormBox("INPUT PEMBELIAN", &formBoxX, &formBoxY, &boxW, &boxH);

                dbPembelian[totalPembelian].id = (totalPembelian > 0) ? dbPembelian[totalPembelian-1].id + 1 : 5000;
                gotoxy(formX, formY+1); printf("ID Transaksi : %d", dbPembelian[totalPembelian].id);

                gotoxy(formX, formY+3); printf("Tanggal      : ");
                gotoxy(formInputX, formY+3); getValidatedString(dbPembelian[totalPembelian].tanggal, 19, formInputX, formY+3);

                gotoxy(formX, formY+5); printf("ID Supplier  : ");
                dbPembelian[totalPembelian].idSupplier = (int)getValidatedNumber(formInputX, formY+5);

                gotoxy(formX, formY+7); printf("ID Produk    : ");
                dbPembelian[totalPembelian].idProduk = (int)getValidatedNumber(formInputX, formY+7);

                gotoxy(formX, formY+9); printf("Jumlah Masuk : ");
                dbPembelian[totalPembelian].jumlah = (int)getValidatedNumber(formInputX, formY+9);

                gotoxy(formX, formY+11); printf("Total Biaya  : ");
                dbPembelian[totalPembelian].totalHarga = getValidatedNumber(formInputX, formY+11);

                totalPembelian++;
                gotoxy(formX, formY+13); printf(">> Stok Masuk Disimpan!");
                Sleep(1000); clearRightContent(); updateSubMenu(selected);
                drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Eksekusi");
            }
        } else if (key == KEY_ESC) {
            if (isPagingMode) { clearRightContent(); isPagingMode = 0; drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Eksekusi"); }
            else if (selected != 3) { selected = 3; updateSubMenu(selected); }
        }
    }
}