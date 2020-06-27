
add_executable(bitfinex-gateway
        main.c
        ws/client-server-protocol.c ws/client-server-protocol.h
        ws/client-connect.c ws/client-connect.h
        ws/data.h
        io/strings.c io/strings.h
        io/c-json.c io/c-json.h
        depth.c depth.h
        bitfinex/bitfinex.c bitfinex/bitfinex.h bitfinex/bitfinex-depth.c bitfinex/bitfinex-depth.h
        io/time.c io/time.h)

target_link_libraries(bitfinex-gateway cjson_lib lws_lib ${ZLIB_LIBRARIES} m ssl crypto)

message(STATUS "CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")

add_custom_command(TARGET bitfinex-gateway
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/mount-origin ${CMAKE_BINARY_DIR}/src/mount-origin
        COMMENT "Copying '${CMAKE_SOURCE_DIR}/mount-origin' to '${CMAKE_BINARY_DIR}/src/mount-origin'")