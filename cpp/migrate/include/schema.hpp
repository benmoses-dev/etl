#pragma once

#include <functional>
#include <libpq-fe.h>
#include <mysql/mysql.h>
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

std::vector<ColumnMapping> getMapping();

std::vector<std::string> mapMysqlRow(MYSQL_ROW &row);

std::string myQuerySQL();

void startCopy(PGconn *pg);

MYSQL_RES *getMysqlResult(const MysqlConfig &myConfig, MYSQL *mysql);

PGconn *getPgConn(const PgsqlConfig &pgConfig);

void endCopy(PGconn *pg);
