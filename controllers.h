#ifndef CONTROLLERS_H
#define CONTROLLERS_H
#include "globals.h"

// Modul Master
void crudSupplier();
void crudProduk(int isAdmin);
void crudKaryawan();
void crudGudang();

// Modul Transaksi & Laporan
void crudPenjualan(int idKaryawanLogin);
void crudPembelian(int idKaryawanLogin);
void menuLaporan();
void viewMyTeam(int roleId);
void employeeMainMenu(int employeeIndex);
void viewPerformanceDetail(int employeeIndex);

#endif