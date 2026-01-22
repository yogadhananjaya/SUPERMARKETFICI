#include "config.h"
#include "globals.h"
#include "ui.h"
#include "controllers.h"

int main() {
    maximizeConsole();
    setPinkTheme();
    system("color DF"); // Fallback color

    generateDummyData(); // Pastikan fungsi ini ada di globals.c

    goFullscreen();
    loadingAnimation();

    while(1) {
        int userIndex = -1;
        int role = loginScreen(&userIndex);

        if (role == -1) break; // Exit application

        // --- MENU ADMIN / MANAGER ---
        if (role == ROLE_ADMIN || role == ROLE_STORE_MANAGER) {
            int selected = 0;
            const char *menuItems[] = {
                "Manajemen Karyawan",
                "Manajemen Produk",
                "Manajemen Supplier",
                "Manajemen Gudang",
                "Laporan & Keuangan", // Digabung
                "Logout"
            };

            int redraw = 1;
            while(1) {
                if(redraw) {
                    drawBaseLayout("ADMINISTRATOR");
                    drawHomeLogo();
                    redraw = 0;
                }

                int startY = HEADER_HEIGHT + 6;
                for(int i=0; i<6; i++) printMenuItem(4, startY+(i*2), (char*)menuItems[i], (i==selected));
                drawNavigationLegend("[PANAH] Pilih Menu | [ENTER] Buka");

                int key = getch();
                if(key == 224) {
                    key = getch();
                    if(key==KEY_UP) selected=(selected>0)?selected-1:5;
                    if(key==KEY_DOWN) selected=(selected<5)?selected+1:0;
                }
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
            // MASUK KE DASHBOARD KARYAWAN (Kasir / Gudang)
            employeeMainMenu(userIndex);
        }
    }
    return 0;
}