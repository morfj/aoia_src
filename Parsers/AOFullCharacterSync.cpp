#include "AOFullCharacterSync.h"

namespace Parsers {

    AOFullCharacterMessage::AOFullCharacterMessage(char *pRaw, unsigned int size)
        : AOMessageBase(pRaw, size)
    {
        popChar();  // padding?
        popInteger(); // flags? 0x19

        // Read inventory and equip
        unsigned int count = pop3F1Count();
        for (unsigned int i = 0; i < count; ++i)
        {
            m_inventory.push_back(AOContainerItemPtr(new AOContainerItem(*this)));
        }

        // Read uploaded nano programs
        count = pop3F1Count();
        for (unsigned int i = 0; i < count; ++i)
        {
            m_nanos.push_back(popInteger());
        }

        // Unknown (but empty) collection
        count = pop3F1Count();
        assert(count == 0);

        // 6 unknown ints
        skip(sizeof(unsigned int) * 6);

        // Stats map (32 bit id, 32 bit value)
        count = pop3F1Count();
        for (unsigned int i = 0; i < count; ++i)
        {
            unsigned int key = popInteger();
            unsigned int val = popInteger();
            m_stats[key] = val;
        }

        // Stats map (32 bit id, 32 bit value)
        count = pop3F1Count();
        for (unsigned int i = 0; i < count; ++i)
        {
            unsigned int key = popInteger();
            unsigned int val = popInteger();
            m_stats[key] = val;
        }

        // Stats map (8 bit id, 8 bit value)
        count = pop3F1Count();
        for (unsigned int i = 0; i < count; ++i)
        {
            unsigned char key = popChar();
            unsigned char val = popChar();
            m_stats[key] = val;
        }

        // Stats map (8 bit id, 16 bit value)
        count = pop3F1Count();
        for (unsigned int i = 0; i < count; ++i)
        {
            unsigned char key = popChar();
            unsigned short val = popShort();
            m_stats[key] = val;
        }

        // 2 unknown ints
        skip(sizeof(unsigned int) * 2);

        // Unknown (but empty) collection
        count = pop3F1Count();
        assert(count == 0);

        // Unknown (but empty) collection
        count = pop3F1Count();
        assert(count == 0);

        // Unknown collection of 16 byte structs.
        // Probably perk information of some sort?
        count = pop3F1Count();
        skip(count * 16);

        unsigned short marker = popShort();
        assert(marker == 0xfdfd);
    }


    unsigned int AOFullCharacterMessage::numitems() const
    {
        return m_inventory.size();
    }


    AOContainerItemPtr AOFullCharacterMessage::getItem(unsigned int index) const
    {
        if (index < m_inventory.size())
        {
            return m_inventory.at(index);
        }
        return AOContainerItemPtr();
    }


    void AOFullCharacterMessage::getAllItems( std::vector<AOContainerItemPtr> &items ) const
    {
        items.insert(items.begin(), m_inventory.begin(), m_inventory.end());
    }


    void AOFullCharacterMessage::getStats( std::map<unsigned int, unsigned int> &stats ) const
    {
        stats.insert(m_stats.begin(), m_stats.end());
    }

}   // namespace
