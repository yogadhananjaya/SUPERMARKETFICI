// Mengimpor konfigurasi utama (misalnya, konstanta tombol).
#include "config.h"
// Mengimpor variabel global dan array database.
#include "globals.h"
// Mengimpor fungsi-fungsi antarmuka pengguna (UI).
#include "ui.h"
// Mengimpor fungsi-fungsi CRUD (Create, Read, Update, Delete) untuk setiap modul.
#include "controllers.h"


int main() {

    // Memaksimalkan jendela konsol saat program dimulai.
    maximizeConsole();
    // Mengatur tema warna kustom (pink) menggunakan fungsi setPinkTheme.
    setPinkTheme();
    // Mengatur warna teks dan latar belakang default konsol (DF).
    system("color DF");

    // Mengisi semua array database (Karyawan, Supplier, dll.) dengan data dummy.
    generateDummyData();

    // Menampilkan animasi loading.
    loadingAnimation();
    // Mengaktifkan mode layar penuh (Alt+Enter).
    goFullscreen();
    // Menghilangkan scrollbar dengan menyamakan ukuran buffer dengan jendela.
    removeScrollbar();
    // Memastikan ukuran layar global sudah terupdate setelah manipulasi jendela.
    updateScreenSize();
    // Menyembunyikan kursor agar tidak mengganggu tampilan UI.
    hideCursor();

    // Memanggil layar login. Jika login gagal (mengembalikan 0), program keluar.
    if (!loginScreen()) return 0;

    // Variabel untuk melacak item menu yang sedang dipilih (dimulai dari KARYAWAN).
    int selected = 1;
    // Jumlah total menu di Menu Utama (1-Karyawan s.d. 5-Keluar).
    int totalMenu = 5;
    // Variabel untuk menyimpan kode tombol yang ditekan.
    int key;

    // Menggambar bingkai menu utama yang statis (kotak, header, footer).
    drawMainMenuStatic();
    // Memperbarui tampilan menu untuk menandai item yang dipilih saat ini.
    updateMainMenuOptions(selected);

    // Loop utama program untuk navigasi menu.
    while(1) {
        // Memastikan kursor tetap tersembunyi.
        hideCursor();
        // Menunggu input dari pengguna (tanpa echo).
        key = getch();


        // Logika untuk menangani tombol panah (Arrow Keys)
        if(key == 224) { // 224 adalah scan code pertama untuk tombol khusus
            key = getch(); // Ambil scan code kedua yang menentukan arah
            // Tombol Panah ATAS
            if(key == KEY_UP) {
                // Navigasi naik: jika sudah di menu 1, pindah ke menu terakhir (totalMenu), jika tidak, kurangi 1.
                selected = (selected == 1) ? totalMenu : selected - 1;
                updateMainMenuOptions(selected);
            }
            // Tombol Panah BAWAH
            else if(key == KEY_DOWN) {
                // Navigasi turun: jika sudah di menu terakhir, pindah ke menu 1, jika tidak, tambah 1.
                selected = (selected == totalMenu) ? 1 : selected + 1;
                updateMainMenuOptions(selected);
            }
        }
        // Logika untuk tombol ENTER (Eksekusi Pilihan)
        else if(key == KEY_ENTER) {
            // Pindah ke fungsi controller yang sesuai.
            switch(selected) {
                case 1: crudKaryawan(); break; // Pindah ke modul Karyawan
                case 2: crudSupplier(); break; // Pindah ke modul Supplier
                case 3: crudGudang(); break;   // Pindah ke modul Gudang
                case 4: crudProduk(); break;   // Pindah ke modul Produk
                case 5: return 0;              // Keluar dari program
            }

            // Setelah controller selesai, gambar ulang Menu Utama dan tandai pilihan terakhir.
            drawMainMenuStatic();
            updateMainMenuOptions(selected);
        }
        // Logika untuk tombol ESC (Keluar Cepat atau Kembali ke Menu Keluar)
        else if (key == KEY_ESC) {
            // Jika pilihan saat ini BUKAN "Keluar", pindahkan pilihan ke "Keluar" (5).
            if (selected != 5) {
                selected = 5;
                updateMainMenuOptions(selected);
            } else {
                // Jika pilihan sudah "Keluar", tekan ESC lagi akan keluar dari program.
                return 0;
            }
        }
    }
    // Ini seharusnya tidak terjangkau, tetapi dikembalikan untuk kelengkapan fungsi main.
    return 0;
}