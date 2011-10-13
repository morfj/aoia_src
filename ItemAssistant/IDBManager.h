#ifndef IDBMANAGER_H
#define IDBMANAGER_H


struct OwnedItemInfo
{
    std::tstring itemloid;
    std::tstring itemhiid;
    std::tstring itemql;
    std::tstring itemname;
    std::tstring ownername;
    std::tstring ownerid;
    std::tstring containername;
    std::tstring containerid;
    unsigned short flags;
    //std::tstring inventoryname;
    //std::tstring inventoryid;
};

typedef boost::shared_ptr<OwnedItemInfo> OwnedItemInfoPtr;
typedef std::map<unsigned int, unsigned int> StatMap;


struct IDBManager
{
    virtual ~IDBManager() {}

    /// Records an item with the specified properties as owned by a the character.
    virtual void InsertItem(unsigned int keylow, unsigned int keyhigh, unsigned short ql, unsigned short flags, unsigned short stack, 
        unsigned int parent, unsigned short slot, unsigned int children, unsigned int owner) = 0;

    /// Retrieve all known info about the item with the specified index.
    virtual OwnedItemInfoPtr GetOwnedItemInfo(unsigned int itemID) = 0;

    /// Store the specified name for the character ID
    virtual void SetToonName(unsigned int charid, std::tstring const& newName) = 0;

    /// Retrieve the stored name of a specified character ID
    virtual std::tstring GetToonName(unsigned int charid) const = 0;

    /// Records the specified shop ID for the character.
    virtual void SetToonShopId(unsigned int charid, unsigned int shopid) = 0;

    /// Retrieves the recorded shop ID for the specified character.
    virtual unsigned int GetToonShopId(unsigned int charid) const = 0;

    /// Retrieves the recorded (if any) character ID associated with the specified shop ID.
    virtual unsigned int GetShopOwner(unsigned int shopid) = 0;

    /// Assign a dimension ID to a specified character.
    virtual void SetToonDimension(unsigned int charid, unsigned int dimensionid) = 0;

    /// Retrieves the dimension a character belongs to. 0 means dimension is unknown.
    virtual unsigned int GetToonDimension(unsigned int charid) const = 0;

    /// Record the current stats for a specific toon.
    virtual void SetToonStats(unsigned int charid, StatMap const& stats) = 0;

    /// Retrieves all the dimension IDs and their descriptions. Returns false if query failed.
    virtual bool GetDimensions(std::map<unsigned int, std::tstring> &dimensions) const = 0;

    /// Determines the first available container slot ID for a specified character and container.
    virtual unsigned int FindNextAvailableContainerSlot(unsigned int charId, unsigned int containerId) = 0;

    //returns the properties value in the AO db for an item in a particular slot in a container.
    virtual unsigned int GetItemProperties(unsigned int charId, unsigned int containerId, unsigned int slot) = 0;

    //searches for items in containerIdToSearchIn with the same keylow and ql as the item specified
    virtual unsigned int FindFirstItemOfSameType(unsigned int charId, unsigned int containerId, unsigned int slot, unsigned int containerIdToSearchIn) = 0;

    /// Lock database for access.
    virtual void Lock() = 0;

    /// Release database lock.
    virtual void UnLock() = 0;
    
    // TODO : Remove this method. Encapsulate in specialized method instead.
    virtual SQLite::TablePtr ExecTable(std::wstring const& sql) const = 0;

    // TODO : Remove this method. Encapsulate in specialized method instead.
    virtual SQLite::TablePtr ExecTable(std::string const& sql) const = 0;

    // TODO : Remove this method. Encapsulate in specialized method instead.
    virtual bool Exec(std::wstring const& sql) const = 0;

    // TODO : Remove this method. Encapsulate in specialized method instead.
    virtual bool Exec(std::string const& sql) const = 0;

    // TODO : Remove this method. Encapsulate in specialized method instead.
    virtual void Begin() const = 0;

    // TODO : Remove this method. Encapsulate in specialized method instead.
    virtual void Commit() const = 0;

    // TODO : Remove this method. Encapsulate in specialized method instead.
    virtual void Rollback() const = 0;
};

typedef boost::shared_ptr<IDBManager> IDBManagerPtr;


#endif // IDBMANAGER_H
