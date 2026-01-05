#ifndef CONTROLLERS_H
#define CONTROLLERS_H

// Modul Master (Gabungan)
void crudSupplier();
void crudProduk(int isAdmin);
void crudKaryawan();
void crudGudang();

// Modul Transaksi
void crudPenjualan(int idKaryawanLogin);
void crudPembelian();

// Modul Laporan & Dashboard
void menuLaporan();
void viewMyTeam(int roleId);
void employeeMainMenu(int employeeIndex);

#endif