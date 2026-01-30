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
void crudPenjualan(int idKaryawanLogin);
void menuRestock(int idKaryawanLogin);
void menuTransaksiGudang(int idKaryawanLogin);

// Helper view (Agar bisa dipanggil dari Laporan)
void viewInvoiceDetail(const char* targetFaktur);
void viewPurchaseDetail(const char* targetFaktur);

// --- REPORT ---
void menuLaporan();

#endif