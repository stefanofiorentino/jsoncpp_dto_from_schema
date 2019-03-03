cmake_minimum_required(VERSION 3.13)
project(jsoncpp_dto_from_schema)

set(CMAKE_CXX_STANDARD 11)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/plustache/include
)

add_executable(
        jsoncpp_dto_from_schema
        main.cpp
        plustache/src/context.cpp
        plustache/src/template.cpp
)

target_link_libraries(
        jsoncpp_dto_from_schema
        jsoncpp
        boost_regex
)

configure_file(${CMAKE_SOURCE_DIR}/schemas/product.json.schema schemas/product.json.schema COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/templates/class.hpp.template templates/class.hpp.template COPYONLY)

add_subdirectory(test)

#add_custom_command(TARGET jsoncpp_dto_from_schema
#        POST_BUILD
#        COMMAND gcc -c product.hpp -S
#        )