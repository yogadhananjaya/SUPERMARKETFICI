#include "controllers.h"
#include "config.h"
#include "structures.h"
#include "globals.h"
#include "ui.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

// --- Helper UI ---
static void updateSubMenu(int selected) {
    printMenuItem(2, HEADER_HEIGHT + 5, "1. Lihat Riwayat", selected == 1);
    printMenuItem(2, HEADER_HEIGHT + 7, "2. Input Baru", selected == 2);
    printMenuItem(2, HEADER_HEIGHT + 9, "0. Kembali", selected == 3);
}

// ==========================================
// CRUD PENJUALAN (SALES)
// ==========================================

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
                if(totalPenjualan >= MAX_DATA) { continue; }
                drawFormBox("INPUT PENJUALAN", &formBoxX, &formBoxY, &boxW, &boxH);

                dbPenjualan[totalPenjualan].id = (totalPenjualan > 0) ? dbPenjualan[totalPenjualan-1].id + 1 : 1000;
                gotoxy(formX, formY+1); printf("ID Transaksi : %d", dbPenjualan[totalPenjualan].id);

                gotoxy(formX, formY+3); printf("Tanggal (DD/MM/YR)  :   ");
                gotoxy(formInputX, formY+3); getValidatedString(dbPenjualan[totalPenjualan].tanggal, 19, formInputX, formY+3);

                gotoxy(formX, formY+5); printf("Nama Pelanggan : ");
                gotoxy(formInputX, formY+5); getValidatedString(dbPenjualan[totalPenjualan].namaPelanggan, 49, formInputX, formY+5);


                int prodIdx = -1;
                while(1) {
                    gotoxy(formX, formY+7); printf("ID Produk      : ");
                    gotoxy(formInputX, formY+7);
                    int tempID = (int)getValidatedNumber(formInputX, formY+7);

                    // Cari Produk
                    for(int i=0; i<totalProduk; i++) {
                        if(dbProduk[i].id == tempID) {
                            prodIdx = i;
                            break;
                        }
                    }

                    if(prodIdx != -1) {
                        dbPenjualan[totalPenjualan].idProduk = tempID;
                        // Tampilkan Info Produk
                        gotoxy(formX, formY+8);
                        printf("Info: %-25s Stok: %d", dbProduk[prodIdx].nama, dbProduk[prodIdx].stok);
                        break;
                    } else {
                        gotoxy(formX, formY+8); printf(">> Produk tidak ditemukan!        ");
                        Beep(750, 200);
                        gotoxy(formInputX, formY+7); printf("                   "); // Hapus input
                    }
                }

                gotoxy(formX, formY+10); printf("Jumlah Beli    : ");
                int qty = (int)getValidatedNumber(formInputX, formY+10);

                // Cek Stok Cukup gak?
                if(qty > dbProduk[prodIdx].stok) {
                    gotoxy(formX, formY+11); printf(">> Stok Kurang! (Sisa: %d)", dbProduk[prodIdx].stok);
                    Beep(750, 500);
                    Sleep(1500);
                    clearRightContent(); updateSubMenu(selected); // Reset form
                    continue;
                }
                dbPenjualan[totalPenjualan].jumlah = qty;

                // Kurangi Stok Real
                dbProduk[prodIdx].stok -= qty;

                // Hitung Otomatis (Qty x Harga Jual Produk)
                long hargaSatuan = dbProduk[prodIdx].harga;
                dbPenjualan[totalPenjualan].totalHarga = qty * hargaSatuan;

                gotoxy(formX, formY+12); printf("Harga Satuan   : Rp. %ld", hargaSatuan);
                gotoxy(formX, formY+13); printf("TOTAL BAYAR    : Rp. %ld ", dbPenjualan[totalPenjualan].totalHarga);

                totalPenjualan++;
                gotoxy(formX, formY+15); printf(">> Transaksi Berhasil!");
                Sleep(1500); clearRightContent(); updateSubMenu(selected);
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

    printCenterRight(HEADER_HEIGHT + 2, "=== RIWAYAT PEMBELIAN ===");
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

                gotoxy(formX, formY+3); printf("Tanggal(DD/MM/YR)   : ");
                gotoxy(formInputX, formY+3); getValidatedString(dbPembelian[totalPembelian].tanggal, 19, formInputX, formY+3);

                // --- 1. VALIDASI SUPPLIER ---
                int suppIdx = -1;
                while(1) {
                    gotoxy(formX, formY+5); printf("ID Supplier  : ");
                    gotoxy(formInputX, formY+5);
                    int tempID = (int)getValidatedNumber(formInputX, formY+5);

                    // Cari Supplier
                    for(int i=0; i<totalSupplier; i++) {
                        if(dbSupplier[i].id == tempID) {
                            suppIdx = i;
                            break;
                        }
                    }

                    if(suppIdx != -1) {
                        dbPembelian[totalPembelian].idSupplier = tempID;
                        // TAMPILKAN NAMA SUPPLIER
                        gotoxy(formX, formY+6);
                        printf("Info: %-30s", dbSupplier[suppIdx].nama);
                        break;
                    } else {
                        gotoxy(formX, formY+6); printf(">> Supplier tidak ditemukan!      ");
                        Beep(750, 200);
                        gotoxy(formInputX, formY+5); printf("                   "); // Clear input
                    }
                }

                // --- 2. VALIDASI PRODUK ---
                int prodIdx = -1;
                while(1) {
                    gotoxy(formX, formY+7); printf("ID Produk    : ");
                    gotoxy(formInputX, formY+7);
                    int tempID = (int)getValidatedNumber(formInputX, formY+7);

                    for(int i=0; i<totalProduk; i++) {
                        if(dbProduk[i].id == tempID) {
                            prodIdx = i;
                            break;
                        }
                    }

                    if(prodIdx != -1) {
                        dbPembelian[totalPembelian].idProduk = tempID;
                        // TAMPILKAN NAMA PRODUK
                        gotoxy(formX, formY+8);
                        printf("Info: %-30s", dbProduk[prodIdx].nama);
                        break;
                    } else {
                        gotoxy(formX, formY+8); printf(">> Produk tidak ditemukan!        ");
                        Beep(750, 200);
                        gotoxy(formInputX, formY+7); printf("                   ");
                    }
                }

                // --- 3. INPUT JUMLAH & HARGA BELI ---
                gotoxy(formX, formY+9); printf("Jumlah Masuk : ");
                int qty = (int)getValidatedNumber(formInputX, formY+9);
                dbPembelian[totalPembelian].jumlah = qty;

                gotoxy(formX, formY+10); printf("Harga Beli/Pc: ");
                long hargaBeli = getValidatedNumber(formInputX, formY+10);

                // --- 4. HITUNG OTOMATIS & UPDATE STOK ---
                dbPembelian[totalPembelian].totalHarga = qty * hargaBeli;

                // Tambah stok ke database produk
                dbProduk[prodIdx].stok += qty;

                gotoxy(formX, formY+12); printf("TOTAL BIAYA  : Rp. %ld ", dbPembelian[totalPembelian].totalHarga);

                totalPembelian++;
                gotoxy(formX, formY+14); printf(">> Stok Berhasil Ditambah!");
                Sleep(1500); clearRightContent(); updateSubMenu(selected);
                drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Pilih");
            }
        } else if (key == KEY_ESC) {
            if (isPagingMode) { clearRightContent(); isPagingMode = 0; drawNavigationLegend("[Panah] Pilih Menu | [ENTER] Eksekusi"); }
            else if (selected != 3) { selected = 3; updateSubMenu(selected); }
        }
    }
}