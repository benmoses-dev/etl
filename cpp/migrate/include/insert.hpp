#pragma once

#include "schema.hpp"
#include <libpq-fe.h>
#include <mysql/mysql.h>

MYSQL_RES *getMysqlResult(const MysqlConfig &myConfig, MYSQL *mysql);

PGconn *getPgConn(const PgsqlConfig &pgConfig);

void insertPG(const PgsqlRow &pgRow, PGconn *pg);

void processRow(const MYSQL_ROW &row, PGconn *pg);
