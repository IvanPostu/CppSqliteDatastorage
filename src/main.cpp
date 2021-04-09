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

  try
  {
    Connection connection = Connection::Memory();

    Execute(connection, "CREATE TABLE Users (Id INTEGER PRIMARY KEY, Name)");

    Execute(connection, "INSERT INTO Users (Id, Name) VALUES (?, ?)", 1, "Ion");
    std:: cout<<"inserted: "<< connection.RowId() <<std::endl;

    Execute(connection, "INSERT INTO Users (Id, Name) VALUES (?, ?)", 22, "John");
    std:: cout<<"inserted: "<< connection.RowId() <<std::endl;


    Execute(connection, "INSERT INTO Users (Id, Name) VALUES (?, ?)", 3, 7761);

    for (Row row : Statement(connection, "SELECT RowId, Id, Name FROM Users"))
    {
      std::cout
          << "[" << row.GetString(0) << "] "
          << row.GetString(1)
          << ": "
          << row.GetString(2)
          << ": " << TypeName(row.GetType())
          << std::endl;
    }
  }
  catch (Exception const &e)
  {
    std::cerr << e.Message.c_str() << std::endl
              << e.Result;
  }

  return EXIT_SUCCESS;
}
