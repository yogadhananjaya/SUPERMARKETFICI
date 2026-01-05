#include "globals.h"

Supplier dbSupplier[MAX_DATA];
int totalSupplier = 0;
Produk dbProduk[MAX_DATA];
int totalProduk = 0;
Karyawan dbKaryawan[MAX_DATA];
int totalKaryawan = 0;
Gudang dbGudang[MAX_DATA];
int totalGudang = 0;
TransaksiPenjualan dbPenjualan[MAX_DATA];
int totalPenjualan = 0;
TransaksiPembelian dbPembelian[MAX_DATA];
int totalPembelian = 0;

int screenWidth = 0;
int screenHeight = 0;
int pageOffset = 0;
int isSidebarActive = 0;

void updateScreenSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    screenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void getTodayDate(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

// Helper untuk mendapatkan nama jabatan dari Role ID
char* getJabatanName(int roleId) {
    switch(roleId) {
        case ROLE_ADMIN: return "Administrator";
        case ROLE_STORE_MANAGER: return "Store Manager";
        case ROLE_HEAD_CASHIER: return "Kepala Kasir";
        case ROLE_CASHIER: return "Kasir";
        case ROLE_HEAD_WAREHOUSE: return "Kepala Gudang";
        case ROLE_STAFF_WAREHOUSE: return "Staff Gudang";
        default: return "Karyawan";
    }
}

void generateDummyData() {
    srand(time(0));
    totalSupplier = 0; totalProduk = 0; totalKaryawan = 0;
    totalGudang = 0; totalPenjualan = 0; totalPembelian = 0;

    // 1. DATA KARYAWAN (Dengan Role ID yang benar)
    // Admin & Manager manual
    struct RawKar { int id; char *n; int r; char *u; char *p; };
    struct RawKar seeds[] = {
        {1, "Super Admin", ROLE_ADMIN, "admin", "admin"},
        {2, "Budi Manager", ROLE_STORE_MANAGER, "budi", "123"},
        {3, "Siska HeadCash", ROLE_HEAD_CASHIER, "siska", "123"},
        {4, "Dedi HeadWh", ROLE_HEAD_WAREHOUSE, "dedi", "123"},
        {5, "Ani Kasir", ROLE_CASHIER, "ani", "123"}
    };

    for(int i=0; i<5; i++) {
        dbKaryawan[totalKaryawan].id = seeds[i].id;
        strcpy(dbKaryawan[totalKaryawan].nama, seeds[i].n);
        dbKaryawan[totalKaryawan].roleId = seeds[i].r;
        strcpy(dbKaryawan[totalKaryawan].jabatan, getJabatanName(seeds[i].r));
        strcpy(dbKaryawan[totalKaryawan].username, seeds[i].u);
        strcpy(dbKaryawan[totalKaryawan].password, seeds[i].p);
        sprintf(dbKaryawan[totalKaryawan].kontak, "0812%04d", i);
        dbKaryawan[totalKaryawan].performa = 80 + (rand()%20);
        totalKaryawan++;
    }

    // Generate 50 Karyawan Random (Campur Kasir & Staff Gudang)
    for(int i=0; i<50; i++) {
        dbKaryawan[totalKaryawan].id = totalKaryawan + 1;
        sprintf(dbKaryawan[totalKaryawan].nama, "Karyawan %d", i+1);
        int role = (i % 2 == 0) ? ROLE_CASHIER : ROLE_STAFF_WAREHOUSE;
        dbKaryawan[totalKaryawan].roleId = role;
        strcpy(dbKaryawan[totalKaryawan].jabatan, getJabatanName(role));
        sprintf(dbKaryawan[totalKaryawan].username, "user%d", totalKaryawan);
        strcpy(dbKaryawan[totalKaryawan].password, "123");
        sprintf(dbKaryawan[totalKaryawan].kontak, "0819%07d", i);
        dbKaryawan[totalKaryawan].performa = 60 + (rand()%40);
        totalKaryawan++;
    }

    // 2. PRODUK (50 Data)
    char *pNames[] = {"Beras", "Gula", "Minyak", "Telur", "Susu", "Kopi", "Teh", "Terigu", "Sabun", "Sampo"};
    for(int i=0; i<55; i++) {
        dbProduk[totalProduk].id = i + 1;
        sprintf(dbProduk[totalProduk].nama, "%s Varian %d", pNames[i%10], (i/10)+1);
        dbProduk[totalProduk].stok = 50 + (rand() % 200);
        dbProduk[totalProduk].harga = 5000 + (rand() % 50) * 500;
        totalProduk++;
    }

    // 3. SUPPLIER (50 Data)
    for(int i=0; i<50; i++) {
        dbSupplier[totalSupplier].id = i+1;
        sprintf(dbSupplier[totalSupplier].nama, "PT Supplier %d", i+1);
        sprintf(dbSupplier[totalSupplier].alamat, "Jl. Industri No. %d", i+1);
        sprintf(dbSupplier[totalSupplier].kontak, "021-555%d", i);
        totalSupplier++;
    }

    // 4. GUDANG (50 Data)
    for(int i=0; i<50; i++) {
        dbGudang[totalGudang].id = i+1;
        sprintf(dbGudang[totalGudang].nama, "Gudang Area %d", i+1);
        sprintf(dbGudang[totalGudang].alamat, "Kawasan %d", i+1);
        totalGudang++;
    }

    // 5. TRANSAKSI DUMMY (Untuk Laporan)
    // Kita buat transaksi di bulan 1-12 tahun 2025
    for(int i=0; i<60; i++) {
        dbPenjualan[totalPenjualan].id = i+1;
        sprintf(dbPenjualan[totalPenjualan].noFaktur, "INV/DUMMY/%d", i);

        int bln = (i % 12) + 1; // 1-12
        sprintf(dbPenjualan[totalPenjualan].tanggal, "%02d/%02d/2025", (rand()%28)+1, bln);

        dbPenjualan[totalPenjualan].idKaryawan = 5; // Ani Kasir
        dbPenjualan[totalPenjualan].idProduk = (i % 50) + 1;
        dbPenjualan[totalPenjualan].jumlah = 1 + (rand() % 5);
        dbPenjualan[totalPenjualan].totalHarga = dbPenjualan[totalPenjualan].jumlah * dbProduk[i%50].harga;

        totalPenjualan++;
    }
}