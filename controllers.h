#ifndef CONTROLLERS_H
#define CONTROLLERS_H

// --- AUTH ---
int loginScreen(int *loggedIndex);

// --- EMPLOYEE DASHBOARD ---
void employeeMainMenu(int idx);
void viewProfile(int idx);

// --- MASTERS (ADMIN) ---
void crudKaryawan();
void crudProduk(int isAdmin);
void crudSupplier();
void crudGudang();

// --- TRANSACTIONS & RESTOCK ---
void crudPenjualan(int idKaryawanLogin);       // Menu Kasir
void menuRestock(int idKaryawanLogin);         // Menu Gudang: Restock Internal
void menuTransaksiGudang(int idKaryawanLogin); // Menu Gudang: Beli ke Supplier

// --- REPORT ---
void menuLaporan();

#endif