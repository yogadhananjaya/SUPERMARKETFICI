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
    char tanggal[20];
    char namaPelanggan[50];
    int idProduk;
    int jumlah;
    long totalHarga;
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