#include "StdAfx.h"
#include "Tabframe.h"


TabFrame::TabFrame(void)
{
}


TabFrame::~TabFrame(void)
{
}


LRESULT TabFrame::OnSelChange(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
   PluginViewInterface* oldplugin = GetActivePluginView();

   baseClass::OnSelChange(0, pnmh, bHandled);

   PluginViewInterface* newplugin = GetActivePluginView();

   if (oldplugin)
   {
      oldplugin->OnActive(false);
   }

   if (newplugin)
   {
      newplugin->OnActive(true);
   }

   //for (unsigned int i = 0; i < m_viewPlugins.size(); i++)
   //{
   //   if (m_viewPlugins[i] == plugin)
   //   {
   //      plugin->OnActive(true);
   //   }
   //   else
   //   {
   //      plugin->OnActive(false);
   //   }
   //}

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


void TabFrame::OnAOMessage(AO::Header *pMsg)
{
	for (unsigned int i = 0; i < m_viewPlugins.size(); i++)
	{
		m_viewPlugins[i]->OnAOMessage(pMsg);
	}
}
