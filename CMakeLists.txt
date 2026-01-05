cmake_minimum_required(VERSION 3.26)
project(SUPERMARKET_FICI C)

set(CMAKE_C_STANDARD 11)

add_executable(SUPERMARKET_FICI
        main.c
        globals.c
        ui.c
        modul_masters.c
        modul_transaksi.c
        modul_laporan.c
        modul_employee_dashboard.c
        ui_config.h
)