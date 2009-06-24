#include "StdAfx.h"
#include "FindPanel.h"
#include "InventoryView.h"
#include <ItemAssistantCore/SettingsManager.h>

using namespace aoia;

FindView::FindView()
    : m_lastQueryChar(-1)
    , m_lastQueryQlMin(-1)
    , m_lastQueryQlMax(-1)
    , m_lastQueryDimension(0)
    , m_pParent(NULL)
{
}


void FindView::SetParent(InventoryView* parent)
{
    m_pParent = parent;
}


LRESULT FindView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    this->SetWindowText(_T("Find View"));

    updateDimensionList();

    CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);

    if (cb.GetCount() > 0)
    {
        int index = CB_ERR;
        if (!SettingsManager::instance().getValue(_T("DefaultDimension")).empty())
        {
            index = cb.FindStringExact(-1, SettingsManager::instance().getValue(_T("DefaultDimension")).c_str());
        }
        if (index == CB_ERR)
        {
            index = 0;
        }
        cb.SetCurSel(index);
        updateCharList(cb.GetItemData(index));
        m_lastQueryDimension = cb.GetItemData(index);

        CComboBox toon_combo = GetDlgItem(IDC_CHARCOMBO);
        if (toon_combo.GetCount() > 0)
        {
            toon_combo.SetCurSel(0);
        }
    }

    DlgResize_Init(false, true, WS_CLIPCHILDREN);
    return 0;
}


LRESULT FindView::OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    LPMSG pMsg = (LPMSG)lParam;
    return this->PreTranslateMsg(pMsg);
}


BOOL FindView::PreTranslateMsg(MSG* pMsg)
{
    return IsDialogMessage(pMsg);
}


LRESULT FindView::onDimensionFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    KillTimer(1);

    WTL::CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);
    int oldselection = cb.GetCurSel();

    updateDimensionList();

    if (oldselection >= 0)
    {
        cb.SetCurSel(oldselection);
    }
    else if (oldselection == -1)
    {
        cb.SetCurSel(0);
    }

    return 0;
}


LRESULT FindView::onDimensionSelection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);
    unsigned int dimension_id = 0;
    int item = -1;
    if ((item = cb.GetCurSel()) != CB_ERR)
    {
        dimension_id = (unsigned int)cb.GetItemData(item);
        TCHAR buffer[256];
        cb.GetLBText(item, buffer);
        SettingsManager::instance().setValue(_T("DefaultDimension"), buffer);
    }

    updateCharList(dimension_id);
    CComboBox toon_combo = GetDlgItem(IDC_CHARCOMBO);
    if (toon_combo.GetCount() > 0)
    {
        toon_combo.SetCurSel(0);
    }

    if (dimension_id != m_lastQueryDimension)
    {
        UpdateFindQuery();
    }

    return 0;
}


LRESULT FindView::OnEnChangeItemtext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    SetTimer(1, 1500);
    return 0;
}


LRESULT FindView::OnCbnSelChangeCharcombo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    UpdateFindQuery();
    return 0;
}


LRESULT FindView::OnTimer(UINT wParam, TIMERPROC lParam)
{
    if (wParam == 1)
    {
        UpdateFindQuery();
        KillTimer(1);
    }
    return 0;
}


void FindView::UpdateFindQuery()
{
    KillTimer(1);

    int item = -1;
    CComboBox cb = GetDlgItem(IDC_CHARCOMBO);
    CComboBox dimension_combo = GetDlgItem(IDC_DIMENSION_COMBO);
    CEdit eb = GetDlgItem(IDC_ITEMTEXT);
    CEdit qlmin = GetDlgItem(IDC_QLMIN);
    CEdit qlmax = GetDlgItem(IDC_QLMAX);

    unsigned int charid = 0;
    item = -1;
    if ((item = cb.GetCurSel()) != CB_ERR)
    {
        charid = (unsigned int)cb.GetItemData(item);
    }

    int dimension_id = -1;
    item = -1;
    if ((item = dimension_combo.GetCurSel()) != CB_ERR)
    {
        dimension_id = (unsigned int)dimension_combo.GetItemData(item);
    }

    TCHAR buffer[MAX_PATH];
    ZeroMemory(buffer, MAX_PATH);
    eb.GetWindowText(buffer, MAX_PATH);
    std::tstring text(buffer);

    int minql = -1;
    ZeroMemory(buffer, MAX_PATH);
    qlmin.GetWindowText(buffer, MAX_PATH);
    std::tstring qlminText(buffer);
    try {
        minql = boost::lexical_cast<int>(qlminText);
    }
    catch(boost::bad_lexical_cast &/*e*/) {
        // Go with the default value
    }

    int maxql = -1;
    ZeroMemory(buffer, MAX_PATH);
    qlmax.GetWindowText(buffer, MAX_PATH);
    std::tstring qlmaxText(buffer);
    try {
        maxql = boost::lexical_cast<int>(qlmaxText);
    }
    catch(boost::bad_lexical_cast &/*e*/) {
        // Go with the default value
    }

    if (text.size() > 2
        && ( m_lastQueryText != text
            || m_lastQueryChar != charid 
            || m_lastQueryQlMin != minql 
            || m_lastQueryQlMax != maxql 
            || m_lastQueryDimension != dimension_id))
    {
        m_lastQueryText = text;
        m_lastQueryChar = charid;
        m_lastQueryQlMin = minql;
        m_lastQueryQlMax = maxql;
        m_lastQueryDimension = dimension_id;
        std::ostringstream sql;

        if (charid > 0) {
            sql << "I.owner = " << charid << " AND ";
        }
        else if (dimension_id > -1)
        {
            sql << CreateSqlPredicate(m_dimensions[dimension_id]) << " AND ";
        }
        if (minql > -1) {
            sql << "I.ql >= " << minql << " AND ";
        }
        if (maxql > -1) {
            sql << "I.ql <= " << maxql << " AND ";
        }

        sql << "keylow IN (SELECT aoid FROM aodb.tblAO WHERE name LIKE \"%" << to_ascii_copy(text) << "%\")";

        m_pParent->UpdateListView(from_ascii_copy(sql.str()));
    }
}


std::string FindView::CreateSqlPredicate(AOManager::DimensionInfo const& dimension) const
{
    std::ostringstream ips;
    for (std::vector<unsigned int>::const_iterator it = dimension.server_ip.begin(); it != dimension.server_ip.end(); ++it)
    {
        if (it != dimension.server_ip.begin())
        {
            ips << ", ";
        }
        ips << *it;
    }

    std::ostringstream sql;
    sql << "T.serverport=" << dimension.server_port << " AND T.serverip IN (" << ips.str() << ")";
    return sql.str();
}


void FindView::updateCharList(int dimension_id)
{
    CComboBox cb = GetDlgItem(IDC_CHARCOMBO);

    cb.ResetContent();
    int item = cb.AddString(_T("-"));
    cb.SetItemData(item, 0);

    std::ostringstream sql;
    sql << "SELECT DISTINCT owner FROM tItems I JOIN tToons T ON I.owner = T.charid ";
    if (dimension_id > -1)
    {
        sql << "WHERE " << CreateSqlPredicate(m_dimensions[dimension_id]);
    }
    sql << " ORDER BY T.charname";

    g_DBManager.lock();
    SQLite::TablePtr pT = g_DBManager.ExecTable(sql.str());
    g_DBManager.unLock();

    if (pT != NULL)
    {
        for (unsigned int i = 0; i < pT->Rows(); i++)
        {
            unsigned int id = boost::lexical_cast<unsigned int>(pT->Data(i,0));

            g_DBManager.lock();
            std::tstring name = g_DBManager.getToonName(id);
            g_DBManager.unLock();

            if (name.empty())
            {
                name = from_ascii_copy(pT->Data()[pT->Columns()*i]);
            }

            if ((item = cb.AddString(name.c_str())) != CB_ERR)
            {
                cb.SetItemData(item, id);
            }
        }
    }
}


void FindView::updateDimensionList()
{
    int item = -1;

    CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);
    cb.ResetContent();

    m_dimensions = AOManager::instance().getDimensions();

    for (unsigned int i = 0; i < m_dimensions.size(); ++i)
    {
        item = cb.AddString(from_ascii_copy(m_dimensions[i].description).c_str());
        if (item != CB_ERR)
        {
            cb.SetItemData(item, i);
        }
    }

    item = cb.AddString(_T("-"));
    if (item != CB_ERR)
    {
        cb.SetItemData(item, -1);
    }
}
