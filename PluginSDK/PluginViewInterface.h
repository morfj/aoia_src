
#pragma once

#include "shared/aopackets.h"


struct PluginViewInterface
{
	virtual void OnAOMessage(AO::Header *pMsg) = 0;
   virtual bool PreTranslateMsg(MSG* pMsg) = 0;
   virtual HWND GetWindow() const = 0;
   virtual void OnActive(bool doActivation) = 0;
};
