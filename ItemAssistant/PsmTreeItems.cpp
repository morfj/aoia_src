#include "stdafx.h"
#include "PsmTreeItems.h"
#include "PlayershopView.h"
#include <sstream>



PsmTreeViewItemBase::PsmTreeViewItemBase(PlayershopView* pOwner)
 : m_pOwner(pOwner)
{
}


PsmTreeViewItemBase::~PsmTreeViewItemBase()
{
}


void PsmTreeViewItemBase::SetOwner(PlayershopView* pOwner) 
{
   m_pOwner = pOwner;
}


unsigned int PsmTreeViewItemBase::AppendMenuCmd(HMENU hMenu, unsigned int firstID, WTL::CTreeItem item) const
{
   return firstID;
}


bool PsmTreeViewItemBase::HandleMenuCmd(unsigned int commandID, WTL::CTreeItem item)
{
   return false;
}

   
void PsmTreeViewItemBase::SetLabel(std::tstring const& newLabel)
{
}


/***************************************************************************/
/** Container Tree View Item                                              **/
/***************************************************************************/

AccountTreeViewItem::AccountTreeViewItem(
   PlayershopView* pOwner, std::tstring accountName)
 : m_label(accountName)
 , PsmTreeViewItemBase(pOwner)
{
}


AccountTreeViewItem::~AccountTreeViewItem()
{
}


void AccountTreeViewItem::OnSelected() 
{

}


bool AccountTreeViewItem::CanEdit() const
{
   return false;
}


std::tstring AccountTreeViewItem::GetLabel() const
{
   return m_label;
}


bool AccountTreeViewItem::HasChildren() const
{
   return true;
}


std::vector<PsmTreeViewItem*> AccountTreeViewItem::GetChildren() const
{
   std::vector<PsmTreeViewItem*> result;

   result.push_back(new CharacterTreeViewItem1(m_pOwner, _T("Senlee")));

   return result;
}


unsigned int AccountTreeViewItem::AppendMenuCmd(HMENU hMenu, unsigned int firstID, WTL::CTreeItem item) const
{
   return firstID;
}


bool AccountTreeViewItem::HandleMenuCmd(unsigned int commandID, WTL::CTreeItem item)
{
   return false;

}



CharacterTreeViewItem1::CharacterTreeViewItem1(PlayershopView* pOwner, std::tstring charName)
 : m_label(charName)
 , PsmTreeViewItemBase(pOwner)
{

}


CharacterTreeViewItem1::~CharacterTreeViewItem1()
{
}


void CharacterTreeViewItem1::OnSelected() 
{ 
   //std::tstringstream str;
   //str << _T("owner = ") << m_charid;
   //m_pOwner->UpdateListView(str.str());
}


bool CharacterTreeViewItem1::CanEdit() const
{
   return false;
}


std::tstring CharacterTreeViewItem1::GetLabel() const
{
	return m_label;
}


void CharacterTreeViewItem1::SetLabel(std::tstring const& newLabel)
{
	m_label = newLabel;
}


bool CharacterTreeViewItem1::HasChildren() const
{
   return false;
}


std::vector<PsmTreeViewItem*> CharacterTreeViewItem1::GetChildren() const
{
   std::vector<PsmTreeViewItem*> result;

   return result;
}


unsigned int CharacterTreeViewItem1::AppendMenuCmd(HMENU hMenu, unsigned int firstID, WTL::CTreeItem item) const
{
   return firstID;
}


bool CharacterTreeViewItem1::HandleMenuCmd(unsigned int commandID, WTL::CTreeItem item)
{
   return false;

}



PlayershopTreeRoot::PlayershopTreeRoot(PlayershopView* pOwner)
 : PsmTreeViewItemBase(pOwner)
{
}


PlayershopTreeRoot::~PlayershopTreeRoot()
{
}


void PlayershopTreeRoot::OnSelected()
{
}


bool PlayershopTreeRoot::CanEdit() const
{
   return false;
}


std::tstring PlayershopTreeRoot::GetLabel() const
{
   return _T("All Accounts");
}


bool PlayershopTreeRoot::HasChildren() const
{
   return true;
}


std::vector<PsmTreeViewItem*> PlayershopTreeRoot::GetChildren() const
{
   std::vector<PsmTreeViewItem*> result;

   result.push_back(new AccountTreeViewItem(m_pOwner,_T("kenneth")));
   result.push_back(new AccountTreeViewItem(m_pOwner,_T("morten")));
 
   return result;
}

