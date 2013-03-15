#include "StdAfx.h"
#include "AOPathHasher.h"
#include <boost/functional/hash.hpp>

namespace {

    // Remove trailing slash if any
    void RemoveTralingSlashFromPath( boost::filesystem::path &folderPath )
    {
        if (folderPath.filename().string() == ".") {
            folderPath = folderPath.parent_path();
        }
    }
    
    // If only a drive letter, remove slash
    void RemoveTralingSlashFromDriveLetter( boost::filesystem::path &folderPath )
    {
        if (!folderPath.has_relative_path()) {
            folderPath = folderPath.root_name();
        }
    }

}

namespace shared {

    std::size_t HashParentPath(boost::filesystem::path folderPath)
    {
        ::RemoveTralingSlashFromPath(folderPath);

        folderPath = folderPath.parent_path();

        ::RemoveTralingSlashFromDriveLetter(folderPath);

        // Convert to string with forward slashes
        std::string folder = folderPath.generic_string();

        // Hash string
        boost::hash<std::string> hasher;
        return hasher(folder);
    }

}
