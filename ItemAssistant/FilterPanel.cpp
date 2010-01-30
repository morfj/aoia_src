#include "StdAfx.h"
#include "FilterPanel.h"
#include "DBManager.h"
#include <ItemAssistantCore/SettingsManager.h>

using namespace aoia;

namespace PatternMatcher {

    FilterPanel::FilterPanel()
    {
    }


    FilterPanel::~FilterPanel()
    {
    }


    LRESULT FilterPanel::onInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        SetWindowText(_T("Filter View"));

        updateDimensionList();
        updateCharList();

        CheckDlgButton(IDC_SHOW_ALL, 1);

        WTL::CProgressBarCtrl progressbar = GetDlgItem(IDC_PROGRESS);
        progressbar.SetRange(0, 100);
        progressbar.SetPos(0);

        signalSettingsChanged();

        return 0;
    }


    LRESULT FilterPanel::onForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        LPMSG pMsg = (LPMSG)lParam;
        return this->PreTranslateMsg(pMsg);
    }


    void FilterPanel::updateDimensionList()
    {
        WTL::CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);

        int oldselection = cb.GetCurSel();
        int item = 0;

        cb.ResetContent();

        std::map<unsigned int, std::tstring> dimensionNames;
        g_DBManager.lock();
        g_DBManager.getDimensions(dimensionNames);
        SQLite::TablePtr pT = g_DBManager.ExecTable(_T("SELECT DISTINCT dimensionid FROM tToons"));
        g_DBManager.unLock();

        // Add named dimensions.
        for (std::map<unsigned int, std::tstring>::iterator it = dimensionNames.begin(); it != dimensionNames.end(); ++it)
        {
            if ((item = cb.AddString(it->second.c_str())) != CB_ERR)
            {
                cb.SetItemData(item, it->first);
            }
        }

        // Add un-named dimensions.
        for (unsigned int i = 0; i < pT->Rows(); ++i)
        {
            unsigned int dimId = boost::lexical_cast<unsigned int>(pT->Data(i, 0));
            std::tstring dimName;
            if (dimensionNames.find(dimId) != dimensionNames.end())
            {
                continue;   // Skip named ones.
            }
            else 
            {
                dimName = _T("Unknown Dimension");
                if (dimId > 0)
                {
                    dimName += STREAM2STR(" (0x" << std::hex << dimId << ")");
                }
            }

            if ((item = cb.AddString(dimName.c_str())) != CB_ERR)
            {
                cb.SetItemData(item, dimId);
            }
        }

        if (oldselection >= 0)
        {
            cb.SetCurSel(oldselection);
        }
        else if (oldselection == -1)
        {
            if (!SettingsManager::instance().getValue(_T("DefaultDimension")).empty())
            {
                oldselection = cb.FindStringExact(-1, SettingsManager::instance().getValue(_T("DefaultDimension")).c_str());
            }
            if (oldselection == CB_ERR)
            {
                oldselection = 0;
            }
            cb.SetCurSel(oldselection);
        }
    }


    void FilterPanel::updateCharList()
    {
        CComboBox cb = GetDlgItem(IDC_CHARCOMBO);
        if (cb.GetCount() < 1)
        {
            WTL::CComboBox cb = GetDlgItem(IDC_CHARCOMBO);
            cb.ResetContent();
            int item = cb.AddString(_T("-"));
            cb.SetItemData(item, 0);
            cb.SetCurSel(0);
            return;
        }

        unsigned int prev_selected_toon = cb.GetItemData(cb.GetCurSel());

        cb.ResetContent();
        int item = cb.AddString(_T("-"));
        cb.SetItemData(item, 0);
        cb.SetCurSel(0);

        boost::format sql("SELECT DISTINCT owner FROM tItems I JOIN tToons T ON I.owner = T.charid WHERE dimensionid = %1% ORDER BY T.charname");
        sql % getDimensionId();

        g_DBManager.lock();
        SQLite::TablePtr pT = g_DBManager.ExecTable(sql.str());

        if (pT != NULL)
        {
            for (unsigned int i = 0; i < pT->Rows(); i++)
            {
                unsigned int id = boost::lexical_cast<unsigned int>(pT->Data(i,0));

                std::tstring name = g_DBManager.getToonName(id);
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
        g_DBManager.unLock();

        bool found = false;
        for (int i = 0; i < cb.GetCount(); ++i)
        {
            unsigned int data = cb.GetItemData(i);
            if (data == prev_selected_toon)
            {
                cb.SetCurSel(i);
                found = true;
                break;
            }
        }

        if (!found)
        {
            cb.SetCurSel(0);
        }
    }


    void FilterPanel::setProgress(unsigned short percent)
    {
        WTL::CProgressBarCtrl progressbar = GetDlgItem(IDC_PROGRESS);
        progressbar.SetPos(percent);
    }


    unsigned int FilterPanel::getDimensionId() const
    {
        WTL::CComboBox dimCb = GetDlgItem(IDC_DIMENSION_COMBO);
        return (unsigned int) dimCb.GetItemData(dimCb.GetCurSel());
    }


    unsigned int FilterPanel::getCharId() const
    {
        unsigned int toonid = 0;

        WTL::CComboBox tooncb = GetDlgItem(IDC_CHARCOMBO);
        if (tooncb.GetCurSel() > 0)
        {
            toonid = (unsigned int)tooncb.GetItemData(tooncb.GetCurSel());
        }

        return toonid;
    }


    float FilterPanel::getAvailFilter() const
    {
        float availfilter = -1.0f;

        if (IsDlgButtonChecked(IDC_SHOW_ALL))
        {
            availfilter = -1.0f;
        }
        else if (IsDlgButtonChecked(IDC_SHOW_PARTIALS))
        {
            availfilter = 0.25f;
        }
        else if (IsDlgButtonChecked(IDC_COMPLETABLE))
        {
            availfilter = 1.0f;
        }

        return availfilter;
    }


    bool FilterPanel::getExcludeAssembled() const
    {
        return IsDlgButtonChecked(IDC_EXCLUDE_ASSEMBLED) == TRUE ? true : false;
    }


    BOOL FilterPanel::PreTranslateMsg(MSG* pMsg)
    {
        return IsDialogMessage(pMsg);
    }


    LRESULT FilterPanel::onDimensionComboSelection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CComboBox cb = GetDlgItem(IDC_DIMENSION_COMBO);
        TCHAR buffer[256];
        cb.GetLBText(cb.GetCurSel(), buffer);
        SettingsManager::instance().setValue(_T("DefaultDimension"), buffer);

        updateCharList();
        signalSettingsChanged();
        return 0;
    }


    LRESULT FilterPanel::onCbnSelchangeCharcombo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        signalSettingsChanged();
        return 0;
    }


    LRESULT FilterPanel::onBnClickedShowAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        signalSettingsChanged();
        return 0;
    }


    LRESULT FilterPanel::onBnClickedShowPartials(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        signalSettingsChanged();
        return 0;
    }


    LRESULT FilterPanel::onBnClickedCompletable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        signalSettingsChanged();
        return 0;
    }


    LRESULT FilterPanel::OnCbnDropdown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        updateCharList();
        return 0;
    }


    LRESULT FilterPanel::onExcludeAssembledPatternsClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        signalSettingsChanged();
        return 0;
    }


    void FilterPanel::disconnect(Connection subscriber)
    {
        subscriber.disconnect();
    }


    FilterPanel::Connection FilterPanel::connectSettingsChanged(SettingsChangedSignal::slot_function_type subscriber)
    {
        return m_settingsChangedSignal.connect(subscriber);
    }


    void FilterPanel::signalSettingsChanged()
    {
        m_settingsChangedSignal();
    }

}   // namespace