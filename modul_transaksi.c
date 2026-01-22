#include "controllers.h"
#include "globals.h"
#include "ui.h"
#include <math.h>

// Struktur Keranjang Sementara
typedef struct {
    int pid;
    int qty;
    long subtotal;
} CartItem;

CartItem cart[100];
int cartCount = 0;

// --- TRANSAKSI PENJUALAN ---

void displayPenjualanTable(int tableX) {
    int startY = HEADER_HEIGHT + 4;
    drawTableBox(tableX-1, startY-1, 95+2, 13);
    textNormal();
    gotoxy(tableX, startY); printf(" %-15s %c %-10s %c %-30s %c %-15s ", "NO FAKTUR", 186, "TANGGAL", 186, "PRODUK", 186, "TOTAL (Rp)");
    gotoxy(tableX, startY+1); for(int i=0; i<95; i++) printf("%c", 205);

    int start = pageOffset * ROWS_PER_PAGE;
    int end = (start + ROWS_PER_PAGE > totalPenjualan) ? totalPenjualan : start + ROWS_PER_PAGE;

    for(int i=start; i<end; i++) {
        int r = startY+2+(i-start);
        char pname[31]="Unknown";
        char rp[30];

        for(int p=0; p<totalProduk; p++) if(dbProduk[p].id == dbPenjualan[i].idProduk) strcpy(pname, dbProduk[p].nama);
        formatRupiah(dbPenjualan[i].totalHarga, rp);

        gotoxy(tableX, r);
        printf(" %-15s %c %-10s %c %-30s %c Rp %-12s ",
             dbPenjualan[i].noFaktur, 186, dbPenjualan[i].tanggal, 186, pname, 186, rp);
    }
}

void crudPenjualan(int idKaryawanLogin) {
    int selected = 0;
    const char *menuItems[] = {"Transaksi Baru", "Riwayat Jual", "Kembali"};
    int tx = getCenterXForTable(95);
    int isPagingMode = 0; pageOffset=0;

    drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo();

    while(1) {
        int startY = HEADER_HEIGHT + 6;
        for(int i=0; i<3; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));

        if(isPagingMode) {
            clearRightContent();
            displayPenjualanTable(tx);
            drawNavigationLegend("[PANAH] Scroll | [ESC] Menu");
        } else {
            drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");
        }

        int key=getch();
        if(key == 224) {
            key=getch();
            if(isPagingMode) {
                // Logic Scroll Simple
            } else {
                 if(key==KEY_UP) selected=(selected>0)?selected-1:2;
                 if(key==KEY_DOWN) selected=(selected<2)?selected+1:0;
            }
        }
        else if(key == 13 && !isPagingMode) {
            if(selected == 2) break; // Kembali
            if(selected == 1) { isPagingMode = 1; pageOffset = 0; }
            if(selected == 0) { // Transaksi Baru
                cartCount = 0;
                long grandTotal = 0;
                char faktur[20], tgl[20];
                sprintf(faktur, "INV/%04d", totalPenjualan+1);
                getTodayDate(tgl);

                int fx, fy, bw, bh;
                while(1) {
                    drawFormBox("KERANJANG BELANJA", &fx, &fy, &bw, &bh);
                    gotoxy(fx+2, fy+1); printf("Faktur: %s | Tgl: %s", faktur, tgl);
                    gotoxy(fx+2, fy+2); printf("Kasir : %s", dbKaryawan[idKaryawanLogin].nama);
                    gotoxy(fx+2, fy+4); printf("%-30s %-5s %-15s", "PRODUK", "QTY", "SUBTOTAL");
                    gotoxy(fx+2, fy+5); for(int i=0; i<65; i++) printf("-");

                    for(int i=0; i<cartCount; i++) {
                        char pn[31]; char subRp[30];
                        for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==cart[i].pid) strcpy(pn, dbProduk[p].nama);
                        formatRupiah(cart[i].subtotal, subRp);
                        gotoxy(fx+2, fy+6+i);
                        printf("%-30s %-5d %15s", pn, cart[i].qty, subRp);
                    }

                    char totalRp[30]; formatRupiah(grandTotal, totalRp);
                    gotoxy(fx+2, fy+13); printf("TOTAL BAYAR: Rp. %s", totalRp);
                    drawNavigationLegend("[T] Tambah Item | [ENTER] Bayar | [ESC] Batal");

                    key=getch();
                    if(key==27) break;
                    if(key==13 && cartCount > 0) {
                        if(getConfirmation(fx+2, fy+14, "Proses Transaksi")) {
                            for(int i=0; i<cartCount; i++) {
                                dbPenjualan[totalPenjualan].id = totalPenjualan+1;
                                strcpy(dbPenjualan[totalPenjualan].noFaktur, faktur);
                                strcpy(dbPenjualan[totalPenjualan].tanggal, tgl);
                                dbPenjualan[totalPenjualan].idKaryawan = dbKaryawan[idKaryawanLogin].id;
                                strcpy(dbPenjualan[totalPenjualan].namaKasir, dbKaryawan[idKaryawanLogin].nama);
                                dbPenjualan[totalPenjualan].idProduk = cart[i].pid;
                                dbPenjualan[totalPenjualan].jumlah = cart[i].qty;
                                dbPenjualan[totalPenjualan].totalHarga = cart[i].subtotal;
                                dbPenjualan[totalPenjualan].totalBayar = grandTotal; // Header info

                                for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==cart[i].pid) dbProduk[p].stok -= cart[i].qty;
                                totalPenjualan++;
                            }
                            showErrorAndWait(fx+2, fy+14, "Transaksi Berhasil!");
                            break;
                        }
                    }
                    if(tolower(key)=='t' && cartCount < 6) {
                        gotoxy(fx+2, fy+14); printf("ID Produk: ");
                        int pid = (int)getValidatedNumber(fx+13, fy+14);
                        int pidx = -1;
                        for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==pid) pidx=p;

                        if(pidx != -1) {
                            gotoxy(fx+2, fy+15);
                            printf(">> %s | Stok:%d", dbProduk[pidx].nama, dbProduk[pidx].stok);
                            gotoxy(fx+45, fy+15); printf("| Qty: ");
                            int qty = (int)getValidatedNumber(fx+52, fy+15);

                            if(qty > 0 && qty <= dbProduk[pidx].stok) {
                                cart[cartCount].pid = pid;
                                cart[cartCount].qty = qty;
                                cart[cartCount].subtotal = qty * dbProduk[pidx].harga;
                                grandTotal += cart[cartCount].subtotal;
                                cartCount++;
                            } else showErrorAndWait(fx+45, fy+15, "Stok Kurang!");
                        } else showErrorAndWait(fx+25, fy+14, "ID Salah!");
                    }
                }
                drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo();
            }
        }
        else if(key == 27) {
             if(isPagingMode) { isPagingMode = 0; clearRightContent(); drawHomeLogo(); }
             else break;
        }
    }
}

// --- PEMBELIAN / RESTOCK ---

void displayPembelianTable(int tableX) {
    int startY = HEADER_HEIGHT + 4;
    drawTableBox(tableX - 1, startY - 1, 95, 13);
    textNormal();
    gotoxy(tableX, startY);
    printf(" %-12s %c %-25s %c %-10s %c %-15s ", "TANGGAL", 186, "ITEM", 186, "QTY", 186, "TOTAL (Rp)");
    gotoxy(tableX, startY + 1); for(int i=0; i<93; i++) printf("%c", 205);

    int start = pageOffset * ROWS_PER_PAGE;
    int end = (start + ROWS_PER_PAGE > totalPembelian) ? totalPembelian : start + ROWS_PER_PAGE;

    for(int i=start; i<end; i++) {
        char rp[30], pname[30]="Unknown";
        formatRupiah(dbPembelian[i].totalHarga, rp);
        for(int p=0; p<totalProduk; p++) if(dbProduk[p].id == dbPembelian[i].idProduk) strcpy(pname, dbProduk[p].nama);

        gotoxy(tableX, startY+2+(i-start));
        printf(" %-12s %c %-25s %c %-10d %c Rp %-12s ",
            dbPembelian[i].tanggal, 186, pname, 186, dbPembelian[i].jumlah, 186, rp);
    }
}

void crudPembelian(int idKaryawanLogin) {
    int selected = 0;
    const char *menuItems[] = {"Input Restock", "Riwayat Beli", "Kembali"};
    int tableX = getCenterXForTable(95);
    int isTableMode = 0; pageOffset = 0;

    drawBaseLayout("GUDANG > RESTOCK"); drawHomeLogo();

    while(1) {
        for(int i=0; i<3; i++) printMenuItem(4, HEADER_HEIGHT+6+(i*2), (char*)menuItems[i], (i==selected));

        if(isTableMode) {
            clearRightContent();
            displayPembelianTable(tableX);
            drawNavigationLegend("[ESC] Menu Utama");
        }

        int key = getch();
        if(key == 224) {
            key = getch();
            if(!isTableMode) {
                if(key==KEY_UP) selected = (selected>0)?selected-1:2;
                if(key==KEY_DOWN) selected = (selected<2)?selected+1:0;
            }
        }
        else if(key == 13 && !isTableMode) {
            if(selected == 2) break;
            if(selected == 1) { isTableMode = 1; pageOffset=0; }
            if(selected == 0) {
                // INPUT RESTOCK
                int fx, fy, bw, bh;
                drawFormBox("FORM RESTOCK BARANG", &fx, &fy, &bw, &bh);

                char tgl[20];
                getTodayDate(tgl);

                gotoxy(fx+2, fy+2); printf("Tanggal : %s", tgl);
                gotoxy(fx+2, fy+4); printf("ID Produk : ");
                int pid = (int)getValidatedNumber(fx+14, fy+4);

                int pidx = -1;
                for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == pid) pidx=i;

                if(pidx != -1) {
                    gotoxy(fx+2, fy+6); printf("Nama : %s", dbProduk[pidx].nama);
                    gotoxy(fx+2, fy+7); printf("Stok Saat Ini : %d", dbProduk[pidx].stok);

                    gotoxy(fx+2, fy+9); printf("Jml Masuk : ");
                    int qty = (int)getValidatedNumber(fx+14, fy+9);

                    gotoxy(fx+2, fy+11); printf("Biaya Total : ");
                    long cost = getValidatedNumber(fx+16, fy+11);

                    if(getConfirmation(fx+2, fy+13, "Simpan Data")) {
                        dbPembelian[totalPembelian].id = totalPembelian+1;
                        strcpy(dbPembelian[totalPembelian].tanggal, tgl);
                        dbPembelian[totalPembelian].idProduk = pid;
                        dbPembelian[totalPembelian].jumlah = qty;
                        dbPembelian[totalPembelian].totalHarga = cost;
                        dbPembelian[totalPembelian].idKaryawan = dbKaryawan[idKaryawanLogin].id;

                        dbProduk[pidx].stok += qty; // Update Stok
                        totalPembelian++;
                        showErrorAndWait(fx+2, fy+15, "Stok Berhasil Ditambah!");
                    }
                } else showErrorAndWait(fx+2, fy+6, "Produk Tidak Ditemukan!");
                drawBaseLayout("GUDANG > RESTOCK"); drawHomeLogo();
            }
        }
        else if(key == 27) {
             if(isTableMode) { isTableMode = 0; clearRightContent(); drawHomeLogo(); }
             else break;
        }
    }
}