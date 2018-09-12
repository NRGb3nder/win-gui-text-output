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
    TextTable(HDC hDc, TableContent &content, int x, int y, int wdthTable,
        HFONT hFont, bool bAlignRows = true, int hghtEmptyTable = 0);
    void Draw();
    int GetHeight();
private:
    HDC m_hDc;
    TableContent m_content;
    int m_x;
    int m_y;
    int m_wdthTable;
    int m_hghtEmptyTable;
    bool m_bAlignRows;

    HFONT m_hFont;
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
    void CalculateMetrics();
    std::vector<int> GetCellHeights(TableContent &content);
    static int GetNumberOfColumns(TableContent &content);
    static int GetMaxElementLength(TableRowContent &row);
};