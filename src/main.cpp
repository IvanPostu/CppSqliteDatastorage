#include <cstdlib>
#include <string>
#include <iostream>

#include "_debug_utils/_debug_assert.hpp"
#include "sqlite/SQLite.hpp"

static const char *TypeName(Type const type)
{
  switch (type)
  {
  case Type::Integer:
    return "Integer";
  case Type::Float:
    return "Float";
  case Type::Blob:
    return "Blob";
  case Type::Null:
    return "Null";
  case Type::Text:
    return "Text";
  }

  _debug_assert(false, "TypeName is invalid !!!");
  return "Invalid";
}

int main(int argc, char **argv)
{
  using namespace std;

  try
  {
    // Connection connection = Connection::Memory();

    Connection connection("build/datastorage.db");

    connection.Profile([](void *, char const *const statement, unsigned long long const time) {
      unsigned long long const ms = time / 1000000;

      if (ms > 10)
      {
        cout << "SQLite profiler: "
             << '(' << ms << " ms) "
             << statement
             << endl;
      }
    });


    Connection backupConn = Connection("build/zzqq.db");

    Backup backup( backupConn, connection);


    Statement count(connection, "SELECT COUNT(*) FROM zz");
    count.Step();

    cout << "count: " << count.GetInt() << endl;
  }
  catch (Exception const &e)
  {
    std::cerr << e.Message.c_str()
              << " : "
              << e.Result
              << std::endl;
  }

  return EXIT_SUCCESS;
}
