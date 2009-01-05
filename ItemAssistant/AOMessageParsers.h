#pragma once

#include "shared/aopackets.h"
#include <shared/Parser.h>


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

	class AOClientMessageHeader
	{
		public:
        AOClientMessageHeader(AO::ClientHeader *pHeader) : m_pHeader(pHeader) { }
		unsigned int msgid()    const { return _byteswap_ulong(m_pHeader->msgid); }
       // unsigned int serverid() const { return _byteswap_ulong(m_pHeader->serverid); }
        unsigned int charid()   const { return _byteswap_ulong(m_pHeader->charId.high); }
     //   unsigned short size()   const { return _byteswap_ushort(m_pHeader->msgsize); }
      //  ObjectId target()       const { return ObjectId(m_pHeader->charId); }

        std::tstring print() const {
            std::tstringstream out;
            out << "MessageHeader:" << "\r\n"
                << "MsgId\t" << msgid() << "\r\n"
                //<< "ServerId\t" << serverid() << "\r\n"
                //<< "CharId\t" << charid() << "\r\n"
                //<< "Size\t" << size() << "\r\n"
                << "CharId\t" << charid() << "\r\n";
            return out.str();
        }
    protected:
        AO::ClientHeader *m_pHeader;
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

		unsigned short flags() const { return _byteswap_ushort(m_pItem->flags); }
		unsigned int nullval() const { return _byteswap_ulong(m_pItem->nullval); }

        unsigned int index() const { return _byteswap_ulong(m_pItem->index); }
        unsigned int ql() const { return _byteswap_ulong(m_pItem->ql); }
        unsigned short stack() const { return _byteswap_ushort(m_pItem->stack); }
        ObjectId containerid() const { return ObjectId(m_pItem->containerid); }
        ObjectId itemid() const { return ObjectId(m_pItem->itemid); }

		std::tstring print() const {
            std::tstringstream out;
            out << "MessageHeader:" << "\r\n"
                << "flags\t" << flags() << "\r\n"
                << "nullval\t" << nullval() << "\r\n"
                << "index\t" << index() << "\r\n"
                << "containerid\t" << containerid().print().c_str() << "\r\n"
                << "itemid\t" << itemid().print().c_str() << "\r\n";
		
            return out.str();
		}

    protected:
        AO::ContItem* m_pItem;
    };


    class AOContainer : public AOMessageHeader
    {
    public:
        AOContainer(AO::Header *pHeader) : AOMessageHeader(pHeader) { }

		unsigned char unknown1() const { return ((AO::Container*)m_pHeader)->unknown1; }
		unsigned char unknown2() const { return ((AO::Container*)m_pHeader)->unknown2; }

        unsigned char numslots() const { return ((AO::Container*)m_pHeader)->numslots; }
        unsigned int numitems() const { /*return (_byteswap_ulong(((AO::Container*)m_pHeader)->unknown2)-1009)/1009;*/ 
            AO::Container* p = ((AO::Container*)m_pHeader);
            unsigned int mass = _byteswap_ulong(p->mass);
            unsigned int result = (mass - 1009) / 1009;
            return result;
        }

        unsigned int tempContainerId() const { return _byteswap_ulong(((AO::ContEnd*)(((char*)m_pHeader)+sizeof(AO::Container)+sizeof(AO::ContItem)*numitems()))->tempContainerId); }
        ObjectId containerid() const { return ((AO::ContEnd*)(((char*)m_pHeader)+sizeof(AO::Container)+sizeof(AO::ContItem)*numitems()))->containerId; }

        AOItem item(int index) const { return AOItem( (AO::ContItem*)(((char*)m_pHeader)+sizeof(AO::Container)+sizeof(AO::ContItem)*index) ); }

		std::tstring print() const {
            std::tstringstream out;
            out << "AOContainer:" << "\r\n"
                << "numslots\t" << numslots() << "\r\n"
                << "numitems\t" << numitems() << "\r\n"
                << "tempContainerId\t" << tempContainerId() << "\r\n"
				<< "unknown1\t" << unknown1() << "\r\n"
                << "unknown2\t" << unknown2() << "\r\n"
                << "containerid\t" << containerid().print().c_str() << "\r\n";
		
            return out.str();
		}


    };


	class AOOpenBackpackOperation : public AOClientMessageHeader
	{
		public:
        AOOpenBackpackOperation(AO::OpenBackpackOperation* pOpenBpOp) : AOClientMessageHeader(&(pOpenBpOp->header)), m_pOpenBpOp(pOpenBpOp) { }

		unsigned int openedbeforeCount() const { return _byteswap_ulong(m_pOpenBpOp->openedbeforeCount); }
		unsigned int counter() const { return _byteswap_ulong(m_pOpenBpOp->counter); }
		unsigned short containerType() const { return _byteswap_ushort(m_pOpenBpOp->backPack.type); }
		ObjectId owner() const { return m_pOpenBpOp->owner;}
		unsigned short containerTempId() const { return _byteswap_ushort(m_pOpenBpOp->backPack.itemSlotId); }



		std::tstring print() const {
            std::tstringstream out;
            out << "AOOpenBackpackOperation:" << "\r\n"
                << "openedbeforeCount\t" << openedbeforeCount() << "\r\n"
                << "counter\t" << counter() << "\r\n"
                << "fromType\t 0x" << std::hex <<containerType() << "\r\n"
                << "fromSlot\t" << containerTempId() << "\r\n";
		
            return out.str();
		}

    protected:
        AO::OpenBackpackOperation* m_pOpenBpOp;
	};

	class AOItemOperation : public AOClientMessageHeader
	{
		public:
        AOItemOperation(AO::ItemOperation* pDelOp) : AOClientMessageHeader(&(pDelOp->header)), m_pDelOp(pDelOp) { }

		unsigned int operationId() const { return _byteswap_ulong(m_pDelOp->operationId); }
		unsigned int unknown3() const { return _byteswap_ulong(m_pDelOp->unknown3); }

		unsigned short fromType() const { return _byteswap_ushort(m_pDelOp->itemToDelete.type); }
		unsigned short fromContainerTempId() const { return _byteswap_ushort(m_pDelOp->itemToDelete.containerTempId); }
		unsigned short fromItemSlotId() const { return _byteswap_ushort(m_pDelOp->itemToDelete.itemSlotId); }
		ObjectId itemId() const { return m_pDelOp->itemId;}
		//unsigned short targetSlotId() const { return _byteswap_ushort(m_pMoveOp->moveData.targetSlot); }

		std::tstring print() const {
            std::tstringstream out;
            out << "AOItemOperation:" << "\r\n"
				<< "operationId\t 0x" << std::hex << operationId() << "\r\n"
				<< "unknown3\t 0x"<< std::hex << unknown3() << "\r\n"
                << "fromType\t 0x"<< std::hex << fromType() << "\r\n"
                << "fromContainerTempId\t" << fromContainerTempId() << "\r\n"
                << "fromItemSlotId\t" << fromItemSlotId() << "\r\n"
                << "itemId\t" << itemId().print().c_str() << "\r\n";
		
            return out.str();
		}

    protected:
        AO::ItemOperation* m_pDelOp;
	};

	class AOMoveOperation : public AOClientMessageHeader
	{
		public:
        AOMoveOperation(AO::MoveOperation* pMoveOp) : AOClientMessageHeader(&(pMoveOp->header)), m_pMoveOp(pMoveOp) { }

		unsigned short fromType() const { return _byteswap_ushort(m_pMoveOp->moveData.fromItem.type); }
		unsigned short fromContainerTempId() const { return _byteswap_ushort(m_pMoveOp->moveData.fromItem.containerTempId); }
		unsigned short fromItemSlotId() const { return _byteswap_ushort(m_pMoveOp->moveData.fromItem.itemSlotId); }
		ObjectId toContainer() const { return m_pMoveOp->moveData.toContainer;}
		unsigned short targetSlotId() const { return _byteswap_ushort(m_pMoveOp->moveData.targetSlot); }

		std::tstring print() const {
            std::tstringstream out;
            out << "AOMoveOperation:" << "\r\n"
                << "fromType\t0x" << std::hex << fromType() << "\r\n"
                << "fromContainerTempId\t" << fromContainerTempId() << "\r\n"
                << "fromItemSlotId\t" << fromItemSlotId() << "\r\n"
                << "toContainer\t" << toContainer().print().c_str() << "\r\n"
                << "targetSlotId\t" << targetSlotId() << "\r\n";
		
            return out.str();
		}

    protected:
        AO::MoveOperation* m_pMoveOp;
	};

	

	class AOPartnerTradeItem : public AOMessageHeader
	{
		public:
        AOPartnerTradeItem(AO::PartnerTradeItem* pTrans) : AOMessageHeader(&(pTrans->header)), m_pTrans(pTrans) { }

		unsigned int operationId() const { return _byteswap_ulong(m_pTrans->operationId); }
		ObjectId itemid() const { return ObjectId(m_pTrans->itemid); }
        unsigned int ql() const { return _byteswap_ulong(m_pTrans->ql); }
		unsigned short flags() const { return _byteswap_ushort(m_pTrans->flags); }
        unsigned short stack() const { return _byteswap_ushort(m_pTrans->stack); }

		unsigned short	partnerFromType() const { return _byteswap_ushort(m_pTrans->partnerInvItem.type); }
		unsigned short	partnerFromContainerTempId() const { return _byteswap_ushort(m_pTrans->partnerInvItem.containerTempId); }
		unsigned short	partnerFromItemSlotId() const { return _byteswap_ushort(m_pTrans->partnerInvItem.itemSlotId); }

		std::tstring print() const {
            std::tstringstream out;
            out << "AOPartnerTradeItem:" << "\r\n"
				<< "operationId\t" << std::hex << operationId() << "\r\n"
				<< "itemid\t" << itemid().print().c_str() << "\r\n"
				<< "ql\t" << ql() << " stack\t" << stack() <<"\r\n";
	
               // << "partnerFromType\t" << std::hex << partnerFromType() << "\r\n"
				//<< "partnerFromContainerTempId\t" << partnerFromContainerTempId() << "\r\n"
			//	<< "partnerFromItemSlotId\t" << partnerFromItemSlotId() << "\r\n"
              
		
            return out.str();
		}

	protected:
        AO::PartnerTradeItem* m_pTrans;
	};

	class AOTradeTransaction : public AOMessageHeader
	{
		public:
        AOTradeTransaction(AO::TradeTransaction* pTrans) : AOMessageHeader(&(pTrans->header)), m_pTrans(pTrans) { }

		unsigned int	unknown2() const { return _byteswap_ulong(m_pTrans->unknown2); }
		
		unsigned short	fromType() const { return _byteswap_ushort(m_pTrans->itemToTrade.type); }
		unsigned short	fromContainerTempId() const { return _byteswap_ushort(m_pTrans->itemToTrade.containerTempId); }
		unsigned short	fromItemSlotId() const { return _byteswap_ushort(m_pTrans->itemToTrade.itemSlotId); }

		unsigned char	operationId() const { return m_pTrans->operationId; }
		//01=accept, 02=decline,03=?start?, 04=commit,05=add item,06=remove item

		ObjectId		fromId() const { return ObjectId(m_pTrans->fromId);}

		std::tstring print() const {
            std::tstringstream out;
            out << "AOTradeTransaction:" << "\r\n"
				<< "unknown2\t" << std::hex << unknown2() << "\r\n"
				<< "operationId\t" << std::hex << operationId() << "\r\n"
                << "fromType\t" << std::hex << fromType() << "\r\n"
				<< "fromContainerTempId\t" << fromContainerTempId() << "\r\n"
				<< "fromItemSlotId\t" << fromItemSlotId() << "\r\n"
                << "fromId\t" << fromId().print().c_str() << "\r\n";
		
            return out.str();
		}

	protected:
        AO::TradeTransaction* m_pTrans;
	};

	class AOBoughtItemFromShop : public AOMessageHeader
	{
		public:
        AOBoughtItemFromShop(AO::BoughtItemFromShop* pBoughItem) : AOMessageHeader(&(pBoughItem->header)), m_pBoughItem(pBoughItem) { }

		unsigned int ql() const { return _byteswap_ulong(m_pBoughItem->ql); }
        unsigned short stack() const { return _byteswap_ushort(m_pBoughItem->stack); }
        ObjectId itemid() const { return ObjectId(m_pBoughItem->itemid); }


		std::tstring print() const {
            std::tstringstream out;
            out << "AOBoughtItemFromShop:" << "\r\n"
                << "ql\t" << ql() << "\r\n"
                << "stack\t" << stack() << "\r\n"
                << "itemid\t" << itemid().print().c_str() << "\r\n";
		
            return out.str();
		}

	protected:
        AO::BoughtItemFromShop* m_pBoughItem;
	};

	

	class AOBackpack : public AOMessageHeader
	{
		public:
        AOBackpack(AO::Backpack* pItem) : AOMessageHeader(&(pItem->header)), m_pItem(pItem) { }

		unsigned char	operationId() const { return m_pItem->operationId; }
		unsigned char	invSlot() const { return m_pItem->invSlot; }
		ObjectId		owner()  const { return ObjectId(m_pItem->ownerId); }

		unsigned int ql() const { return _byteswap_ulong(m_pItem->ql); }
		unsigned int keyLow() const { return _byteswap_ulong(m_pItem->itemKeyLow); }
		unsigned int keyHigh() const { return _byteswap_ulong(m_pItem->itemKeyHigh); }
		unsigned int flags() const { return _byteswap_ulong(m_pItem->flags); }
	
	
		std::tstring print() const {
            std::tstringstream out;
            out << "AOBackpack:" << "\r\n"
                << "operationId\t 0x" << std::hex << operationId() << "\r\n"
                << "invSlot\t" << invSlot() << "\r\n"
                << "ql\t" << ql() << "\r\n"
                << "keyLow\t" << keyLow() << "keyHigh\t" << keyHigh() << "\r\n"
                << "flags\t" << flags() << "\r\n";
		
            return out.str();
		}

    protected:
        AO::Backpack* m_pItem;
	};

	class AOItemMoved : public AOMessageHeader
	{
		public:
        AOItemMoved(AO::ItemMoved* pMoveOp) : AOMessageHeader(&(pMoveOp->header)), m_pMoveOp(pMoveOp) { }

		unsigned short fromType() const { return _byteswap_ushort(m_pMoveOp->moveData.fromItem.type); }
		unsigned short fromContainerTempId() const { return _byteswap_ushort(m_pMoveOp->moveData.fromItem.containerTempId); }
		unsigned short fromItemSlotId() const { return _byteswap_ushort(m_pMoveOp->moveData.fromItem.itemSlotId); }
		ObjectId toContainer() const { return m_pMoveOp->moveData.toContainer;}
		unsigned short targetSlotId() const { return _byteswap_ushort(m_pMoveOp->moveData.targetSlot); }

		std::tstring print() const {
            std::tstringstream out;
            out << "AOItemMoved:" << "\r\n"
                << "fromType\t 0x" << std::hex << fromType() << "\r\n"
                << "fromContainerTempId\t" << fromContainerTempId() << "\r\n"
                << "fromItemSlotId\t" << fromItemSlotId() << "\r\n"
                << "toContainer\t" << toContainer().print().c_str() << "\r\n"
                << "targetSlotId\t" << targetSlotId() << "\r\n";
		
            return out.str();
		}

    protected:
        AO::ItemMoved* m_pMoveOp;
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


namespace Parsers {

	class AOClientMessageBase
        : public Parser
    {
    public:
        enum HeaderType {
            UNKNOWN_MESSAGE,
            MSG_TYPE_1 = 0x00000001,
            MSG_TYPE_A = 0x0000000A,
            MSG_TYPE_B = 0x0000000B,
            MSG_TYPE_D = 0x0000000D,
            MSG_TYPE_E = 0x0000000E,
        };
	

        AOClientMessageBase(char *pRaw, unsigned int size)
            : Parser(pRaw, size) 
            , m_type(UNKNOWN_MESSAGE)
            , m_characterid(0)
            , m_messageid(0)
        {

			//Client Send Header parsing:

			//H3 = I4 msgType  I4 Zero     I4  someId  I4   02     I4 msgId I4+I4 charId
            // Parse and validate header
            unsigned int t = popInteger();
            if (t == 0x0000000A) {
                m_type = MSG_TYPE_A;
            }
            //else if (t == 0x0000000D) {
            //    m_type = MSG_TYPE_D;
            //   // char c = popChar();
            //   // assert(c == 0x0A);
            //}
            else if (t == 0x0000000B) {
                m_type = MSG_TYPE_B;
            }
            //else if (t == 0x0000000E) {
            //    m_type = MSG_TYPE_E;
            //}
            else if (t == 0x00000001) {
                m_type = MSG_TYPE_1;
            }
            else {
                Logger::instance().log(STREAM2STR(_T("Error unknown client message header: ") << t));
                return;
            }

            skip(4);    // 0x00 00 00 00
          
            skip(4);    // Receiver: Instance ID? Dimension? Subsystem?
            
			skip(4); //??ex 00 00 00 02 or 00 00 0b ed  on logoff

			m_messageid = popInteger();

			skip(4);    // Target Entity: Instance ID? Dimension? Subsystem? 50000
			//m_entityid = popInteger();

			m_characterid = popInteger();

        }

        HeaderType headerType() const { return m_type; }
        unsigned int characterId() const { return m_characterid; }
        unsigned int messageId() const { return m_messageid; }

    private:
        HeaderType m_type;
        unsigned int m_characterid;
        unsigned int m_messageid;
    };

    class AOMessageBase
        : public Parser
    {
    public:
        enum HeaderType {
            UNKNOWN_MESSAGE,
            MSG_TYPE_1 = 0xDFDF0001,
            MSG_TYPE_A = 0xDFDF000A,
            MSG_TYPE_B = 0xDFDF000B,
            MSG_TYPE_D = 0xDFDF000D,
            MSG_TYPE_E = 0xDFDF000E,
        };
	

        AOMessageBase(char *pRaw, unsigned int size)
            : Parser(pRaw, size) 
            , m_type(UNKNOWN_MESSAGE)
            , m_size(0)
            , m_characterid(0)
            , m_messageid(0)
            , m_entityid(0)
        {
            // Parse and validate header
            unsigned int t = popInteger();
            if (t == 0xDFDF000A) {
                m_type = MSG_TYPE_A;
            }
            else if (t == 0xDFDF000D) {
                m_type = MSG_TYPE_D;
                char c = popChar();
                assert(c == 0x0A);
            }
            else if (t == 0xDFDF000B) {
                m_type = MSG_TYPE_B;
            }
            else if (t == 0xDFDF000E) {
                m_type = MSG_TYPE_E;
            }
            else if (t == MSG_TYPE_1) {
                m_type = MSG_TYPE_1;
            }
            else {
                Logger::instance().log(STREAM2STR(_T("Error unknown message header: ") << t));
                return;
            }

            skip(2);    // 0x0001		protocol version?
            m_size = popShort();
            skip(4);    // Receiver: Instance ID? Dimension? Subsystem?
            m_characterid = popInteger();
            m_messageid = popInteger();
            skip(4);    // Target Entity: Instance ID? Dimension? Subsystem?
            m_entityid = popInteger();

			/*

			Client Send Header parsing:

			//H3 = I4 msgType  I4 Zero     I4  someId  I4   02     I4 msgId I4+I4 charId
            // Parse and validate header
            unsigned int t = popInteger();
            if (t == 0x0000000A) {
                m_type = MSG_TYPE_A;
            }
            //else if (t == 0x0000000D) {
            //    m_type = MSG_TYPE_D;
            //   // char c = popChar();
            //   // assert(c == 0x0A);
            //}
            else if (t == 0x0000000B) {
                m_type = MSG_TYPE_B;
            }
            //else if (t == 0x0000000E) {
            //    m_type = MSG_TYPE_E;
            //}
            else if (t == 0x00000001) {
                m_type = MSG_TYPE_1;
            }
            else {
                Logger::instance().log(STREAM2STR(_T("Error unknown client message header: ") << t));
                return;
            }

            skip(4);    // 0x00 00 00 00
          
            skip(4);    // Receiver: Instance ID? Dimension? Subsystem?
            
			skip(4); //??ex 00 00 00 02 or 00 00 0b ed  on logoff

			m_messageid = popInteger();

			skip(4);    // Target Entity: Instance ID? Dimension? Subsystem? 50000
			//m_entityid = popInteger();

			m_characterid = popInteger();

			*/
        }

        HeaderType headerType() const { return m_type; }
        unsigned int size() const { return m_size; }
        unsigned int characterId() const { return m_characterid; }
        unsigned int messageId() const { return m_messageid; }
        unsigned int entityId() const { return m_entityid; }

    private:
        HeaderType m_type;
        unsigned int m_size;
        unsigned int m_characterid;
        unsigned int m_messageid;
        unsigned int m_entityid;
    };


    class AOObjectId
    {
    public:
        AOObjectId()
            : m_low(0)
            , m_high(0)
        {
        }

        AOObjectId(Parser &p)
        {
            m_low = p.popInteger();
            m_high = p.popInteger();
        }

        unsigned int low() const { return m_low; }
        unsigned int high() const { return m_high; }

    private:
        unsigned int m_low;
        unsigned int m_high;
    };


    class AOContainerItem
    {
    public:
        AOContainerItem(Parser &p)
        {
            m_index = p.popInteger();
            p.skip(2);  // 0x0021 ??
            m_stack = p.popShort();
            m_containerid = AOObjectId(p);
            m_itemid = AOObjectId(p);
            m_ql = p.popInteger();
            p.skip(4);
        }

        unsigned int index() const { return m_index; }
        unsigned short stack() const { return m_stack; }
        AOObjectId containerId() const { return m_containerid; }
        AOObjectId itemId() const { return m_itemid; }
        unsigned int ql() const { return m_ql; }

    private:
        unsigned int m_index;
        unsigned short m_stack;
        AOObjectId m_containerid;
        AOObjectId m_itemid;
        unsigned int m_ql;
    };


    class AOContainer
        : public AOMessageBase
    {
    public:
        AOContainer(char *pRaw, unsigned int size)
            : AOMessageBase(pRaw, size)
        {
            skip(4);                    // 0x01000000 object type?
            m_slots = popChar();        // 0x15 for backpacks
            skip(4);                    // 0x00000003 ??? introduced after 16.1 or so..

            m_itemCount = (popInteger() - 1009)/1009;   // mass? seems related to number of items in backpack. 1009 + 1009*numitems
            for (unsigned int i = 0; i < m_itemCount; ++i) {
                m_items.push_back(AOContainerItem(*this));
            }

            m_containerId = AOObjectId(*this);
            m_counter = popInteger();   // Number of times this message has been received?
            skip(4);
        }

        unsigned char numslots() const { return m_slots; }
        unsigned int numitems() const { return m_itemCount; }
        unsigned int counter() const { return m_counter; }
        AOObjectId containerid() const { return m_containerId; }
        AOContainerItem item(int index) const { return m_items.at(index); }

    private:
        unsigned char m_slots;
        unsigned int m_itemCount;
        std::vector<AOContainerItem> m_items;
        AOObjectId m_containerId;
        unsigned int m_counter;
    };


    class AOPlayerShopInfo
        : public AOMessageBase
    {
    public:
        AOPlayerShopInfo(char *pRaw, unsigned int size)
            : AOMessageBase(pRaw, size)
        {
            // Parse payload
            skip(126);  // ?
            m_label = popString();
            skip(19);   // ?
            m_ownerid = popInteger();
            skip(1);
            m_ownerName = popString();
            skip(4);    // ?
            m_guildName = popString();
        }

        unsigned int shopId() const { return entityId(); }
        unsigned int ownerId() const { return m_ownerid; }
        std::string label() const { return m_label; }
        std::string ownerName() const { return m_ownerName; }
        std::string guildName() const { return m_guildName; }

    private:
        unsigned int m_ownerid;
        std::string m_label;
        std::string m_ownerName;
        std::string m_guildName;
    };


    class AOPlayerShopContent
        : public AOMessageBase
    {
    public:
        AOPlayerShopContent(char *pRaw, unsigned int size)
            : AOMessageBase(pRaw, size)
        {
            // Starts the same as a container...
            skip(4);                    // 0x01000000 object type?
            m_slots = popChar();        // 0x15 for backpacks
            skip(4);                    // 0x00000003 ??? introduced after 16.1 or so..

            m_itemCount = (popInteger() - 1009)/1009;   // mass? seems related to number of items in backpack. 1009 + 1009*numitems
            for (unsigned int i = 0; i < m_itemCount; ++i) {
                m_items.push_back(AOContainerItem(*this));
            }

            // ...but has a different ending.
            skip(4);    // ?
            m_shopid = popInteger();
            skip(12);    // ?
            for (unsigned int i = 0; i < 21; ++i) {
                m_prices[i] = popInteger();
            }
            skip(1);    // ?
        }

        unsigned char numslots() const { return m_slots; }
        unsigned int numitems() const { return m_itemCount; }
        AOContainerItem item(int index) const { return m_items.at(index); }
        unsigned int shopid() const { return m_shopid; }
        unsigned int price(int index) const { return m_prices[index]; }

    private:
        unsigned char m_slots;
        unsigned int m_itemCount;
        std::vector<AOContainerItem> m_items;
        unsigned int m_shopid;
        unsigned int m_prices[21];     // Array of prices per shop slot
    };

};  // namespace Parsers