#ifndef DATAGRIDCONTROL_H
#define DATAGRIDCONTROL_H

#include "DataGridModel.h"

namespace aoia {

    /**
    * Observer interface the for implementations of the DataGridModelInterface.
    */
    //struct DataGridModelObserverInterface
    //{
    //    virtual void onItemAdded(DataGridModelInterface* source, unsigned int index) = 0;
    //    virtual void onItemRemoved(DataGridModelInterface* source, unsigned int index) = 0;
    //    virtual void onAllItemsRemoved(DataGridModelInterface* source) = 0;
    //};
    //
    //typedef boost::shared_ptr<DataGridModelObserverInterface> DataGridModelObserverInterfacePtr;


    /**
    * Template for implementing a DataGridModelObserver.
    * T must implement onItemAdded, onItemRemoved and onAllItemsRemoved.
    */
    //template <typename T>
    //class DataGridModelObserver
    //  : public DataGridModelObserverInterface
    //{
    //    DataGridModelObserver(T& owner)
    //        : m_owner(owner)
    //    { }
    //
    //    /// \sa DataGridModelObserverInterface
    //    virtual void onItemAdded(DataGridModelInterface* source, unsigned int index) { m_owner.onItemAdded(index); }
    //    virtual void onItemRemoved(DataGridModelInterface* source, unsigned int index) { m_owner.onItemRemoved(index); }
    //    virtual void onAllItemsRemoved(DataGridModelInterface* source) { m_owner.onAllItemsRemoved(); }
    //
    //    T& m_owner;
    //};


    /**
     * \brief This is a list-view control implemented using the standard Win32 
     * control set up as a virtual list-view control.
     *
     * Data is provided through a data-model implementing DataGridModelInterface.
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

    protected:
        LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
        LRESULT onGetDispInfo(int wParam, LPNMHDR lParam, BOOL &bHandled);
        //LRESULT onCacheHint(int wParam, LPNMHDR lParam, BOOL &bHandled);
        //LRESULT onFindItem(int wParam, LPNMHDR lParam, BOOL &bHandled);

        void onItemAdded(unsigned int index);
        void onItemRemoved(unsigned int index);
        void onAllItemsRemoved();

        void updateColumns();
        void autosizeColumns();

    private:
        DataGridModelPtr m_model;
        mutable WTL::CListViewCtrl m_listView;  // This is mutable because WTL is fucked up when it comes to const methods.

        boost::signals::connection m_addSignalConnection;
        boost::signals::connection m_removeSignalConnection;
        boost::signals::connection m_clearSignalConnection;
    };

    typedef boost::shared_ptr<DataGridControl> DataGridControlPtr;

}   // namespace

#endif // DATAGRIDCONTROL_H
