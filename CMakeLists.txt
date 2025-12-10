cmake_minimum_required(VERSION 3.26)
project(SUPERMARKET_FICI C)

set(CMAKE_C_STANDARD 23)

add_executable(SUPERMARKET_FICI
        main.c
        globals.c
        ui.c
        modul_supplier.c
        modul_produk.c
        modul_karyawan.c
        modul_gudang.c
)