cmake_minimum_required(VERSION 3.16)

project(dbdiff LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(sqlite3 STATIC
    external/sqlite3/sqlite3.c
)

target_include_directories(sqlite3
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite3/include
)

add_library(dbdiff STATIC
    include/dbdiff.h src/dbdiff.cpp
    include/IDatabaseConnection.h
    include/ISchemaInspector.h
    include/IRowReader.h
    include/ConnectionConfig.h
    include/BackendRegistry.h                       src/BackendRegistry.cpp

    include/sqlite/SqliteBackendFactory.h           src/sqlite/SqliteBackendFactory.cpp
    include/sqlite/SqliteConnection.h               src/sqlite/SqliteConnection.cpp
    include/sqlite/SqliteSchemaInspector.h          src/sqlite/SqliteSchemaInspector.cpp
    include/sqlite/SqliteRowReader.h                src/sqlite/SqliteRowReader.cpp
    include/util/Sqlutil.h                          src/util/Sqlutil.cpp

    include/TableReadSpec.h

    include/sqlserver/SqlServerBackendFactory.h     src/sqlserver/SqlServerBackendFactory.cpp
    include/sqlserver/SqlServerConnection.h         src/sqlserver/SqlServerConnection.cpp
    include/sqlserver/SqlServerSchemaInspector.h    src/sqlserver/SqlServerSchemaInspector.cpp
    include/sqlserver/SqlServerRowReader.h          src/sqlserver/SqlServerRowReader.cpp
    include/sqlserver/OdbcDiagnostics.h             src/sqlserver/OdbcDiagnostics.cpp
    include/sqlserver/SqlServerRuntimeError.h
    include/DatabaseSchemaInfoTypes.h
    include/DatabaseSchemaBuilder.h                 src/DatabaseSchemaBuilder.cpp
    include/DatabaseDiffReport.h
    include/TableComperator.h                       src/TableComperator.cpp


    # adjust file list as you add more .cpps
)

# Public headers for consumers of the library
target_include_directories(dbdiff
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
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
