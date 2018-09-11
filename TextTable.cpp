#include "TextTable.h"

const float TextTable::CHAR_WIDTH_COEFF = 1.5;
const COLORREF TextTable::TABLE_BRUSH_COLOR = RGB(255, 255, 255);
const COLORREF TextTable::TABLE_PEN_COLOR = RGB(0, 0, 0);
const int TextTable::TABLE_PEN_WIDTH = 1;
const int TextTable::TABLE_PEN_TYPE = PS_SOLID;

TextTable::TextTable(HDC hDc, TableContent &content, int x, int y, int wdthTable, HFONT hFont,
    bool bAlignRows) : m_hDc(hDc), m_content(content), m_x(x), m_y(y), m_wdthTable(wdthTable), 
    m_hFont(hFont), m_bAlignRows(bAlignRows), m_hghtTableMax(0), m_hghtTableActual(0)
{
    CalculateMetrics();
}

void TextTable::Draw()
{
    HFONT hOldFont = (HFONT)SelectObject(m_hDc, m_hFont);
    const HBRUSH hBr = CreateSolidBrush(TABLE_BRUSH_COLOR);
    const HBRUSH hOldBr = (HBRUSH)SelectObject(m_hDc, hBr);
    const HPEN hPn = CreatePen(TABLE_PEN_TYPE, TABLE_PEN_WIDTH, TABLE_PEN_COLOR);
    const HPEN hOldPn = (HPEN)SelectObject(m_hDc, hPn);

    int xCell = m_x;
    int yCell = m_y;
    int hghtCell;
    for (unsigned iRow = 0; iRow < m_cRows; iRow++) {
        hghtCell = m_bAlignRows ? m_hghtCellMax : m_ahghtCell.at(iRow);
        for (unsigned iCol = 0; iCol < m_cCols; iCol++) {
            Rectangle(m_hDc, xCell - TABLE_PEN_WIDTH, yCell,
                xCell + m_wdthCell, yCell + hghtCell + TABLE_PEN_WIDTH);

            RECT rectCell;
            rectCell.left = xCell + TABLE_PEN_WIDTH;
            rectCell.top = yCell + TABLE_PEN_WIDTH;
            rectCell.right = xCell + m_wdthCell - TABLE_PEN_WIDTH;
            rectCell.bottom = yCell + hghtCell - TABLE_PEN_WIDTH;
            if (m_content.size() > iRow && m_content[iRow].size() > iCol) {
                DrawText(m_hDc, (LPCTSTR)m_content[iRow][iCol].c_str(), -1, &rectCell,
                    DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
            }

            xCell += m_wdthCell;
        }
        xCell = m_x;
        yCell += hghtCell;
    }

    SelectObject(m_hDc, hOldPn);
    DeleteObject(hPn);
    SelectObject(m_hDc, hOldBr);
    DeleteObject(hBr);
    SelectObject(m_hDc, hOldFont);
}

int TextTable::GetHeight()
{
    return m_bAlignRows ? m_hghtTableMax : m_hghtTableActual;
}

void TextTable::CalculateMetrics()
{
    HFONT hOldFont = (HFONT)SelectObject(m_hDc, m_hFont);
    GetTextMetrics(m_hDc, &m_txtmtrc);
    SelectObject(m_hDc, hOldFont);

    m_cCols = TextTable::GetNumberOfColumns(m_content);
    m_cRows = m_content.size();

    m_wdthCell = m_wdthTable / m_cCols;
    m_ahghtCell = GetCellHeights(m_content);

    m_hghtCellMax = *std::max_element(m_ahghtCell.begin(), m_ahghtCell.end());
    m_hghtTableMax = m_hghtCellMax * m_ahghtCell.size();
    m_hghtTableActual = std::accumulate(m_ahghtCell.begin(), m_ahghtCell.end(), 0, std::plus<int>());
}

std::vector<int> TextTable::GetCellHeights(TableContent &content) {

    const int wdthCellInChars = (int)std::ceil((double)m_wdthCell / (m_txtmtrc.tmAveCharWidth * CHAR_WIDTH_COEFF + m_txtmtrc.tmOverhang));
    std::vector<int> ahghtCell;
    for (TableRowContent &row : content) {
        ahghtCell.push_back(
            (m_txtmtrc.tmInternalLeading + m_txtmtrc.tmExternalLeading + m_txtmtrc.tmHeight) *
            (int)std::ceil(((double)GetMaxElementLength(row) / wdthCellInChars)));
    }
    return ahghtCell;
}

int TextTable::GetNumberOfColumns(TableContent &content)
{
    int iCols = 0;
    for (TableRowContent &row : content)
    {
        if ((int)row.size() > iCols) {
            iCols = row.size();
        }
    }
    return iCols;
}

int TextTable::GetMaxElementLength(TableRowContent &row)
{
    int iMaxLength = 0;
    for (std::wstring &element : row) {
        if (((int)element.length() > iMaxLength)) {
            iMaxLength = element.length();
        }
    }
    return iMaxLength;
}