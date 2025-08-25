#pragma once

#include "schema.hpp"

void getConfig(MysqlConfig &myConfig, PgsqlConfig &pgConfig);

std::string getPgConnInfo(const PgsqlConfig &pgConfig);
