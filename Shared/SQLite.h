#pragma once

#include <shared/UnicodeSupport.h>
#include <boost/smart_ptr.hpp>
#include <vector>

// Forward declarations
struct sqlite3;

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
        struct QueryFailedException : public std::exception
        {
            QueryFailedException(std::tstring const& msg) : std::exception(to_ascii_copy(msg).c_str()) {}
        };

        Db();
        virtual ~Db();

        bool Init(std::tstring const& filename = _T("init.db"));
        void Term();

        bool Exec(std::tstring const& sql) const;
        bool Exec(std::string const& sql) const;
        TablePtr ExecTable(std::tstring const& sql) const;

        void Begin() const;
        void Commit() const;
        void Rollback() const;

    private:
        sqlite3 *m_pDb;
    };

}  // namespace SQLite
