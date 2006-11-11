#pragma once


class Db;


int ImportRecords( unsigned int _Type, Db& itemDB );
int CreateLocalDatabase(Db& itemDB, const char* AoDir);

int OpenLocalDB();
void* GetDataChunk(Db& itemDB, unsigned int _KeyHi, unsigned int _KeyLo, unsigned int* _pSize );


// As of version 15.3.3 according to Jayde @ aodb.info, MAX(LENGTH(name)) is 104, adding some padding here...
#define AODB_MAX_NAME_LEN 127


typedef struct
{
	char			pName[AODB_MAX_NAME_LEN + 1];
	unsigned int	Value;
	unsigned int	QL;
	unsigned int	IconKey;
} MissionItem;

