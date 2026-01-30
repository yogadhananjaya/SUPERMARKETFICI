#include "controllers.h"
#include "globals.h"
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct { int pid; int qty; long price; long subtotal; } CartItem;
CartItem cart[100]; int cartCount = 0;

int selectProductOverlay() {
    int tableX = getCenterXForTable(95); int startY = HEADER_HEIGHT + 2;
    int selectedIndex = 0; int page = 0; int hideId = 0;
    clearRightContent();
    while(1) {
        int activeIndices[MAX_DATA]; int activeCount = 0;
        for(int i=0; i<totalProduk; i++) if(dbProduk[i].isActive) activeIndices[activeCount++] = i;

        int totalPages = (int)ceil((double)activeCount / ROWS_PER_PAGE);
        int start = page * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > activeCount) ? activeCount : start + ROWS_PER_PAGE;
        drawTableBox(tableX - 1, startY - 1, 95 + 2, 13);
        textNormal(); gotoxy(tableX, startY); printf(" %-5s %c %-5s %c %-40s %c %-10s %c %-20s ", "NO", 186, "ID", 186, "NAMA PRODUK", 186, "STOK", 186, "HARGA (Rp)");
        gotoxy(tableX, startY + 1); for(int i=0; i<95; i++) printf("%c", 205);
        for(int i = start; i < end; i++) {
            int idx = activeIndices[i];
            char rp[30], noStr[10]; formatRupiah(dbProduk[idx].harga, rp);
            if(hideId) strcpy(noStr, "***"); else sprintf(noStr, "%03d", i+1);
            int rowY = startY + 2 + (i - start); gotoxy(tableX, rowY);
            if(i == selectedIndex) textHighlightTheme(); else textNormal();
            printf(" %-5s %c %03d   %c %-40s %c %10d %c %19s ", noStr, 186, dbProduk[idx].id, 186, dbProduk[idx].nama, 186, dbProduk[idx].stok, 186, rp); textNormal();
        }
        drawNavigationLegend("[PANAH] Pilih | [TAB] Hide No | [ENTER] Pilih | [ESC] Batal");
        int key = getch();
        if(key == 224) { key = getch(); if(key == KEY_UP && selectedIndex > 0) { selectedIndex--; if(selectedIndex < start) page--; } if(key == KEY_DOWN && selectedIndex < activeCount - 1) { selectedIndex++; if(selectedIndex >= end) page++; } }
        else if(key == KEY_TAB) { hideId = !hideId; } else if(key == 13) { return dbProduk[activeIndices[selectedIndex]].id; } else if(key == 27) { return -1; }
    }
}

int selectSupplierOverlay() {
    int tableX = getCenterXForTable(95); int startY = HEADER_HEIGHT + 2;
    int selectedIndex = 0; int page = 0; int hideId = 0;
    clearRightContent();
    while(1) {
        int activeIndices[MAX_DATA]; int activeCount = 0;
        for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].isActive) activeIndices[activeCount++] = i;

        int totalPages = (int)ceil((double)activeCount / ROWS_PER_PAGE);
        int start = page * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > activeCount) ? activeCount : start + ROWS_PER_PAGE;
        drawTableBox(tableX - 1, startY - 1, 95 + 2, 13);
        textNormal(); gotoxy(tableX, startY); printf(" %-5s %c %-30s %c %-50s ", "NO", 186, "NAMA SUPPLIER", 186, "ALAMAT");
        gotoxy(tableX, startY + 1); for(int i=0; i<95; i++) printf("%c", 205);
        for(int i = start; i < end; i++) {
            int idx = activeIndices[i];
            char noStr[10]; if(hideId) strcpy(noStr, "***"); else sprintf(noStr, "%03d", i+1);
            int rowY = startY + 2 + (i - start); gotoxy(tableX, rowY);
            if(i == selectedIndex) textHighlightTheme(); else textNormal();
            char sName[31]; strncpy(sName, dbSupplier[idx].nama, 30); sName[30]=0;
            char sAddr[51]; strncpy(sAddr, dbSupplier[idx].alamat, 50); sAddr[50]=0;
            printf(" %-5s %c %-30s %c %-50s ", noStr, 186, sName, 186, sAddr); textNormal();
        }
        drawNavigationLegend("[PANAH] Pilih | [TAB] Hide No | [ENTER] Pilih | [ESC] Batal");
        int key = getch();
        if(key == 224) { key = getch(); if(key == KEY_UP && selectedIndex > 0) { selectedIndex--; if(selectedIndex < start) page--; } if(key == KEY_DOWN && selectedIndex < activeCount - 1) { selectedIndex++; if(selectedIndex >= end) page++; } }
        else if(key == KEY_TAB) { hideId = !hideId; } else if(key == 13) { return dbSupplier[activeIndices[selectedIndex]].id; } else if(key == 27) { return -1; }
    }
}

void viewInvoiceDetail(const char* targetFaktur) {
    clearRightContent(); int fx, fy, bw, bh;
    char kasirName[50] = "-"; char tgl[20] = "-"; long grandTotal = 0;
    for(int i=0; i<totalPenjualan; i++) { if(strcmp(dbPenjualan[i].noFaktur, targetFaktur) == 0) { strcpy(kasirName, dbPenjualan[i].namaKasir); strcpy(tgl, dbPenjualan[i].tanggal); grandTotal = dbPenjualan[i].totalBayar; break; } }
    drawFormBox("DETAIL TRANSAKSI", &fx, &fy, &bw, &bh); bh = 18; drawShadowBox(fx, fy, bw, bh);
    gotoxy(fx+2, fy+2); printf("No Faktur : %s", targetFaktur); gotoxy(fx+40, fy+2); printf("Tanggal   : %s", tgl); gotoxy(fx+2, fy+3); printf("Kasir     : %s", kasirName);

    // Header Tabel Detail
    int listY = fy + 5;
    gotoxy(fx+2, listY); printf("%-3s %-25s %-12s %-5s %-15s", "NO", "PRODUK", "HARGA", "QTY", "SUBTOTAL");
    gotoxy(fx+2, listY+1); for(int i=0; i<60; i++) printf("-");

    int row = 0;
    for(int i=0; i<totalPenjualan; i++) {
        if(strcmp(dbPenjualan[i].noFaktur, targetFaktur) == 0) {
            char pName[31] = "Unknown"; for(int p=0; p<totalProduk; p++) { if(dbProduk[p].id == dbPenjualan[i].idProduk) { strcpy(pName, dbProduk[p].nama); break; } }

            // Hitung harga satuan (reverse engineer dari total / jumlah, atau ambil dari master jika belum berubah,
            // tapi karena struct tidak simpan harga satuan saat transaksi, kita hitung rata-rata)
            long unitPrice = dbPenjualan[i].totalHarga / dbPenjualan[i].jumlah;

            char rpUnit[20], rpSub[20];
            formatRupiah(unitPrice, rpUnit);
            formatRupiah(dbPenjualan[i].totalHarga, rpSub);

            if(row < 10) {
                gotoxy(fx+2, listY+2+row);
                printf("%-3d %-25s %-12s %-5d %-15s", row+1, pName, rpUnit, dbPenjualan[i].jumlah, rpSub);
            } row++;
        }
    }
    char rpTotal[30]; formatRupiah(grandTotal, rpTotal); gotoxy(fx+2, listY+13); for(int i=0; i<60; i++) printf("-"); gotoxy(fx+2, listY+14); printf("TOTAL BAYAR : Rp %s", rpTotal);
    drawNavigationLegend("[ESC] Kembali"); while(getch() != 27);
}

void viewPurchaseDetail(const char* targetFaktur) {
    clearRightContent(); int fx, fy, bw, bh;
    char supName[50] = "Unknown"; char tgl[20] = "-"; long grandTotal = 0;
    int supId = -1;
    for(int i=0; i<totalPembelian; i++) { if(strcmp(dbPembelian[i].noFaktur, targetFaktur) == 0) { supId = dbPembelian[i].idSupplier; strcpy(tgl, dbPembelian[i].tanggal); break; } }
    for(int i=0; i<totalSupplier; i++) { if(dbSupplier[i].id == supId) strcpy(supName, dbSupplier[i].nama); }
    drawFormBox("DETAIL PEMBELIAN", &fx, &fy, &bw, &bh); bh = 18; drawShadowBox(fx, fy, bw, bh);
    gotoxy(fx+2, fy+2); printf("No Faktur : %s", targetFaktur); gotoxy(fx+40, fy+2); printf("Tanggal   : %s", tgl);
    gotoxy(fx+2, fy+3); printf("Supplier  : %s", supName);

    int listY = fy + 5;
    gotoxy(fx+2, listY); printf("%-3s %-25s %-12s %-5s %-15s", "NO", "PRODUK", "HARGA", "QTY", "SUBTOTAL");
    gotoxy(fx+2, listY+1); for(int i=0; i<60; i++) printf("-");

    int row = 0;
    for(int i=0; i<totalPembelian; i++) {
        if(strcmp(dbPembelian[i].noFaktur, targetFaktur) == 0) {
            char pName[31] = "Unknown"; for(int p=0; p<totalProduk; p++) { if(dbProduk[p].id == dbPembelian[i].idProduk) { strcpy(pName, dbProduk[p].nama); break; } }

            long unitPrice = dbPembelian[i].totalHarga / dbPembelian[i].jumlah;
            char rpUnit[20], rpSub[20];
            formatRupiah(unitPrice, rpUnit);
            formatRupiah(dbPembelian[i].totalHarga, rpSub);
            grandTotal += dbPembelian[i].totalHarga;

            if(row < 10) {
                gotoxy(fx+2, listY+2+row);
                printf("%-3d %-25s %-12s %-5d %-15s", row+1, pName, rpUnit, dbPembelian[i].jumlah, rpSub);
            } row++;
        }
    }
    char rpTotal[30]; formatRupiah(grandTotal, rpTotal); gotoxy(fx+2, listY+13); for(int i=0; i<60; i++) printf("-"); gotoxy(fx+2, listY+14); printf("TOTAL BAYAR : Rp %s", rpTotal);
    drawNavigationLegend("[ESC] Kembali"); while(getch() != 27);
}

void crudPenjualan(int idKaryawanLogin) {
    int currentRole = dbKaryawan[idKaryawanLogin].roleId;
    int selected = 0; const char *menuItems[] = {"Transaksi Baru", "Riwayat Jual", "Kembali"};
    int tx = getCenterXForTable(95); int isHistoryMode = 0; int historyCursor = 0; int historyPage = 0; int hideId = 0;
    drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo(currentRole);

    while(1) {
        int startY = HEADER_HEIGHT + 6; for(int i=0; i<3; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
        if(isHistoryMode) {
            clearRightContent();
            int uniqueIndices[MAX_DATA]; int uniqueCount = 0; char lastFaktur[20] = "";
            for(int i=0; i<totalPenjualan; i++) { if(strcmp(dbPenjualan[i].noFaktur, lastFaktur) != 0) { uniqueIndices[uniqueCount++] = i; strcpy(lastFaktur, dbPenjualan[i].noFaktur); } }
            drawTableBox(tx-1, startY-1, 95, 13); textNormal(); gotoxy(tx, startY); printf(" %-5s %c %-20s %c %-15s %c %-20s %c %-15s ", "NO", 186, "NO FAKTUR", 186, "TANGGAL", 186, "KASIR", 186, "TOTAL (Rp)"); gotoxy(tx, startY+1); for(int i=0; i<93; i++) printf("%c", 205);
            int totalPages = (int)ceil((double)uniqueCount / ROWS_PER_PAGE); if(totalPages == 0) totalPages = 1;
            int start = historyPage * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > uniqueCount) ? uniqueCount : start + ROWS_PER_PAGE;
            for(int i=start; i<end; i++) {
                int dataIdx = uniqueIndices[i]; char rp[30], noStr[10]; formatRupiah(dbPenjualan[dataIdx].totalBayar, rp);
                if (hideId) strcpy(noStr, "***"); else sprintf(noStr, "%03d", i+1);
                int rowY = startY+2+(i-start); gotoxy(tx, rowY); if(i == historyCursor) textHighlightTheme(); else textNormal();
                printf(" %-5s %c %-20s %c %-15s %c %-20s %c Rp %-12s ", noStr, 186, dbPenjualan[dataIdx].noFaktur, 186, dbPenjualan[dataIdx].tanggal, 186, dbPenjualan[dataIdx].namaKasir, 186, rp); textNormal();
            }
            drawNavigationLegend("[PANAH] Pilih | [ENTER] Detail | [TAB] Hide No | [ESC] Menu");
            int key = getch();
            if(key == 224) { key = getch(); if(key == KEY_UP && historyCursor > 0) { historyCursor--; if(historyCursor < start) historyPage--; } if(key == KEY_DOWN && historyCursor < uniqueCount - 1) { historyCursor++; if(historyCursor >= end) historyPage++; } }
            else if(key == 13 && uniqueCount > 0) { int dataIdx = uniqueIndices[historyCursor]; viewInvoiceDetail(dbPenjualan[dataIdx].noFaktur); drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo(currentRole); }
            else if(key == KEY_TAB) { hideId = !hideId; } else if(key == 27) { isHistoryMode = 0; clearRightContent(); drawHomeLogo(currentRole); }
            continue;
        } else { drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka"); }
        int key=getch();
        if(key == 224) { key=getch(); if(!isHistoryMode) { if(key==KEY_UP) selected=(selected>0)?selected-1:2; if(key==KEY_DOWN) selected=(selected<2)?selected+1:0; } }
        else if(key == 13 && !isHistoryMode) {
            if(selected == 2) break; if(selected == 1) { isHistoryMode = 1; historyCursor = 0; historyPage = 0; }
            if(selected == 0) {
                cartCount = 0; long grandTotal = 0; char faktur[20], tgl[20];
                int uniqueInv = 0; char lastF[20]=""; for(int k=0; k<totalPenjualan; k++) { if(strcmp(dbPenjualan[k].noFaktur, lastF)!=0) { uniqueInv++; strcpy(lastF, dbPenjualan[k].noFaktur); } }
                sprintf(faktur, "INV/%04d", uniqueInv + 1); getTodayDate(tgl);
                int pid = selectProductOverlay(); if(pid == -1) { drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo(currentRole); continue; }
                int pidx = -1; for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == pid) pidx = i;
                if(pidx != -1) {
                     clearRightContent(); int bx, by, bw, bh; drawFormBox("INPUT QUANTITY", &bx, &by, &bw, &bh);
                     gotoxy(bx+2, by+2); printf("Produk : %s", dbProduk[pidx].nama); gotoxy(bx+2, by+6); printf("Jumlah : "); int qty = (int)getValidatedNumber(bx+11, by+6);
                     if(qty > 0 && qty <= dbProduk[pidx].stok) { cart[cartCount].pid = pid; cart[cartCount].qty = qty; cart[cartCount].price = dbProduk[pidx].harga; cart[cartCount].subtotal = qty * dbProduk[pidx].harga; grandTotal += cart[cartCount].subtotal; cartCount++; } else { showErrorAndWait(bx+2, by+8, "Stok tidak cukup!"); }
                }
                while(1) {
                    drawBaseLayout("KASIR > TRANSAKSI BARU"); int fx=SIDEBAR_WIDTH+2, fy=HEADER_HEIGHT+2;
                    gotoxy(fx, fy); printf("FAKTUR: %s | %s", faktur, tgl); gotoxy(fx, fy+2); printf("KERANJANG BELANJA:");

                    // UPDATE UI: Menampilkan Harga Satuan
                    for(int i=0; i<cartCount; i++) {
                        char pn[30], rpUnit[20], rpSub[20];
                        for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==cart[i].pid) strcpy(pn, dbProduk[p].nama);
                        formatRupiah(cart[i].price, rpUnit);
                        formatRupiah(cart[i].subtotal, rpSub);
                        gotoxy(fx, fy+3+i); printf("%d. %-15s @%s x%d = %s", i+1, pn, rpUnit, cart[i].qty, rpSub);
                    }

                    char gtStr[30]; formatRupiah(grandTotal, gtStr); gotoxy(fx, fy+15); printf("TOTAL: Rp %s", gtStr);
                    drawNavigationLegend("[T] Tambah Item | [P] Proses Pembayaran | [ESC] Batal");
                    int cmd = getch(); if(cmd == 27) { if(getConfirmation(fx, fy+17, "Batalkan?")) break; }
                    if(tolower(cmd) == 't') { int nextPid = selectProductOverlay(); if(nextPid != -1) { int nextIdx = -1; for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == nextPid) nextIdx = i; if(nextIdx != -1) { clearRightContent(); int bx, by, bw, bh; drawFormBox("INPUT QUANTITY", &bx, &by, &bw, &bh); gotoxy(bx+2, by+2); printf("Produk : %s", dbProduk[nextIdx].nama); gotoxy(bx+2, by+6); printf("Jumlah : "); int qty = (int)getValidatedNumber(bx+11, by+6); if(qty > 0 && qty <= dbProduk[nextIdx].stok) { cart[cartCount].pid = nextPid; cart[cartCount].qty = qty; cart[cartCount].price = dbProduk[nextIdx].harga; cart[cartCount].subtotal = qty * dbProduk[nextIdx].harga; grandTotal += cart[cartCount].subtotal; cartCount++; } else { showErrorAndWait(bx+2, by+8, "Stok Invalid!"); } } } }
                    if(tolower(cmd) == 'p' && cartCount > 0) {
                        clearRightContent(); int px, py, pw, ph; drawFormBox("PEMBAYARAN", &px, &py, &pw, &ph);
                        char strTotal[30]; formatRupiah(grandTotal, strTotal); gotoxy(px+2, py+2); printf("Total Tagihan : Rp %s", strTotal);
                        long bayar = 0; while(1) { gotoxy(px+2, py+4); printf("Uang Diterima : Rp            "); bayar = getValidatedNumber(px+21, py+4); if(bayar < grandTotal) { showErrorAndWait(px+2, py+6, "Uang Tidak Cukup!"); } else { long kembalian = bayar - grandTotal; char strKembali[30]; formatRupiah(kembalian, strKembali); gotoxy(px+2, py+6); printf("Kembalian     : Rp %s       ", strKembali); getch(); break; } }
                        for(int i=0; i<cartCount; i++) {
                            dbPenjualan[totalPenjualan].id = totalPenjualan+1; strcpy(dbPenjualan[totalPenjualan].noFaktur, faktur); strcpy(dbPenjualan[totalPenjualan].tanggal, tgl); dbPenjualan[totalPenjualan].idKaryawan = idKaryawanLogin; strcpy(dbPenjualan[totalPenjualan].namaKasir, dbKaryawan[idKaryawanLogin].nama); dbPenjualan[totalPenjualan].idProduk = cart[i].pid; dbPenjualan[totalPenjualan].jumlah = cart[i].qty; dbPenjualan[totalPenjualan].totalHarga = cart[i].subtotal; dbPenjualan[totalPenjualan].totalBayar = grandTotal;
                            for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==cart[i].pid) dbProduk[p].stok -= cart[i].qty; totalPenjualan++;
                        }
                        // +POIN KASIR
                        dbKaryawan[idKaryawanLogin].performa += 1; // +1 Melakukan Transaksi
                        dbRiwayat[totalRiwayat].id = totalRiwayat+1; dbRiwayat[totalRiwayat].idKaryawan = dbKaryawan[idKaryawanLogin].id;
                        strcpy(dbRiwayat[totalRiwayat].aktivitas, "Melakukan Transaksi (Kasir)"); dbRiwayat[totalRiwayat].poin = 1;
                        strcpy(dbRiwayat[totalRiwayat].tanggal, tgl); totalRiwayat++;

                        saveAllData(); showErrorAndWait(px+2, py+8, "Transaksi Sukses!"); break;
                    }
                }
                drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo(currentRole);
            }
        } else if(key == 27) { if(!isHistoryMode) break; }
    }
}

void menuRestock(int idKaryawanLogin) {
    int currentRole = dbKaryawan[idKaryawanLogin].roleId;
    int selected = 0; const char *menuItems[] = {"Input Restock", "Lihat Stok Barang", "Kembali"};
    drawBaseLayout("GUDANG > RESTOCK"); drawHomeLogo(currentRole);
    int stockPage = 0; int isViewingStock = 0;

    while(1) {
        if(isViewingStock) {
            clearRightContent(); int tableX = getCenterXForTable(95); int startY = HEADER_HEIGHT + 4;
            drawTableBox(tableX - 1, startY - 1, 95 + 2, 13); textNormal();
            gotoxy(tableX, startY); printf(" %-5s %c %-40s %c %-10s %c %-20s ", "NO", 186, "NAMA PRODUK", 186, "STOK", 186, "HARGA (Rp)");
            gotoxy(tableX, startY + 1); for(int i=0; i<95; i++) printf("%c", 205);

            int activeIndices[MAX_DATA]; int activeCount = 0;
            for(int i=0; i<totalProduk; i++) if(dbProduk[i].isActive) activeIndices[activeCount++] = i;

            int totalPages = (int)ceil((double)activeCount / ROWS_PER_PAGE); if(totalPages == 0) totalPages = 1;
            int start = stockPage * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > activeCount) ? activeCount : start + ROWS_PER_PAGE;
            for(int i = start; i < end; i++) {
                int idx = activeIndices[i];
                char rp[30], noStr[10]; formatRupiah(dbProduk[idx].harga, rp); sprintf(noStr, "%03d", i+1);
                gotoxy(tableX, startY + 2 + (i - start)); printf(" %-5s %c %-40s %c %10d %c %19s ", noStr, 186, dbProduk[idx].nama, 186, dbProduk[idx].stok, 186, rp);
            }
            drawNavigationLegend("[PANAH] Scroll | [ESC] Kembali Menu");
            int key = getch(); if (key == 224) { key = getch(); if(key == KEY_UP && stockPage > 0) stockPage--; if(key == KEY_DOWN && stockPage < totalPages - 1) stockPage++; }
            else if (key == 27) { isViewingStock = 0; stockPage = 0; clearRightContent(); drawHomeLogo(currentRole); }
            continue;
        }

        for(int i=0; i<3; i++) printMenuItem(4, HEADER_HEIGHT+6+(i*2), (char*)menuItems[i], (i==selected));
        int key = getch(); if(key==224) { key=getch(); if(key==KEY_UP) selected=(selected>0)?selected-1:2; if(key==KEY_DOWN) selected=(selected<2)?selected+1:0; }
        else if(key==13) {
            if(selected==2) break;
            if(selected==0) {
                 int pid = selectProductOverlay();
                 if(pid != -1) {
                     int pidx = -1; for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == pid) pidx = i;
                     if (pidx != -1) {
                         clearRightContent(); int fx, fy, bw, bh; drawFormBox("INPUT RESTOCK", &fx, &fy, &bw, &bh);
                         gotoxy(fx+2, fy+2); printf("Produk : %s", dbProduk[pidx].nama); gotoxy(fx+2, fy+4); printf("Stok   : %d", dbProduk[pidx].stok);
                         gotoxy(fx+2, fy+6); printf("Tambah : "); int qty = (int)getValidatedNumber(fx+11, fy+6);
                         if(qty > 0) {
                             dbProduk[pidx].stok += qty;
                             dbKaryawan[idKaryawanLogin].performa += 1; // +POIN
                             dbRiwayat[totalRiwayat].id = totalRiwayat+1; dbRiwayat[totalRiwayat].idKaryawan = dbKaryawan[idKaryawanLogin].id;
                             strcpy(dbRiwayat[totalRiwayat].aktivitas, "Mengangkat Barang (Restock)"); dbRiwayat[totalRiwayat].poin = 1;
                             getTodayDate(dbRiwayat[totalRiwayat].tanggal); totalRiwayat++;
                             saveAllData(); showErrorAndWait(fx+2, fy+8, "Stok Ditambah (+1 Poin)!");
                         }
                     }
                 }
                 drawBaseLayout("GUDANG > RESTOCK"); drawHomeLogo(currentRole);
            }
            if(selected==1) { isViewingStock = 1; stockPage = 0; }
        }
    }
}

void menuTransaksiGudang(int idKaryawanLogin) {
    int currentRole = dbKaryawan[idKaryawanLogin].roleId;
    int selected = 0; const char *menuItems[] = {"Pembelian Baru", "Riwayat Pembelian", "Kembali"};
    int tx = getCenterXForTable(95); int isHistoryMode = 0; int historyCursor = 0; int historyPage = 0; int hideId = 0;
    drawBaseLayout("GUDANG > TRANSAKSI"); drawHomeLogo(currentRole);

    while(1) {
        int startY = HEADER_HEIGHT + 6; for(int i=0; i<3; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
        if(isHistoryMode) {
            clearRightContent();
            int uniqueIndices[MAX_DATA]; int uniqueCount = 0; char lastFaktur[20] = "";
            for(int i=0; i<totalPembelian; i++) { if(strcmp(dbPembelian[i].noFaktur, lastFaktur) != 0) { uniqueIndices[uniqueCount++] = i; strcpy(lastFaktur, dbPembelian[i].noFaktur); } }
            drawTableBox(tx-1, startY-1, 95, 13); textNormal();
            gotoxy(tx, startY); printf(" %-5s %c %-20s %c %-15s %c %-40s ", "NO", 186, "NO FAKTUR", 186, "TANGGAL", 186, "SUPPLIER");
            gotoxy(tx, startY+1); for(int i=0; i<93; i++) printf("%c", 205);
            int totalPages = (int)ceil((double)uniqueCount / ROWS_PER_PAGE); if(totalPages == 0) totalPages = 1;
            int start = historyPage * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > uniqueCount) ? uniqueCount : start + ROWS_PER_PAGE;
            for(int i=start; i<end; i++) {
                int dataIdx = uniqueIndices[i]; char noStr[10]; if (hideId) strcpy(noStr, "***"); else sprintf(noStr, "%03d", i+1);
                char supName[40] = "Unknown"; for(int s=0; s<totalSupplier; s++) if(dbSupplier[s].id == dbPembelian[dataIdx].idSupplier) strcpy(supName, dbSupplier[s].nama);
                int rowY = startY+2+(i-start); gotoxy(tx, rowY); if(i == historyCursor) textHighlightTheme(); else textNormal();
                printf(" %-5s %c %-20s %c %-15s %c %-40s ", noStr, 186, dbPembelian[dataIdx].noFaktur, 186, dbPembelian[dataIdx].tanggal, 186, supName); textNormal();
            }
            drawNavigationLegend("[PANAH] Pilih | [ENTER] Detail | [TAB] Hide No | [ESC] Menu");
            int key = getch(); if(key == 224) { key = getch(); if(key == KEY_UP && historyCursor > 0) { historyCursor--; if(historyCursor < start) historyPage--; } if(key == KEY_DOWN && historyCursor < uniqueCount - 1) { historyCursor++; if(historyCursor >= end) historyPage++; } }
            else if(key == 13 && uniqueCount > 0) { int dataIdx = uniqueIndices[historyCursor]; viewPurchaseDetail(dbPembelian[dataIdx].noFaktur); drawBaseLayout("GUDANG > TRANSAKSI"); drawHomeLogo(currentRole); }
            else if(key == KEY_TAB) { hideId = !hideId; } else if(key == 27) { isHistoryMode = 0; clearRightContent(); drawHomeLogo(currentRole); }
            continue;
        }
        else { drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka"); }
        int key = getch(); if(key==224) { key=getch(); if(key==KEY_UP) selected=(selected>0)?selected-1:2; if(key==KEY_DOWN) selected=(selected<2)?selected+1:0; }
        else if(key==13) {
            if(selected==2) break; if(selected==1) { isHistoryMode = 1; historyCursor = 0; historyPage = 0; }
            if(selected==0) {
                int supId = selectSupplierOverlay();
                if(supId != -1) {
                    char sName[50] = "Unknown"; for(int i=0; i<totalSupplier; i++) if(dbSupplier[i].id == supId) strcpy(sName, dbSupplier[i].nama);
                    cartCount = 0; long grandTotal = 0; char faktur[20], tgl[20];
                    int uniquePO = 0; char lastF[20]=""; for(int k=0; k<totalPembelian; k++) { if(strcmp(dbPembelian[k].noFaktur, lastF)!=0) { uniquePO++; strcpy(lastF, dbPembelian[k].noFaktur); } }
                    sprintf(faktur, "PO/%04d", uniquePO + 1); getTodayDate(tgl);

                    while(1) {
                        drawBaseLayout("GUDANG > PEMBELIAN"); int fx=SIDEBAR_WIDTH+2, fy=HEADER_HEIGHT+2;
                        gotoxy(fx, fy); printf("FAKTUR: %s | %s", faktur, tgl); gotoxy(fx, fy+1); printf("SUPPLIER: %s", sName);
                        gotoxy(fx, fy+3); printf("KERANJANG BELANJA:");

                        // UPDATE UI: Menampilkan Harga Satuan
                        for(int i=0; i<cartCount; i++) {
                            char pn[30], rpUnit[20], rpSub[20];
                            for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==cart[i].pid) strcpy(pn, dbProduk[p].nama);
                            formatRupiah(cart[i].price, rpUnit);
                            formatRupiah(cart[i].subtotal, rpSub);
                            gotoxy(fx, fy+4+i); printf("%d. %-15s @%s x%d = %s", i+1, pn, rpUnit, cart[i].qty, rpSub);
                        }

                        char gtStr[30]; formatRupiah(grandTotal, gtStr); gotoxy(fx, fy+16); printf("TOTAL BELANJA: Rp %s", gtStr);
                        drawNavigationLegend("[T] Tambah Item | [P] Proses Pembelian | [ESC] Batal");
                        int cmd = getch(); if(cmd == 27) { if(getConfirmation(fx, fy+18, "Batalkan Pembelian?")) break; }

                        if(tolower(cmd) == 't') {
                             int pid = selectProductOverlay();
                             if(pid != -1) {
                                 int pidx = -1; for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == pid) pidx = i;
                                 if(pidx != -1) {
                                     clearRightContent(); int bx, by, bw, bh; drawFormBox("INPUT ITEM", &bx, &by, &bw, &bh);
                                     gotoxy(bx+2, by+2); printf("Produk : %s", dbProduk[pidx].nama); gotoxy(bx+2, by+4); printf("Jumlah : "); int qty = (int)getValidatedNumber(bx+11, by+4);
                                     gotoxy(bx+2, by+6); printf("Hrg Beli/Pcs : Rp "); long price = getValidatedNumber(bx+18, by+6);
                                     if(qty > 0 && price > 0) { cart[cartCount].pid = pid; cart[cartCount].qty = qty; cart[cartCount].price = price; cart[cartCount].subtotal = qty * price; grandTotal += cart[cartCount].subtotal; cartCount++; }
                                 }
                             }
                        }
                        if(tolower(cmd) == 'p' && cartCount > 0) {
                            if(getConfirmation(fx, fy+18, "Simpan Transaksi")) {
                                for(int i=0; i<cartCount; i++) {
                                    dbPembelian[totalPembelian].id = totalPembelian + 1; strcpy(dbPembelian[totalPembelian].noFaktur, faktur); strcpy(dbPembelian[totalPembelian].tanggal, tgl); dbPembelian[totalPembelian].idKaryawan = idKaryawanLogin; dbPembelian[totalPembelian].idSupplier = supId; dbPembelian[totalPembelian].idProduk = cart[i].pid; dbPembelian[totalPembelian].jumlah = cart[i].qty; dbPembelian[totalPembelian].totalHarga = cart[i].subtotal;
                                    // AUTO PRICING & STOCK UPDATE
                                    for(int p=0; p<totalProduk; p++) {
                                        if(dbProduk[p].id == cart[i].pid) {
                                            dbProduk[p].stok += cart[i].qty;
                                            double sellingPrice = cart[i].price + (cart[i].price * 0.10) + (cart[i].price * 0.005);
                                            dbProduk[p].harga = (long)sellingPrice;
                                        }
                                    }
                                    totalPembelian++;
                                }
                                // +POIN GUDANG (TRANSAKSI)
                                dbKaryawan[idKaryawanLogin].performa += 1; // +1 Transaksi
                                dbRiwayat[totalRiwayat].id = totalRiwayat+1; dbRiwayat[totalRiwayat].idKaryawan = dbKaryawan[idKaryawanLogin].id;
                                strcpy(dbRiwayat[totalRiwayat].aktivitas, "Melakukan Transaksi (Gudang)"); dbRiwayat[totalRiwayat].poin = 1;
                                strcpy(dbRiwayat[totalRiwayat].tanggal, tgl); totalRiwayat++;

                                saveAllData(); showErrorAndWait(fx, fy+20, "Pembelian Berhasil Disimpan (+1 Poin)!"); break;
                            }
                        }
                    }
                }
                drawBaseLayout("GUDANG > TRANSAKSI"); drawHomeLogo(currentRole);
            }
        }
    }
}