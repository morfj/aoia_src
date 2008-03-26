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
      sql << _T("> 2 AND parent NOT IN (SELECT DISTINCT children FROM tItems)");
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

      g_DBManager.Lock();
      SQLite::TablePtr pT = g_DBManager.ExecTable(sql.str());
      g_DBManager.UnLock();

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

      g_DBManager.Lock();
      SQLite::TablePtr pT = g_DBManager.ExecTable(sql.str());
      g_DBManager.UnLock();

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
      m_commands[firstID] = InternalCommand::CMD_DELETE;
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
      case InternalCommand::CMD_DELETE:
         {
            g_DBManager.Lock();
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
            g_DBManager.UnLock();
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



CharacterTreeViewItem::CharacterTreeViewItem(InventoryView* pOwner, unsigned int charid)
 : m_charid(charid)
 , SqlTreeViewItemBase(pOwner)
{
   g_DBManager.Lock();
   m_label = g_DBManager.GetToonName(charid);
   g_DBManager.UnLock();

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
   g_DBManager.Lock();
   std::tstring result = g_DBManager.GetToonName(m_charid);
   g_DBManager.UnLock();

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
   g_DBManager.Lock();
   g_DBManager.SetToonName(m_charid, newLabel);
   g_DBManager.UnLock();
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
   result.push_back(new ContainerTreeViewItem(m_pOwner, m_charid, 0, _T(""), _T("Unknown"))); // Unknown

   return result;
}


unsigned int CharacterTreeViewItem::AppendMenuCmd(HMENU hMenu, unsigned int firstID, WTL::CTreeItem item) const
{
   //m_commands[firstID] = InternalCommand::CMD_EDIT;
   //AppendMenu(hMenu, MF_STRING, firstID++, _T("Edit Toon Name"));
   m_commands[firstID] = InternalCommand::CMD_DELETE;
   AppendMenu(hMenu, MF_STRING, firstID++, _T("Delete Items From DB"));
   return firstID;
}


bool CharacterTreeViewItem::HandleMenuCmd(unsigned int commandID, WTL::CTreeItem item)
{
   if (m_commands.find(commandID) != m_commands.end())
   {
      switch(m_commands[commandID])
      {
      case InternalCommand::CMD_EDIT:
         {
            item.EditLabel();
         }
         break;
      case InternalCommand::CMD_DELETE:
         {
            g_DBManager.Lock();
            g_DBManager.Begin();
            std::tstringstream sql;
            sql << _T("DELETE FROM tItems WHERE owner = ") << m_charid;
            if (g_DBManager.Exec(sql.str()))
            {
               g_DBManager.Commit();
            }
            else
            {
               g_DBManager.Rollback();
            }
            g_DBManager.UnLock();
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



InventoryTreeRoot::InventoryTreeRoot(InventoryView* pOwner)
 : SqlTreeViewItemBase(pOwner)
{
}


InventoryTreeRoot::~InventoryTreeRoot()
{
}


void InventoryTreeRoot::OnSelected()
{
}


bool InventoryTreeRoot::CanEdit() const
{
   return false;
}


std::tstring InventoryTreeRoot::GetLabel() const
{
   return _T("All Characters");
}


bool InventoryTreeRoot::HasChildren() const
{
   return true;
}


std::vector<MFTreeViewItem*> InventoryTreeRoot::GetChildren() const
{
   std::vector<MFTreeViewItem*> result;

   // Init contents from DB
   g_DBManager.Lock();
   SQLite::TablePtr pT = g_DBManager.ExecTable(_T("SELECT DISTINCT owner FROM tItems ORDER BY owner ASC"));
   g_DBManager.UnLock();

   if (pT != NULL) {
      for (size_t i = 0; i < pT->Rows(); ++i) {
         unsigned int charId = boost::lexical_cast<unsigned int>(pT->Data(i,0));
         result.push_back(new CharacterTreeViewItem(m_pOwner, charId));
      }
   }
   
   return result;
}
