
#include <shared/localdb.h>
#include <shared/AODB.h>
#include <shared/ctree.h>

//#include <db_cxx.h>
#include <shlobj.h>

extern IFIL udtAODBfile;


//int CreateLocalDatabase(Db& itemDB, const char* AoDir)
//{
//	DWORD dwVersion, dwWindowsMajorVersion;
//
//
//	// Delete old database
//	DeleteFile( "AODatabase.bdb" );
//	DeleteFile( "tmp.bdb" );
//
//	// Open AO's ctree databasse
//	if( InitAODatabase( AoDir ) != AODB_ERR_NOERROR )
//		return FALSE;
//
//	// Under XP, for some reasons it's a lot faster
//	// when forcing a 512 bytes page size
//	// but it's not the case for 98/ME...
//	dwVersion = GetVersion();
//	dwWindowsMajorVersion = ( DWORD )( LOBYTE( LOWORD( dwVersion ) ) );
//
//	if( !( dwVersion >= 0x80000000 && dwWindowsMajorVersion >= 4 ) )
//		itemDB.set_pagesize( 512 );
//
//	// Use a big cache to try to get decent performances
//	// under win98/ME...
//	itemDB.set_cachesize( 0, 1024 * 1024 * 32, 64 );
//
//	if( itemDB.open( NULL, "tmp.bdb", NULL, DB_HASH, DB_CREATE, 0 ) )
//	{
//		itemDB.close(0);
//		ReleaseAODatabase();
//		return FALSE;
//	}


	// Import playfield records in local DB
	//if( !ImportRecords( AODB_TYP_PF, itemDB ) )
	//{
	//	itemDB.close(0);
	//	ReleaseAODatabase();
	//	return FALSE;
	//}

	//// Import item records in local DB
	//if( !ImportRecords( AODB_TYP_ITEM, itemDB ) )
	//{
	//	itemDB.close(0);
	//	ReleaseAODatabase();
	//	return FALSE;
	//}

	// Import icon records in local DB
	//if( !ImportRecords( AODB_TYP_ICON, itemDB ) )
	//{
	//	itemDB.close(0);
	//	ReleaseAODatabase();
	//	return FALSE;
	//}

//
//	itemDB.close(0);
//	ReleaseAODatabase();
//
//	rename( "tmp.bdb", "AODatabase.bdb" );
//
//	return TRUE;
//}

//
//int ImportRecords( unsigned int _Type, Db& itemDB )
//{
//	unsigned int Key[2];
//	unsigned int Size;
//	char* pBuffer;
//	MissionItem sItem;
//
//	unsigned int numItems = 0, numBytes=0;
//	char ps[256];
//
//	// Alloc a temporary buffer to store the record
//	// A bit oversized but then i'm sure that everything
//	// will fit, and I feel lazy about first getting
//	// the record size and then allocating the proper amount of ram,
//	// at least for database conversion purpose.
//	if( !( pBuffer = (char*)malloc( 524288 ) ) )
//		return FALSE;
//
//	// Get first record of requested type
//	Key[0] = _byteswap_ulong( _Type );
//	Key[1] = 0;
//
//	Size = 524288;
//	
//	if( GetGTEVRecord(udtAODBfile.tfilno + 1, &Key, pBuffer, &Size ) )
//	{
//		free( pBuffer );
//		return FALSE;
//	}
//
//	unsigned int* pKey = (unsigned int*)pBuffer;
//
//	Dbt bdbKey(pBuffer, 8);
//	Dbt bdbData;
//
//   switch (_Type)
//   {
//   case AODB_TYP_ITEM:
//      bdbData.set_data(&sItem);
//      bdbData.set_size(sizeof(sItem));
//      break;
//   case AODB_TYP_PF:
//      bdbData.set_data(pBuffer + 0x20);
//      break;
//   default:
//      bdbData.set_data(pBuffer + 0x18);
//   }
//
//	// Read the records in sequence until a different type (or no more records)
//	// is reached, and store them in the local db
//	while( *pKey == _Type )
//	{
//		//puCheckMessages();
//      switch (_Type)
//      {
//      case AODB_TYP_ITEM:
//         {
//            char *a_xData = pBuffer + 0x18;
//            char *a_xDataPtr = a_xData;
//            // Reducing size of database here...
//            /* Set ptr to first slot */
//            a_xDataPtr = a_xData + 0xc; // + 0x24;
//            memset(&sItem, 0, sizeof(sItem));
//            /* Process the slots we require */
//            while (a_xDataPtr + 8 <= a_xData + Size)
//            {
//               unsigned int lStat = *((unsigned long *)a_xDataPtr);
//               unsigned int lValue = *((unsigned long *)a_xDataPtr+1);
//               a_xDataPtr += 0x8;
//
//               switch (lStat)
//               {
//                  /* Quality */
//               case 0x36:
//                  sItem.QL = (unsigned short)lValue;
//                  break;
//
//                  /* Icon */
//               case 0x4F:
//                  sItem.IconKey = lValue;
//                  break;
//
//                  /* Item Value */
//               case 0x4A:
//                  sItem.Value = lValue;
//                  break;
//
//                  /* Item name and description */
//               case 0x15:
//                  if (lValue == 0x21)
//                  {
//                     unsigned short  lNameLen = *((unsigned short *)a_xDataPtr);
//                     unsigned short  lDescLen = *((unsigned short *)a_xDataPtr+1);
//                     a_xDataPtr += 0x4;
//
//                     /* Store the item name */
//                     if (lNameLen > AODB_MAX_NAME_LEN)
//                        lNameLen = AODB_MAX_NAME_LEN;
//                     memcpy(sItem.pName, a_xDataPtr, lNameLen);
//                     sItem.pName[lNameLen] = 0;
//                  }
//                  break;
//               }
//            }
//         }
//         break;
//
//      case AODB_TYP_PF:
//         bdbData.set_size((unsigned)strlen((char*)bdbData.get_data())+1);
//         break;
//      default:
//         bdbData.set_size(Size - 0x18);
//      }
//		if( itemDB.put( NULL, &bdbKey, &bdbData, 0 ) )
//			return FALSE;
//		numItems++;
//		numBytes += bdbData.get_size();
//		Size = 524288;
//		if( NextVRecord(udtAODBfile.tfilno + 1, pBuffer, &Size ) )
//			break;
//
//	}
//
//	free( pBuffer );
//	sprintf(ps, "Recorded %d records, %d total size\n", numItems, numBytes);
//	OutputDebugString(ps);
//
//	return TRUE;
//}

//int OpenLocalDB()
//{
//	if( g_pDB->open( g_pDB, "AODatabase.bdb", NULL, DB_UNKNOWN, DB_RDONLY, 0 ) )
//		return FALSE;
//
//	return TRUE;
//}

//void* GetDataChunk(Db& itemDB, unsigned int _KeyHi, unsigned int _KeyLo, unsigned int* _pSize )
//{
//	unsigned int Key[2];
//
//	Key[0] = _KeyHi;
//	Key[1] = _KeyLo;
//
//	Dbt bdbKey(Key, 8);
//
//	Dbt bdbData;
//	bdbData.set_flags(DB_DBT_MALLOC);
//
//	if( itemDB.get( NULL, &bdbKey, &bdbData, 0 ) )
//		return NULL;
//
//	if( _pSize )
//		*_pSize = bdbData.get_size();
//
//	return bdbData.get_data();
//}