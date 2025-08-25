#include "schema.hpp"
#include "binary.hpp"
#include "io_helper.hpp"
#include <iostream>

#define pgtable "table"
#define mytable "table"

// Todo: make this more convenient

std::vector<ColumnMapping> getMapping() {
    return {{"id", PgType::INT64, int64Converter},
            {"name", PgType::TEXT, textConverter},
            {"created_at", PgType::TIMESTAMPTZ, timestamptzConverter}};
}

std::vector<std::string> mapMysqlRow(MYSQL_ROW &row) {
    return {row[0] ? row[0] : "", row[1] ? row[1] : "", row[2] ? row[2] : ""};
}

std::string myQuerySQL() {
    std::string cols = "id, name, created_at";
    return "SELECT " + cols + " FROM " + mytable + "";
}

void startCopy(PGconn *pg) {
    std::string copySql =
        "COPY " + std::string(pgtable) + " (id, name, created_at) FROM STDIN BINARY";
    PGresult *pgres = PQexec(pg, copySql.c_str());
    PQclear(pgres);
}

MYSQL_RES *getMysqlResult(const MysqlConfig &myConfig, MYSQL *mysql) {
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
    PGconn *pg = PQconnectdb(conninfo.c_str());
    if (PQstatus(pg) != CONNECTION_OK) {
        std::cerr << "PostgreSQL connection failed: " << PQerrorMessage(pg) << std::endl;
        throw;
    }
    return pg;
}

void endCopy(PGconn *pg) {
    PGresult *copyRes = PQgetResult(pg);
    if (PQresultStatus(copyRes) != PGRES_COMMAND_OK) {
        std::cerr << "COPY failed: " << PQerrorMessage(pg) << std::endl;
    }
    PQclear(copyRes);
}
