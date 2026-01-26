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
    int performa; // Total Poin akumulasi
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
    char namaKasir[50];
    int idProduk;
    int jumlah;
    long totalHarga;
    long totalBayar;
} TransaksiPenjualan;

typedef struct {
    int id;
    char noFaktur[20];
    char tanggal[20];
    int idKaryawan;
    int idSupplier;
    int idProduk;
    int jumlah;
    long totalHarga;
} TransaksiPembelian;

// NEW: Struct Riwayat Pekerjaan
typedef struct {
    int id;
    int idKaryawan;
    char aktivitas[50]; // "Melakukan Transaksi", "Terlambat", dll
    int poin;           // +1, -2, dll
    char tanggal[20];
} RiwayatKerja;

#endif