#include "config.h"
#include "globals.h"
#include "ui.h"
#include "controllers.h" // Pastikan ini ada biar CRUD kedetek

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

    if (!loginScreen()) return 0;

    int selected = 1;
    int totalMenu = 5;
    int key;

    drawMainMenuStatic();
    updateMainMenuOptions(selected);

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
        // Logic Enter
        else if(key == KEY_ENTER) {
            switch(selected) {
                case 1: crudKaryawan(); break;
                case 2: crudSupplier(); break;
                case 3: crudGudang(); break;
                case 4: crudProduk(); break;
                case 5: return 0;
            }

            drawMainMenuStatic();
            updateMainMenuOptions(selected);
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
    return 0;
}