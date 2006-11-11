#pragma once

#include "PluginViewInterface.h"


template < class T >
class ItemAssistView
   : public CWindowImpl<T>
   , public PluginViewInterface
{
   typedef ItemAssistView<T> ThisType;
public:
	BEGIN_MSG_MAP_EX(ThisType)
      MSG_WM_ERASEBKGND(OnEraseBkgnd)
	END_MSG_MAP()

   LRESULT OnEraseBkgnd(HDC dc) { return 1; }

   ItemAssistView(void)
   {
   }

   virtual ~ItemAssistView(void)
   {
   }

   virtual bool PreTranslateMsg(MSG* pMsg)
   {
      // Override in derive classes
      return false;
   }

   virtual void OnAOMessage(AO::Header *pMsg)
   {
      // Override in derive classes
   }

   virtual HWND GetWindow() const
   {
      return m_hWnd;
   }

   virtual void OnActive(bool doActivation)
   {
   }
};
