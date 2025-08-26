#include "binary.hpp"
#include "schema.hpp"
#include <iostream>

int main() {
    std::vector<ColumnMapping> mapping = {
        {"id", PgType::INT64, int64Converter},
        {"username", PgType::TEXT, textConverter},
        {"created_at", PgType::TIMESTAMPTZ, timestamptzConverter}};

    try {
        DBHelper dbHelper("mysql_table", "pg_table", mapping);
        dbHelper.startCopy();
        MYSQL_ROW row;
        while ((row = dbHelper.getMysqlRow())) {
            dbHelper.writeRow(row);
        }
        dbHelper.endCopy();
    } catch (const std::exception &e) {
        std::cerr << "Error during copy: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error during copy." << std::endl;
        return 1;
    }

    return 0;
}
