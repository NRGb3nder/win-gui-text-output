#include "TextTable.h"

const COLORREF TextTable::TABLE_BRUSH_COLOR = RGB(255, 255, 255);
const COLORREF TextTable::TABLE_PEN_COLOR = RGB(0, 0, 0);
const int TextTable::TABLE_PEN_WIDTH = 1;
const int TextTable::TABLE_PEN_TYPE = PS_SOLID;

TextTable::TextTable(TableContent &content, int x, int y, int wdthTable, HFONT hFont,
    bool bAlignRows) : m_content(content),
    m_x(x), m_y(y), m_wdthTable(wdthTable), m_hFont(hFont), m_bAlignRows(bAlignRows), m_ahghtCell(nullptr)
{}

TextTable::~TextTable()
{
    if (m_ahghtCell) {
        delete m_ahghtCell;
    }
}

void TextTable::Draw(HWND hWnd)
{
    PAINTSTRUCT ps;
    const HDC hWndDc = BeginPaint(hWnd, &ps);

    HFONT hOldFont = (HFONT)SelectObject(hWndDc, m_hFont);
    TEXTMETRIC txtmtrc;
    GetTextMetrics(hWndDc, &txtmtrc);
    CalculateMetrics(txtmtrc);

    const int hghtTable = m_bAlignRows ? m_hghtTableMax : m_hghtTableActual;

    DrawTable(hWndDc);

    SelectObject(hWndDc, hOldFont);
    EndPaint(hWnd, &ps);
}

HFONT TextTable::SetFont(HFONT hFont)
{
    HFONT hOldFont = m_hFont;
    m_hFont = hFont;
    return hOldFont;
}

HFONT TextTable::GetFont()
{
    return m_hFont;
}

void TextTable::SetXY(int x, int y)
{
    m_x = x;
    m_y = y;
}

int TextTable::GetX()
{
    return m_x;
}

int TextTable::GetY()
{
    return m_y;
}

int TextTable::SetTableWidth(int wdth)
{
    int wdthTableOld = m_wdthTable;
    m_wdthTable = wdth;
    return wdthTableOld;
}

int TextTable::GetTableWidth()
{
    return m_wdthTable;
}

void TextTable::AlignRows(bool bOption)
{
    m_bAlignRows;
}

void TextTable::DrawTable(HDC hDc)
{
    const HBRUSH hBr = CreateSolidBrush(TABLE_BRUSH_COLOR);
    const HBRUSH hOldBr = (HBRUSH)SelectObject(hDc, hBr);
    const HPEN hPn = CreatePen(TABLE_PEN_TYPE, TABLE_PEN_WIDTH, TABLE_PEN_COLOR);
    const HPEN hOldPn = (HPEN)SelectObject(hDc, hPn);

    int xCell = m_x;
    int yCell = m_y;
    int hghtCell;
    for (INT iRow = 0; iRow < m_cRows; iRow++) {
        hghtCell = m_bAlignRows ? m_hghtCellMax : m_ahghtCell->at(iRow);
        for (INT iCol = 0; iCol < m_cCols; iCol++) {
            Rectangle(hDc, xCell - TABLE_PEN_WIDTH, yCell,
                xCell + m_wdthCell, yCell + hghtCell + TABLE_PEN_WIDTH);

            RECT rectCell;
            rectCell.left = xCell + TABLE_PEN_WIDTH;
            rectCell.top = yCell + TABLE_PEN_WIDTH;
            rectCell.right = xCell + m_wdthCell - TABLE_PEN_WIDTH;
            rectCell.bottom = yCell + hghtCell - TABLE_PEN_WIDTH;
            if (m_content.size() > iRow && m_content[iRow].size() > iCol) {
                DrawText(hDc, (LPCTSTR)m_content[iRow][iCol].c_str(), -1, &rectCell, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
            }
           
            xCell += m_wdthCell;
        }
        xCell = m_x;
        yCell += hghtCell;
    }

    SelectObject(hDc, hOldBr);
    DeleteObject(hBr);
    SelectObject(hDc, hOldPn);
    DeleteObject(hPn);
}

void TextTable::CalculateMetrics(TEXTMETRIC txtmtrc)
{
    m_cCols = TextTable::GetNumberOfColumns(m_content);

    auto fnCountElements = [](int acc, const TableRowContent &row) {
        return acc + row.size();
    };
    int cElements = std::accumulate(m_content.begin(), m_content.end(), 
        0, fnCountElements);
    m_cRows = (int)std::ceil((double)cElements / m_cCols);

    m_wdthCell = m_wdthTable / m_cCols;
    m_ahghtCell = GetCellHeights(m_content, txtmtrc);

    m_hghtCellMax = *std::max_element(m_ahghtCell->begin(), m_ahghtCell->end());
    m_hghtTableMax = m_hghtCellMax * m_ahghtCell->size();
    m_hghtTableActual = std::accumulate(m_ahghtCell->begin(), m_ahghtCell->end(), 0, std::plus<int>());
}

std::vector<int> *TextTable::GetCellHeights(TableContent &content, TEXTMETRIC txtmtrc) {

    const int wdthCellInChars = (int)std::ceil((double)m_wdthCell / (txtmtrc.tmAveCharWidth + txtmtrc.tmOverhang));
    std::vector<int> *ahghtCell = new std::vector<int>;
    for (TableRowContent &row : content) {
        ahghtCell->push_back((txtmtrc.tmExternalLeading + txtmtrc.tmHeight) * (int)std::ceil(((double)GetMaxElementLength(row) / wdthCellInChars)));
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
    for (std::string &element : row) {
        if (((int)element.length() > iMaxLength)) {
            iMaxLength = element.length();
        }
    }
    return iMaxLength;
}