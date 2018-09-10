#pragma once

#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
#include <vector>
#include <windows.h>
#include <tchar.h>

typedef std::vector<std::string> TableRowContent;
typedef std::vector<TableRowContent> TableContent; 

class TextTable
{
public:
    TextTable(TableContent &content, int x, int y, int wdthTable, HFONT hFont,
        bool bAlignRows = true);
    ~TextTable();
    void Draw(HWND hWnd);
    HFONT SetFont(HFONT hFont);
    HFONT GetFont();
    void SetXY(int x, int y);
    int GetX();
    int GetY();
    int SetTableWidth(int wdth);
    int GetTableWidth();
    void AlignRows(bool bOption);
private:
    HFONT m_hFont;
    TableContent &m_content;
    int m_x;
    int m_y;
    int m_wdthTable;
    bool m_bAlignRows;

    int m_wdthCell;
    std::vector<int> *m_ahghtCell;
    int m_hghtCellMax;
    int m_hghtTableMax;
    int m_hghtTableActual;
    int m_cCols;
    int m_cRows;
    static const COLORREF TABLE_BRUSH_COLOR;
    static const COLORREF TABLE_PEN_COLOR;
    static const int TABLE_PEN_WIDTH;
    static const int TABLE_PEN_TYPE;

    TextTable(const TextTable &) = delete;
    void DrawTable(HDC hDc);
    void CalculateMetrics(TEXTMETRIC txtmtrc);
    std::vector<int> *GetCellHeights(TableContent &content, TEXTMETRIC txtmtrc);
    static int GetNumberOfColumns(TableContent &content);
    static int GetMaxElementLength(TableRowContent &row);
};