#ifndef IDENTIFYVIEW_H
#define IDENTIFYVIEW_H

#include <PluginSDK/ItemAssistView.h>

class IdentifyView
    : public ItemAssistView<IdentifyView>
{
    typedef ItemAssistView<IdentifyView> inherited;

public:
    DECLARE_WND_CLASS(NULL)

    IdentifyView();
    virtual ~IdentifyView();

    BEGIN_MSG_MAP_EX(IdentifyView)
        MSG_WM_CREATE(onCreate)
        MSG_WM_SIZE(onSize)
        CHAIN_MSG_MAP(inherited)
        REFLECT_NOTIFICATIONS()
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

protected:
    LRESULT onCreate(LPCREATESTRUCT createStruct);
    LRESULT onSize(UINT wParam, CSize newSize);

    struct Identifyable
    {
        unsigned int fromId;    // HighID of the source item. (What will turn in to this identifyable item)
        unsigned int lowId;     // LowID of destination item.
        unsigned int highId;    // HighID of destination item.

    };

private:
    WTL::CListViewCtrl m_identifyableList;
    WTL::CListViewCtrl m_itemList;
};

#endif // IDENTIFYVIEW_H
