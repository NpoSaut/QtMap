#ifndef MASQARADE_H
#define MASQARADE_H

//  ╔═══════════════════════════════════════════════════════════╗
//  ║                ЦВЕТНОЙ ВЫВОД НА КОНСОЛЬ                   ║
//  ╟───────────────────────────────────────────────────────────╢
//  ║   Содержит определения для цветного вывода на консоль     ║
//  ║   для двух основных платформ (Windows, Linux).            ║
//  ║   предоставляет возможность красиво выводить дебаги       ║
//  ╟───────────────────────────────────────────────────────────╢
//  ║   Необходимо добавить в начало main() строки:             ║
//  ║   #ifdef WIN32                                            ║
//  ║       winConsoleandler = GetStdHandle(STD_OUTPUT_HANDLE); ║
//  ║   #endif                                                  ║
//  ║   и ещё в какое-то место строки:                          ║
//  ║   #ifdef WIN32                                            ║
//  ║       HANDLE winConsoleandler;                            ║
//  ║   #endif                                                  ║
//  ╚═══════════════════════════════════════════════════════════╝

#ifdef WIN32

#include <Windows.h>
#define SET_COLOR(fg, bg) SetConsoleTextAttribute(winConsoleandler, fg + 16*bg)
#define RESET_COLOR SET_COLOR(7,0)

#define CL_BLACK    0
#define CL_BLUE     1
#define CL_GREEN    2
#define CL_CYAN     3
#define CL_RED      4
#define CL_VIOLET   5
#define CL_YELLOW   6
#define CL_WHITE    7

#define CL_LIGHT(color) color + 8
#define CL_GRAY     CL_LIGHT(CL_BLACK)
#define CL_BLUE_L   CL_LIGHT(CL_BLUE)
#define CL_GREEN_L  CL_LIGHT(CL_GREEN)
#define CL_CYAN_L   CL_LIGHT(CL_CYAN)
#define CL_RED_L    CL_LIGHT(CL_RED)
#define CL_VIOLET_L CL_LIGHT(CL_VIOLET)
#define CL_YELLOW_L CL_LIGHT(CL_YELLOW)
#define CL_WHITE_L  CL_LIGHT(CL_WHITE)

extern HANDLE winConsoleandler;

#else

#define SET_COLOR(fg, bg) printf("\033[%d;%d;%dm", (fg >> 4) & 0x01, 30 + (fg & 0xf), 40 + (bg & 0xf))
#define RESET_COLOR SET_COLOR(CL_WHITE,CL_BLACK)

#define CL_BLACK    0x00
#define CL_BLUE     0x04
#define CL_GREEN    0x02
#define CL_CYAN     0x06
#define CL_RED      0x01
#define CL_VIOLET   0x05
#define CL_YELLOW   0x03
#define CL_WHITE    0x07

#define CL_GRAY     0x10
#define CL_BLUE_L   0x14
#define CL_GREEN_L  0x12
#define CL_CYAN_L   0x16
#define CL_RED_L    0x11
#define CL_VIOLET_L 0x15
#define CL_YELLOW_L 0x13
#define CL_WHITE_L  0x17

#endif

#define CPRINTF(__color__, ...) SET_COLOR(__color__, CL_BLACK); printf(__VA_ARGS__); fflush(stdout); RESET_COLOR;



#endif // MASQARADE_H
