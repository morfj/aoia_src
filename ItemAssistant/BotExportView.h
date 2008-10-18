#pragma once


#include "shared/aopackets.h"
#include <PluginSDK/ItemAssistView.h>


class BotExportView
  : public ItemAssistView<BotExportView>
{
   typedef ItemAssistView<BotExportView> inherited;
public:
	DECLARE_WND_CLASS(NULL)

   BotExportView(void);
   virtual ~BotExportView(void);

	BEGIN_MSG_MAP_EX(BotExportView)
		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
      CHAIN_MSG_MAP(inherited)
      DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	virtual void OnAOMessage(AOMessageBase &msg);

protected:
   LRESULT OnCreate(LPCREATESTRUCT createStruct);
   LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

};
