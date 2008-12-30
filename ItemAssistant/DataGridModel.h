#ifndef DATAGRIDMODEL_H
#define DATAGRIDMODEL_H

#include <boost/signal.hpp>

namespace aoia {

    /**
     * This is the interface a DataGridControl model will have to implement to be 
     * able to populate the DataGridControl.
     */
    class DataGridModel
    {
    public:
        typedef boost::signal<void (unsigned int)>  item_signal_t;
        typedef boost::signal<void ()>  collection_signal_t;
        typedef boost::signals::connection connection_t;

        DataGridModel();

        connection_t connectItemAdded(item_signal_t::slot_function_type subscriber);
        connection_t connectItemRemoved(item_signal_t::slot_function_type subscriber);
        connection_t connectAllRemoved(collection_signal_t::slot_function_type subscriber);
        void disconnect(connection_t subscriber);

        /// Return number of columns.
        virtual unsigned int getColumnCount() const = 0;

        /// Return the column header for a specific column.
        virtual std::tstring getColumnName(unsigned int index) const = 0;

        /// Return the number of items.
        virtual unsigned int getItemCount() const = 0;

        /// Return the name of a specified item.
        virtual std::tstring getItemProperty(unsigned int index, unsigned int column) const = 0;

    protected:
        void signalItemAdded(unsigned int index);
        void signalItemRemoved(unsigned int index);
        void signalAllCleared();

    private:
        /// Signal triggered when an item is added.
        item_signal_t m_itemAddedSignal;

        /// Signal triggered when an item has been removed.
        item_signal_t m_itemRemovedSignal;

        /// Signal triggered when all items have been removed.
        collection_signal_t m_allClearedSignal;
    };

    typedef boost::shared_ptr<DataGridModel> DataGridModelPtr;

}   // namespace

#endif // DATAGRIDMODEL_H
