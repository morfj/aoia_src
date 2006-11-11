#pragma once


#include "shared/Mutex.h"

#include <vector>


namespace SQLite {

   class Db;
   class Table;

   typedef boost::shared_ptr<Db> DbPtr;
   typedef boost::shared_ptr<Table> TablePtr;


   class Table
   {
   public:
      Table(int nrow, int ncol, char **result)
      {
         m_headers.reserve(ncol);
         m_data.reserve(nrow*ncol);

         for(int i=0; i < ncol; ++i)
         {
            m_headers.push_back(result[i]);   /* First row heading */
         }
         for(int i=0; i < ncol*nrow; ++i)
         {
            if (result[ncol+i] == NULL)
            {
               m_data.push_back(std::string());
            }
            else
            {
               m_data.push_back(result[ncol+i]);
            }
         }
      }

      std::vector<std::string> const& Headers() const { return m_headers; }
      std::vector<std::string> const& Data() const { return m_data; }
      std::string Data(unsigned int row, unsigned int col) const {
         if (row < Rows() && col < Columns())
         {
            return m_data.at(Columns()*row + col);
         }
         return "";
      }

      size_t Columns() const { return m_headers.size(); }
      size_t Rows() const { return m_headers.size() > 0 ? m_data.size() / m_headers.size() : 0; }

   private:
      std::vector<std::string> m_headers;
      std::vector<std::string> m_data;
   };


   class Db
   {
   public:
      Db();
      virtual ~Db();

      bool Init(std::tstring const& filename = _T("init.db"));
      void Term();

      bool Exec(std::tstring const& sql);
      TablePtr ExecTable(std::tstring const& sql);

      void Begin();
      void Commit();
      void Rollback();

   private:
      sqlite3 *m_pDb;
   };

}  // namespace SQLite


class DBManager : public SQLite::Db
{
public:
   DBManager(void);
   virtual ~DBManager(void);

   bool Init(std::tstring dbfile);
   void Term();

   std::tstring AOFolder() const { return m_aofolder; }

   void InsertItem(unsigned int keylow, unsigned int keyhigh, unsigned short ql, unsigned short stack, 
      unsigned int parent, unsigned short slot, unsigned int children, unsigned int owner);
   
   std::tstring GetToonName(unsigned int charid) const;
   void SetToonName(unsigned int charid, std::tstring const& newName);

   void Lock() { m_mutex.MutexOn(); }
   void UnLock() { m_mutex.MutexOff(); }

protected:
   virtual std::tstring GetFolder(HWND hWndOwner, std::tstring const& title);

private:
   std::tstring   m_aofolder;
   Mutex          m_mutex;
};
