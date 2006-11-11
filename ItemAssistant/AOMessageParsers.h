#pragma once

#include "shared/aopackets.h"


namespace Native {

	class ObjectId
	{
	public:
		ObjectId(AO::AoObjectId const& id)
			: m_low(_byteswap_ulong(id.low))
			, m_high(_byteswap_ulong(id.high))
		{
		}

      unsigned int Low() const { return m_low; }
      unsigned int High() const { return m_high; }

      std::tstring print() const {
         std::tstringstream out;
         out << "[" << Low() << "|" << High() << "]";
         return out.str();
      }
	private:
		unsigned int	m_low;
		unsigned int	m_high;
	};

   
   class AOMessageHeader
   {
   public:
      AOMessageHeader(AO::Header *pHeader) : m_pHeader(pHeader) { }

      unsigned int msgid()    const { return _byteswap_ulong(m_pHeader->msgid); }
      unsigned int serverid() const { return _byteswap_ulong(m_pHeader->serverid); }
      unsigned int charid()   const { return _byteswap_ulong(m_pHeader->charid); }
      unsigned short size()   const { return _byteswap_ushort(m_pHeader->msgsize); }
      ObjectId target()       const { return ObjectId(m_pHeader->target); }

      std::tstring print() const {
         std::tstringstream out;
         out << "MessageHeader:" << "\r\n"
            << "MsgId\t" << msgid() << "\r\n"
            << "ServerId\t" << serverid() << "\r\n"
            << "CharId\t" << charid() << "\r\n"
            << "Size\t" << size() << "\r\n"
            << "Target\t" << target().print().c_str() << "\r\n";
         return out.str();
      }
   protected:
      AO::Header *m_pHeader;
   };


   class AOItem
   {
   public:
      AOItem(AO::ContItem* pItem) : m_pItem(pItem) { }

      unsigned int index() const { return _byteswap_ulong(m_pItem->index); }
      unsigned int ql() const { return _byteswap_ulong(m_pItem->ql); }
      unsigned short stack() const { return _byteswap_ushort(m_pItem->stack); }
      ObjectId containerid() const { return ObjectId(m_pItem->containerid); }
      ObjectId itemid() const { return ObjectId(m_pItem->itemid); }

   protected:
      AO::ContItem* m_pItem;
   };


   class AOContainer : public AOMessageHeader
   {
   public:
      AOContainer(AO::Header *pHeader) : AOMessageHeader(pHeader) { }

      unsigned char numslots() const { return ((AO::Container*)m_pHeader)->numslots; }
      unsigned int numitems() const { /*return (_byteswap_ulong(((AO::Container*)m_pHeader)->unknown2)-1009)/1009;*/ 
         AO::Container* p = ((AO::Container*)m_pHeader);
         unsigned int mass = _byteswap_ulong(p->mass);
         unsigned int result = (mass - 1009) / 1009;
         return result;
      }
      unsigned int counter() const { return _byteswap_ulong(((AO::ContEnd*)(((char*)m_pHeader)+sizeof(AO::ContItem)*numitems()))->counter); }
      ObjectId containerid() const { return ((AO::ContEnd*)(((char*)m_pHeader)+sizeof(AO::Container)+sizeof(AO::ContItem)*numitems()))->containerId; }

      AOItem item(int index) const { return AOItem( (AO::ContItem*)(((char*)m_pHeader)+sizeof(AO::Container)+sizeof(AO::ContItem)*index) ); }
   };


   class AOBank : public AOMessageHeader
   {
   public:
      AOBank(AO::Bank* pBank) : AOMessageHeader(&(pBank->header)), m_pBank(pBank) { }

      unsigned int numitems() const { return (_byteswap_ulong(m_pBank->mass)-1009)/1009; }
      AOItem item(int index) const { return AOItem((AO::ContItem*)(((char*)m_pBank)+sizeof(AO::Bank)+sizeof(AO::ContItem)*index)); }

   protected:
      AO::Bank* m_pBank;
   };


   class AOEquip : public AOMessageHeader
   {
   public:
      AOEquip(AO::Equip* pRaw) : AOMessageHeader(&(pRaw->header)), m_pRaw(pRaw) { }

      unsigned int numitems() const { return (_byteswap_ulong(m_pRaw->mass)-1009)/1009; }
      AOItem item(int index) const { return AOItem((AO::ContItem*)(((char*)m_pRaw)+sizeof(AO::Equip)+sizeof(AO::ContItem)*index)); }

   protected:
      AO::Equip* m_pRaw;
   };

};	// namespace
