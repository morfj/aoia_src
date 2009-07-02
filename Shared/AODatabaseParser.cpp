#include "stdafx.h"
#include "AODatabaseParser.h"
#include "AOItemParser.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>

using namespace boost;
using namespace boost::assign;
using namespace boost::algorithm;
using namespace boost::iostreams;
using namespace boost::filesystem;

struct DataBaseRecordHeader
{
    unsigned int record_size;
    unsigned int payload_size;
    unsigned int payload_type;
};


AODatabaseParser::AODatabaseParser(std::vector<std::string> const& aodbfiles)
  : m_current_file_offset(0)
{
    m_buffer.reset(new char[1024*1024]); // Creating a 1 MB buffer for record parsing.

    unsigned int accumulated_offset = 0;
    for (std::vector<std::string>::const_iterator it = aodbfiles.begin(); it != aodbfiles.end(); ++it)
    {
        path file(*it);
        if (exists(file) && is_regular(file))
        {
            m_file_offsets[accumulated_offset] = *it;
            accumulated_offset += file_size(file);
        }
        else
        {
            throw Exception("Invalid database file specified.");
        }
    }
}


AODatabaseParser::~AODatabaseParser()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}


shared_ptr<ao_item> AODatabaseParser::GetItem(unsigned int offset) const
{
    shared_ptr<ao_item> retval;

    EnsureFileOpen(offset);
    m_file.seekg(offset - m_current_file_offset);

    // Check that the offset has the magic 0xFAFA cookie.
    if (m_file.peek() != 0xFA)
    {
        return retval;
    }
    m_file.ignore();
    if (m_file.peek() != 0xFA)
    {
        return retval;
    }
    m_file.ignore();

    // Load record header
    DataBaseRecordHeader record_header;
    m_file.read((char*)&record_header, sizeof(DataBaseRecordHeader));
    if (m_file.fail())
    {
        return retval;
    }

    // Load record payload
    m_file.read(m_buffer.get(), record_header.payload_size);
    if (m_file.fail())
    {
        return retval;
    }

    // Parse payload
    try
    {
        retval.reset(new AOItemParser(m_buffer.get(), record_header.payload_size));
    }
    catch (std::exception &e)
    {
        std::ostringstream oss;
        oss << "Caught exception trying to parse item at offset " << offset << ". Original error: '" << e.what() << "'.";
        throw Exception(oss.str());
    }

    return retval;
}


void AODatabaseParser::EnsureFileOpen(unsigned int offset) const
{
    if (m_file.is_open())
    {
        if (offset < m_current_file_offset ||
            offset > m_current_file_offset + m_current_file_size)
        {
            // If the offset is outside the current file, we have to open another file.
            m_file.close();
            OpenFileFromOffset(offset);
        }
    }
    else
    {
        OpenFileFromOffset(offset);
    }
}


std::pair<unsigned int, std::string> AODatabaseParser::GetFileFromOffset(unsigned int offset) const
{
    std::map<unsigned int, std::string>::const_iterator it = m_file_offsets.upper_bound(offset);
    if (it == m_file_offsets.begin())
    {
        throw Exception("Unable to locate database file for offset.");
    }
    --it;   // Decrement to get to the item we are after.
    if (it == m_file_offsets.end())
    {
        throw Exception("Unable to locate database file for offset.");
    }

    return make_pair(it->first, it->second);
}


void AODatabaseParser::OpenFileFromOffset(unsigned int offset) const
{
    std::pair<unsigned int, std::string> offset_file_info = GetFileFromOffset(offset);
    m_current_file_offset = offset_file_info.first;
    m_current_file_size = file_size(offset_file_info.second);
    m_file.open(offset_file_info.second.c_str(), std::ifstream::in | std::ifstream::binary);

    if (!m_file.is_open())
    {
        throw Exception("Unable to open database file.");
    }
}
