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

extern int screenWidth;
extern int screenHeight;
extern int pageOffset;


void generateDummyData();
void updateScreenSize();

#endif