#include "stdafx.h"
#include "InvTreeItems.h"
#include "InventoryView.h"
#include <sstream>


SqlTreeViewItemBase::SqlTreeViewItemBase(InventoryView* pOwner)
    : m_pOwner(pOwner)
{
}


SqlTreeViewItemBase::~SqlTreeViewItemBase()
{
}


void SqlTreeViewItemBase::SetOwner(InventoryView* pOwner) 
{
    m_pOwner = pOwner;
}


unsigned int SqlTreeViewItemBase::AppendMenuCmd(HMENU hMenu, unsigned int firstID, WTL::CTreeItem item) const
{
    return firstID;
}


bool SqlTreeViewItemBase::HandleMenuCmd(unsigned int commandID, WTL::CTreeItem item)
{
    return false;
}


void SqlTreeViewItemBase::SetLabel(std::tstring const& newLabel)
{
}


/***************************************************************************/
/** Container Tree View Item                                              **/
/***************************************************************************/

ContainerTreeViewItem::ContainerTreeViewItem(
    InventoryView* pOwner, unsigned int charid, unsigned int containerid, 
    std::tstring const& constraints, std::tstring const& label)
    : m_charid(charid)
    , m_containerid(containerid)
    , m_constraints(constraints)
    , SqlTreeViewItemBase(pOwner)
{
    if (label.empty())
    {
        std::tstringstream str;
        std::tstring containerName = ServicesSingleton::Instance()->GetContainerName(m_charid, m_containerid);

        if (containerName.empty())
        {
            str << "Backpack: " << m_containerid;
        }
        else
        {
            str << containerName;
        }

        m_label = str.str();
    }
    else
    {
        m_label = label;
    }
}


ContainerTreeViewItem::~ContainerTreeViewItem()
{
}


void ContainerTreeViewItem::OnSelected() 
{
    std::tstringstream sql;
    sql << _T("owner = ") << m_charid << _T(" AND parent ");

    if (m_containerid == 0)
    {
        sql << _T("> 3 AND parent NOT IN (SELECT DISTINCT children FROM tItems)");
    }
    else
    {
        sql << _T(" = ") << m_containerid;
    }

    if (!m_constraints.empty())
    {
        sql << _T(" AND ") << m_constraints;
    }

    m_pOwner->UpdateListView(sql.str());
}


bool ContainerTreeViewItem::CanEdit() const
{
    return false;
}


std::tstring ContainerTreeViewItem::GetLabel() const
{
    return m_label;
}


bool ContainerTreeViewItem::HasChildren() const
{
    bool result = false;

    if (m_containerid == 1 || m_containerid == 2)
    {
        // Init contents from DB
        std::tstringstream sql;
        sql << _T("SELECT DISTINCT children FROM tItems WHERE children > 0 AND parent = ") << m_containerid << _T(" AND owner = ") << m_charid;
        if (!m_constraints.empty())
        {
            sql << _T(" AND ") << m_constraints;
        }

        g_DBManager.lock();
        SQLite::TablePtr pT = g_DBManager.ExecTable(sql.str());
        g_DBManager.unLock();

        if (pT != NULL)
        {
            result = pT->Rows() > 0;
        }
    }

    return result;
}


std::vector<MFTreeViewItem*> ContainerTreeViewItem::GetChildren() const
{
    std::vector<MFTreeViewItem*> result;

    if (m_containerid == 1 || m_containerid == 2)
    {
        // Init contents from DB
        std::tstringstream sql;
        sql << _T("SELECT children FROM tItems WHERE children > 0 AND parent = ") << m_containerid << _T(" AND owner = ") << m_charid;
        if (!m_constraints.empty())
        {
            sql << _T(" AND ") << m_constraints;
        }

        g_DBManager.lock();
        SQLite::TablePtr pT = g_DBManager.ExecTable(sql.str());
        g_DBManager.unLock();

        if (pT != NULL)
        {
            for (size_t i = 0; i < pT->Rows(); ++i)
            {
                unsigned int contid = boost::lexical_cast<unsigned int>(pT->Data(i,0));
                result.push_back(new ContainerTreeViewItem(m_pOwner, m_charid, contid));
            }
        }
    }

    return result;
}


unsigned int ContainerTreeViewItem::AppendMenuCmd(HMENU hMenu, unsigned int firstID, WTL::CTreeItem item) const
{
    if (m_containerid != 0)
    {
        m_commands[firstID] = SqlTreeViewItemBase::CMD_DELETE;
        AppendMenu(hMenu, MF_STRING, firstID++, _T("Delete Items From DB"));
    }
    return firstID;
}


bool ContainerTreeViewItem::HandleMenuCmd(unsigned int commandID, WTL::CTreeItem item)
{
    if (m_commands.find(commandID) != m_commands.end())
    {
        switch(m_commands[commandID])
        {
        case SqlTreeViewItemBase::CMD_DELETE:
            {
                g_DBManager.lock();
                g_DBManager.Begin();
                std::tstringstream sql;
                sql << _T("DELETE FROM tItems WHERE parent = ") << m_containerid << _T("; DELETE FROM tItems WHERE children = ") << m_containerid;
                if (g_DBManager.Exec(sql.str()))
                {
                    g_DBManager.Commit();
                }
                else
                {
                    g_DBManager.Rollback();
                }
                g_DBManager.unLock();
            }
            break;
        default:
            break;
        }
    }
    else
    {
        return false;
    }
    return true;
}


/***************************************************************************/
/** Character Tree View Item                                              **/
/***************************************************************************/

CharacterTreeViewItem::CharacterTreeViewItem(InventoryView* pOwner, unsigned int charid)
    : m_charid(charid)
    , SqlTreeViewItemBase(pOwner)
{
    g_DBManager.lock();
    m_label = g_DBManager.getToonName(charid);
    g_DBManager.unLock();

    if (m_label.empty())
    {
        std::tstringstream str;
        str << charid;
        m_label = str.str();
    }
}


CharacterTreeViewItem::~CharacterTreeViewItem()
{
}


void CharacterTreeViewItem::OnSelected() 
{ 
    std::tstringstream str;
    str << _T("owner = ") << m_charid;
    m_pOwner->UpdateListView(str.str());
}


bool CharacterTreeViewItem::CanEdit() const
{
    return false;
}


std::tstring CharacterTreeViewItem::GetLabel() const
{
    g_DBManager.lock();
    std::tstring result = g_DBManager.getToonName(m_charid);
    g_DBManager.unLock();

    if (result.empty())
    {
        std::tstringstream str;
        str << m_charid;
        result = str.str();
    }

    return result;
}


void CharacterTreeViewItem::SetLabel(std::tstring const& newLabel)
{
    g_DBManager.lock();
    g_DBManager.setToonName(m_charid, newLabel);
    g_DBManager.unLock();
}


bool CharacterTreeViewItem::HasChildren() const
{
    return true;
}


std::vector<MFTreeViewItem*> CharacterTreeViewItem::GetChildren() const
{
    std::vector<MFTreeViewItem*> result;

    result.push_back(new ContainerTreeViewItem(m_pOwner, m_charid, 1, _T(""), _T("Bank"))); // bank
    result.push_back(new ContainerTreeViewItem(m_pOwner, m_charid, 2, _T("slot > 63"), _T("Inventory"))); // inventory
    result.push_back(new ContainerTreeViewItem(m_pOwner, m_charid, 2, _T("slot < 16"), _T("Weapons"))); // Weapons tab
    result.push_back(new ContainerTreeViewItem(m_pOwner, m_charid, 2, _T("slot >= 16 AND slot < 32"), _T("Cloth"))); // Armor tab
    result.push_back(new ContainerTreeViewItem(m_pOwner, m_charid, 2, _T("slot >= 32 AND slot < 47"), _T("Implants"))); // Implants tab
    result.push_back(new ContainerTreeViewItem(m_pOwner, m_charid, 2, _T("slot >= 47 AND slot < 64"), _T("Social"))); // Social tab
    result.push_back(new ContainerTreeViewItem(m_pOwner, m_charid, 3, _T(""), _T("Shop"))); // Player shop
#ifdef DEBUG
    result.push_back(new ContainerTreeViewItem(m_pOwner, m_charid, 0, _T(""), _T("Unknown"))); // Unknown
#endif

    return result;
}


unsigned int CharacterTreeViewItem::AppendMenuCmd(HMENU hMenu, unsigned int firstID, WTL::CTreeItem item) const
{
    m_commands[firstID] = SqlTreeViewItemBase::CMD_DELETE;
    AppendMenu(hMenu, MF_STRING, firstID++, _T("Delete Toon"));
    m_commands[firstID] = SqlTreeViewItemBase::CMD_EXPORT;
    AppendMenu(hMenu, MF_STRING, firstID++, _T("Export Items..."));
    return firstID;
}


bool CharacterTreeViewItem::HandleMenuCmd(unsigned int commandID, WTL::CTreeItem item)
{
    if (m_commands.find(commandID) != m_commands.end())
    {
        switch(m_commands[commandID])
        {
        case SqlTreeViewItemBase::CMD_DELETE:
            {
                bool ok = true;
                g_DBManager.lock();
                g_DBManager.Begin();
                {
                    std::tstringstream sql;
                    sql << _T("DELETE FROM tItems WHERE owner = ") << m_charid;
                    ok = g_DBManager.Exec(sql.str());
                }
                if (ok)
                {
                    std::tstringstream sql;
                    sql << _T("DELETE FROM tToons WHERE charid = ") << m_charid;
                    ok = g_DBManager.Exec(sql.str());
                }
                if (ok)
                {
                    g_DBManager.Commit();
                }
                else
                {
                    g_DBManager.Rollback();
                }
                g_DBManager.unLock();
            }
            break;

        case SqlTreeViewItemBase::CMD_EXPORT:
            {
                std::tstringstream str;
                str << _T("owner = ") << m_charid;
                m_pOwner->exportToCSV(str.str());
            }
            break;

        default:
            break;
        }
    }
    else
    {
        return false;
    }
    return true;
}


/***************************************************************************/
/** Dimension Node                                                        **/
/***************************************************************************/

DimensionNode::DimensionNode(AOManager::DimensionInfo dimension, InventoryView* pOwner)
    : SqlTreeViewItemBase(pOwner)
    , m_label(from_ascii_copy(dimension.description))
    , m_dimension(dimension)
{
}


DimensionNode::~DimensionNode()
{
}


void DimensionNode::OnSelected()
{
}


std::vector<MFTreeViewItem*> DimensionNode::GetChildren() const
{
    std::vector<MFTreeViewItem*> result;

    // Get list of toons for this dimension from the DB
    g_DBManager.lock();
    SQLite::TablePtr pT = g_DBManager.ExecTable(GetSqlQuery());
    g_DBManager.unLock();

    if (pT != NULL)
    {
        for (size_t i = 0; i < pT->Rows(); ++i)
        {
            unsigned int charId = boost::lexical_cast<unsigned int>(pT->Data(i,0));
            result.push_back(new CharacterTreeViewItem(m_pOwner, charId));
        }
    }

    return result;
}


bool DimensionNode::HasChildren() const
{
    g_DBManager.lock();
    SQLite::TablePtr pT = g_DBManager.ExecTable(GetSqlQuery());
    g_DBManager.unLock();

    if (pT != NULL && pT->Rows() > 0)
    {
        return boost::lexical_cast<unsigned int>(pT->Data(0,0)) > 0 ? true : false;
    }

    return false;
}


std::tstring DimensionNode::GetSqlQuery() const
{
    std::tostringstream ips;
    for (std::vector<unsigned int>::const_iterator it = m_dimension.server_ip.begin(); it != m_dimension.server_ip.end(); ++it)
    {
        if (it != m_dimension.server_ip.begin())
        {
            ips << _T(", ");
        }
        ips << *it;
    }

    return STREAM2STR("SELECT charid FROM tToons WHERE serverport="
        << m_dimension.server_port << " AND serverip IN (" << ips.str() << ")");
}
