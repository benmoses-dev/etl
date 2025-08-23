#pragma once

#include "etl_types.hpp"

void getConfig(MysqlConfig &myConfig, PgsqlConfig &pgConfig);

std::string getPgConnInfo(PgsqlConfig &pgConfig);
