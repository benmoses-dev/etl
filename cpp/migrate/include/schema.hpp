#pragma once

#include <functional>
#include <libpq-fe.h>
#include <memory>
#include <mariadb/mysql.h>
#include <string>

struct MysqlConfig {
    std::string myname;
    std::string myhost;
    std::string myuser;
    std::string mypass;
    int myport;
};

struct PgsqlConfig {
    std::string pgname;
    std::string pghost;
    std::string pguser;
    std::string pgpass;
    int pgport;
};

enum class PgType { INT32, INT64, TEXT, TIMESTAMPTZ, MACADDR };

struct ColumnMapping {
    std::string name;
    PgType type;
    std::function<std::vector<char>(const std::string &)> converter;
};

struct MysqlDeleter {
    void operator()(MYSQL *mysql) const noexcept;
};

struct MysqlResDeleter {
    void operator()(MYSQL_RES *res) const noexcept;
};

struct PgDeleter {
    void operator()(PGconn *pg) const noexcept;
};

using MysqlPtr = std::unique_ptr<MYSQL, MysqlDeleter>;
using MysqlResPtr = std::unique_ptr<MYSQL_RES, MysqlResDeleter>;
using PgPtr = std::unique_ptr<PGconn, PgDeleter>;

class DBHelper {
  public:
    DBHelper(const std::string &fromTable, const std::string &toTable,
             const std::vector<ColumnMapping> &mapping);

    void startCopy();
    MYSQL_ROW getMysqlRow();
    void writeRow(const MYSQL_ROW &row);
    void endCopy();

  private:
    MysqlPtr mysql;
    MysqlResPtr res;
    PgPtr pg;

    const std::vector<ColumnMapping> &mapping;
    const std::string fromTable;
    const std::string toTable;

    MysqlConfig myConfig;
    PgsqlConfig pgConfig;

    void initPGConnection();
    void initMysqlConnection();
};
