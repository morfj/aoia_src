#pragma once

#include <vector>
#include <string>
#include <map>


// Puts the hex value of the specified char into a string
#define CHAR2HEXSTR( character ) \
   (((std::ostringstream&)(std::ostringstream().flush() << std::setfill('0') \
   << std::setw(2) << std::hex << std::uppercase \
   << static_cast<unsigned int>(static_cast<unsigned char>(character)))).str())

// Encodes the character to an escape character followed by the hex code
#define ENCODECHAR( character, escapechar ) \
   (((std::ostringstream&)(std::ostringstream().flush() << escapechar \
   << std::uppercase << std::setfill('0') << std::setw(2) << std::hex \
   << static_cast<unsigned int>(static_cast<unsigned char>(character)))).str()) 

// Wraps streaming data into a text string.
//#define STREAM2STR( streamdef ) \
//   (((std::ostringstream&)(std::ostringstream().flush() << streamdef)).str()) 


class SharedServices
{
public:
   SharedServices();
   virtual ~SharedServices(void);

   std::tstring GetContainerName(unsigned int charid, unsigned int containerid) const;
   std::map<std::tstring, std::tstring> GetAOItemInfo(unsigned int lowkey) const;

protected:
   std::vector<std::tstring> GetAccountNames() const;
   std::tstring MakeContainerName(unsigned int charid, unsigned int containerid) const;

private:
   std::vector<std::tstring> m_accounts;

   mutable std::map< __int64, std::pair<std::tstring, FILETIME> > m_containerFileCache;
   mutable std::map< __int64, std::tstring > m_containerDBCache;
};
