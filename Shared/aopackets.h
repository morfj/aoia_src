#pragma once

namespace AO {

    // USe these instead of sizeof(...) or you will get wrong results due to alignment errors.
    // Maybe redo structs with proper #pragma pack and/or __declspec(align(#))?
    const int AoObjectIdSize		= 8;
    const int HeaderSize			= 28;
    const int ContainerBeginSize	= 9;
    const int ContainerItemSize		= 32;
    const int ContainerEndSize		= 16;

#pragma pack(push, 1)
    struct AoObjectId
    {
        unsigned int low;
        unsigned int high;
    };

    /** 
     * SHIFT F9 info: (adjuster inside HR8)
     *
     * 15.7.3:
     *	Pos: 109.4,149.4,115.0, PF: 3027
     *	Zone: 17, Area: Omni-1 Trade Backyard 8, CharID: 50000:522643323
     *	ServerID: 3104 UTC: Wed Dec 22 16:11:08 2004
     *	Pf Proxy: Model=51100:3027 GroupSelector=0 Subgroup=0 RunningPlayfield=40016:3027
     *	Suppression field at 100%. No fighting possible.
     *	Version: 15.7.3_EP1. MapID: 15.7.3_08.12.2004_14.21.41
     *
     * 16.2.2:
     *  Pos: 109.9,149.8,115.0, PF: 3027
     *  109.9 149.8 y 115.0 3027
     *  Zone: 17, Area: Omni-1 Trade Backyard 8, CharID: 50000:522643323
     *  ServerID: 3064 UTC: Sat Apr 15 12:24:30 2006
     *   51100:3027/0/0/40016:3027
     *  Suppression field at 100%. No fighting possible.
     *  Version: 16.2.2_EP1. MapID: 16.2.2.0_22.03.2006_14.03.16
     *
     * 17.2.0:
     *  Extended location information:
     *  - 107.4, 147.5, 115.0 (107.4 147.5 y 115.0 3027)
     *  - Pf Proxy: Model=51100:3027 GS=0 SG=0 R=3027, resource: 3027
     *  - zone: 17, area: "Omni-1 Trade Backyard 8"
     *  Server id: 1114, character id: 50000:522643323, time: 2007-03-18 17:13:49 (UTC)
     *  Version: 17.2.0_EP1, map id: 17.2.0_14.03.2007_15.23.14, build: 46091.
     *
     * (From RK2)
     *  Extended location information:
     *  - 850.8, 789.1, 38.0 (850.8 789.1 y 38.0 1923801)
     *  - Pf Proxy: Model=51102:4582 GS=3 SG=0 R=1923801, resource: 4582
     *  - zone: 971, area: "ICC Shuttleport"
     *  Server id: 3149, character id: 50000:854759930, time: 2007-02-11 20:12:41 (UTC)
     *  Version: 17.1.1_EP1, map id: 17.1.1_08.02.2007_15.57.44, build: 45960.
     */
    struct Header
    {
        unsigned int	headerid;	// 0xDFDF000A
        unsigned short	unknown1;	// 0x0001		protocol version?
        unsigned short	msgsize;	//				size incl header
        unsigned int	serverid;	// 0x00000C20	ServerID
        unsigned int	charid;		// 0x1F26E77B	== Adjuster

        unsigned int	msgid;		//

        // Target of message?
        AoObjectId		target;		// 0x0000C350	dimension? (50000:<charid>) +  charid
    };

    struct Container
    {
        Header         header;
        unsigned int	unknown1;	// 0x01000000 object type?
        unsigned char	numslots;	// 0x15 for a backpack
        unsigned int	unknown2;	// 0x00000003 ??? introduced after 16.1 or so..
        unsigned int	mass;	      // mass? seems related to number of items in backpack. 1009 + 1009*numitems
    };

    struct ContItem						// Size 0x20
    {
        unsigned int	index;
        unsigned short	unknown0;	// 0x0021 ??
        unsigned short	stack;
        AoObjectId     containerid;
        AoObjectId		itemid;		// low + high id
        unsigned int	ql;
        unsigned int	nullval;
    };

    struct ContEnd
    {
        AoObjectId		containerId;
        unsigned int	counter;	// Number of times this message has been received?
        unsigned int	nullval;
    };

    struct Bank                      // size 17
    {
        Header         header;
        unsigned int   unknown1;      // 0x01000000 wtf?
        char           unknown2[9];
        unsigned int   mass;          // 1009*items+1009
    };

    struct Equip
    {
        Header         header;
        unsigned int   unknown1;      // 0x00000000 ??
        char           unknown2[1];   // 0x12 ??
        unsigned int   mass;          // 1009*items+1009
    };

    struct AoString
    {
        unsigned char  strLen;        // Number of bytes allocated to 'str'
        const char     str;           // array of strLen number of bytes.
    };

    struct MobInfo
    {
        Header         header;
        unsigned int   unknown1;
        unsigned int   unknown2;
        unsigned int   unknown3;
        unsigned int   unknown4;
        unsigned int   unknown5;
        unsigned short unknown6;
        AoString       characterName;
    };

#pragma pack(pop)


    enum MsgIds
    {
        //MSG_UNKNOWN	=	0x47777000,
        //MSG_UNKNOWN	=	0x5E477770,
        MSG_SKILLSYNC	=	0x3E205660,
        MSG_CONTAINER	=	0x4E536976,
        MSG_BACKPACK   =  0x465A5D73,
        MSG_CHAT		   =	0x5F4B442A,
        MSG_MISSIONS	=	0x5C436609,
        //MSG_BANK		   =	0x7F283C34,
        MSG_BANK       =  0x343c287f,    // size 2285.. dynamic most likely
        MSG_POS_SYNC   =  0x5E477770,    // size = 55 bytes
        MSG_UNKNOWN_1  =  0x25314D6D,    // size = 53 bytes
        MSG_UNKNOWN_2  =  0x00000001,    // size = 40 bytes
        MSG_UNKNOWN_3  =  0x0000000b,    // size = 548 bytes
        MSG_FULLSYNC   =  0x29304349,    // size 4252.. dynamic?  Contains equip and inv + atribs (?) +++
        MSG_UNKNOWN_4  =  0x54111123,
        MSG_MOB_SYNC   =  0x271B3A6B,
    };

    enum ToonAtribIds
    {
        // TOON 
        BREED                      = 4,
        SIDE                       = 33,
        TITLE_LEVEL                = 37,
        LEVEL                      = 54,
        GENDER                     = 59,
        PROFESSION                 = 60,
        CLAN_TOKENS                = 62,
        OMNI_TOKENS                = 75,

        // FACTION VALUES
        FACTION_OMNI_AF            = 560,
        FACTION_SENTINELS          = 561,
        FACTION_OMNI_MED           = 562,
        FACTION_GAIA               = 563,
        FACTION_OMNI_TRANS         = 564,
        FACTION_VANGUARDS          = 565,
        FACTION_GUARDIAN_OF_SHADOWS = 566,
        FACTION_FOLLOWERS          = 567,
        FACTION_OPERATORS          = 568,
        FACTION_UNREDEEMED         = 569,
        FACTION_DEVOTED            = 570,
        FACTION_CONSERVERS         = 571,
        FACTION_REDEEMED           = 572,

        // ABILITIES
        STRENGTH                   = 16,
        AGILITY                    = 17,
        STAMINA                    = 18,
        INTELLIGENCE               = 19,
        SENSE                      = 20,
        PSYCHIC                    = 21,
        BODY_DEVELOPMENT           = 152,
        NANO_ENERGY_POOL           = 132,

        // BODY
        MARTIAL_ARTS               = 100,
        BRAWL                      = 142,
        DIMACH                     = 144,
        RIPOSTE                    = 143,
        ADVENTURING                = 137,
        SWIM                       = 138,

        // MELEE
        BLUNT_WEAPONS_1H           = 102,
        EDGED_WEAPONS_1H           = 103,
        PIERCING                   = 106,
        EDGED_WEAPONS_2H           = 105,
        BLUNT_WEAPONS_2H           = 107,
        MELEE_ENERGY               = 104,
        PARRY                      = 145,
        SNEAK_ATTACK               = 146,
        MULTI_MELEE                = 101,
        FAST_ATTACK                = 147,

        // MISC WEAPONS
        THROWING_KNIFE             = 108,
        GRENADE                    = 109,
        HEAVY_WEAPONS              = 110,

        // RANGED
        BOW                        = 111,
        PISTOL                     = 112,
        ASSAULT_RIFLE              = 116,
        SUBMACHINE_GUN             = 114,
        SHOTGUN                    = 115,
        RIFLE                      = 113,
        RANGED_ENERGY              = 133,
        FLING_SHOT                 = 150,
        AIMED_SHOT                 = 151,
        BURST                      = 148,
        FULL_AUTO                  = 167,
        BOW_SPECIAL_ATTACK         = 121,
        MULTI_RANGED               = 134,

        // SPEED
        MELEE_INITIATIVE           = 118,
        RANGED_INITIATIVE          = 119,
        PHYSICAL_INITIATIVE        = 120,
        NANOC_INIT                 = 149,
        DODGE_RNG                  = 154,
        EVADE_CLSC                 = 155,
        DUCK_EXP                   = 153,
        NANO_RESIST                = 168,
        RUN_SPEED                  = 156,

        // TRADE & REPAIR
        MECHANICAL_ENGINEERING     = 125,
        ELECTRICAL_ENGINEERING     = 126,
        FIELD_QUANTUM_PHYSICS      = 157,
        WEAPON_SMITHING            = 158,
        PHARMACEUTICALS            = 159,
        NANO_PROGRAMMING           = 160,
        COMPUTER_LITERACY          = 161,
        PSYCHOLOGY                 = 162,
        CHEMISTRY                  = 163,
        TUTORING                   = 141,

        // NANO & AIDING
        MATERIAL_METAMORPHOSE      = 127,
        BIOLOGICAL_METAMORPHOSE    = 128,
        PSYCHOLOGICAL_MODIFICATION = 129,
        MATERIAL_CREATION          = 130,
        TIME_AND_SPACE             = 131,
        SENSE_IMPROVEMENT          = 122,
        FIRST_AID                  = 123,
        TREATMENT                  = 124,

        // SPYING
        CONCEALMENT                = 164,
        BREAKING_ENTRY             = 165,
        TRAP_DISARM                = 135,
        PERCEPTION                 = 136,

        // NAVIGATION
        VEHICLE_AIR                = 139,
        VEHICLE_GROUND             = 166,
        VEHICLE_WATER              = 117,
        MAP_NAVIGATION             = 140,
    };

};	// namespace
