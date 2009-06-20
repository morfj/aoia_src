#include "StdAfx.h"
#include "DataQueue.h"


DataItem::DataItem(unsigned long _type, unsigned int _size, char* _data, unsigned int ip, unsigned int port)
    : m_type(_type)
    , m_size(_size + sizeof(unsigned int) * 2)
{
    m_data.reset(new char[m_size]);
    memcpy(m_data.get(), &ip, sizeof(unsigned int));
    memcpy(m_data.get() + sizeof(unsigned int), &port, sizeof(unsigned int));
    memcpy(m_data.get() + sizeof(unsigned int) * 2, _data, _size);
}


unsigned long DataItem::type() const
{
    return m_type;
}


char* DataItem::data() const
{
    return m_data.get();
}


unsigned int DataItem::size() const
{
    return m_size;
}


/************************************************************************/


DataQueue::DataQueue()
{
}


DataQueue::~DataQueue()
{
}


void DataQueue::push(DataItemPtr item)
{
    boost::lock_guard<boost::mutex> guard(m_mutex);
    m_queue.push(item);
}


DataItemPtr DataQueue::pop()
{
    boost::lock_guard<boost::mutex> guard(m_mutex);
    DataItemPtr pData = m_queue.front();
    m_queue.pop();
    return pData;
}
