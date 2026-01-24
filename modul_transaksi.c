#include "controllers.h"
#include "globals.h"
#include "ui.h"
#include <math.h>

typedef struct { int pid; int qty; long subtotal; } CartItem;
CartItem cart[100];
int cartCount = 0;

int selectProductOverlay() {
    int tableX = getCenterXForTable(95);
    int startY = HEADER_HEIGHT + 2;
    int selectedIndex = 0; int page = 0; int hideId = 1;
    clearRightContent();

    while(1) {
        int totalPages = (int)ceil((double)totalProduk / ROWS_PER_PAGE);
        int start = page * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > totalProduk) ? totalProduk : start + ROWS_PER_PAGE;
        drawTableBox(tableX - 1, startY - 1, 95 + 2, 13);
        textNormal(); gotoxy(tableX, startY); printf(" %-5s %c %-40s %c %-10s %c %-20s ", "ID", 186, "NAMA PRODUK", 186, "STOK", 186, "HARGA (Rp)");
        gotoxy(tableX, startY + 1); for(int i=0; i<95; i++) printf("%c", 205);

        for(int i = start; i < end; i++) {
            char rp[30], idStr[10]; formatRupiah(dbProduk[i].harga, rp);
            if(hideId) strcpy(idStr, "***"); else sprintf(idStr, "%03d", dbProduk[i].id);
            int rowY = startY + 2 + (i - start); gotoxy(tableX, rowY);
            if(i == selectedIndex) textHighlightTheme(); else textNormal();
            printf(" %-5s %c %-40s %c %-10d %c Rp %-17s ", idStr, 186, dbProduk[i].nama, 186, dbProduk[i].stok, 186, rp); textNormal();
        }
        drawNavigationLegend("[PANAH] Pilih | [TAB] Hide ID | [ENTER] Pilih | [ESC] Batal");
        int key = getch();
        if(key == 224) { key = getch(); if(key == KEY_UP && selectedIndex > 0) { selectedIndex--; if(selectedIndex < start) page--; } if(key == KEY_DOWN && selectedIndex < totalProduk - 1) { selectedIndex++; if(selectedIndex >= end) page++; } }
        else if(key == KEY_TAB) { hideId = !hideId; } else if(key == 13) { return dbProduk[selectedIndex].id; } else if(key == 27) { return -1; }
    }
}

void viewInvoiceDetail(const char* targetFaktur) {
    clearRightContent(); int fx, fy, bw, bh;
    char kasirName[50] = "-"; char tgl[20] = "-"; long grandTotal = 0;
    for(int i=0; i<totalPenjualan; i++) { if(strcmp(dbPenjualan[i].noFaktur, targetFaktur) == 0) { strcpy(kasirName, dbPenjualan[i].namaKasir); strcpy(tgl, dbPenjualan[i].tanggal); grandTotal = dbPenjualan[i].totalBayar; break; } }
    drawFormBox("DETAIL TRANSAKSI", &fx, &fy, &bw, &bh); bh = 18; drawShadowBox(fx, fy, bw, bh);
    gotoxy(fx+2, fy+2); printf("No Faktur : %s", targetFaktur); gotoxy(fx+40, fy+2); printf("Tanggal   : %s", tgl); gotoxy(fx+2, fy+3); printf("Kasir     : %s", kasirName);
    int listY = fy + 5; gotoxy(fx+2, listY); printf("%-3s %-30s %-5s %-15s", "NO", "PRODUK", "QTY", "SUBTOTAL"); gotoxy(fx+2, listY+1); for(int i=0; i<60; i++) printf("-");
    int row = 0;
    for(int i=0; i<totalPenjualan; i++) {
        if(strcmp(dbPenjualan[i].noFaktur, targetFaktur) == 0) {
            char pName[31] = "Unknown"; for(int p=0; p<totalProduk; p++) { if(dbProduk[p].id == dbPenjualan[i].idProduk) { strcpy(pName, dbProduk[p].nama); break; } }
            char rpSub[30]; formatRupiah(dbPenjualan[i].totalHarga, rpSub);
            if(row < 10) { gotoxy(fx+2, listY+2+row); printf("%-3d %-30s %-5d Rp %s", row+1, pName, dbPenjualan[i].jumlah, rpSub); } row++;
        }
    }
    char rpTotal[30]; formatRupiah(grandTotal, rpTotal); gotoxy(fx+2, listY+13); for(int i=0; i<60; i++) printf("-"); gotoxy(fx+2, listY+14); printf("TOTAL BAYAR : Rp %s", rpTotal);
    drawNavigationLegend("[ESC] Kembali ke Daftar"); while(getch() != 27);
}

void crudPenjualan(int idKaryawanLogin) {
    int currentRole = dbKaryawan[idKaryawanLogin].roleId; // GET ROLE
    int selected = 0; const char *menuItems[] = {"Transaksi Baru", "Riwayat Jual", "Kembali"};
    int tx = getCenterXForTable(95); int isHistoryMode = 0; int historyCursor = 0; int historyPage = 0; int hideId = 1;
    drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo(currentRole); // FIX

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
                int dataIdx = uniqueIndices[i]; char rp[30], idStr[10]; formatRupiah(dbPenjualan[dataIdx].totalBayar, rp);
                if (hideId) strcpy(idStr, "***"); else sprintf(idStr, "%d", i+1);
                int rowY = startY+2+(i-start); gotoxy(tx, rowY); if(i == historyCursor) textHighlightTheme(); else textNormal();
                printf(" %-5s %c %-20s %c %-15s %c %-20s %c Rp %-12s ", idStr, 186, dbPenjualan[dataIdx].noFaktur, 186, dbPenjualan[dataIdx].tanggal, 186, dbPenjualan[dataIdx].namaKasir, 186, rp); textNormal();
            }
            drawNavigationLegend("[PANAH] Pilih | [ENTER] Detail | [TAB] Hide ID | [ESC] Menu");
            int key = getch();
            if(key == 224) { key = getch(); if(key == KEY_UP && historyCursor > 0) { historyCursor--; if(historyCursor < start) historyPage--; } if(key == KEY_DOWN && historyCursor < uniqueCount - 1) { historyCursor++; if(historyCursor >= end) historyPage++; } }
            else if(key == 13 && uniqueCount > 0) { int dataIdx = uniqueIndices[historyCursor]; viewInvoiceDetail(dbPenjualan[dataIdx].noFaktur); drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo(currentRole); /* FIX */ }
            else if(key == KEY_TAB) { hideId = !hideId; }
            else if(key == 27) { isHistoryMode = 0; clearRightContent(); drawHomeLogo(currentRole); /* FIX */ }
            continue;
        } else { drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka"); }

        int key=getch();
        if(key == 224) { key=getch(); if(!isHistoryMode) { if(key==KEY_UP) selected=(selected>0)?selected-1:2; if(key==KEY_DOWN) selected=(selected<2)?selected+1:0; } }
        else if(key == 13 && !isHistoryMode) {
            if(selected == 2) break;
            if(selected == 1) { isHistoryMode = 1; historyCursor = 0; historyPage = 0; }
            if(selected == 0) {
                cartCount = 0; long grandTotal = 0; char faktur[20], tgl[20];
                int uniqueInv = 0; char lastF[20]="";
                for(int k=0; k<totalPenjualan; k++) { if(strcmp(dbPenjualan[k].noFaktur, lastF)!=0) { uniqueInv++; strcpy(lastF, dbPenjualan[k].noFaktur); } }
                sprintf(faktur, "INV/%04d", uniqueInv + 1); getTodayDate(tgl);
                int pid = selectProductOverlay();
                if(pid == -1) { drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo(currentRole); continue; }
                int pidx = -1; for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == pid) pidx = i;
                if(pidx != -1) {
                     clearRightContent(); int bx, by, bw, bh; drawFormBox("INPUT QUANTITY", &bx, &by, &bw, &bh);
                     gotoxy(bx+2, by+2); printf("Produk : %s", dbProduk[pidx].nama); gotoxy(bx+2, by+6); printf("Jumlah : "); int qty = (int)getValidatedNumber(bx+11, by+6);
                     if(qty > 0 && qty <= dbProduk[pidx].stok) { cart[cartCount].pid = pid; cart[cartCount].qty = qty; cart[cartCount].subtotal = qty * dbProduk[pidx].harga; grandTotal += cart[cartCount].subtotal; cartCount++; } else { showErrorAndWait(bx+2, by+8, "Stok tidak cukup!"); }
                }

                while(1) {
                    drawBaseLayout("KASIR > TRANSAKSI BARU"); int fx=SIDEBAR_WIDTH+2, fy=HEADER_HEIGHT+2;
                    gotoxy(fx, fy); printf("FAKTUR: %s | %s", faktur, tgl); gotoxy(fx, fy+2); printf("KERANJANG BELANJA:");
                    for(int i=0; i<cartCount; i++) { char pn[30]; for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==cart[i].pid) strcpy(pn, dbProduk[p].nama); gotoxy(fx, fy+3+i); printf("%d. %-20s x%d", i+1, pn, cart[i].qty); }
                    char gtStr[30]; formatRupiah(grandTotal, gtStr); gotoxy(fx, fy+15); printf("TOTAL: Rp %s", gtStr);
                    drawNavigationLegend("[T] Tambah Item | [P] Proses Pembayaran | [ESC] Batal");
                    int cmd = getch(); if(cmd == 27) { if(getConfirmation(fx, fy+17, "Batalkan?")) break; }
                    if(tolower(cmd) == 't') {
                        int nextPid = selectProductOverlay();
                        if(nextPid != -1) {
                            int nextIdx = -1; for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == nextPid) nextIdx = i;
                            if(nextIdx != -1) {
                                clearRightContent(); int bx, by, bw, bh; drawFormBox("INPUT QUANTITY", &bx, &by, &bw, &bh); gotoxy(bx+2, by+2); printf("Produk : %s", dbProduk[nextIdx].nama); gotoxy(bx+2, by+6); printf("Jumlah : "); int qty = (int)getValidatedNumber(bx+11, by+6);
                                if(qty > 0 && qty <= dbProduk[nextIdx].stok) { cart[cartCount].pid = nextPid; cart[cartCount].qty = qty; cart[cartCount].subtotal = qty * dbProduk[nextIdx].harga; grandTotal += cart[cartCount].subtotal; cartCount++; } else { showErrorAndWait(bx+2, by+8, "Stok Invalid!"); }
                            }
                        }
                    }
                    if(tolower(cmd) == 'p' && cartCount > 0) {
                        clearRightContent(); int px, py, pw, ph; drawFormBox("PEMBAYARAN", &px, &py, &pw, &ph);
                        char strTotal[30]; formatRupiah(grandTotal, strTotal); gotoxy(px+2, py+2); printf("Total Tagihan : Rp %s", strTotal);
                        long bayar = 0; while(1) { gotoxy(px+2, py+4); printf("Uang Diterima : Rp           "); bayar = getValidatedNumber(px+21, py+4); if(bayar < grandTotal) { showErrorAndWait(px+2, py+6, "Uang Tidak Cukup!"); } else { long kembalian = bayar - grandTotal; char strKembali[30]; formatRupiah(kembalian, strKembali); gotoxy(px+2, py+6); printf("Kembalian     : Rp %s      ", strKembali); getch(); break; } }
                        for(int i=0; i<cartCount; i++) { dbPenjualan[totalPenjualan].id = totalPenjualan+1; strcpy(dbPenjualan[totalPenjualan].noFaktur, faktur); strcpy(dbPenjualan[totalPenjualan].tanggal, tgl); dbPenjualan[totalPenjualan].idKaryawan = idKaryawanLogin; strcpy(dbPenjualan[totalPenjualan].namaKasir, dbKaryawan[idKaryawanLogin].nama); dbPenjualan[totalPenjualan].idProduk = cart[i].pid; dbPenjualan[totalPenjualan].jumlah = cart[i].qty; dbPenjualan[totalPenjualan].totalHarga = cart[i].subtotal; dbPenjualan[totalPenjualan].totalBayar = grandTotal; for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==cart[i].pid) dbProduk[p].stok -= cart[i].qty; totalPenjualan++; }
                        saveAllData(); showErrorAndWait(px+2, py+8, "Transaksi Sukses!"); break;
                    }
                }
                drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo(currentRole); // FIX
            }
        } else if(key == 27) { if(!isHistoryMode) break; }
    }
}

void crudPembelian(int idKaryawanLogin) {
    int currentRole = dbKaryawan[idKaryawanLogin].roleId; // GET ROLE
    int selected = 0; const char *menuItems[] = {"Input Restock", "Riwayat Beli", "Kembali"};
    drawBaseLayout("GUDANG > RESTOCK"); drawHomeLogo(currentRole); // FIX
    while(1) {
        for(int i=0; i<3; i++) printMenuItem(4, HEADER_HEIGHT+6+(i*2), (char*)menuItems[i], (i==selected));
        int key = getch();
        if(key==224) { key=getch(); if(key==KEY_UP) selected=(selected>0)?selected-1:2; if(key==KEY_DOWN) selected=(selected<2)?selected+1:0; }
        else if(key==13) {
            if(selected==2) break;
            if(selected==0) {
                 int fx, fy, bw, bh; drawFormBox("RESTOCK", &fx, &fy, &bw, &bh);
                 int pid = selectProductOverlay();
                 if(pid != -1) {
                     gotoxy(fx+2, fy+2); printf("ID Produk: %d", pid);
                     gotoxy(fx+2, fy+4); printf("Jumlah: "); int qty = (int)getValidatedNumber(fx+10, fy+4);
                     dbPembelian[totalPembelian].id = totalPembelian+1; dbPembelian[totalPembelian].idProduk = pid; dbPembelian[totalPembelian].jumlah = qty; for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==pid) dbProduk[p].stok += qty; totalPembelian++; saveAllData(); showErrorAndWait(fx+2, fy+6, "Stok Bertambah!");
                 }
                 drawBaseLayout("GUDANG > RESTOCK"); drawHomeLogo(currentRole); // FIX
            }
        }
    }
}