include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_postgresql_extension(
        alpg
        VERSION 0.0.1
        SOURCES alpg.c
        SCRIPTS alpg--0.0.1.sql
)