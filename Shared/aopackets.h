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
        AoObjectId		target;		// 0x0000C350	dimension? (50000:<charid>)
    };

	/*struct ClientHeader
    {
		unsigned int	headerid;	// 0x0000000A
		unsigned short	unknown1;	// 0x0000		protocol version?
        unsigned short	msgsize_not_filled;	//				size incl header
		unsigned int	clientid;	// 0x67a6de6a	ClientID?         
        unsigned int	unknown2;	//??ex 00 00 00 02 or 00 00 0b ed  on logoff
		unsigned int	msgid;
        AoObjectId		charId;		// 0x0000C350	(50000:<charid>)
    };*/

	struct InvItemId
	{
		unsigned short	unknown4;//00 00 (part of fromType probably)
		unsigned short	type;//00 68 for inventory etc..
		unsigned short	containerTempId;//the temporary id of the container
		unsigned short	itemSlotId;//The slot id of the item in its container/inventory.
	};

	struct MoveData
	{
		unsigned char	unknown1;//00

		InvItemId		fromItem;

		//unsigned short	unknown2;//00 00 (part of fromType maybe)
		//unsigned short	fromType;
		//unsigned short	fromContainerTempId;// If backpack: the slot id in the inventory that holds the container
		//unsigned short	fromItemSlotId;//The slot id of the item in its container/inventory.
		AoObjectId		toContainer;
		unsigned short	unknown3; //00
		unsigned short	targetSlot; //6f (invalid) except when moving to wear window.
	};

	/*struct MoveOperation
	{
		ClientHeader	header;
		MoveData		moveData;
	};*/

	struct ItemMoved
	{
		Header			header;
		MoveData		moveData;
	};


	/*struct OpenBackpackOperation
	{
		ClientHeader header;
		unsigned char	unknown1;//01
		unsigned int	unknown2;//00 00 00 00
		unsigned int	counter;
		unsigned int	unknown3;//00 00 00 03
		unsigned int	openedbeforeCount; //00 00 00 03
		AoObjectId		owner;//?
		InvItemId		backPack;
		//unsigned short	unknown4;//00 00 (part of fromType probably)
		//unsigned short	fromType;//68 for inventory
		//unsigned short	unknown5;//00 00 (fromContainerTempId probably, but we dont open a backpack from within another backpack)
		//unsigned short	containerTempId;//temp id (temporary)

		//01 00 00 00 00 00 00 00 1b 00 00 00 03 00 00 00 01 00 00 c3 50 67 a6 de 6a 00 00 00 68 00 00 00 43
	};*/

	struct CharacterAction
	{
		//MSG_CHAR_OPERATION
		Header header;
		//00 00 00 00 70 00 00 00 00 00 00 00 68 00 00 00 4e 00 01 3f 5d 00 01 3f 5c 00 00
		unsigned char	unknown1;//01
		unsigned int	operationId;//00 00 00 70 
		unsigned int	unknown3;//00 00 00 00
		InvItemId		itemToDelete;
		AoObjectId		itemId;
		unsigned short	zeroes2; //00 00
	};

	struct ServerCharacterAction
	{
		//MSG_CHAR_OPERATION
		Header header;
		//00 00 00 00 70 00 00 00 00 00 00 00 68 00 00 00 4e 00 01 3f 5d 00 01 3f 5c 00 00
		unsigned char	unknown1;//01
		unsigned int	operationId;//00 00 00 70 
		unsigned int	unknown3;//00 00 00 00
		InvItemId		itemToDelete;
		AoObjectId		itemId;
		unsigned short	zeroes2; //00 00
	};

	struct TradeTransaction
	{
		Header header;
		//00 00 00 00 01 05 00 00 c3 50 67 a6 de 6a 00 00 00 68 00 00 00 54
		unsigned char	unknown1;//01
		unsigned int	unknown2;//00 00 00 01 
		unsigned char	operationId;//01=accept, 02=decline,03=?start?, 04=commit,05=add item,06=remove item
		AoObjectId		fromId;
		InvItemId		itemToTrade;
	};

	struct PartnerTradeItem
	{
		//MSG_PARTNER_TRADE
		Header header;
		//00 00 01 d1 35 00 01 d1 35 00 00 00 01 00 00 00 01|00 00 00 55|00 00 00 68 00 00 00 4f |00 00 00 00 00 00 00 00 
		unsigned char	unknown1;//01
		AoObjectId		itemid;
		unsigned int	ql;
		unsigned short	flags;
		unsigned short	stack;
		unsigned int	operationId;//55=Add, 56=remove
		InvItemId		partnerInvItem;
		InvItemId		myInvItemNotUsed; //

	};

	struct BoughtItemFromShop
	{
		Header			header;
		//df df 00 0a 00 01 00 2d 00 00 0b c4 67 a6 de 6a 05 2e 2f 0c 00 00 c3 50 67 a6 de 6a 00 00 00 54 69 00 00 54 69 00 00 00 01 00 00 01 90 
		unsigned char	unknown1;//00
		AoObjectId		itemid;
		unsigned int	ql;
		unsigned short	flags;
		unsigned short	stack;
	};

	struct Backpack
	{
		//TODO: This is probably a key/value container with the bp props.
		//Being lazy and using the order I have seen on ~100 messages.
		//If someone buys a backpack and gets the wrong backpack name before zoneing, there is a bug here!

		Header          header;
		//*00 00 00 00 0b 00 00 c3 50 44 bb 17 ae 
		unsigned char	unknown1; //0x00
		unsigned int	unknown2; //0x00 00 00 0b  always (rk1 and 2)
		AoObjectId		ownerId;
		unsigned int	zone; //id of the zone you're in
		unsigned int	unCons_f424f;
		unsigned int	unZeroes1;
		unsigned char	operationId; //65=bought bp, 01=opened bp, 0e=?, what is got bp in trade?
		unsigned char	invSlot; //or 6f "find a spot" if you buy one
		unsigned int	unknMass;//1b 97 gives short msg (1009*7) , 1f 88 (1009*8) gives long. probably number of key/val pairs following.
		
		unsigned int		unZeroes2;
		unsigned int	flags; //seems to be a bitwise bp flag (probably unique, nodrop etc)

		unsigned int		unknownKey17;//always 0x17!
		unsigned int	itemKey1;

		unsigned int		unknownKey2bd;//always 0x2bd!
		unsigned int	ql;

		unsigned int		unknownKey2be;//always 0x2be!
		unsigned int	itemKeyLow;//I guess

		unsigned int		unknownKey2bf;//always 0x2bf!
		unsigned int	itemKeyHigh;//I guess

		unsigned int		unknownKey19c;//always 0x19c!
		unsigned int		unknown01;//always 0x01!

		//after this we got two different sizes.. based on unknMass I think


	};

    struct Container
    {
        Header          header;
        unsigned int	unknown1;	// 0x01000000 object type?
        unsigned char	numslots;	// 0x15 for a backpack
        unsigned int	unknown2;	// 0x00000003 ??? introduced after 16.1 or so..
        unsigned int	mass;	      // mass? seems related to number of items in backpack. 1009 + 1009*numitems
    };

    struct ContItem						// Size 0x20
    {
        unsigned int	index;
        unsigned short	flags;	// 0x0021 ?? 1=normal item? 2=backpack 32 = nodrop? 34=nodrop bp?
        unsigned short	stack;
        AoObjectId      containerid;
        AoObjectId		itemid;		// low + high id
        unsigned int	ql;
        unsigned int	nullval;
    };


    struct ContEnd
    {
        AoObjectId		containerId;
        unsigned int	tempContainerId;	// Virtual id of the container
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
        unsigned char   strLen;         // Number of bytes allocated to 'str'
        const char      str;            // array of strLen number of bytes.
    };

    struct MobInfo
    {
        Header         header;
        char           unknown[42];
        AoString       characterName;
    };

#pragma pack(pop)

	enum InventoryIds
    {
		INV_BANK		= 1,
		INV_TOONINV		= 2,
		INV_PLAYERSHOP	= 3,
		INV_TRADE		= 4,//The stuff I put on the table in a trade
		INV_TRADEPARTNER= 5,//The stuff a tradepartner adds to a trade.
		INV_OVERFLOW	= 6,
		INV_HOTSWAPTEMP	= 7,//used by IA when hotswapping as temp storage.
		INV_OTHER_PLAYERSHOP= 8, //Current playershop in trade.
		//remember to update MSG_FULLSYNC to clear the inv you want cleared on zone.
	};

	enum CharacterActionIds
	{
		CHAR_ACTION_SPLITSTACK	= 0x34,
		CHAR_ACTION_JOINSTACKS	= 0x35,
		CHAR_ACTION_DELETEITEM	= 0x70,
		CHAR_ACTION_PLAYERSHOP	= 0xb4,

		CHAR_ACTION_DELETE_TEMP_ITEM  = 0x2f,

		CHAR_ACTION_RUN_NANO	= 0x13,
		CHAR_ACTION_RUN_PERK	= 0xB3,
		CHAR_ACTION_UNKNOWN1	= 0x69,
		CHAR_ACTION_TRADESKILL	= 0x51,
		CHAR_ACTION_LOGOFF1		= 0xd2,
		CHAR_ACTION_LOGOFF2		= 0x78,
		CHAR_ACTION_SNEAK_LEAVE1= 0xaa,
		CHAR_ACTION_SNEAK_LEAVE2= 0xa1,
		CHAR_ACTION_SNEAK_ENTER	= 0xa2,
		CHAR_ACTION_SNEAK		= 0xa3,
		CHAR_ACTION_SKILL_AVAIL	= 0xa4,
		CHAR_ACTION_ZONE		= 0xa7,//when starting to zone
		CHAR_ACTION_ZONE_DATA	= 0xb4,//MANY of these when entering new pf
		CHAR_ACTION_ZONED		= 0xb5,//after all xb4 messages.
		CHAR_ACTION_UNABLE_SKILL_LOCKED	= 0x84,
		CHAR_ACTION_JUMP		= 0x92,
		CHAR_ACTION_MOVED		= 0x89,
		CHAR_ACTION_STAND		= 0x57,
		CHAR_ACTION_OTHERTOON   = 0x62,//unknown, something to do with other players I think

		//0xdd = add/remove from Tradeskill window
		//opId == 0x80 => use an elevator
		//opId == 0x18 => leave team.
		//opId == 0x41 => cancel running nano.
		//0x57 ??? All zeroes, when you stand up

	};

    enum MsgIds
    {
        //MSG_UNKNOWN	=	0x47777000,
        //MSG_UNKNOWN	=	0x5E477770,
        MSG_SKILLSYNC	=	0x3E205660,
        MSG_CONTAINER	=	0x4E536976,
        MSG_BACKPACK	=	0x465A5D73,
        MSG_CHAT		=	0x5F4B442A,
        MSG_MISSIONS	=	0x5C436609,
        //MSG_BANK		   =	0x7F283C34,
        MSG_BANK		=	0x343c287f,    // size 2285.. dynamic most likely
       // MSG_POS_SYNC	=	0x5E477770,    // size = 55 bytes Thats wrong?
        MSG_UNKNOWN_1	=	0x25314D6D,    // size = 53 bytes
        MSG_UNKNOWN_2	=	0x00000001,    // size = 40 bytes
        MSG_UNKNOWN_3	=	0x0000000b,    // size = 548 bytes
        MSG_FULLSYNC	=	0x29304349,    // size 4252.. dynamic?  Contains equip and inv + atribs (?) +++
        MSG_UNKNOWN_4	=	0x54111123,
        MSG_MOB_SYNC	=	0x271B3A6B,
        MSG_SHOP_ITEMS	=	0x353F4F52,     // Content of a player shop
        MSG_SHOP_INFO	=	0x2e072a78,   // Information about a playershop. Received on zone into a playershop.
		MSG_ITEM_MOVE	=	0x47537a24,    //Client or server msg move item between inv/bank/wear/backpack
		MSG_ITEM_BOUGHT	=	0x052e2f0c,	//Sent from server when buying stuff, 1 message pr item.
		MSG_OPENBACKPACK=	0x52526858,//1196653092: from client when opening backpack
		MSG_CHAR_OPERATION= 0x5e477770,  //was MSG_ITEM_OPERATION //from client when deleting an item, splitting or joining, nano, perk, tradeskill+++
		MSG_SHOP_TRANSACTION=0x36284f6e,  //from server and client when adding/removing/accepting shop stuff
		MSG_PARTNER_TRADE = 0x35505644, //from server when trade partner adds/removes items.
		MSG_SHOW_TEXT	 =	0x206b4b73, //from server when showing yellow text (like "You succesfully combined.../XXX is already fully loaded")
		MSG_SPAWN_REWARD =  0x3b11256f,
		//MSG_DELETE_MISH??=	0x212c487a 
		/*
		followed by a move slotId0 from overflow each!
		spiked food sack: (id 0187C4)
DFDF000A	00010077	00000BF9	991FC4E6	3B11256F
0000C73D	498B058E	00000000	0B0000C3	50991FC4
E6000002	1C000F42	4F000000	00716F00	001F8800
00000080	00000300	00001700	0187C400	0002BD00
00000100	0002BE00	0187C400	0002BF00	0187C400
00019C00	00000100	0001EB00	00000100	000000FD

mission key:
DFDF000A	00010089	00000BF9	991FC4E6	3B11256F
0000C76D	0907C318	00000000	0B0000C3	50991FC4
E6000002	1C000F42	4F000000	00716F00	001B9700
00000080	00020500	00001700	006FA100	0002BD00
00000100	0002BE00	006FA100	0002BF00	006FA100
00019C00	00000100	00001A4D	69737369	6F6E206B
65792074	6F206120	6275696C	64696E67	00FDFDFD

[5500] operationId  0xe 
[5500] operationId  0x71 

//Also TODO: mish items deleted when mish deleted not auto

		*/
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
