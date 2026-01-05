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
    int roleId; // 0:Admin, 1:Mgr, 2:KepKas, 3:Kasir, 4:KepGud, 5:StfGud
    char kontak[20];
    char username[30];
    char password[30];
    int performa; // 0-100
} Karyawan;

typedef struct {
    int id;
    char nama[50];
    char alamat[100];
} Gudang;

typedef struct {
    int id;
    char noFaktur[20]; // Penanda satu keranjang belanja
    char tanggal[20];
    int idKaryawan;    // Yang melayani transaksi
    int idProduk;
    int jumlah;
    long totalHarga;   // Subtotal per item
} TransaksiPenjualan;

typedef struct {
    int id;
    char tanggal[20];
    int idSupplier;
    int idProduk;
    int jumlah;
    long totalHarga;
} TransaksiPembelian;

#endif