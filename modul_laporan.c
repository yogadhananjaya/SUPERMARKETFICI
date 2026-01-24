#include "controllers.h"
#include "globals.h"
#include "ui.h"

void menuLaporan() {
    int selected = 0; const char *menuItems[] = {"Laporan Penjualan", "Laporan Pembelian", "Kembali"};
    drawBaseLayout("PUSAT LAPORAN"); drawHomeLogo(ROLE_ADMIN); // FIX
    while(1) {
        for(int i=0; i<3; i++) printMenuItem(4, HEADER_HEIGHT+6+(i*2), (char*)menuItems[i], (i==selected));
        int key = getch();
        if(key == 224) { key = getch(); if(key==KEY_UP) selected = (selected>0)?selected-1:2; if(key==KEY_DOWN) selected = (selected<2)?selected+1:0; }
        else if(key == 13) {
            if(selected == 2) break;
            int bln, thn; int fx = SIDEBAR_WIDTH+5, fy=HEADER_HEIGHT+5; int bw, bh;
            drawFormBox(selected==0 ? "FILTER LAP. JUAL" : "FILTER LAP. BELI", &fx, &fy, &bw, &bh);
            gotoxy(fx+2, fy+2); printf("Bulan (1-12) : "); bln = (int)getValidatedNumber(fx+18, fy+2);
            gotoxy(fx+2, fy+4); printf("Tahun (YYYY) : "); thn = (int)getValidatedNumber(fx+18, fy+4);
            clearRightContent(); int tx = getCenterXForTable(90); int startY = HEADER_HEIGHT+3; long grandTotal = 0; int row = 0;
            drawTableBox(tx-1, startY, 92, 14);
            if(selected == 0) {
                 gotoxy(tx, startY+1); printf(" %-12s %-15s %-30s %-15s ", "TANGGAL", "NO FAKTUR", "ITEM", "TOTAL");
                 gotoxy(tx, startY+2); for(int i=0; i<90; i++) printf("-");
                 for(int i=0; i<totalPenjualan; i++) {
                      int d, m, y; sscanf(dbPenjualan[i].tanggal, "%d/%d/%d", &d, &m, &y);
                      if(m==bln && y==thn) {
                           char rp[30], pn[30]; formatRupiah(dbPenjualan[i].totalHarga, rp); strcpy(pn, "Item Hapus"); for(int p=0; p<totalProduk; p++) if(dbProduk[p].id == dbPenjualan[i].idProduk) strcpy(pn, dbProduk[p].nama);
                           gotoxy(tx, startY+3+row); printf(" %-12s %-15s %-30s Rp %12s ", dbPenjualan[i].tanggal, dbPenjualan[i].noFaktur, pn, rp);
                           grandTotal += dbPenjualan[i].totalHarga; row++; if(row >= 10) break;
                      }
                 }
            } else {
                 gotoxy(tx, startY+1); printf(" %-12s %-30s %-10s %-15s ", "TANGGAL", "ITEM", "QTY", "BIAYA");
                 gotoxy(tx, startY+2); for(int i=0; i<90; i++) printf("-");
                 for(int i=0; i<totalPembelian; i++) {
                      int d, m, y; sscanf(dbPembelian[i].tanggal, "%d/%d/%d", &d, &m, &y);
                      if(m==bln && y==thn) {
                           char rp[30], pn[30]; formatRupiah(dbPembelian[i].totalHarga, rp); strcpy(pn, "Unknown"); for(int p=0; p<totalProduk; p++) if(dbProduk[p].id == dbPembelian[i].idProduk) strcpy(pn, dbProduk[p].nama);
                           gotoxy(tx, startY+3+row); printf(" %-12s %-30s %-10d Rp %12s ", dbPembelian[i].tanggal, pn, dbPembelian[i].jumlah, rp);
                           grandTotal += dbPembelian[i].totalHarga; row++; if(row >= 10) break;
                      }
                 }
            }
            char totalRp[50]; formatRupiah(grandTotal, totalRp);
            gotoxy(tx, startY+13); printf(" TOTAL %s: Rp %s", selected==0?"PENDAPATAN":"PENGELUARAN", totalRp);
            drawNavigationLegend("[ESC] Kembali"); while(getch() != 27);
            drawBaseLayout("PUSAT LAPORAN"); drawHomeLogo(ROLE_ADMIN); // FIX
        }
    }
}