cmake_minimum_required(VERSION 3.13)
project(jsoncpp_dto_from_schema)

set(CMAKE_CXX_STANDARD 11)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/plustache/include
)

add_library(plustache
        plustache/src/context.cpp
        plustache/src/template.cpp
)

add_executable(
        jsoncpp_dto_from_schema
        main.cpp
)

target_link_libraries(
        jsoncpp_dto_from_schema
        jsoncpp
        plustache
        boost_regex
)

configure_file(${CMAKE_SOURCE_DIR}/schemas/product.json.schema schemas/product.json.schema COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/schemas/product.tags.json.schema schemas/product.tags.json.schema COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/templates/class.hpp.template templates/class.hpp.template COPYONLY)

add_subdirectory(test)

add_custom_target(generate_dto ALL
        COMMAND jsoncpp_dto_from_schema
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating DTO classes"
        VERBATIM )

add_dependencies(generate_dto
        jsoncpp_dto_from_schema
        )

add_subdirectory(swig)