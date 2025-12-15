#ifndef CONTROLLERS_H
#define CONTROLLERS_H

void crudSupplier();
// REVISI: Menambahkan parameter isAdmin (1=Admin, 0=View Only)
void crudProduk(int isAdmin);
void crudKaryawan();
void crudGudang();

// REVISI: Menambahkan controller untuk dashboard karyawan
void employeeMainMenu(int employeeIndex);

#endif