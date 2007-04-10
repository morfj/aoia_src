#include "stdafx.h"
#include "PsmTreeItems.h"
#include "PlayershopView.h"
#include <sstream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/exception.hpp>
//#include <boost/iostreams/stream_buffer.hpp>
//#include <boost/iostreams/stream.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#define TIXML_USE_STL
#include <TinyXml/tinyxml.h>


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

void PsmTreeViewItemBase::OnSelected() 
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

   m_pOwner->UpdateListView(GetAllSoldItems());
}


std::vector<std::tstring> AccountTreeViewItem::GetAllSoldItems()
{
   std::vector<PsmTreeViewItem*> children = GetChildren();
   std::vector<std::tstring> v;
   for(unsigned int i=0; i<children.size(); i++)
   {
      std::vector<std::tstring> items = ((CharacterTreeViewItem1*)children[i])->GetAllSoldItems();
      for(unsigned int j=0; j<items.size(); j++)
      {
         v.push_back(items[j]);
      }
   }
   return v;
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

   std::tstring filename;
   filename = STREAM2STR( g_DBManager.AOFolder() << _T("\\Prefs\\") << m_label);
   boost::filesystem::path p(to_utf8_copy(filename), boost::filesystem::native);

   boost::filesystem::path account(to_utf8_copy(filename), boost::filesystem::native);
   boost::filesystem::directory_iterator character(account), filesEnd;

   for (; character != filesEnd; ++character)
   {
      if(is_directory(*character)){
         boost::filesystem::path logFile = (*character).root_path() / boost::filesystem::path("\\PlayerShopLog.html", boost::filesystem::native);
         unsigned int charID = atoi((*character).leaf().substr(4).c_str());
         if(charID != 0)
         {
            result.push_back(new CharacterTreeViewItem1(m_pOwner, charID,this));
            if(boost::filesystem::exists(logFile) && !boost::filesystem::is_directory(logFile)){
               //we have a playershop file!!
               //m_hasLogFile = true;
            }
         }  
      }
   }

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



CharacterTreeViewItem1::CharacterTreeViewItem1(PlayershopView* pOwner, unsigned int charID, const AccountTreeViewItem* pParent)
 : m_charid(charID)
 , PsmTreeViewItemBase(pOwner)
{
   m_pParent = pParent;
   std::tstring str = g_DBManager.GetToonName(m_charid);
   if(str.empty()){
      std::tstringstream ss;
      ss << charID;
      m_label = std::tstring(ss.str());
   }else{
      m_label = str;
   }
}


CharacterTreeViewItem1::~CharacterTreeViewItem1()
{
}


void CharacterTreeViewItem1::OnSelected() 
{ 
   
   
   m_pOwner->UpdateListView(GetAllSoldItems());

}

std::vector<std::tstring> CharacterTreeViewItem1::GetAllSoldItems()
{
   std::tstring filename;
   filename = STREAM2STR( g_DBManager.AOFolder() << _T("\\Prefs\\") << m_pParent->GetLabel() << "\\Char" << m_charid << "\\PlayerShopLog.html");

   boost::filesystem::path p(to_utf8_copy(filename),boost::filesystem::native);

   std::ifstream in(p.string().c_str());
   std::string line;
   std::string text;
   std::vector<std::tstring> v;
   while(in){
      line.clear();
      std::getline(in,line);
      if(!line.empty())
      {
         text += line;
      }
   }

   // Now that we have the whole file, lets parse it

   // Text located between the two following tags is to be considered an item sold
   std::string startTag = "<div indent=wrapped>" ;
   std::string endTag   = "</div>" ;

   while(text.length() > 0)
   {
   
      std::string::size_type start = text.find( startTag, 0 );
      std::string::size_type end  = text.find( endTag , 0 );
      if( start != std::string::npos && end != std::string::npos)
      {
         v.push_back(from_ascii_copy(text.substr(start+startTag.length(),end-start-startTag.length())));
         text = text.substr(end+endTag.length());
      }
      else
      {
         text = "";
      }
   }
   return v;
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
   std::vector<PsmTreeViewItem*> children = GetChildren();
   std::vector<std::tstring> v;
   for(unsigned int i=0; i<children.size(); i++)
   {
      std::vector<std::tstring> items = ((AccountTreeViewItem*)children[i])->GetAllSoldItems();
      for(unsigned int j=0; j<items.size(); j++)
      {
         v.push_back(items[j]);
      }
   }

   m_pOwner->UpdateListView(v);
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
   
   std::tstring filename;
   filename = STREAM2STR( g_DBManager.AOFolder() << _T("\\Prefs") );
   if(filename.empty()){
      return result;
   }
   boost::filesystem::path p(boost::filesystem::path(to_utf8_copy(filename), boost::filesystem::native));

   boost::filesystem::directory_iterator account(p), dir_end;

   if(!is_directory(p))
   {
      return result;
   }

   for (;account != dir_end; ++account)
   {
      boost::filesystem::path acc = *account;
      if (is_directory(acc))
	   { // we found an account ?
         
         result.push_back(new AccountTreeViewItem(m_pOwner, from_ascii_copy(acc.leaf())));
	   }
   }
	
   return result;
}

