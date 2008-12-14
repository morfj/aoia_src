#include "StdAfx.h"
#include "IdentifyView.h"
#include <Shared/SQLite.h>
#include "DBManager.h"


using namespace SQLite;


IdentifyView::IdentifyView()
{
}


IdentifyView::~IdentifyView()
{
}


LRESULT IdentifyView::onCreate(LPCREATESTRUCT createStruct)
{
    // Create child windows
    DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
    m_identifyableList.Create(m_hWnd, rcDefault, NULL, style, WS_EX_CLIENTEDGE);
    m_itemList.Create(m_hWnd, rcDefault, NULL, style, WS_EX_CLIENTEDGE);

    // Populate the list of items we can identify
    TablePtr pTable = g_DBManager.ExecTable(_T("SELECT I.aoid, AO.name, I.purpose FROM tblIdentify I JOIN tblAO AO ON I.highid = AO.aoid"));

    if (pTable)
    {
        m_identifyableList.AddColumn(_T("Item"), 0);
        m_identifyableList.AddColumn(_T("Purpose"), 1);

        int index = INT_MAX - 1;
        for (unsigned int i = 0; i < pTable->Rows(); ++i)
        {
            try 
            {
                unsigned int aoid = boost::lexical_cast<unsigned int>(pTable->Data(i, 0));
                std::string name = pTable->Data(i, 1);
                std::string purpose = pTable->Data(i, 2);

                index = m_identifyableList.AddItem(index, 0, from_ascii_copy(name).c_str());
                index = m_identifyableList.AddItem(index, 1, from_ascii_copy(purpose).c_str());
                m_identifyableList.SetItemData(index, aoid);
            }
            catch (boost::bad_lexical_cast &/*e*/)
            {
                // Skip this row
            }
        }
    }

    return 0;
}


LRESULT IdentifyView::onSize(UINT wParam, CSize newSize)
{
    m_identifyableList.SetWindowPos(NULL, 0, 0, newSize.cx / 3, newSize.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_DEFERERASE | SWP_NOSENDCHANGING);
    m_itemList.SetWindowPos(NULL, newSize.cx / 3, 0, newSize.cx, newSize.cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE | SWP_DEFERERASE | SWP_NOSENDCHANGING);

    return 0;
}
