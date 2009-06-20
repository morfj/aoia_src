#ifndef NETUTILS_H
#define NETUTILS_H

#include <vector>


namespace NetUtils
{
    /// Returns all known IPv4 addresses for the specified hostname.
    std::vector<unsigned int> LookupHost(std::string const& hostname);
};


#endif // NETUTILS_H
