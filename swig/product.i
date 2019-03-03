/* example.i */
%module ProductModule
%{
#include "../cmake-build-debug/product.hpp"
%}
%include "std_string.i"
%include "typemaps.i"
%include "stl.i"
%include "../cmake-build-debug/product.hpp"
