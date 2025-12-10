cmake_minimum_required(VERSION 3.26) # Versi CMake standar CLion baru
project(SUPERMARKET_FICI C)

# Mengatur standar bahasa C ke versi 23 sesuai permintaanmu
set(CMAKE_C_STANDARD 23)

# Ini bagian paling PENTING.
# Kita mendaftarkan SEMUA file .c (source code) yang sudah kita pecah tadi.
add_executable(SUPERMARKET_FICI
        main.c
        globals.c
        ui.c
        modul_supplier.c
        modul_produk.c
        modul_karyawan.c
        modul_gudang.c
)