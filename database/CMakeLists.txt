cmake_minimum_required(VERSION 3.22)
project(Database)

set(CMAKE_CXX_STANDARD 20)

find_package(SQLite3 REQUIRED)

find_package(libpqxx REQUIRED)
find_package(PostgreSQL REQUIRED)

add_library(Database SHARED 
    database_connection/db_database_connection.cc
    # Other source files will be appended below
)

# Directories to include for Database target
target_include_directories(Database PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/database_connection
    # Other include directories will be appended below
)
set_property(TARGET Database PROPERTY POSITION_INDEPENDENT_CODE ON)
set_target_properties(Database PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Link external libraries if any
target_link_libraries(Database PUBLIC SQLite::SQLite3 fmt::fmt libpqxx::pqxx libpq::pq libpq::pgport libpq::pgcommon)

# Collect source files from subdirectories
file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
set(subdir_sources "")
foreach(child ${children})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child} AND NOT child MATCHES "database_connection")
        list(APPEND subdir_sources "${child}/db_${child}.cc")
        target_include_directories(Database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${child})
    endif()
endforeach()

# Add subdirectory source files to the Database target
target_sources(Database PRIVATE ${subdir_sources})
