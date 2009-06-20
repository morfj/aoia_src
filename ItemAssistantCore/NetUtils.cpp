#include "StdAfx.h"
#include "NetUtils.h"


namespace NetUtils
{

    std::vector<unsigned int> LookupHost(std::string const& hostname)
    {
        std::vector<unsigned int> retval;

        addrinfo hints;
        ZeroMemory (&hints, sizeof(addrinfo));
        hints.ai_family = AF_UNSPEC;
        //hints.ai_flags = AI_NUMERICHOST;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        addrinfo* results = NULL;
        addrinfo* current = NULL;
        DWORD error = 0;

        if (error = ::getaddrinfo(hostname.c_str(), NULL, &hints, &results) == 0)
        {
            for (current = results; current != NULL; current = current->ai_next)
            {
                if (current->ai_family != AF_INET)
                {
                    continue;
                }
                if (current->ai_protocol != IPPROTO_TCP)
                {
                    continue;
                }

                unsigned int ip = _byteswap_ulong(*(int*)(current->ai_addr->sa_data + 2));
                retval.push_back(ip);
            }

            freeaddrinfo(results);
        }

        return retval;
    }

}
