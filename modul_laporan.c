#include "controllers.h"
#include "globals.h"
#include "ui.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void menuLaporan() {
    int selected = 0; const char *menuItems[] = {"Laporan Penjualan", "Laporan Pembelian", "Kembali"};
    drawBaseLayout("PUSAT LAPORAN"); drawHomeLogo(ROLE_ADMIN);
    while(1) {
        for(int i=0; i<3; i++) printMenuItem(4, HEADER_HEIGHT+6+(i*2), (char*)menuItems[i], (i==selected));
        int key = getch();
        if(key == 224) { key = getch(); if(key==KEY_UP) selected = (selected>0)?selected-1:2; if(key==KEY_DOWN) selected = (selected<2)?selected+1:0; }
        else if(key == 13) {
            if(selected == 2) break;

            // Filter Bulan & Tahun
            int bln, thn; int fx = SIDEBAR_WIDTH+5, fy=HEADER_HEIGHT+5; int bw, bh;
            drawFormBox(selected==0 ? "FILTER LAP. JUAL" : "FILTER LAP. BELI", &fx, &fy, &bw, &bh);
            gotoxy(fx+2, fy+2); printf("Bulan (1-12) : "); bln = (int)getValidatedNumber(fx+18, fy+2);
            gotoxy(fx+2, fy+4); printf("Tahun (YYYY) : "); thn = (int)getValidatedNumber(fx+18, fy+4);

            clearRightContent();
            int tx = getCenterXForTable(95); int startY = HEADER_HEIGHT+3;

            // Header Laporan
            char headerText[50]; sprintf(headerText, "LAPORAN PERIODE %02d/%d", bln, thn);
            gotoxy(tx, startY-1); printf("=== %s ===", headerText);

            // LOGIKA CONSOLIDATION (Mengumpulkan Invoice Unik)
            int uniqueIndices[MAX_DATA];
            int uniqueCount = 0;
            char seenFaktur[MAX_DATA][20]; // Array untuk menyimpan faktur yg sudah dicek
            int seenCount = 0;

            long grandTotalPeriod = 0;

            if(selected == 0) { // PENJUALAN
                for(int i=0; i<totalPenjualan; i++) {
                    int d, m, y; sscanf(dbPenjualan[i].tanggal, "%d/%d/%d", &d, &m, &y);
                    if(m==bln && y==thn) {
                        // Cek apakah faktur ini sudah ada di list
                        int exists = 0;
                        for(int s=0; s<seenCount; s++) {
                            if(strcmp(seenFaktur[s], dbPenjualan[i].noFaktur) == 0) {
                                exists = 1; break;
                            }
                        }
                        if(!exists) {
                            strcpy(seenFaktur[seenCount++], dbPenjualan[i].noFaktur);
                            uniqueIndices[uniqueCount++] = i; // Simpan index baris pertama invoice ini
                            grandTotalPeriod += dbPenjualan[i].totalBayar; // totalBayar di struct penjualan sudah GrandTotal Invoice
                        }
                    }
                }
            } else { // PEMBELIAN
                // Khusus pembelian, totalHarga per row adalah subtotal. Kita harus hitung GrandTotal per Invoice manual.
                // Step 1: Kumpulkan faktur unik
                for(int i=0; i<totalPembelian; i++) {
                     int d, m, y; sscanf(dbPembelian[i].tanggal, "%d/%d/%d", &d, &m, &y);
                     if(m==bln && y==thn) {
                        int exists = 0;
                        for(int s=0; s<seenCount; s++) {
                            if(strcmp(seenFaktur[s], dbPembelian[i].noFaktur) == 0) {
                                exists = 1; break;
                            }
                        }
                        if(!exists) {
                            strcpy(seenFaktur[seenCount++], dbPembelian[i].noFaktur);
                            uniqueIndices[uniqueCount++] = i;
                        }
                     }
                }
                // Hitung Grand Total Periode
                for(int i=0; i<totalPembelian; i++) {
                    int d, m, y; sscanf(dbPembelian[i].tanggal, "%d/%d/%d", &d, &m, &y);
                    if(m==bln && y==thn) {
                        grandTotalPeriod += dbPembelian[i].totalHarga;
                    }
                }
            }

            // NAVIGATION VARS
            int cursor = 0; int page = 0;

            while(1) {
                drawTableBox(tx-1, startY, 97, 14);

                if(selected == 0) {
                     gotoxy(tx, startY+1); printf(" %-12s %-20s %-30s %-20s ", "TANGGAL", "NO FAKTUR", "KASIR", "TOTAL INVOICE");
                } else {
                     gotoxy(tx, startY+1); printf(" %-12s %-20s %-30s %-20s ", "TANGGAL", "NO FAKTUR", "SUPPLIER", "TOTAL INVOICE");
                }
                gotoxy(tx, startY+2); for(int i=0; i<95; i++) printf("-");

                int totalPages = (int)ceil((double)uniqueCount / ROWS_PER_PAGE); if(totalPages == 0) totalPages = 1;
                int start = page * ROWS_PER_PAGE; int end = (start + ROWS_PER_PAGE > uniqueCount) ? uniqueCount : start + ROWS_PER_PAGE;

                for(int i = start; i < end; i++) {
                    int idx = uniqueIndices[i]; // Ini index dari array DB asli
                    int rowY = startY + 3 + (i - start);
                    gotoxy(tx, rowY);
                    if(i == cursor) textHighlightTheme(); else textNormal();

                    if(selected == 0) { // JUAL
                        char rp[30]; formatRupiah(dbPenjualan[idx].totalBayar, rp);
                        printf(" %-12s %-20s %-30s Rp %17s ", dbPenjualan[idx].tanggal, dbPenjualan[idx].noFaktur, dbPenjualan[idx].namaKasir, rp);
                    } else { // BELI
                        // Hitung total invoice khusus faktur ini
                        long invoiceTotal = 0;
                        char supName[31] = "Unknown";
                        // Cari nama supplier
                        for(int s=0; s<totalSupplier; s++) if(dbSupplier[s].id == dbPembelian[idx].idSupplier) strcpy(supName, dbSupplier[s].nama);

                        // Sum totalHarga item di invoice ini
                        for(int b=0; b<totalPembelian; b++) {
                            if(strcmp(dbPembelian[b].noFaktur, dbPembelian[idx].noFaktur) == 0) {
                                invoiceTotal += dbPembelian[b].totalHarga;
                            }
                        }
                        char rp[30]; formatRupiah(invoiceTotal, rp);
                        printf(" %-12s %-20s %-30s Rp %17s ", dbPembelian[idx].tanggal, dbPembelian[idx].noFaktur, supName, rp);
                    }
                    textNormal();
                }

                char totalRp[50]; formatRupiah(grandTotalPeriod, totalRp);
                gotoxy(tx, startY+13); printf(" GRAND TOTAL PERIODE: Rp %s", totalRp);
                drawNavigationLegend("[PANAH] Scroll | [ENTER] Detail | [ESC] Kembali");

                int navKey = getch();
                if(navKey == 224) {
                    navKey = getch();
                    if(navKey == KEY_UP && cursor > 0) { cursor--; if(cursor < start) page--; }
                    if(navKey == KEY_DOWN && cursor < uniqueCount - 1) { cursor++; if(cursor >= end) page++; }
                }
                else if(navKey == 13 && uniqueCount > 0) {
                    // VIEW DETAIL
                    int idx = uniqueIndices[cursor];
                    if(selected == 0) viewInvoiceDetail(dbPenjualan[idx].noFaktur);
                    else viewPurchaseDetail(dbPembelian[idx].noFaktur);

                    // Redraw Report Base
                    clearRightContent();
                    drawBaseLayout("PUSAT LAPORAN"); drawHomeLogo(ROLE_ADMIN);
                    gotoxy(tx, startY-1); printf("=== %s ===", headerText);
                }
                else if(navKey == 27) break;
            }

            drawBaseLayout("PUSAT LAPORAN"); drawHomeLogo(ROLE_ADMIN);
        }
    }
}