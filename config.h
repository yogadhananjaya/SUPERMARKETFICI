#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#define MAX_DATA 500 // Dinaikkan untuk menampung dummy data
#define HEADER_HEIGHT 8
#define ROWS_PER_PAGE 10

// Key Mapping
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_TAB 9
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_BACKSPACE 8

// Warna
#define COLOR_HOT_PINK 13
#define COLOR_WHITE 15

// Role ID Definitions
#define ROLE_ADMIN 0
#define ROLE_STORE_MANAGER 1
#define ROLE_HEAD_CASHIER 2
#define ROLE_CASHIER 3
#define ROLE_HEAD_WAREHOUSE 4
#define ROLE_STAFF_WAREHOUSE 5

#endif