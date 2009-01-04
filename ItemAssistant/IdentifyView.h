#ifndef IDENTIFYVIEW_H
#define IDENTIFYVIEW_H

#include <PluginSDK/ItemAssistView.h>
#include "DataGridControl.h"
#include "IdentifyListDataModel.h"
#include <atlsplit.h>


class IdentifyView
    : public ItemAssistView<IdentifyView>
    , public WTL::CDialogResize<IdentifyView>
{
    typedef ItemAssistView<IdentifyView> inherited;

public:
    DECLARE_WND_CLASS(NULL)

    IdentifyView();
    virtual ~IdentifyView();

    BEGIN_MSG_MAP_EX(IdentifyView)
        MSG_WM_CREATE(onCreate)
        NOTIFY_CODE_HANDLER_EX(LVN_ITEMCHANGING, onListItemChanging)
        CHAIN_MSG_MAP(inherited)
        CHAIN_MSG_MAP(WTL::CDialogResize<IdentifyView>)
        REFLECT_NOTIFICATIONS()
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    enum ChildIDs
    {
        IDW_SPLITTER = 1,
        IDW_IDENTLIST,
        IDW_DATAGRID,
    };

    BEGIN_DLGRESIZE_MAP(IdentifyView)
        DLGRESIZE_CONTROL(IDW_SPLITTER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        //DLGRESIZE_CONTROL(IDC_IDENTLIST, DLSZ_SIZE_Y)
        //DLGRESIZE_CONTROL(IDC_DATAGRID, DLSZ_SIZE_X | DLSZ_SIZE_Y)
    END_DLGRESIZE_MAP()

protected:
    LRESULT onCreate(LPCREATESTRUCT createStruct);
    LRESULT onListItemChanging(LPNMHDR lParam);

    struct Identifyable
    {
        unsigned int fromId;    // HighID of the source item. (What will turn in to this identifyable item)
        unsigned int lowId;     // LowID of destination item.
        unsigned int highId;    // HighID of destination item.

    };

private:
    WTL::CSplitterWindow m_splitter;
    aoia::IdentifyListDataModelPtr m_identifyListModel;
    aoia::DataGridControlPtr m_identifyList;
    aoia::DataGridControlPtr m_datagrid;
};

#endif // IDENTIFYVIEW_H
