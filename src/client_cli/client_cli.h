#ifndef HEADER_CLIENT_CLI
#define HEADER_CLIENT_CLI

#include <string>
#include <vector>

#include "sqlite/SQLite.hpp"

class ClientCli
{
private:
  Connection *m_connection;
  static const char* SELECT_QUERY;

public:
  ClientCli()
      : m_connection(nullptr)
  {
  }

  ~ClientCli()
  {
    if (m_connection != nullptr)
    {
      delete m_connection;
    }
  }

  void OpenNewDatabaseConnection(std::string db_name) noexcept
  {
    if (m_connection != nullptr)
    {
      delete m_connection;
    }

    m_connection = new Connection(db_name.c_str());

    Execute(*m_connection, "CREATE TABLE IF NOT EXISTS people (name TEXT)");
  }

  void SavePeople(std::string peopleName) noexcept
  {
    Statement m_insert(*m_connection, "INSERT INTO people (name) VALUES (?1)");
    m_insert.Reset(peopleName);
    m_insert.Execute();
  }

  std::vector<std::string> SelectPeoples() noexcept
  {
    std::vector<std::string> ret;

    Statement select(*m_connection, SELECT_QUERY);

    std::cout << "Run: " << SELECT_QUERY << std::endl;

    for(Row row : select)
    {
      ret.emplace_back(row.GetString());
    }

    return ret;
  }

  void CreateBackup(std::string backupName) noexcept
  {
    Connection backupConn(backupName.c_str());
    Backup backup( backupConn, *m_connection);
    backup.CreateBackup();
  }



};

const char* ClientCli::SELECT_QUERY = "SELECT name FROM people";

#endif
