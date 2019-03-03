include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/test
        ${CMAKE_SOURCE_DIR}/test/Catch2/single_include/catch2
)

add_executable(
        jsoncpp_dto_from_schema_test
        main.cpp
)

target_link_libraries(
        jsoncpp_dto_from_schema_test
        jsoncpp
)

add_dependencies(jsoncpp_dto_from_schema_test
                generate_dto
)
