#include "etl_helper.hpp"
#include "etl_types.hpp"
#include <iostream>
#include <libpq-fe.h>
#include <mysql/mysql.h>
#include <string>

void cleanup(MYSQL *mysql, PGconn *pg, MYSQL_RES *res) {
    if (mysql)
        mysql_close(mysql);
    if (pg)
        PQfinish(pg);
    if (res)
        mysql_free_result(res);
}

int main() {
    MysqlConfig myConfig;
    PgsqlConfig pgConfig;
    getConfig(myConfig, pgConfig);

    std::cout << "MySQL connection: " << myConfig.myuser << "@" << myConfig.myhost << ":"
              << myConfig.myport << "/" << myConfig.myname << std::endl;

    MYSQL *mysql = mysql_init(nullptr);
    if (!mysql_real_connect(mysql, myConfig.myhost.c_str(), myConfig.myuser.c_str(),
                            myConfig.mypass.c_str(), myConfig.myname.c_str(),
                            myConfig.myport, nullptr, 0)) {
        std::cerr << "MySQL connection failed: " << mysql_error(mysql) << std::endl;
        return 1;
    }

    std::string conninfo = getPgConnInfo(pgConfig);

    std::cout << "PostgreSQL connection info: " << conninfo << std::endl;

    PGconn *pg = PQconnectdb(conninfo.c_str());
    if (PQstatus(pg) != CONNECTION_OK) {
        std::cerr << "PostgreSQL connection failed: " << PQerrorMessage(pg) << std::endl;
        cleanup(mysql, pg, nullptr);
        return 1;
    }

    std::string querySQL = myQuerySQL();
    if (mysql_query(mysql, querySQL.c_str())) {
        std::cerr << "MySQL query failed: " << mysql_error(mysql) << "\n";
        cleanup(mysql, pg, nullptr);
        return 1;
    }
    MYSQL_RES *res = mysql_use_result(mysql);
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res))) {
        MysqlRow myRow;
        myRow.id = row[0] ? std::stoi(row[0]) : 0;
        if (myRow.id == 0)
            continue;
        myRow.name = row[1] ? row[1] : "";
        myRow.created_at = row[2] ? row[2] : "";

        PgsqlRow pgRow = mapRow(myRow);

        std::string insertSQL = pgInsertSQL(pgRow);
        PGresult *resInsert = PQexec(pg, insertSQL.c_str());
        if (PQresultStatus(resInsert) != PGRES_COMMAND_OK) {
            std::cerr << "Postgres insert failed: " << PQerrorMessage(pg) << std::endl;
        }
        PQclear(resInsert);
    }

    cleanup(mysql, pg, res);

    return 0;
}
