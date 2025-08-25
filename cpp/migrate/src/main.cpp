#include "insert.hpp"
#include "io_helper.hpp"

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

    PGconn *pg;
    try {
        pg = getPgConn(pgConfig);
    } catch (...) {
        cleanup(nullptr, pg, nullptr);
        return 1;
    }

    MYSQL *mysql = mysql_init(nullptr);
    MYSQL_RES *res;
    try {
        res = getMysqlResult(myConfig, mysql);
    } catch (...) {
        cleanup(mysql, pg, nullptr);
        return 1;
    }
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(res))) {
        processRow(row, pg);
    }

    cleanup(mysql, pg, res);

    return 0;
}
