#ifndef ITEMLISTDATAMODEL_H
#define ITEMLISTDATAMODEL_H

#include "DataGridModel.h"
#include <Shared/SQLite.h>
#include <set>


namespace aoia {

    /**
     * Datamodel that searches for all recorded items that matches a set of AOIDs.
     */
    class ItemListDataModel
        : public DataGridModel
    {
    public:
        ItemListDataModel(std::set<unsigned int> const& aoids);
        virtual ~ItemListDataModel();

        /// Return number of columns.
        virtual unsigned int getColumnCount() const;

        /// Return the column header for a specific column.
        virtual std::tstring getColumnName(unsigned int index) const;

        /// Return the number of items.
        virtual unsigned int getItemCount() const;

        /// Return the name of a specified item.
        virtual std::tstring getItemProperty(unsigned int index, unsigned int column) const;

        /// Return the AOID of the specified item.
        unsigned int getItemId(unsigned int index) const;

    protected:
        // Enumeration of publicly visible columns.
        enum ColumnID
        {
            COL_ITEM_NAME = 0,
            COL_ITEM_QL,
            COL_TOON_NAME,
            COL_BACKPACK_NAME,
            COL_BACKPACK_LOCATION,
            COL_COUNT       // This should always be last!
        };

    private:
        SQLite::TablePtr m_result;
    };

    typedef boost::shared_ptr<ItemListDataModel> ItemListDataModelPtr;

}

#endif // ITEMLISTDATAMODEL_H
