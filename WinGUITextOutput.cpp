#include <windows.h>
#include <fstream>
#include <sstream>
#include <codecvt>
#include "TextTable.h"

#define E_FILE_NOT_OPENED "Can not open file"

#define WINDOW_CLASS_NAME L"WinGUITextOutputMainWindow"
#define WINDOW_TITLE L"Windows GUI Text Output Lab"
#define WINDOW_DEFAULT_WIDTH 1000
#define WINDOW_DEFAULT_HEIGHT 500
#define WINDOW_MIN_WIDTH 400
#define WINDOW_MIN_HEIGHT 400

#define TABLE_HORIZONTAL_MARGIN (WINDOW_MIN_WIDTH / 10)
#define TABLE_VERTICAL_MARGIN (WINDOW_MIN_HEIGHT / 10)

#define VSCROLLBAR_STEP 5

#define CHECKBOX_ALIGN_ROWS_CLASS_NAME L"Button"
#define CHECKBOX_ALIGN_ROWS_CAPTION L"Align Rows"
#define CHECKBOX_ALIGN_ROWS_X TABLE_HORIZONTAL_MARGIN
#define CHECKBOX_ALIGN_ROWS_Y (TABLE_VERTICAL_MARGIN / 3)
#define CHECKBOX_ALIGN_ROWS_WIDTH 100
#define CHECKBOX_ALIGN_ROWS_HEIGHT 20

#define IDM_HELP 0x00
#define IDM_AUTHOR 0x01
#define IDM_OPEN 0x02
#define ID_ALIGN_ROWS_CHECKBOX 0x03

#define FILE_MENU_ITEM_TITLE L"File"
#define FILE_OPEN_MENU_ITEM_TITLE L"Open"
#define HELP_MENU_ITEM_TITLE L"Help"
#define AUTHOR_MENU_ITEM_TITLE L"Author"

#define TEXT_ERROR_CAPTION L"Error"
#define TEXT_FILE_NOT_OPENED L"Can not open file"
#define TEXT_HELP L"No help here right now"
#define TEXT_ABOUT_AUTHOR L"Arseni Rynkevich\r\nStudent group 651003\r\nhttps://github.com/NRGb3nder"

#define OPEN_DIALOG_DEFAULT_EXTENSION L"txt"
#define OPEN_DIALOG_FILTER L"Text Files (*.txt)\0*.txt\0"

#define TABLE_DELIMITER '|'

VOID CreateMainWindowClass(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

VOID OnCreate(HWND hWnd, LPARAM lParam);
VOID OnDestroy();
VOID OnCommand(HWND hWnd, WPARAM wParam);
VOID OnTryPaint(HWND hWnd);
VOID OnTryGetResizeInfo(LPARAM lParam);
VOID OnTryVerticalScroll(HWND hWnd, WPARAM wParam);

VOID AddMenu(HWND hWnd);
VOID ShowOpenTableFileDialog(HWND hWnd);
TTableContent GetFileContent(std::wstring sPath, WCHAR cDelimiter);
std::wstring Trim(std::wstring &s);

HMENU hMenu;
HMENU hFileSubMenu;
HWND hCheckBox;
HFONT hFont;

INT yScrollBarPos = 0;
INT hghtTable = 0;

TTableContent content;
BOOL bAlignRows = FALSE;

INT CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
    CreateMainWindowClass(hInstance);

    CONST HWND hWnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, 
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX |
        WS_VISIBLE | WS_VSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, 
        WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

VOID CreateMainWindowClass(HINSTANCE hInstance)
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
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CREATE:
        OnCreate(hWnd, lParam);
        break;
    case WM_DESTROY:
        OnDestroy();
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        OnCommand(hWnd, wParam);
        break;
    case WM_PAINT:
        OnTryPaint(hWnd);
        break;
    case WM_GETMINMAXINFO:
        OnTryGetResizeInfo(lParam);
        break;
    case WM_VSCROLL:
        OnTryVerticalScroll(hWnd, wParam);
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

VOID OnCreate(HWND hWnd, LPARAM lParam)
{
    ShowScrollBar(hWnd, SB_VERT, FALSE);
    hFont = CreateFont(18, 0, 0, 0, 300, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial Narrow");
    AddMenu(hWnd);
    hCheckBox = CreateWindow(CHECKBOX_ALIGN_ROWS_CLASS_NAME,
        CHECKBOX_ALIGN_ROWS_CAPTION,
        WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
        CHECKBOX_ALIGN_ROWS_X, CHECKBOX_ALIGN_ROWS_Y,
        CHECKBOX_ALIGN_ROWS_WIDTH, CHECKBOX_ALIGN_ROWS_HEIGHT,
        hWnd, (HMENU)ID_ALIGN_ROWS_CHECKBOX,
        ((LPCREATESTRUCT)lParam)->hInstance, NULL);
}

VOID OnDestroy()
{
    DeleteObject(hFont);
}

VOID OnCommand(HWND hWnd, WPARAM wParam)
{
    switch (wParam) {
    case IDM_HELP:
        MessageBox(hWnd, TEXT_HELP, HELP_MENU_ITEM_TITLE, MB_OK | MB_ICONINFORMATION);
        break;
    case IDM_AUTHOR:
        MessageBox(hWnd, TEXT_ABOUT_AUTHOR, AUTHOR_MENU_ITEM_TITLE, MB_OK | MB_ICONINFORMATION);
        break;
    case IDM_OPEN:
        ShowOpenTableFileDialog(hWnd);
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        break;
    case ID_ALIGN_ROWS_CHECKBOX:
        CheckDlgButton(hWnd, ID_ALIGN_ROWS_CHECKBOX, bAlignRows ? BST_UNCHECKED : BST_CHECKED);
        bAlignRows = !bAlignRows;
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        break;
    }
}

VOID OnTryPaint(HWND hWnd)
{   
    PAINTSTRUCT ps;
    CONST HDC hWndDc = BeginPaint(hWnd, &ps);

    RECT wndRect;
    GetClientRect(hWnd, &wndRect);
    CONST INT hghtWnd = wndRect.bottom - wndRect.top;
    CONST INT wdthWnd = wndRect.right - wndRect.left;
    CONST INT wdthTable = wdthWnd - 2 * TABLE_HORIZONTAL_MARGIN;

    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    GetScrollInfo(hWnd, SB_VERT, &si);

    MoveWindow(hCheckBox, CHECKBOX_ALIGN_ROWS_X, CHECKBOX_ALIGN_ROWS_Y - si.nPos,
        CHECKBOX_ALIGN_ROWS_WIDTH, CHECKBOX_ALIGN_ROWS_HEIGHT, TRUE);

    TextTable *textTable = new TextTable(hWndDc, content, TABLE_HORIZONTAL_MARGIN, 
        TABLE_VERTICAL_MARGIN - si.nPos, wdthTable, hFont, bAlignRows, hghtWnd - 2 * TABLE_VERTICAL_MARGIN);

    GetScrollInfo(hWnd, SB_VERT, &si);
    hghtTable = textTable->GetHeight();
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE;
    si.nMin = 0;
    si.nMax = hghtTable + TABLE_VERTICAL_MARGIN * 2 - hghtWnd;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    textTable->Draw();

    delete textTable;

    EndPaint(hWnd, &ps);
}

VOID OnTryGetResizeInfo(LPARAM lParam)
{
    LPMINMAXINFO lpMMI;
    lpMMI = (LPMINMAXINFO)lParam;
    lpMMI->ptMinTrackSize.x = WINDOW_MIN_WIDTH;
    lpMMI->ptMinTrackSize.y = WINDOW_MIN_HEIGHT;
}

VOID OnTryVerticalScroll(HWND hWnd, WPARAM wParam)
{
    SCROLLINFO si;
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    GetScrollInfo(hWnd, SB_VERT, &si);

    switch (LOWORD(wParam)) {
    case SB_LINEUP:
    case SB_PAGEUP:
        si.nPos -= VSCROLLBAR_STEP;
        break;
    case SB_LINEDOWN:
    case SB_PAGEDOWN:
        si.nPos += VSCROLLBAR_STEP;
        break;
    case SB_THUMBTRACK:
        si.nPos = si.nTrackPos;
        break;
    }

    si.fMask = SIF_POS;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
    GetScrollInfo(hWnd, SB_VERT, &si);

    if (si.nPos != yScrollBarPos)
    {
        ScrollWindow(hWnd, 0, yScrollBarPos - si.nPos, NULL, NULL);
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        yScrollBarPos = si.nPos;
    }
}

VOID AddMenu(HWND hWnd)
{
    hMenu = CreateMenu();
    hFileSubMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileSubMenu, FILE_MENU_ITEM_TITLE);
    AppendMenu(hFileSubMenu, MF_STRING, IDM_OPEN, FILE_OPEN_MENU_ITEM_TITLE);
    AppendMenu(hMenu, MF_STRING, IDM_HELP, HELP_MENU_ITEM_TITLE);
    AppendMenu(hMenu, MF_STRING, IDM_AUTHOR, AUTHOR_MENU_ITEM_TITLE);
    SetMenu(hWnd, hMenu);
}

VOID ShowOpenTableFileDialog(HWND hWnd)
{
    OPENFILENAME ofn;
    WCHAR szFileName[MAX_PATH];

    ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = OPEN_DIALOG_FILTER;
    ofn.lpstrFile = NULL;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = OPEN_DIALOG_DEFAULT_EXTENSION;
    ofn.lpstrFile = szFileName;
    ofn.lpstrFile[0] = '\0';

    if (GetOpenFileName(&ofn)) {
        try {
            content = GetFileContent(szFileName, TABLE_DELIMITER);
        } catch (std::exception *e) {
            MessageBox(hWnd, TEXT_FILE_NOT_OPENED, TEXT_ERROR_CAPTION, MB_OK | MB_ICONERROR);
            delete e;
        }
    }
}

TTableContent GetFileContent(std::wstring sPath, WCHAR cDelimiter)
{
    std::wifstream ifs(sPath);
    ifs.imbue(std::locale(ifs.getloc(), new std::codecvt_utf8<WCHAR>));
    if (!ifs) {
        throw new std::exception(E_FILE_NOT_OPENED);
    }

    TTableContent content;
    std::wstring sLine;

    std::getline(ifs, sLine);
    if (!(ifs.eof() && Trim(sLine).empty())) {
        do {
            if (sLine.size()) {
                std::wistringstream iss(sLine);
                TTableRowContent rowContent;
                std::wstring sWord;
                while (std::getline(iss, sWord, cDelimiter)) {
                    rowContent.push_back(Trim(sWord));
                }
                content.push_back(rowContent);
            }
        } while (std::getline(ifs, sLine));
    }

    return content;
}

std::wstring Trim(std::wstring &s)
{
    CONST std::wstring WHITESPACE = L" \n\r\t\f\v\uFEFF";
    SIZE_T iStart = s.find_first_not_of(WHITESPACE);
    if (iStart == std::string::npos) {
        return L"";
    }
    SIZE_T iEnd = s.find_last_not_of(WHITESPACE);
    return s.substr(iStart, iEnd - iStart + 1);
}