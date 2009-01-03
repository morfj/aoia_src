#ifndef ITEMASSISTVIEW_H
#define ITEMASSISTVIEW_H

#include <PluginSDK/PluginViewInterface.h>


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
        // Override in derived classes
        return false;
    }

    virtual void OnAOServerMessage(AOMessageBase &msg)
    {
        // Override in derived classes
    }

    virtual void OnAOClientMessage(AOClientMessageBase &msg)
    {
        // Override in derived classes
    }

    virtual HWND GetWindow() const
    {
        return m_hWnd;
    }

    virtual void OnActive(bool doActivation)
    {
        // Override in derived classes
    }

    virtual HWND GetToolbar() const
    {
        return m_toolbar.m_hWnd;
    }

protected:
    WTL::CToolBarCtrl m_toolbar;
};

#endif // ITEMASSISTVIEW_H
