#ifndef GLOBALS_H
#define GLOBALS_H
#include "config.h"
#include "structures.h"

extern Supplier dbSupplier[MAX_DATA];
extern int totalSupplier;

extern Produk dbProduk[MAX_DATA];
extern int totalProduk;

extern Karyawan dbKaryawan[MAX_DATA];
extern int totalKaryawan;

extern Gudang dbGudang[MAX_DATA];
extern int totalGudang;

extern TransaksiPenjualan dbPenjualan[MAX_DATA];
extern int totalPenjualan;

extern TransaksiPembelian dbPembelian[MAX_DATA];
extern int totalPembelian;

// NEW
extern RiwayatKerja dbRiwayat[MAX_DATA];
extern int totalRiwayat;

extern int screenWidth;
extern int screenHeight;
extern int pageOffset;
extern int isSidebarActive;

// Fungsi Global
void generateDummyData();
void loadAllData();
void saveAllData();
void updateScreenSize();
void getTodayDate(char *buffer);
char* getJabatanName(int roleId);

#endif