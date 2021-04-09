#include <cstdlib>
#include <string>
#include <iostream>

#include "_debug_utils/_debug_assert.hpp"
#include "client_cli/client_cli.h"


int main(int argc, char **argv)
{
  using namespace std;

  try
  {

    ClientCli client = ClientCli();

    string dbName;
    char c;
    int numberOfPeoples;

    while (true)
    {
      cout << "Enter the name of a new or existing database: ";
      cin >> dbName;
      client.OpenNewDatabaseConnection(dbName.c_str());

      cout << "How many people do you want to save to the database: ";
      cin >> numberOfPeoples;

      for (int i = 0; i < numberOfPeoples; i++)
      {
        string peopleName;
        cout <<'[' << i + 1 <<"] Enter a name for the person: ";
        cin >> peopleName;

        client.SavePeople(peopleName);
      }

      auto peoples = client.SelectPeoples();
      for (string s : peoples)
      {
        cout << s << endl;
      }

      cout << "If you want to create backup for database, write \'y\': ";
      cin >> c;

      if (c == 'y')
      {
        cout << "Enter the name of a backup database: ";
        cin >> dbName;
        client.CreateBackup(dbName);
      }

      cout << "If you want to exit, write \'q\' or else write any other: ";
      cin >> c;

      if (c == 'q')
        break;
    }
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
