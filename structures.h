#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct {
    int id;
    char nama[50];
    char alamat[100];
    char kontak[20];
    int isActive; // Soft Delete: 1=Aktif, 0=Dihapus
} Supplier;

typedef struct {
    int id;
    char nama[50];
    int stok;
    long harga; // Harga Jual
    int isActive;
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
    int isActive;
} Karyawan;

typedef struct {
    int id;
    char nama[50];
    char alamat[100];
    int isActive;
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

typedef struct {
    int id;
    int idKaryawan;
    char aktivitas[50];
    int poin;
    char tanggal[20];
} RiwayatKerja;

#endif