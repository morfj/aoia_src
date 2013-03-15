#ifndef AOPATHHASHER_H
#define AOPATHHASHER_H

#include <boost/filesystem.hpp>

namespace shared {

    // Given the installation folder for AO, calculates the hash used in the LocalAppData path.
    std::size_t HashParentPath(boost::filesystem::path folderPath);

}   // namespace

#endif // AOPATHHASHER_H
