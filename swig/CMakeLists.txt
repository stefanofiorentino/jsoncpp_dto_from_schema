find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)
pkg_check_modules(PYTHON REQUIRED python)

FUNCTION(PREPEND var prefix)
    SET(listVar "")
    FOREACH(f ${ARGN})
        LIST(APPEND listVar "${prefix}${f}")
    ENDFOREACH(f)
    SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

PREPEND(PYTHON_INCLUDE_DIRS_PREPEND "-I" ${PYTHON_INCLUDE_DIRS})

configure_file(${CMAKE_SOURCE_DIR}/swig/product.i product.i COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/swig/launcher.sh launcher.sh COPYONLY)


add_custom_target(jsoncpp_dto_from_schema_test_launcher
        COMMAND jsoncpp_dto_from_schema_test
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Launching Header-Only lib files generator"
        VERBATIM )
add_dependencies(jsoncpp_dto_from_schema_test_launcher
        jsoncpp_dto_from_schema_test)


add_custom_target(swig_python ALL
        COMMAND mkdir -p python
        COMMAND swig -python -c++ -outdir python product.i
        COMMAND g++ -fpic -std=c++11 -x c++ -c ${CMAKE_BINARY_DIR}/product.hpp product_wrap.cxx ${PYTHON_INCLUDE_DIRS_PREPEND} -I${JSONCPP_INCLUDE_DIRS} -I${CMAKE_SOURCE_DIR}
        COMMAND g++ -shared product.o product_wrap.o -o _ProductModule.so
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/swig
        COMMENT "Generating python module by SWIG"
        VERBATIM )
add_dependencies(swig_python
        jsoncpp_dto_from_schema_test_launcher
        )


add_custom_target(swig_python_test ALL
        COMMAND /bin/bash launcher.sh
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/swig
        COMMENT "Test python module by SWIG"
        VERBATIM )

add_dependencies(swig_python_test
        swig_python
        )
