#include <windows.h>
#include <fstream>
#include <sstream>
#include <codecvt>
#include "TextTable.h"

#define ERROR_CAPTION L"Error"
#define E_FILE_NOT_OPENED "Can not open file"

#define WINDOW_CLASS_NAME L"WinGUITextOutputMainWindow"
#define WINDOW_TITLE L"Windows GUI Text Output Lab"
#define DEFAULT_WINDOW_WIDTH 1000
#define DEFAULT_WINDOW_HEIGHT 500
#define MIN_WINDOW_WIDTH 400
#define MIN_WINDOW_HEIGHT 400

#define IDM_HELP 0x00
#define IDM_AUTHOR 0x01
#define IDM_OPEN 0x02

#define FILE_MENU_ITEM_TITLE L"File"
#define FILE_OPEN_MENU_ITEM_TITLE L"Open"
#define HELP_MENU_ITEM_TITLE L"Help"
#define AUTHOR_MENU_ITEM_TITLE L"Author"

#define TEXT_FILE_NOT_OPENED L"Can not open file"
#define TEXT_HELP L"No help here right now"
#define TEXT_ABOUT_AUTHOR L"Arseni Rynkevich\r\nStudent group 651003\r\nhttps://github.com/NRGb3nder"

#define TABLE_HORIZONTAL_MARGIN (MIN_WINDOW_WIDTH / 10)
#define TABLE_VERTICAL_MARGIN (MIN_WINDOW_HEIGHT / 10)

#define TABLE_DELIMITER '|'

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void AddMenu(HWND hWnd);
void ShowOpenTableFileDialog(HWND hWnd);
void OnPaint(HWND hWnd);
TableContent GetFileContent(std::wstring sPath, wchar_t cDelimiter);

HMENU hMenu;
HMENU hFileSubMenu;
HFONT hFont;

TableContent content;

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

    const HWND hWnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW | 
        WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, 
        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

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
    RECT rectWnd;

    switch (message) {
    case WM_PAINT:
        OnPaint(hWnd);
        break;
    case WM_CREATE:
        AddMenu(hWnd);
        hFont = CreateFont(18, 0, 0, 0, 300, false, false, false, 
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
            DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial Narrow");
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
            MessageBox(hWnd, TEXT_HELP, HELP_MENU_ITEM_TITLE, MB_OK | MB_ICONINFORMATION);
            break;
        case IDM_AUTHOR:
            MessageBox(hWnd, TEXT_ABOUT_AUTHOR, AUTHOR_MENU_ITEM_TITLE, MB_OK | MB_ICONINFORMATION);
            break;
        case IDM_OPEN:
            ShowOpenTableFileDialog(hWnd);
            GetClientRect(hWnd, &rectWnd);
            InvalidateRect(hWnd, &rectWnd, true);
            break;
        }
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

void ShowOpenTableFileDialog(HWND hWnd)
{
    OPENFILENAME ofn;
    wchar_t szFileName[MAX_PATH];

    ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0";
    ofn.lpstrFile = NULL;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"txt";
    ofn.lpstrFile = szFileName;
    ofn.lpstrFile[0] = '\0';

    if (GetOpenFileName(&ofn)) {
        try {
            content = GetFileContent(szFileName, TABLE_DELIMITER);
        } catch (std::exception *e) {
            MessageBox(hWnd, TEXT_FILE_NOT_OPENED, ERROR_CAPTION, MB_OK | MB_ICONERROR);
            delete e;
        }
    }
}

void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    const HDC hWndDc = BeginPaint(hWnd, &ps);
    if (content.size()) {
        RECT wndRect;
        GetClientRect(hWnd, &wndRect);
        const int wdthWnd = wndRect.right - wndRect.left;

        const int wdthTable = wdthWnd - 2 * TABLE_HORIZONTAL_MARGIN;
        TextTable *textTable = new TextTable(content, TABLE_HORIZONTAL_MARGIN, TABLE_VERTICAL_MARGIN, wdthTable, hFont, false);
        textTable->Draw(hWndDc);
        delete textTable;
    }
    EndPaint(hWnd, &ps);
}

TableContent GetFileContent(std::wstring sPath, wchar_t cDelimiter)
{
    std::wifstream ifs(sPath);
    ifs.imbue(std::locale(ifs.getloc(), new std::codecvt_utf8<wchar_t>));
    if (!ifs) {
        throw new std::exception("Can not open file");
    }

    TableContent content;
    std::wstring sLine;
    while (std::getline(ifs, sLine)) {
        if (!sLine.empty()) {
            std::wistringstream iss(sLine);
            TableRowContent rowContent;
            std::wstring sWord;
            while (std::getline(iss, sWord, cDelimiter)) {
                rowContent.push_back(sWord);
            }
            content.push_back(rowContent);
        }
    }

    return content;
}