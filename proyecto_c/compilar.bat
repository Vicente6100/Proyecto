@echo off
gcc -Wall -Wextra -std=c11 main.c menu.c topk.c tdas/heap.c tdas/extra.c tdas/list.c tdas/map.c -o programa.exe
if errorlevel 1 exit /b 1
programa.exe
