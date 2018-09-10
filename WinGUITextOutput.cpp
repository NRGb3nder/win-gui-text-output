#include <windows.h>
#include <tchar.h>
#include "TextTable.h"

#define WINDOW_CLASS_NAME _T("WinGUITextOutputMainWindow")
#define WINDOW_TITLE _T("Windows GUI Text Output Lab")
#define DEFAULT_WINDOW_WIDTH 1000
#define DEFAULT_WINDOW_HEIGHT 500
#define MIN_WINDOW_WIDTH 400
#define MIN_WINDOW_HEIGHT 400

#define IDM_HELP 0x00
#define IDM_AUTHOR 0x01
#define IDM_OPEN 0x02

#define FILE_MENU_ITEM_TITLE _T("File")
#define FILE_OPEN_MENU_ITEM_TITLE _T("Open")
#define HELP_MENU_ITEM_TITLE _T("Help")
#define AUTHOR_MENU_ITEM_TITLE _T("Author")

#define TEXT_HELP _T("No help here right now.")
#define TEXT_ABOUT_AUTHOR _T("Arseni Rynkevich\r\nStudent group 651003\r\nhttps://github.com/NRGb3nder")

#define TABLE_HORIZONTAL_MARGIN (MIN_WINDOW_WIDTH / 10)
#define TABLE_VERTICAL_MARGIN (MIN_WINDOW_HEIGHT / 10)

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void AddMenu(HWND hWnd);
void OnPaint(HWND hWnd);

HMENU hMenu;
HMENU hFileSubMenu;
HFONT hFont;

bool useDoubleBuffering = true;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wndc = { 0 };
    wndc.style = CS_HREDRAW | CS_VREDRAW;
    wndc.lpfnWndProc = WndProc;
    wndc.hInstance = hInstance;
    wndc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wndc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wndc.lpszClassName = WINDOW_CLASS_NAME;

    RegisterClass(&wndc);

    const HWND hWnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPMINMAXINFO lpMMI;

    switch (message) {
    case WM_CREATE:
        AddMenu(hWnd);
        hFont = CreateFont(0, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Arial");
        break;
    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        break;
    case WM_GETMINMAXINFO:
        lpMMI = (LPMINMAXINFO)lParam;
        lpMMI->ptMinTrackSize.x = MIN_WINDOW_WIDTH;
        lpMMI->ptMinTrackSize.y = MIN_WINDOW_HEIGHT;
        break;
    case WM_COMMAND:
        switch (wParam) {
        case IDM_HELP:
            MessageBox(hWnd, TEXT_HELP, HELP_MENU_ITEM_TITLE, MB_OK);
            break;
        case IDM_AUTHOR:
            MessageBox(hWnd, TEXT_ABOUT_AUTHOR, AUTHOR_MENU_ITEM_TITLE, MB_OK);
            break;
        }
        break;
    case WM_PAINT:
        OnPaint(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void AddMenu(HWND hWnd)
{
    hMenu = CreateMenu();
    hFileSubMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileSubMenu, FILE_MENU_ITEM_TITLE);
    AppendMenu(hFileSubMenu, MF_STRING, IDM_OPEN, FILE_OPEN_MENU_ITEM_TITLE);
    AppendMenu(hMenu, MF_STRING, IDM_HELP, HELP_MENU_ITEM_TITLE);
    AppendMenu(hMenu, MF_STRING, IDM_AUTHOR, AUTHOR_MENU_ITEM_TITLE);
    SetMenu(hWnd, hMenu);
}

void OnPaint(HWND hWnd)
{
    RECT wndRect;
    GetClientRect(hWnd, &wndRect);
    const int wdthWnd = wndRect.right - wndRect.left;

   const int wdthTable = wdthWnd - 2 * TABLE_HORIZONTAL_MARGIN;
    TableContent content = { 
        { _T("Oneasfasfasfafasf"), _T("Twasfasfasfasfasfasfaso"), _T("Thrasfasffsfsee") },
        { _T("Fosfsfsfsfsfur"), _T("Fsfihashashahve") },
        { _T("Shahhahashsahashahashashasashaix"), _T("Seven"), _T("Eighsahashashasht"), _T("Nihne") }
    };
    TextTable *textTable = new TextTable(content, TABLE_HORIZONTAL_MARGIN, TABLE_VERTICAL_MARGIN, wdthTable, hFont, false);
    textTable->Draw(hWnd);
}