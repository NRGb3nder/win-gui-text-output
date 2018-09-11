#pragma once

#include <cmath>
#include <algorithm>
#include <numeric>
#include <string>
#include <vector>
#include <windows.h>

typedef std::vector<std::wstring> TableRowContent;
typedef std::vector<TableRowContent> TableContent; 

class TextTable
{
public:
    TextTable(TableContent content, int x, int y, int wdthTable, HFONT hFont,
        bool bAlignRows = true);
    void Draw(HDC hDc);
    HFONT SetFont(HFONT hFont);
    HFONT GetFont();
    TableContent &SetContent(TableContent content);
    TableContent &GetContent();
    void SetXY(int x, int y);
    int GetX();
    int GetY();
    int SetTableWidth(int wdth);
    int GetTableWidth();
    void AlignRows(bool bOption);
private:
    HFONT m_hFont;
    TableContent m_content;
    int m_x;
    int m_y;
    int m_wdthTable;
    bool m_bAlignRows;

    TEXTMETRIC m_txtmtrc;
    int m_wdthCell;
    std::vector<int> m_ahghtCell;
    int m_hghtCellMax;
    int m_hghtTableMax;
    int m_hghtTableActual;
    unsigned m_cCols;
    unsigned m_cRows;
    static const float CHAR_WIDTH_COEFF;
    static const COLORREF TABLE_BRUSH_COLOR;
    static const COLORREF TABLE_PEN_COLOR;
    static const int TABLE_PEN_WIDTH;
    static const int TABLE_PEN_TYPE;

    TextTable(const TextTable &) = delete;
    void DrawTable(HDC hDc);
    void CalculateMetrics(HDC hWndDc);
    std::vector<int> GetCellHeights(TableContent &content);
    static int GetNumberOfColumns(TableContent &content);
    static int GetMaxElementLength(TableRowContent &row);
};