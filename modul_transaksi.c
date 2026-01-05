#include "controllers.h"
#include "globals.h"
#include "ui.h"

// Struktur Keranjang Sementara
typedef struct {
    int pid;
    int qty;
    long subtotal;
} CartItem;

CartItem cart[100];
int cartCount = 0;

void displayPenjualanTable(int tableX) {
    int startY = HEADER_HEIGHT + 4, tableWidth = 95;
    int totalP = (int)ceil((double)totalPenjualan/ROWS_PER_PAGE);
    if(totalP==0) totalP=1;

    drawTableBox(tableX-1, startY-1, tableWidth+2, 13);
    textNormal();
    gotoxy(tableX, startY); printf(" %-15s %c %-10s %c %-30s %c %-15s ", "NO FAKTUR", 186, "TANGGAL", 186, "PRODUK", 186, "TOTAL (Rp)");
    gotoxy(tableX, startY+1); for(int i=0; i<tableWidth; i++) printf("%c", 205);

    int start = pageOffset * ROWS_PER_PAGE;
    int end = start + ROWS_PER_PAGE;
    if(end > totalPenjualan) end = totalPenjualan;

    for(int i=start; i<end; i++) {
        int r = startY+2+(i-start);
        char pname[31]="Unknown";
        for(int p=0; p<totalProduk; p++) if(dbProduk[p].id == dbPenjualan[i].idProduk) strcpy(pname, dbProduk[p].nama);
        gotoxy(tableX, r);
        printf(" %-15s %c %-10s %c %-30s %c %-15ld ",
             dbPenjualan[i].noFaktur, 186, dbPenjualan[i].tanggal, 186, pname, 186, dbPenjualan[i].totalHarga);
    }
    gotoxy(tableX, startY+13); printf("Hal %d/%d | Total Data: %d", pageOffset+1, totalP, totalPenjualan);
}

void crudPenjualan(int idKaryawanLogin) {
    int selected = 0;
    const int totalMenu = 3;
    const char *menuItems[] = {"Transaksi Baru", "Riwayat Jual", "Kembali"};
    int tx = getCenterXForTable(95);
    int isPagingMode = 0; pageOffset=0;

    // SETUP LAYOUT
    drawBaseLayout("TRANSAKSI PENJUALAN");
    drawHomeLogo();
    int needContentRedraw = 0;

    while(1) {
        // 1. Gambar Menu Sidebar (Update Highlight)
        int startY = HEADER_HEIGHT + 6;
        for(int i=0; i<totalMenu; i++) printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));

        // 2. Update Konten Kanan
        if(needContentRedraw) {
            if(isPagingMode) {
                clearRightContent();
                displayPenjualanTable(tx);
            } else {
                drawHomeLogo();
            }
            needContentRedraw = 0;
        }

        // 3. Legend
        if(!isPagingMode) drawNavigationLegend("[PANAH] Pilih | [ENTER] Buka");
        else drawNavigationLegend("[PANAH] Scroll | [ESC] Menu");

        // 4. Input
        int key=getch();
        if(key == 224) {
            key=getch();
            if(isPagingMode) {
                 int maxP = (int)ceil((double)totalPenjualan/ROWS_PER_PAGE);
                 if(key==KEY_DOWN && pageOffset < maxP-1) { pageOffset++; needContentRedraw=1; }
                 else if(key==KEY_UP && pageOffset>0) { pageOffset--; needContentRedraw=1; }
            } else {
                 if(key==KEY_UP) { selected--; if(selected<0) selected=totalMenu-1; }
                 else if(key==KEY_DOWN) { selected++; if(selected>=totalMenu) selected=0; }
            }
        }
        else if(key == 13 && !isPagingMode) {
            if(selected == 2) break; // Kembali

            if(selected == 1) { // Riwayat
                isPagingMode = 1; pageOffset = 0; needContentRedraw = 1;
            }
            else { // Transaksi Baru
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
                        char pn[31];
                        for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==cart[i].pid) strcpy(pn, dbProduk[p].nama);
                        gotoxy(fx+2, fy+6+i);
                        printf("%-30s %-5d %10ld", pn, cart[i].qty, cart[i].subtotal);
                    }

                    gotoxy(fx+2, fy+13); printf("TOTAL BAYAR: Rp. %ld", grandTotal);
                    drawNavigationLegend("[T] Tambah Item | [ENTER] Bayar | [ESC] Batal");

                    key=getch();
                    if(key==27) break;
                    if(key==13 && cartCount > 0) {
                        for(int i=0; i<cartCount; i++) {
                            dbPenjualan[totalPenjualan].id = totalPenjualan+1;
                            strcpy(dbPenjualan[totalPenjualan].noFaktur, faktur);
                            strcpy(dbPenjualan[totalPenjualan].tanggal, tgl);
                            dbPenjualan[totalPenjualan].idKaryawan = dbKaryawan[idKaryawanLogin].id;
                            dbPenjualan[totalPenjualan].idProduk = cart[i].pid;
                            dbPenjualan[totalPenjualan].jumlah = cart[i].qty;
                            dbPenjualan[totalPenjualan].totalHarga = cart[i].subtotal;
                            for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==cart[i].pid) dbProduk[p].stok -= cart[i].qty;
                            totalPenjualan++;
                        }
                        showErrorAndWait(fx+2, fy+14, "Transaksi Berhasil!");
                        break;
                    }
                    if(tolower(key)=='t' && cartCount < 6) {
                        gotoxy(fx+2, fy+14); printf("ID Produk: ");
                        int pid = (int)getValidatedNumber(fx+13, fy+14);
                        int pidx = -1;
                        for(int p=0; p<totalProduk; p++) if(dbProduk[p].id==pid) pidx=p;

                        if(pidx != -1) {
                             gotoxy(fx+2, fy+15);
                             printf(">> %s (Rp.%ld) | Stok:%d", dbProduk[pidx].nama, dbProduk[pidx].harga, dbProduk[pidx].stok);
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
                // Redraw Layout setelah kembali dari mode input
                drawBaseLayout("TRANSAKSI PENJUALAN"); drawHomeLogo();
            }
        }
        else if (key == 27) {
            if(isPagingMode) { isPagingMode = 0; needContentRedraw = 1; }
            else break;
        }
    }
}

void crudPembelian() {
    int fx, fy, bw, bh;
    drawBaseLayout("RESTOCK / PEMBELIAN");
    drawFormBox("INPUT RESTOCK", &fx, &fy, &bw, &bh);

    char tgl[20]; getTodayDate(tgl);
    dbPembelian[totalPembelian].id = totalPembelian+1;
    strcpy(dbPembelian[totalPembelian].tanggal, tgl);

    gotoxy(fx+2, fy+2); printf("Tgl Otomatis: %s", tgl);
    gotoxy(fx+2, fy+4); printf("ID Supplier : "); dbPembelian[totalPembelian].idSupplier = (int)getValidatedNumber(fx+16, fy+4);
    gotoxy(fx+2, fy+6); printf("ID Produk   : ");
    int pid = (int)getValidatedNumber(fx+16, fy+6); dbPembelian[totalPembelian].idProduk = pid;
    gotoxy(fx+2, fy+8); printf("Jml Masuk   : ");
    int qty = (int)getValidatedNumber(fx+16, fy+8); dbPembelian[totalPembelian].jumlah = qty;
    gotoxy(fx+2, fy+10); printf("Total Biaya : "); dbPembelian[totalPembelian].totalHarga = getValidatedNumber(fx+16, fy+10);

    for(int i=0; i<totalProduk; i++) if(dbProduk[i].id == pid) dbProduk[i].stok += qty;
    totalPembelian++;
    showErrorAndWait(fx+2, fy+12, "Stok Bertambah!");
}