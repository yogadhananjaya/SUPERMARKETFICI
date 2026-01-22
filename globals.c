#include "globals.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Definisi Variabel Global
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

// Fungsi Helper Tanggal
void getTodayDate(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

// Fungsi Helper Nama Jabatan
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

// --- GENERATOR DATA DUMMY YANG LEBIH CANGGIH ---
void generateDummyData() {
    srand(time(NULL)); // Seed random

    // Reset Total
    totalKaryawan = 0; totalProduk = 0; totalSupplier = 0;
    totalGudang = 0; totalPenjualan = 0; totalPembelian = 0;

    // ==========================================
    // 1. GENERATE KARYAWAN (> 60 Data)
    // ==========================================

    // A. Akun Utama (Penting untuk Login)
    struct RawKar { int id; char *n; int r; char *u; char *p; };
    struct RawKar seeds[] = {
        {1, "Super Admin", ROLE_ADMIN, "admin", "admin"},
        {2, "Budi Manager", ROLE_STORE_MANAGER, "manager", "123"},
        {3, "Siska HeadCash", ROLE_HEAD_CASHIER, "hkasir", "123"},
        {4, "Dedi HeadWh", ROLE_HEAD_WAREHOUSE, "hgudang", "123"},
        {5, "Ani Kasir", ROLE_CASHIER, "kasir", "123"},
        {6, "Joko Gudang", ROLE_STAFF_WAREHOUSE, "gudang", "123"}
    };

    for(int i=0; i<6; i++) {
        dbKaryawan[totalKaryawan].id = seeds[i].id;
        strcpy(dbKaryawan[totalKaryawan].nama, seeds[i].n);
        dbKaryawan[totalKaryawan].roleId = seeds[i].r;
        strcpy(dbKaryawan[totalKaryawan].jabatan, getJabatanName(seeds[i].r));
        strcpy(dbKaryawan[totalKaryawan].username, seeds[i].u);
        strcpy(dbKaryawan[totalKaryawan].password, seeds[i].p);
        sprintf(dbKaryawan[totalKaryawan].kontak, "0812%04d99%d", i, i);
        dbKaryawan[totalKaryawan].performa = 85 + (rand()%15); // 85-100
        totalKaryawan++;
    }

    // B. Generate 60 Karyawan Tambahan Random
    const char *firstNames[] = {"Adi", "Budi", "Citra", "Dewi", "Eko", "Fajar", "Gita", "Hadi", "Indah", "Joko", "Kiki", "Lina", "Maya", "Nina", "Oscar", "Putri", "Rina", "Sari", "Tono", "Vina"};
    const char *lastNames[] = {"Santoso", "Wijaya", "Saputra", "Utami", "Pratama", "Hidayat", "Kusuma", "Nugroho", "Wibowo", "Siregar", "Putra", "Dewi", "Susanti", "Suryani", "Handayani"};

    for(int i=0; i<60; i++) {
        dbKaryawan[totalKaryawan].id = totalKaryawan + 1;

        // Random Nama
        sprintf(dbKaryawan[totalKaryawan].nama, "%s %s",
            firstNames[rand() % 20], lastNames[rand() % 15]);

        // Random Role (Banyakan Kasir & Staff Gudang)
        int roleDice = rand() % 10;
        int r = (roleDice < 6) ? ROLE_CASHIER : ROLE_STAFF_WAREHOUSE;

        dbKaryawan[totalKaryawan].roleId = r;
        strcpy(dbKaryawan[totalKaryawan].jabatan, getJabatanName(r));

        // Username unik: nama depan + id
        char lowerName[20];
        int j=0;
        while(dbKaryawan[totalKaryawan].nama[j] != ' ' && dbKaryawan[totalKaryawan].nama[j] != 0) {
            lowerName[j] = tolower(dbKaryawan[totalKaryawan].nama[j]);
            j++;
        }
        lowerName[j] = 0;

        sprintf(dbKaryawan[totalKaryawan].username, "%s%d", lowerName, dbKaryawan[totalKaryawan].id);
        strcpy(dbKaryawan[totalKaryawan].password, "123");
        sprintf(dbKaryawan[totalKaryawan].kontak, "081%d%08d", (rand()%9)+1, rand());
        dbKaryawan[totalKaryawan].performa = 60 + (rand() % 41); // 60-100

        totalKaryawan++;
    }

    // ==========================================
    // 2. GENERATE PRODUK (100 Data)
    // ==========================================
    const char *brands[] = {"Indomie", "Sedap", "Sari Roti", "Ultra Milk", "Aqua", "Le Minerale", "Chitato", "Lays", "Oreo", "Biskuat", "Sunlight", "Rinso", "Lifebuoy", "Pepsodent", "Clear", "Rexona", "Bango", "ABC", "Indomilk", "Kapal Api"};
    const char *types[] = {"Goreng", "Kuah", "Coklat", "Vanilla", "Strawberry", "Pedas", "Asin", "Jeruk", "Melon", "Anggur", "Botol", "Kotak", "Sachet", "Refill", "Besar", "Kecil", "Jumbo", "Family Pack", "Premium", "Gold"};

    for(int i=0; i<100; i++) {
        dbProduk[totalProduk].id = i + 1;

        // Nama Produk Kombinasi
        sprintf(dbProduk[totalProduk].nama, "%s %s",
            brands[rand() % 20], types[rand() % 20]);

        dbProduk[totalProduk].stok = rand() % 300; // 0 - 299

        // Harga (Kelipatan 500 biar rapi)
        int basePrice = 2000 + (rand() % 100) * 1000;
        dbProduk[totalProduk].harga = basePrice;

        totalProduk++;
    }

    // ==========================================
    // 3. GENERATE SUPPLIER (60 Data)
    // ==========================================
    const char *ptNames[] = {"Sumber", "Jaya", "Makmur", "Sentosa", "Abadi", "Gemilang", "Bintang", "Cahaya", "Sinar", "Maju"};
    const char *suffix[] = {"Pangan", "Logistik", "Distribusindo", "Niaga", "Sejahtera", "Group", "Persada", "Food", "Beverage", "Indo"};
    const char *cities[] = {"Jakarta", "Bandung", "Surabaya", "Semarang", "Bekasi", "Tangerang", "Bogor", "Depok"};

    for(int i=0; i<60; i++) {
        dbSupplier[totalSupplier].id = 100 + i;

        sprintf(dbSupplier[totalSupplier].nama, "PT %s %s",
            ptNames[rand() % 10], suffix[rand() % 10]);

        sprintf(dbSupplier[totalSupplier].alamat, "Jl. Industri No.%d, %s",
            rand()%100 + 1, cities[rand() % 8]);

        sprintf(dbSupplier[totalSupplier].kontak, "021-55%05d", rand()%100000);

        totalSupplier++;
    }

    // ==========================================
    // 4. GENERATE GUDANG (60 Data)
    // ==========================================
    for(int i=0; i<60; i++) {
        dbGudang[totalGudang].id = 9000 + i;
        sprintf(dbGudang[totalGudang].nama, "Gudang Area %c-%d", 'A' + (i%5), (i/5)+1);
        sprintf(dbGudang[totalGudang].alamat, "Kawasan Industri %s Blok %c No.%d",
            cities[rand()%8], 'A'+(rand()%5), rand()%50+1);
        totalGudang++;
    }

    // ==========================================
    // 5. GENERATE TRANSAKSI PENJUALAN (150 Data)
    // ==========================================
    // Kita buat data tersebar di tahun 2024-2025
    for(int i=0; i<150; i++) {
        dbPenjualan[totalPenjualan].id = i + 1;
        sprintf(dbPenjualan[totalPenjualan].noFaktur, "INV/AUTO/%04d", i+1);

        // Random Tanggal (Bulan 1-12)
        int d = (rand() % 28) + 1;
        int m = (rand() % 12) + 1;
        int y = 2025; // Fokus 2025
        sprintf(dbPenjualan[totalPenjualan].tanggal, "%02d/%02d/%04d", d, m, y);

        // Random Kasir (Cari ID yang rolenya kasir)
        // Simplifikasi: Ambil random ID 5-50 (asumsi kasir ada disitu)
        int idKasir = 5 + (rand() % 20);
        dbPenjualan[totalPenjualan].idKaryawan = dbKaryawan[idKasir].id;

        // Simpan nama kasir (Snapshot)
        strcpy(dbPenjualan[totalPenjualan].namaKasir, dbKaryawan[idKasir].nama);

        // Produk
        int pIdx = rand() % totalProduk;
        dbPenjualan[totalPenjualan].idProduk = dbProduk[pIdx].id;
        dbPenjualan[totalPenjualan].jumlah = (rand() % 5) + 1; // 1-5 item
        dbPenjualan[totalPenjualan].totalHarga = (long)dbPenjualan[totalPenjualan].jumlah * dbProduk[pIdx].harga;

        // Total Bayar (Untuk dummy ini kita samakan dengan totalHarga per item biar gampang,
        // aslinya 1 faktur bisa banyak item, tapi struktur struct kita 1 row 1 item)
        dbPenjualan[totalPenjualan].totalBayar = dbPenjualan[totalPenjualan].totalHarga;

        totalPenjualan++;
    }

    // ==========================================
    // 6. GENERATE PEMBELIAN/RESTOCK (70 Data)
    // ==========================================
    for(int i=0; i<70; i++) {
        dbPembelian[totalPembelian].id = i + 1;

        int d = (rand() % 28) + 1;
        int m = (rand() % 12) + 1;
        sprintf(dbPembelian[totalPembelian].tanggal, "%02d/%02d/2025", d, m);

        dbPembelian[totalPembelian].idKaryawan = 4; // Dedi Head Gudang
        dbPembelian[totalPembelian].idSupplier = dbSupplier[rand() % totalSupplier].id;

        int pIdx = rand() % totalProduk;
        dbPembelian[totalPembelian].idProduk = dbProduk[pIdx].id;
        dbPembelian[totalPembelian].jumlah = (rand() % 50) + 10; // 10-60 item restock

        // Harga Beli biasanya 80% dari Harga Jual
        long hargaBeliSatuan = dbProduk[pIdx].harga * 0.8;
        dbPembelian[totalPembelian].totalHarga = hargaBeliSatuan * dbPembelian[totalPembelian].jumlah;

        totalPembelian++;
    }
}