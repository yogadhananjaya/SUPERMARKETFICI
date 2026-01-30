#include "globals.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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
RiwayatKerja dbRiwayat[MAX_DATA];
int totalRiwayat = 0;

int screenWidth = 0;
int screenHeight = 0;
int pageOffset = 0;
int isSidebarActive = 0;

void getTodayDate(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

char* getJabatanName(int roleId) {
    switch(roleId) {
        case ROLE_ADMIN: return "Administrator";
        case ROLE_CASHIER: return "Kasir";
        case ROLE_WAREHOUSE: return "Staff Gudang";
        default: return "Karyawan";
    }
}

// Simple Bubble Sort untuk memastikan data terurut by ID
void sortKaryawanByID() {
    for(int i=0; i<totalKaryawan-1; i++) {
        for(int j=0; j<totalKaryawan-i-1; j++) {
            if(dbKaryawan[j].id > dbKaryawan[j+1].id) {
                Karyawan temp = dbKaryawan[j];
                dbKaryawan[j] = dbKaryawan[j+1];
                dbKaryawan[j+1] = temp;
            }
        }
    }
}

void sortProdukByID() {
    for(int i=0; i<totalProduk-1; i++) {
        for(int j=0; j<totalProduk-i-1; j++) {
            if(dbProduk[j].id > dbProduk[j+1].id) {
                Produk temp = dbProduk[j];
                dbProduk[j] = dbProduk[j+1];
                dbProduk[j+1] = temp;
            }
        }
    }
}

// --- PERSISTENCE ---
void saveAllData() {
    FILE *f;
    f = fopen("karyawan.dat", "wb"); if(f) { fwrite(&totalKaryawan, sizeof(int), 1, f); fwrite(dbKaryawan, sizeof(Karyawan), totalKaryawan, f); fclose(f); }
    f = fopen("produk.dat", "wb"); if(f) { fwrite(&totalProduk, sizeof(int), 1, f); fwrite(dbProduk, sizeof(Produk), totalProduk, f); fclose(f); }
    f = fopen("supplier.dat", "wb"); if(f) { fwrite(&totalSupplier, sizeof(int), 1, f); fwrite(dbSupplier, sizeof(Supplier), totalSupplier, f); fclose(f); }
    f = fopen("gudang.dat", "wb"); if(f) { fwrite(&totalGudang, sizeof(int), 1, f); fwrite(dbGudang, sizeof(Gudang), totalGudang, f); fclose(f); }
    f = fopen("jual.dat", "wb"); if(f) { fwrite(&totalPenjualan, sizeof(int), 1, f); fwrite(dbPenjualan, sizeof(TransaksiPenjualan), totalPenjualan, f); fclose(f); }
    f = fopen("beli.dat", "wb"); if(f) { fwrite(&totalPembelian, sizeof(int), 1, f); fwrite(dbPembelian, sizeof(TransaksiPembelian), totalPembelian, f); fclose(f); }
    f = fopen("riwayat.dat", "wb"); if(f) { fwrite(&totalRiwayat, sizeof(int), 1, f); fwrite(dbRiwayat, sizeof(RiwayatKerja), totalRiwayat, f); fclose(f); }
}

void loadAllData() {
    FILE *f;
    f = fopen("karyawan.dat", "rb"); if(f) { fread(&totalKaryawan, sizeof(int), 1, f); fread(dbKaryawan, sizeof(Karyawan), totalKaryawan, f); fclose(f); }
    f = fopen("produk.dat", "rb"); if(f) { fread(&totalProduk, sizeof(int), 1, f); fread(dbProduk, sizeof(Produk), totalProduk, f); fclose(f); }
    f = fopen("supplier.dat", "rb"); if(f) { fread(&totalSupplier, sizeof(int), 1, f); fread(dbSupplier, sizeof(Supplier), totalSupplier, f); fclose(f); }
    f = fopen("gudang.dat", "rb"); if(f) { fread(&totalGudang, sizeof(int), 1, f); fread(dbGudang, sizeof(Gudang), totalGudang, f); fclose(f); }
    f = fopen("jual.dat", "rb"); if(f) { fread(&totalPenjualan, sizeof(int), 1, f); fread(dbPenjualan, sizeof(TransaksiPenjualan), totalPenjualan, f); fclose(f); }
    f = fopen("beli.dat", "rb"); if(f) { fread(&totalPembelian, sizeof(int), 1, f); fread(dbPembelian, sizeof(TransaksiPembelian), totalPembelian, f); fclose(f); }
    f = fopen("riwayat.dat", "rb"); if(f) { fread(&totalRiwayat, sizeof(int), 1, f); fread(dbRiwayat, sizeof(RiwayatKerja), totalRiwayat, f); fclose(f); }
}

// --- DUMMY DATA ---
void generateDummyData() {
    srand(time(NULL));
    totalKaryawan = 0; totalProduk = 0; totalSupplier = 0;
    totalGudang = 0; totalPenjualan = 0; totalPembelian = 0; totalRiwayat = 0;

    const char *names[] = {"Budi", "Siti", "Agus", "Dewi", "Rian", "Nina", "Eko", "Maya", "Joko", "Rina"};

    // 1. ADMIN
    dbKaryawan[0].id=1; strcpy(dbKaryawan[0].nama,"Super Admin"); dbKaryawan[0].roleId=ROLE_ADMIN;
    strcpy(dbKaryawan[0].jabatan,"Administrator"); strcpy(dbKaryawan[0].username,"admin");
    strcpy(dbKaryawan[0].password,"admin"); strcpy(dbKaryawan[0].kontak,"08123456789");
    dbKaryawan[0].performa=0; dbKaryawan[0].isActive=1;
    totalKaryawan++;

    // 2. STAFF
    for(int i=1; i<50; i++) {
        int idx = totalKaryawan;
        dbKaryawan[idx].id = idx + 1;
        sprintf(dbKaryawan[idx].nama, "%s User %d", names[rand()%10], i);
        if(i <= 25) dbKaryawan[idx].roleId = ROLE_CASHIER; else dbKaryawan[idx].roleId = ROLE_WAREHOUSE;
        strcpy(dbKaryawan[idx].jabatan, getJabatanName(dbKaryawan[idx].roleId));
        sprintf(dbKaryawan[idx].username, "user%d", idx+1);
        strcpy(dbKaryawan[idx].password, "123");
        sprintf(dbKaryawan[idx].kontak, "081%08d", rand());
        dbKaryawan[idx].performa = 0; // Data baru 0
        dbKaryawan[idx].isActive = 1; // Aktif
        totalKaryawan++;
    }

    // 3. Produk
    const char *pNames[] = {"Indomie", "Aqua", "Roti", "Susu", "Sabun", "Shampo", "Kopi", "Teh", "Gula", "Beras"};
    for(int i=0; i<50; i++) {
        dbProduk[i].id = i + 1;
        sprintf(dbProduk[i].nama, "%s Varian %d", pNames[rand()%10], i+1);
        dbProduk[i].stok = rand() % 200;
        dbProduk[i].harga = (rand() % 50 + 1) * 500;
        dbProduk[i].isActive = 1;
        totalProduk++;
    }

    // 4. Supplier
    for(int i=0; i<50; i++) {
        dbSupplier[i].id = 100 + i;
        sprintf(dbSupplier[i].nama, "PT Supplier %d Sejahtera", i+1);
        sprintf(dbSupplier[i].alamat, "Jl. Industri Raya No. %d", i+1);
        sprintf(dbSupplier[i].kontak, "021-555%04d", i);
        dbSupplier[i].isActive = 1;
        totalSupplier++;
    }

    // 5. Gudang
    for(int i=0; i<50; i++) {
        dbGudang[i].id = 9000 + i;
        sprintf(dbGudang[i].nama, "Gudang Blok %c-%d", 'A'+(i%5), i+1);
        sprintf(dbGudang[i].alamat, "Kawasan Pergudangan No. %d", i+1);
        dbGudang[i].isActive = 1;
        totalGudang++;
    }

    // sorting awal
    sortKaryawanByID();
    sortProdukByID();

    saveAllData();
}