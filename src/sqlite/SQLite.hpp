#ifndef HEADER_SQLITE
#define HEADER_SQLITE

#include <string>

#include "sqlite-3.35.4/sqlite3.h"
#include "Handle.hpp"

#include "_debug_utils/_debug_assert.hpp"

enum class Type
{
  Integer = SQLITE_INTEGER,
  Float = SQLITE_FLOAT,
  Blob = SQLITE_BLOB,
  Null = SQLITE_NULL,
  Text = SQLITE_TEXT,
};

const char *TypeName(Type const type)
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

struct Exception
{
  int Result = 0;
  std::string Message;

  explicit Exception(sqlite3 *const connection)
      : Result(sqlite3_extended_errcode(connection)),
        Message(sqlite3_errmsg(connection))
  {
  }
};

class Connection
{

  struct ConnectionHandleTraits : HandleTraits<sqlite3 *>
  {
    static void Close(Type value) noexcept
    {
      int close_result = sqlite3_close(value);
      _debug_assert(SQLITE_OK == close_result,
                    "SQLite database resource close error CODE:" << close_result);
    }
  };

  using ConnectionHandle = Handle<ConnectionHandleTraits>;
  ConnectionHandle m_handle;

  template <typename F, typename C>
  void InternalOpen(F open, C const *const filename)
  {
    Connection temp;

    if (SQLITE_OK != open(filename, temp.m_handle.Set()))
    {
      ThrowLastError();
    }

    swap(m_handle, temp.m_handle);
  }

public:
  Connection() noexcept = default;

  sqlite3 *GetAbi() const noexcept
  {
    return m_handle.Get();
  }

  void ThrowLastError() const
  {
    throw Exception(GetAbi());
  }

  template <typename C>
  explicit Connection(C const *const filename)
  {
    using namespace std;
    Open(filename);
#ifdef NDEBUG
#else
    this->Profile([](void *, char const *const statement,
                    unsigned long long const time) {
      unsigned long long const ms = time / 1000000;

      if (ms > 10)
      {
        cout << "SQLite profiler: "
             << '(' << ms << " ms) "
             << statement
             << endl;
      }
    });
#endif
  }

  static Connection Memory()
  {
    return Connection(":memory:");
  }

  explicit operator bool() const noexcept
  {
    return static_cast<bool>(m_handle);
  }

  void Open(char const *const filename)
  {
    InternalOpen(sqlite3_open, filename);
  }

  long long RowId() const noexcept
  {
    return sqlite3_last_insert_rowid(GetAbi());
  }

  template <typename F>
  void Profile(F callback, void *const context = nullptr)
  {
    sqlite3_profile(GetAbi(), callback, context);
  }
};

class Backup
{
  struct BackupHandleTraits : HandleTraits<sqlite3_backup *>
  {
    static void Close(Type value) noexcept
    {
      sqlite3_backup_finish(value);
    }
  };

  using BackupHandle = Handle<BackupHandleTraits>;
  BackupHandle m_handle;

  Connection const *m_destination = nullptr;

public:
  Backup(Connection const &destination,
         Connection const &source,
         char const *const destinationName = "main",
         char const *const sourceName = "main")
      : m_handle(sqlite3_backup_init(
            destination.GetAbi(),
            destinationName,
            source.GetAbi(),
            sourceName)),
        m_destination(&destination)
  {
    if (!m_handle)
    {
      destination.ThrowLastError();
    }
  }

  sqlite3_backup *GetAbi() const noexcept
  {
    return m_handle.Get();
  }

  bool Step(int const pages = -1)
  {
    int rc = sqlite3_backup_step(GetAbi(), pages);
    bool res = rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED;

    return res;
  }

  void CreateBackup() noexcept
  {
    while (Step(16))
    {
      sqlite3_sleep(128);
    }
  }
};

template <typename T>
struct Reader
{
  int GetInt(int const column = 0) const noexcept
  {
    return sqlite3_column_int(static_cast<T const *>(this)->GetAbi(), column);
  }

  char const *GetString(int const column = 0) const noexcept
  {
    return reinterpret_cast<char const *>(
        sqlite3_column_text(static_cast<T const *>(this)->GetAbi(), column));
  }

  int GetStringLength(int const column = 0) const noexcept
  {
    return sqlite3_column_bytes(static_cast<T const *>(this)->GetAbi(), column);
  }

  Type GetType(int const column = 0) const noexcept
  {
    return static_cast<Type>(
        sqlite3_column_type(static_cast<T const *>(this)->GetAbi(), column));
  }

  std::string GetTypeStr(int const column = 0) const noexcept
  {
    return std::string(TypeName(GetType(column)));
  }
};

class Row : public Reader<Row>
{
  sqlite3_stmt *m_statement = nullptr;

public:
  sqlite3_stmt *GetAbi() const noexcept
  {
    return m_statement;
  }

  Row(sqlite3_stmt *const statement) noexcept
      : m_statement(statement)
  {
  }
};

class Statement : public Reader<Statement>
{

  struct StatementHandleTraits : HandleTraits<sqlite3_stmt *>
  {
    static void Close(Type value) noexcept
    {
      int result = sqlite3_finalize(value);
      _debug_assert(SQLITE_OK == result, "Statement finalized with error code: " << result);
    }
  };

  using StatementHandle = Handle<StatementHandleTraits>;
  StatementHandle m_handle;

  template <typename F, typename C, typename... Values>
  void InternalPrepare(Connection const &connection, F prepare, C const *const text,
                       Values &&...values)
  {
    _debug_assert(connection, "connection is closed!!!");

    if (SQLITE_OK != prepare(connection.GetAbi(), text, -1, m_handle.Set(), nullptr))
    {
      connection.ThrowLastError();
    }

    BindAll(std::forward<Values>(values)...);
  }

  void InternalBind(int) const noexcept
  {
  }

  template <typename First, typename... Rest>
  void InternalBind(int const index, First &&first, Rest &&...rest) const
  {
    Bind(index, std::forward<First>(first));
    InternalBind(index + 1, std::forward<Rest>(rest)...);
  }

public:
  Statement() noexcept = default;

  template <typename C, typename... Values>
  Statement(Connection const &connection,
            C const *text,
            Values &&...values)
  {

    Prepare(connection, text, std::forward<Values>(values)...);
  }

  explicit operator bool() const noexcept
  {
    return static_cast<bool>(m_handle);
  }

  sqlite3_stmt *GetAbi() const noexcept
  {
    return m_handle.Get();
  }

  void ThrowLastError() const
  {
    throw Exception(sqlite3_db_handle(GetAbi()));
  }

  template <typename... Values>
  void Prepare(Connection const &connection,
               char const *const text,
               Values &&...values)
  {
    InternalPrepare(connection, sqlite3_prepare_v2, text, std::forward<Values>(values)...);
  }

  bool Step() const
  {
    int sqlite3_result = sqlite3_step(GetAbi());

    if (SQLITE_ROW == sqlite3_result)
    {
      bool result = true;
      return result;
    }

    if (SQLITE_DONE == sqlite3_result)
    {
      bool result = false;
      return result;
    }

    ThrowLastError();
    return false;
  }

  void Execute() const
  {
    _debug_assert(!Step(), "The Step() should return false");
  }

  void Bind(int const index, int const value) const
  {
    if (SQLITE_OK != sqlite3_bind_int(GetAbi(), index, value))
    {
      ThrowLastError();
    }
  }

  void Bind(int const index, char const *const value, int const size = -1) const
  {
    if (SQLITE_OK != sqlite3_bind_text(GetAbi(), index, value, size, SQLITE_STATIC))
    {
      ThrowLastError();
    }
  }

  void Bind(int const index, const std::string &value) const
  {
    Bind(index, value.c_str(), value.size());
  }

  void Bind(int const index, const std::string &&value) const
  {
    if (SQLITE_OK != sqlite3_bind_text(GetAbi(), index, value.c_str(),
                                       value.size(), SQLITE_TRANSIENT))
    {
      ThrowLastError();
    }
  }

  template <typename... Values>
  void BindAll(Values &&...values) const
  {
    InternalBind(1, std::forward<Values>(values)...);
  }

  template <typename... Values>
  void Reset(Values &&...values) const
  {
    if (SQLITE_OK != sqlite3_reset(GetAbi()))
    {
      ThrowLastError();
    }

    BindAll(values...);
  }
};

class RowIterator
{
  Statement const *m_statement = nullptr;

public:
  RowIterator() noexcept = default;

  RowIterator(Statement const &statement)
  {
    if (statement.Step())
    {
      m_statement = &statement;
    }
  }

  RowIterator &operator++() noexcept
  {
    if (!m_statement->Step())
    {
      m_statement = nullptr;
    }

    return *this;
  }

  bool operator!=(RowIterator const &other) const noexcept
  {
    return m_statement != other.m_statement;
  }

  Row operator*() const noexcept
  {
    return Row(m_statement->GetAbi());
  }
};

inline RowIterator begin(Statement const &statement) noexcept
{
  return RowIterator(statement);
}

inline RowIterator end(Statement const &statement) noexcept
{
  return RowIterator();
}

template <typename C, typename... Values>
void Execute(Connection const &connection,
             C const *const text,
             Values &&...values)
{
  Statement(connection, text, std::forward<Values>(values)...).Execute();
}

void BeginTransaction(Connection const &connection)
{
  Statement(connection, "BEGIN");
}

void CommitTransaction(Connection const &connection)
{
  Statement(connection, "COMMIT");
}

#endif
