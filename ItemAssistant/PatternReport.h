#pragma once

class PatternReport
{
public:
   PatternReport(unsigned int pbid, unsigned int toonid = 0);
   virtual ~PatternReport();

   std::tstring toString() const;

private:
   unsigned int m_pbid;
   unsigned int m_toonid;

   std::tstring m_avail;
   std::tstring m_toonname;
   std::tstring m_time;
   std::tstring m_pbname;
   std::tstring m_table;
};
