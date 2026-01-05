#include "controllers.h"
#include "globals.h"
#include "ui.h"

void menuLaporan() {
    int bln, thn;
    drawBaseLayout("LAPORAN BULANAN");
    int fx = SIDEBAR_WIDTH+5, fy=HEADER_HEIGHT+5;

    gotoxy(fx, fy);   printf("=== FILTER LAPORAN ===");
    gotoxy(fx, fy+2); printf("Bulan (1-12) : "); bln = (int)getValidatedNumber(fx+16, fy+2);
    gotoxy(fx, fy+4); printf("Tahun (YYYY) : "); thn = (int)getValidatedNumber(fx+16, fy+4);

    clearRightContent();
    int tx = getCenterXForTable(90);
    int startY = HEADER_HEIGHT+3;

    drawTableBox(tx-1, startY, 92, 14);
    gotoxy(tx, startY+1); printf(" %-12s %-15s %-30s %-10s %-15s ", "TANGGAL", "FAKTUR", "PRODUK", "QTY", "TOTAL");
    gotoxy(tx, startY+2); for(int i=0; i<90; i++) printf("-");

    long totalOmset = 0;
    int row = 0;

    for(int i=0; i<totalPenjualan; i++) {
        int d, m, y;
        sscanf(dbPenjualan[i].tanggal, "%d/%d/%d", &d, &m, &y);

        if(m == bln && y == thn) {
            char pn[31];
            for(int p=0; p<totalProduk; p++) if(dbProduk[p].id == dbPenjualan[i].idProduk) strcpy(pn, dbProduk[p].nama);

            gotoxy(tx, startY+3+row);
            printf(" %-12s %-15s %-30s %-10d %15ld ",
                dbPenjualan[i].tanggal, dbPenjualan[i].noFaktur, pn, dbPenjualan[i].jumlah, dbPenjualan[i].totalHarga);

            totalOmset += dbPenjualan[i].totalHarga;
            row++;
            if(row >= 9) break; // Limit tampilan sederhana
        }
    }

    if(row == 0) {
        gotoxy(tx+5, startY+5); printf(">> Tidak ada data pada periode ini.");
    }

    gotoxy(tx, startY+12); printf("------------------------------------------------------------------------------------------");
    gotoxy(tx, startY+13); printf(" TOTAL OMSET BULAN INI: Rp. %ld", totalOmset);

    drawNavigationLegend("Tekan [ESC] Kembali");
    while(getch() != KEY_ESC);
}