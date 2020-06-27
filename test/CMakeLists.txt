
include_directories("${PROJECT_SOURCE_DIR}/src")

file(COPY resources DESTINATION .)

set(TEST_LIBS cmocka_lib)
list(APPEND TEST_LIBS cjson_lib lws_lib ${ZLIB_LIBRARIES} m ssl crypto)

set(COMMON_SOURCES ${PROJECT_SOURCE_DIR}/src/depth.c)
list(APPEND COMMON_SOURCES
        ${PROJECT_SOURCE_DIR}/src/io/c-json.c
        ${PROJECT_SOURCE_DIR}/src/io/c-json.h
        ${PROJECT_SOURCE_DIR}/src/io/strings.c
        ${PROJECT_SOURCE_DIR}/src/io/strings.h
        ${PROJECT_SOURCE_DIR}/src/io/time.c
        ${PROJECT_SOURCE_DIR}/src/io/time.h
        ${PROJECT_SOURCE_DIR}/test/file.c
        ${PROJECT_SOURCE_DIR}/test/file.h
        )

add_cmocka_test(test_bitfinex_depth
        SOURCES
        ${PROJECT_SOURCE_DIR}/test/bitfinex/test-bitfinex-depth.c
        ${PROJECT_SOURCE_DIR}/src/bitfinex/bitfinex-depth.c
        ${PROJECT_SOURCE_DIR}/src/bitfinex/bitfinex-depth.h
        ${COMMON_SOURCES}
        LINK_LIBRARIES ${TEST_LIBS}
        LINK_OPTIONS --coverage)

