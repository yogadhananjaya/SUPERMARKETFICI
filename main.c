#include "config.h"
#include "globals.h"
#include "ui.h"
#include "controllers.h"

int main() {
    maximizeConsole();
    setPinkTheme();
    system("color DF");

    generateDummyData();

    goFullscreen();
    loadingAnimation();

    while(1) {
        int userIndex = -1;
        int role = loginScreen(&userIndex);

        if (role == -1) break; // Exit

        // --- MENU ADMIN / MANAGER ---
        if (role == ROLE_ADMIN || role == ROLE_STORE_MANAGER) {
            int selected = 0;
            const int totalMenu = 6;
            const char *menuItems[] = {
                "Manajemen Karyawan",
                "Manajemen Produk",
                "Manajemen Supplier",
                "Manajemen Gudang",
                "Laporan Bulanan",
                "Logout"
            };

            // FLAG: Menandakan apakah perlu gambar ulang seluruh layar (layout dasar)
            int needFullRedraw = 1;

            while(1) {
                // 1. Gambar Layout Dasar HANYA jika diminta (misal baru masuk menu)
                if (needFullRedraw) {
                    drawBaseLayout("MENU ADMINISTRATOR");
                    drawHomeLogo(); // Gambar logo di kanan sekali saja
                    needFullRedraw = 0;
                }

                // 2. Gambar Menu Items (Looping di sini tidak bikin kedip karena tidak ada CLS)
                // Posisi X=4 menjamin ada di Sidebar Kiri
                int startY = HEADER_HEIGHT + 6;
                for(int i=0; i<totalMenu; i++) {
                    printMenuItem(4, startY + (i*2), (char*)menuItems[i], (i == selected));
                }

                // Gambar Legend Navigasi (statis di bawah)
                drawNavigationLegend("[PANAH] Pilih Menu | [ENTER] Buka");

                // 3. Input Handler
                int key = getch();
                if(key == 224) { // Panah
                    key = getch();
                    if(key == KEY_UP) {
                        selected--;
                        if(selected < 0) selected = totalMenu - 1;
                    }
                    else if(key == KEY_DOWN) {
                        selected++;
                        if(selected >= totalMenu) selected = 0;
                    }
                    // Loop akan berulang dan hanya redraw menu item (smooth)
                }
                else if(key == 13) { // Enter
                    if(selected == 5) break; // Logout

                    // Masuk ke submenu
                    if(selected == 0) crudKaryawan();
                    else if(selected == 1) crudProduk(1);
                    else if(selected == 2) crudSupplier();
                    else if(selected == 3) crudGudang();
                    else if(selected == 4) menuLaporan();

                    // SETELAH KEMBALI DARI SUBMENU, KITA HARUS REDRAW LAYOUT UTAMA
                    needFullRedraw = 1;
                }
            }
        }
        else {
            employeeMainMenu(userIndex);
        }
    }
    return 0;
}