#include "config.h"
#include "globals.h"
#include "ui.h"
#include "controllers.h"

int main() {
    maximizeConsole();
    setPinkTheme();
    system("color DF");
    generateDummyData();
    loadingAnimation();
    goFullscreen();
    removeScrollbar();
    updateScreenSize();
    hideCursor();

    int userIndex = -1;
    // REVISI: Login mengembalikan role (1:Admin, 2:Karyawan)
    int role = loginScreen(&userIndex);

    if (role == 0) return 0; // Exit jika ESC di login screen

    // --- REVISI: LOGIC PERCABANGAN DASHBOARD ---

    if (role == 2) {
        // JIKA LOGIN SEBAGAI KARYAWAN
        employeeMainMenu(userIndex);
    }
    else if (role == 1) {
        // JIKA LOGIN SEBAGAI ADMIN (Code lama menu utama admin)
        int selected = 1;
        int totalMenu = 5;
        int key;

        drawMainMenuStatic();
        updateMainMenuOptions(selected);
        drawNavigationLegend("[Panah] Navigasi | [ENTER] Pilih | [ESC] Logout");

        while(1) {
            hideCursor();
            key = getch();

            if(key == 224) {
                key = getch();
                if(key == KEY_UP) {
                    selected = (selected == 1) ? totalMenu : selected - 1;
                    updateMainMenuOptions(selected);
                }
                else if(key == KEY_DOWN) {
                    selected = (selected == totalMenu) ? 1 : selected + 1;
                    updateMainMenuOptions(selected);
                }
            }
            else if(key == KEY_ENTER) {
                switch(selected) {
                    case 1: crudKaryawan(); break;
                    case 2: crudSupplier(); break;
                    case 3: crudGudang(); break;
                    // REVISI: crudProduk dipanggil dengan flag 1 (Admin)
                    case 4: crudProduk(1); break;
                    case 5: return 0;
                }
                drawMainMenuStatic();
                updateMainMenuOptions(selected);
                drawNavigationLegend("[Panah] Navigasi | [ENTER] Pilih | [ESC] Logout");
            }
            else if (key == KEY_ESC) {
                if (selected != 5) {
                    selected = 5;
                    updateMainMenuOptions(selected);
                } else {
                    return 0;
                }
            }
        }
    }

    return 0;
}