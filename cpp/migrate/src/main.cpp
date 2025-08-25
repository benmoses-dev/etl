#include "binary.hpp"
#include "io_helper.hpp"
#include "schema.hpp"

void cleanup(MYSQL *mysql, PGconn *pg, MYSQL_RES *res) {
    if (mysql)
        mysql_close(mysql);
    if (pg)
        PQfinish(pg);
    if (res)
        mysql_free_result(res);
}

struct Cleanup {
    MYSQL *mysql;
    PGconn *pg;
    MYSQL_RES *res;
    ~Cleanup() { cleanup(mysql, pg, res); }
};

int main() {
    MysqlConfig myConfig;
    PgsqlConfig pgConfig;
    getConfig(myConfig, pgConfig);
    std::vector<ColumnMapping> mapping = getMapping();

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

    {
        Cleanup guard{mysql, pg, res};
        try {
            startCopy(pg);
            std::vector<char> header = makeBinaryHeader();
            PQputCopyData(pg, header.data(), header.size());
            while ((row = mysql_fetch_row(res))) {
                std::vector<std::string> mysqlRow = mapMysqlRow(row);
                std::vector<char> rowData = makeBinaryRow(mysqlRow, mapping);
                PQputCopyData(pg, rowData.data(), rowData.size());
            }
            std::vector<char> trailer = makeBinaryTrailer();
            PQputCopyData(pg, trailer.data(), trailer.size());
            PQputCopyEnd(pg, nullptr);
            endCopy(pg);
        } catch (...) {
            return 1;
        }
    }

    return 0;
}
