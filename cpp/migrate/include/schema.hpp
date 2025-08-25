#pragma once

#include <string>

#define pgtable "table"
#define mytable "table"

struct MysqlRow {
    int id;
    std::string name;
    std::string created_at;
};

std::string myQuerySQL();

struct PgsqlRow {
    int id;
    std::string name;
    std::string created_at;
};

std::string pgInsertSQL(const PgsqlRow &row);

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

PgsqlRow mapRow(const MysqlRow &row);
