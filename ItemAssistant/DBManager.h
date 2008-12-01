#pragma once

#include <shared/SQLite.h>
#include <shared/Mutex.h>
#include <vector>


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
    //std::tstring inventoryname;
    //std::tstring inventoryid;
};

typedef boost::shared_ptr<OwnedItemInfo> OwnedItemInfoPtr;


class DBManager : public SQLite::Db
{
public:
    DBManager(void);
    virtual ~DBManager(void);

    bool Init(std::tstring dbfile);
    void Term();

    void InsertItem(unsigned int keylow, unsigned int keyhigh, unsigned short ql, unsigned short stack, 
        unsigned int parent, unsigned short slot, unsigned int children, unsigned int owner);

    std::tstring GetToonName(unsigned int charid) const;
    void SetToonName(unsigned int charid, std::tstring const& newName);

    void UpdateToonShopId(unsigned int charid, unsigned int shopid);
    unsigned int GetShopOwner(unsigned int shopid);

    OwnedItemInfoPtr GetOwnedItemInfo(unsigned int itemID);

    void Lock() { m_mutex.MutexOn(); }
    void UnLock() { m_mutex.MutexOff(); }

protected:
    bool SyncLocalItemsDB(std::tstring const& localfile, std::tstring const& aofolder);

    unsigned int GetAODBSchemeVersion(std::tstring const& filename) const;
    unsigned int GetDBVersion() const;
    void UpdateDBVersion(unsigned int fromVersion) const;
    void CreateDBScheme() const;

private:
    Mutex          m_mutex;
};
