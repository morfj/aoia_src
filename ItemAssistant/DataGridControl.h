#ifndef DATAGRIDCONTROL_H
#define DATAGRIDCONTROL_H

#include "DataGridModel.h"
#include <set>


namespace aoia {

    /**
     * \brief This is a list-view control implemented using the standard Win32 
     * control set up as a virtual list-view control.
     *
     * Data is provided through a data-model deriving from DataGridModel class.
     *
     * \note
     * If you are embedding this control in your window or dialog, remember to 
     * add REFLECT_NOTIFICATIONS() to the end of your message map.
     */
    class DataGridControl
        : public ATL::CWindowImpl<DataGridControl>
    {
    public:
        DECLARE_WND_SUPERCLASS(_T("VirtualListView"), WC_LISTVIEW);

        DataGridControl();
        virtual ~DataGridControl();

        BEGIN_MSG_MAP(DataGridControl)
            MESSAGE_HANDLER(WM_CREATE, onCreate)
            REFLECTED_NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, onGetDispInfo)
            //REFLECTED_NOTIFY_CODE_HANDLER(LVN_ODCACHEHINT, onCacheHint)
            //REFLECTED_NOTIFY_CODE_HANDLER(LVN_ODFINDITEM, onFindItem)
        END_MSG_MAP()

        void setModel(DataGridModelPtr model);
        DataGridModelPtr getModel() const { return m_model; }

        void autosizeColumnsUseHeader();
        void autosizeColumnsUseData(bool onlyVisible = true);

        unsigned int getSelectedCount() const;
        std::set<unsigned int> getSelectedItems() const;

    protected:
        LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
        LRESULT onGetDispInfo(int wParam, LPNMHDR lParam, BOOL &bHandled);
        //LRESULT onCacheHint(int wParam, LPNMHDR lParam, BOOL &bHandled);
        //LRESULT onFindItem(int wParam, LPNMHDR lParam, BOOL &bHandled);

        void onAllItemsUpdated();
        void onItemAdded(unsigned int index);
        void onItemRemoved(unsigned int index);
        void onAllItemsRemoved();

        void updateColumns();

    private:
        DataGridModelPtr m_model;
        mutable WTL::CListViewCtrl m_listView;  // This is mutable because WTL is fucked up when it comes to const methods.

        boost::signals::connection m_addSignalConnection;
        boost::signals::connection m_removeSignalConnection;
        boost::signals::connection m_clearSignalConnection;
        boost::signals::connection m_updateSignalConnection;
    };

    typedef boost::shared_ptr<DataGridControl> DataGridControlPtr;

}   // namespace

#endif // DATAGRIDCONTROL_H
