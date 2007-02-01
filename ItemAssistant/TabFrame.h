#pragma once

#include <PluginSDK/PluginViewInterface.h>
#include "InventoryView.h"
#include "AoMsgView.h"
#include "PatternMatchView.h"
#include "BotExportView.h"
#include "PlayershopView.h"

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
   void OnAOMessage(AO::Header *pMsg);

private:
   std::vector<PluginViewInterface*> m_viewPlugins;

   InventoryView              m_InventoryView;
	AoMsgView                  m_MsgView;
   PatternMatchView           m_PatternView;
   PlayershopView             m_PlayershopView;
   BotExportView              m_BotExportView;
};
