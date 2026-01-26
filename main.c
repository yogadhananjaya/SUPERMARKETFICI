#include "config.h"
#include "globals.h"
#include "ui.h"
#include "controllers.h"

int main() {
    maximizeConsole();
    setPinkTheme();
    system("color DF");

    loadAllData();
    if (totalKaryawan == 0) {
        generateDummyData();
    }

    goFullscreen();
    loadingAnimation();

    while(1) {
        int userIndex = -1;
        int role = loginScreen(&userIndex);

        if (role == -1) {
            saveAllData();
            break;
        }

        if (role == ROLE_ADMIN) {
            int selected = 0;
            const char *menuItems[] = {"Manajemen Karyawan", "Manajemen Produk", "Manajemen Supplier", "Manajemen Gudang", "Laporan & Keuangan", "Logout"};
            int redraw = 1;
            while(1) {
                if(redraw) {
                    drawBaseLayout("ADMINISTRATOR");
                    drawHomeLogo(ROLE_ADMIN); // FIX
                    redraw = 0;
                }
                int startY = HEADER_HEIGHT + 6;
                for(int i=0; i<6; i++) printMenuItem(4, startY+(i*2), (char*)menuItems[i], (i==selected));
                drawNavigationLegend("[PANAH] Pilih Menu | [ENTER] Buka");
                int key = getch();
                if(key == 224) { key = getch(); if(key==KEY_UP) selected=(selected>0)?selected-1:5; if(key==KEY_DOWN) selected=(selected<5)?selected+1:0; }
                else if(key == 13) {
                    if(selected == 5) break;
                    if(selected == 0) crudKaryawan();
                    if(selected == 1) crudProduk(1);
                    if(selected == 2) crudSupplier();
                    if(selected == 3) crudGudang();
                    if(selected == 4) menuLaporan();
                    redraw = 1;
                }
            }
        }
        else {
            employeeMainMenu(userIndex);
        }
    }
    return 0;
}
