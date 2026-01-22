#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include <windows.h>

// --- DIMENSI LAYAR ---
#define SIDEBAR_WIDTH         30
#define HEADER_HEIGHT         8
#define ROWS_PER_PAGE         10

// --- TEMA WARNA (HEX) ---
#define THEME_PRIMARY_COLOR   0xDF  // Pink BG, White Text
#define THEME_HIGHLIGHT_COLOR 0xFD  // White BG, Pink Text

// --- TEMA WARNA (RGB) ---
#define RGB_PRIMARY_BG        RGB(255, 20, 147)
#define RGB_PRIMARY_FG        RGB(255, 255, 255)

// --- KARAKTER ASCII (BOX DRAWING) ---
#define CH_HLINE              205 // ═
#define CH_VLINE              186 // ║
#define CH_TOP_LEFT           201 // ╔
#define CH_TOP_RIGHT          187 // ╗
#define CH_BOT_LEFT           200 // ╚
#define CH_BOT_RIGHT          188 // ╝
#define CH_TEE_TOP            203 // ╦  <-- INI YANG SEBELUMNYA HILANG
#define CH_TEE_BOT            202 // ╩  <-- INI YANG SEBELUMNYA HILANG

// --- KODE TOMBOL ---
#define KEY_UP                72
#define KEY_DOWN              80
#define KEY_LEFT              75
#define KEY_RIGHT             77
#define KEY_ENTER             13
#define KEY_ESC               27
#define KEY_TAB               9
#define KEY_BACKSPACE         8

#endif