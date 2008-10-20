#include "StdAfx.h"
#include "Tabframe.h"


TabFrame::TabFrame()
  : m_toobarVisibility(true)
{
    //SetForwardNotifications(true);
}


TabFrame::~TabFrame()
{
}


LRESULT TabFrame::OnSelChange(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
    PluginViewInterface* oldplugin = GetActivePluginView();
    baseClass::OnSelChange(0, pnmh, bHandled);
    PluginViewInterface* newplugin = GetActivePluginView();

    if (oldplugin) {
        oldplugin->OnActive(false);
    }

    if (newplugin)
    {
        newplugin->OnActive(true);

        // Assign new toolbar.
        m_activeViewToolbar.Detach();
        m_activeViewToolbar.Attach(newplugin->GetToolbar());

        int nBandIndex = m_rebarControl.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
        if (nBandIndex < 0)
        {
            // Insert new band
            WTL::CFrameWindowImplBase<>::AddSimpleReBarBandCtrl(m_rebarControl, m_activeViewToolbar, ATL_IDW_BAND_FIRST + 1, NULL, TRUE);
            m_rebarControl.ShowBand(m_rebarControl.IdToIndex(ATL_IDW_BAND_FIRST + 1), m_toobarVisibility);
            m_rebarControl.LockBands(true);
        }
        else
        {
            // Replace band
            REBARBANDINFO rbbi;
            ZeroMemory(&rbbi, sizeof(REBARBANDINFO));
            rbbi.cbSize = sizeof(REBARBANDINFO);
            rbbi.fMask = RBBIM_CHILD;
            rbbi.hwndChild = m_activeViewToolbar.m_hWnd;

            m_rebarControl.SetBandInfo(nBandIndex, &rbbi);
        }
    }

    bHandled = TRUE;
    return 0;
}


LRESULT TabFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    baseClass::OnCreate(uMsg, wParam, lParam, bHandled);

    DWORD style = WS_CHILD | /*WS_VISIBLE |*/ WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    m_InventoryView.Create(*this, 0, 0, style);
    AddTab(m_InventoryView, _T("Inventory"));
    m_viewPlugins.push_back(&m_InventoryView);

    m_PatternView.Create(*this, 0, 0, style);
    AddTab(m_PatternView, _T("Pattern Matcher"));
    m_viewPlugins.push_back(&m_PatternView);

    m_PlayershopView.Create(*this, 0, 0, style);
    AddTab(m_PlayershopView, _T("Playershop Monitor"));
    m_viewPlugins.push_back(&m_PlayershopView);

    m_PlayershopView.StartMonitoring();

    //m_BotExportView.Create(*this, 0, 0, style);
    //AddTab(m_BotExportView, "Bot Export");
    //m_viewPlugins.push_back(&m_BotExportView);

#ifdef _DEBUG
    m_MsgView.Create(*this, 0, 0, style);
    AddTab(m_MsgView, _T("Messages (Debug)"));
    m_viewPlugins.push_back(&m_MsgView);
#endif

    DisplayTab(m_InventoryView);

    return 0;
}


PluginViewInterface* TabFrame::GetActivePluginView()
{
    PluginViewInterface* result = NULL;

    HWND hWnd = GetActiveView();
    if (hWnd != NULL)
    {
        for (unsigned int i = 0; i < m_viewPlugins.size(); i++)
        {
            if (m_viewPlugins[i]->GetWindow() == hWnd)
            {
                result = m_viewPlugins[i];
                break;
            }
        }
    }

    return result;
}


void TabFrame::OnAOMessage(AOMessageBase &msg)
{
    for (unsigned int i = 0; i < m_viewPlugins.size(); i++)
    {
        m_viewPlugins[i]->OnAOMessage(msg);
    }
}


void TabFrame::SetToolbarVisibility(bool visible)
{
    m_toobarVisibility = visible;
    m_rebarControl.ShowBand(m_rebarControl.IdToIndex(ATL_IDW_BAND_FIRST + 1), m_toobarVisibility);
}
