cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(Migrate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(migrate src/main.cpp src/io_helper.cpp src/schema.cpp src/binary.cpp)

target_include_directories(migrate PRIVATE include)

find_package(PostgreSQL REQUIRED)
target_include_directories(migrate PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(migrate PRIVATE ${PostgreSQL_LIBRARIES})

find_path(MYSQL_INCLUDE_DIR mysql/mysql.h)
find_library(MYSQL_LIBRARIES NAMES mysqlclient)
target_include_directories(migrate PRIVATE ${MYSQL_INCLUDE_DIR})
target_link_libraries(migrate PRIVATE ${MYSQL_LIBRARIES})
