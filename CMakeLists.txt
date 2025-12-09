cmake_minimum_required(VERSION 3.16)

project(dbdiff LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------
# sqlite3 static library
# -------------------------------
add_library(sqlite3 STATIC
    external/sqlite3/sqlite3.c
)

target_include_directories(sqlite3
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite3/include
)

# -------------------------------
# dbdiff static library
# -------------------------------

set(DBDIFF_PUBLIC_HEADERS
    include/dbdiff/dbdiff.h
    include/dbdiff/ConnectionConfig.h
    include/dbdiff/DatabaseDiffReport.h
    include/dbdiff/DatabaseSchemaInfoTypes.h
    include/dbdiff/TableStructureComperator.h
    include/dbdiff/ComparisonRunner.h

)

set(DBDIFF_INTERNAL_HEADERS
    # General core
    src/TableReadSpec.h
    src/TableComperator.h
    src/DatabaseSchemaBuilder.h
    src/IDatabaseConnection.h
    src/IRowReader.h
    src/ISchemaInspector.h
    src/BackendRegistry.h
    src/DatabaseComperator.h

    # Sqlite backend
    src/sqlite/SqliteBackend.h
    src/sqlite/SqliteConnection.h
    src/sqlite/SqliteSchemaInspector.h
    src/sqlite/SqliteRowReader.h
    src/util/Sqlutil.h

    # SQL Server backend
    src/sqlserver/SqlServerBackend.h
    src/sqlserver/SqlServerConnection.h
    src/sqlserver/SqlServerSchemaInspector.h
    src/sqlserver/SqlServerRowReader.h
    src/sqlserver/OdbcDiagnostics.h
)


set(DBDIFF_SOURCES
    # General core

    src/BackendRegistry.cpp
    src/DatabaseSchemaBuilder.cpp
    src/DatabaseComperator.cpp
    src/TableComperator.cpp
    src/TableStructureComperator.cpp
    src/ComparisonRunner.cpp

    # Sqlite backend
    src/sqlite/SqliteBackend.cpp
    src/sqlite/SqliteConnection.cpp
    src/sqlite/SqliteSchemaInspector.cpp
    src/sqlite/SqliteRowReader.cpp
    src/util/Sqlutil.cpp

    # SQL Server backend
    src/sqlserver/SqlServerBackend.cpp
    src/sqlserver/SqlServerConnection.cpp
    src/sqlserver/SqlServerSchemaInspector.cpp
    src/sqlserver/SqlServerRowReader.cpp
    src/sqlserver/OdbcDiagnostics.cpp

)

add_library(dbdiff STATIC
    ${DBDIFF_PUBLIC_HEADERS}
    ${DBDIFF_INTERNAL_HEADERS}
    ${DBDIFF_SOURCES}
)

# Public headers for consumers of the library
target_include_directories(dbdiff
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


target_link_libraries(dbdiff
    PUBLIC sqlite3
    PUBLIC odbc32
)

target_compile_definitions(dbdiff PRIVATE DBDIFF_LIBRARY)

# Simple console executable that uses the library
add_executable(dbdiff_cli
    apps/cli_main.cpp      # create this file
)

target_link_libraries(dbdiff_cli
    PRIVATE dbdiff
)
