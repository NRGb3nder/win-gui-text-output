#include <windows.h>
#include "TableDrawingRoutines.h"

#define TABLE_BRUSH_COLOR RGB(255, 255, 255)
#define TABLE_PEN_COLOR RGB(0, 0, 0)
#define TABLE_PEN_WIDTH 1
#define TABLE_PEN_TYPE PS_SOLID

VOID DrawTable(HWND hWnd, INT x, INT y, INT iWidth, INT iHeight, BOOL doubleBuffered)
{
    PAINTSTRUCT ps;
    HDC hWndDc;
    hWndDc = BeginPaint(hWnd, &ps);

    if (doubleBuffered) {
        INT iActualWidth = iWidth + TABLE_PEN_WIDTH - 1;
        INT iActualHeight = iHeight + TABLE_PEN_WIDTH - 1;

        HDC hMemDc;
        hMemDc = CreateCompatibleDC(hWndDc);
        HBITMAP hBmp = CreateCompatibleBitmap(hMemDc, iActualWidth, iActualHeight);
        SelectObject(hMemDc, hBmp);
        DrawTableFrame(hMemDc, 0, 0, iWidth, iHeight);
        BitBlt(hWndDc, x, y, iActualWidth, iActualHeight, hMemDc, 0, 0, SRCCOPY);
        DeleteDC(hMemDc);
        DeleteObject(hBmp);
    } else {
        DrawTableFrame(hWndDc, x, y, iWidth, iHeight);
    }

    EndPaint(hWnd, &ps);
}

VOID DrawTableFrame(HDC hDc, INT x, INT y, INT iWidth, INT iHeight)
{
    HBRUSH hBr = CreateSolidBrush(TABLE_BRUSH_COLOR);
    HBRUSH hOldBr = (HBRUSH)SelectObject(hDc, hBr);
    HPEN hPn = CreatePen(TABLE_PEN_TYPE, TABLE_PEN_WIDTH, TABLE_PEN_COLOR);
    HPEN hOldPn = (HPEN)SelectObject(hDc, hPn);

    Rectangle(hDc, x, y, x + iWidth, y + iHeight);

    SelectObject(hDc, hOldBr);
    DeleteObject(hBr);
    SelectObject(hDc, hOldPn);
    DeleteObject(hPn);
}