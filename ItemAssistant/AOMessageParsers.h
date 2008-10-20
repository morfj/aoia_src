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


namespace Parsers {

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
                Logger::instance()->log(STREAM2STR(_T("Error unknown message header: ") << t));
                return;
            }

            skip(2);    // 0x0001		protocol version?
            m_size = popShort();
            skip(4);    // Receiver: Instance ID? Dimension? Subsystem?
            m_characterid = popInteger();
            m_messageid = popInteger();
            skip(4);    // Target Entity: Instance ID? Dimension? Subsystem?
            m_entityid = popInteger();
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