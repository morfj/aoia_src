#ifndef TABFRAME_H
#define TABFRAME_H

#include <PluginSDK/PluginViewInterface.h>
#include "InventoryView.h"
#include "AoMsgView.h"
#include "PatternMatchView.h"
#include "PlayershopView.h"
#include "IdentifyView.h"


class TabFrame
    : public CTabbedChildWindow< CDotNetTabCtrl<CTabViewTabItem> >
{
    typedef CTabbedChildWindow< CDotNetTabCtrl<CTabViewTabItem> > baseClass;

public:
    TabFrame(void);
    virtual ~TabFrame(void);

    BEGIN_MSG_MAP(TabFrame)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnSelChange)
        CHAIN_MSG_MAP(baseClass)
    END_MSG_MAP()

    LRESULT OnSelChange(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    PluginViewInterface* GetActivePluginView();
    void OnAOServerMessage(AOMessageBase &msg);
    void OnAOClientMessage(AOMessageBase &msg);
    void SetToolBarPanel(HWND panel) { m_rebarControl.Attach(panel); }

    /// Sets the state of the toolbar for the active view.
    void SetToolbarVisibility(bool visible);

private:
    std::vector<PluginViewInterface*> m_viewPlugins;

    InventoryView       m_InventoryView;
    AoMsgView           m_MsgView;
    PatternMatchView    m_PatternView;
    PlayershopView      m_PlayershopView;
    IdentifyView        m_identifyView;

    WTL::CReBarCtrl m_rebarControl;
    WTL::CToolBarCtrl m_activeViewToolbar;

    bool m_toobarVisibility;
};

#endif // TABFRAME_H
