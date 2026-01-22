#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct {
    int id;
    char nama[50];
    char alamat[100];
    char kontak[20];
} Supplier;

typedef struct {
    int id;
    char nama[50];
    int stok;
    long harga;
} Produk;

typedef struct {
    int id;
    char nama[50];
    char jabatan[30];
    int roleId;
    char kontak[20];
    char username[30];
    char password[30];
    int performa;
} Karyawan;

typedef struct {
    int id;
    char nama[50];
    char alamat[100];
} Gudang;

typedef struct {
    int id;
    char noFaktur[20];
    char tanggal[20];
    int idKaryawan;
    char namaKasir[50]; // Snapshot nama saat transaksi
    int idProduk;
    int jumlah;
    long totalHarga;
    long totalBayar; // Untuk header faktur
} TransaksiPenjualan;

// Struktur simpel untuk menyimpan header riwayat (biar hemat memori/file)
typedef struct {
    char noFaktur[20];
    char tanggal[20];
    char namaKasir[50];
    long totalBayar;
} RiwayatPenjualan;

typedef struct {
    int id;
    char noFaktur[20];
    char tanggal[20];
    int idKaryawan; // Update: Siapa yang input stok
    int idSupplier;
    int idProduk;
    int jumlah;
    long totalHarga;
} TransaksiPembelian;

#endif