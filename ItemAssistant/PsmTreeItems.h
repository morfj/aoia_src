#pragma once

#include "PsmTreeView.h"
#include "DBManager.h"
#include <strstream>


class PlayershopView;



class PsmTreeViewItemBase : public PsmTreeViewItem
{
public:
   PsmTreeViewItemBase(PlayershopView* pOwner);
   virtual ~PsmTreeViewItemBase();

   virtual void SetOwner(PlayershopView* pOwner);
   virtual void SetLabel(std::tstring const& newLabel);
   virtual unsigned int AppendMenuCmd(HMENU hMenu, unsigned int firstID, WTL::CTreeItem item) const;
   virtual bool HandleMenuCmd(unsigned int commandID, WTL::CTreeItem item);

protected:
   PlayershopView* m_pOwner;

   enum InternalCommand {
      CMD_EDIT = 1,
      CMD_DELETE,
   };

   mutable std::map<unsigned int, InternalCommand> m_commands;
};



class AccountTreeViewItem : public PsmTreeViewItemBase
{
public:
   AccountTreeViewItem(PlayershopView* pOwner, std::tstring accountName);
   virtual ~AccountTreeViewItem();

   virtual void OnSelected();
   virtual bool CanEdit() const;
   virtual bool CanDelete() const { return true; }
   virtual std::tstring GetLabel() const;
   virtual std::vector<PsmTreeViewItem*> GetChildren() const;
   virtual bool HasChildren() const;
   virtual unsigned int AppendMenuCmd(HMENU hMenu, unsigned int firstID, WTL::CTreeItem item) const;
   virtual bool HandleMenuCmd(unsigned int commandID, WTL::CTreeItem item);

private:
   std::tstring m_label;
};


class CharacterTreeViewItem1 : public PsmTreeViewItemBase
{
public:
   CharacterTreeViewItem1(PlayershopView* pOwner, std::tstring charName);
   virtual ~CharacterTreeViewItem1();

   virtual void OnSelected();
   virtual bool CanEdit() const;
   virtual bool CanDelete() const { return true; }
   virtual std::tstring GetLabel() const;
   virtual void SetLabel(std::tstring const& newLabel);
   virtual std::vector<PsmTreeViewItem*> GetChildren() const;
   virtual bool HasChildren() const;
   virtual unsigned int AppendMenuCmd(HMENU hMenu, unsigned int firstID, WTL::CTreeItem item) const;
   virtual bool HandleMenuCmd(unsigned int commandID, WTL::CTreeItem item);

private:
   unsigned int m_charid;
   std::tstring m_label;
};


class PlayershopTreeRoot : public PsmTreeViewItemBase
{
public:
   PlayershopTreeRoot(PlayershopView* pOwner = NULL);
   virtual ~PlayershopTreeRoot();

   virtual void OnSelected();
   virtual bool CanEdit() const;
   virtual bool CanDelete() const { return false; }
   virtual std::tstring GetLabel() const;
   virtual std::vector<PsmTreeViewItem*> GetChildren() const;
   virtual bool HasChildren() const;
};

