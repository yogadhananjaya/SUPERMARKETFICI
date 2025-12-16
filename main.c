#include "config.h"
#include "globals.h"
#include "ui.h"
#include "controllers.h"


void updateAdminMenu(int selected) {
    int boxW = 50, boxH = 16;
    int menuX = (screenWidth - boxW) / 2;
    int menuY = (screenHeight - boxH) / 2;
    if (menuY <= HEADER_HEIGHT) menuY = HEADER_HEIGHT + 4;

    int midX = menuX + (boxW / 2) - 12;
    int startY = menuY + 5;

    printMenuItem(midX, startY,     "1. KARYAWAN", selected == 1);
    printMenuItem(midX, startY + 1, "2. SUPPLIER", selected == 2);
    printMenuItem(midX, startY + 2, "3. GUDANG",   selected == 3);
    printMenuItem(midX, startY + 3, "4. PRODUK",   selected == 4);
    printMenuItem(midX, startY + 4, "5. PENJUALAN", selected == 5);
    printMenuItem(midX, startY + 5, "6. PEMBELIAN", selected == 6);
    printMenuItem(midX, startY + 6, "0. LOGOUT",   selected == 7);
}

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

    while (1) {
        int userIndex = -1;


        int role = loginScreen(&userIndex);

        if (role == 0) {

            system("cls");
            gotoxy(screenWidth/2 - 10, screenHeight/2);
            printf("Terima Kasih!");
            Sleep(1000);
            break;
        }

        if (role == 2) {
            employeeMainMenu(userIndex);
        }
        else if (role == 1) {

            int selected = 1;
            int totalMenu = 7;
            int key;

            drawMainMenuStatic();
            drawNavigationLegend("[Panah] Navigasi | [ENTER] Pilih");
            updateAdminMenu(selected);

            while(1) {
                hideCursor();
                key = getch();

                if(key == 224) {
                    key = getch();
                    if(key == KEY_UP) {
                        selected = (selected == 1) ? totalMenu : selected - 1;
                        updateAdminMenu(selected);
                    }
                    else if(key == KEY_DOWN) {
                        selected = (selected == totalMenu) ? 1 : selected + 1;
                        updateAdminMenu(selected);
                    }
                }
                else if(key == KEY_ENTER) {
                    if (selected == 7) {
                        break;
                    }

                    switch(selected) {
                        case 1: crudKaryawan(); break;
                        case 2: crudSupplier(); break;
                        case 3: crudGudang(); break;
                        case 4: crudProduk(1); break;
                        case 5: crudPenjualan(); break;
                        case 6: crudPembelian(); break;
                    }

                    drawMainMenuStatic();
                    drawNavigationLegend("[Panah] Navigasi | [ENTER] Pilih");
                    updateAdminMenu(selected);
                }
                else if (key == KEY_ESC) {
                    break;
                }
            }
        }

    }

    return 0;
}