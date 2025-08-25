#include "insert.hpp"
#include "io_helper.hpp"
#include <iostream>

MYSQL_RES *getMysqlResult(const MysqlConfig &myConfig, MYSQL *mysql) {
    std::cout << "MySQL connection: " << myConfig.myuser << "@" << myConfig.myhost << ":"
              << myConfig.myport << "/" << myConfig.myname << std::endl;
    if (!mysql_real_connect(mysql, myConfig.myhost.c_str(), myConfig.myuser.c_str(),
                            myConfig.mypass.c_str(), myConfig.myname.c_str(),
                            myConfig.myport, nullptr, 0)) {
        std::cerr << "MySQL connection failed: " << mysql_error(mysql) << std::endl;
        throw;
    }
    std::string querySQL = myQuerySQL();
    if (mysql_query(mysql, querySQL.c_str())) {
        std::cerr << "MySQL query failed: " << mysql_error(mysql) << std::endl;
        throw;
    }
    return mysql_use_result(mysql);
}

PGconn *getPgConn(const PgsqlConfig &pgConfig) {
    std::string conninfo = getPgConnInfo(pgConfig);
    std::cout << "PostgreSQL connection info: " << conninfo << std::endl;
    PGconn *pg = PQconnectdb(conninfo.c_str());
    if (PQstatus(pg) != CONNECTION_OK) {
        std::cerr << "PostgreSQL connection failed: " << PQerrorMessage(pg) << std::endl;
        throw;
    }
    return pg;
}

void insertPG(const PgsqlRow &pgRow, PGconn *pg) {
    std::string insertSQL = pgInsertSQL(pgRow);
    PGresult *resInsert = PQexec(pg, insertSQL.c_str());
    if (PQresultStatus(resInsert) != PGRES_COMMAND_OK) {
        std::cerr << "Postgres insert failed: " << PQerrorMessage(pg) << std::endl;
    }
    PQclear(resInsert);
}

void processRow(const MYSQL_ROW &row, PGconn *pg) {
    MysqlRow myRow;
    myRow.id = row[0] ? std::stoi(row[0]) : 0;
    if (myRow.id == 0)
        return;
    myRow.name = row[1] ? row[1] : "";
    myRow.created_at = row[2] ? row[2] : "";

    PgsqlRow pgRow = mapRow(myRow);
    insertPG(pgRow, pg);
}
