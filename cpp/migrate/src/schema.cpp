#include "schema.hpp"

std::string myQuerySQL() {
    std::string cols = "id, name, created_at";
    return "SELECT " + cols + " FROM " + mytable + "";
}

std::string pgInsertSQL(const PgsqlRow &row) {
    std::string cols = "(id, name, created_at)";
    std::string vals =
        "(" + std::to_string(row.id) + ", '" + row.name + "', '" + row.created_at + "')";
    return "INSERT INTO " + std::string(pgtable) + " " + cols + " VALUES " + vals;
}

PgsqlRow mapRow(const MysqlRow &row) {
    PgsqlRow pg;
    pg.id = row.id;
    pg.name = row.name;
    pg.created_at = row.created_at;
    return pg;
}
