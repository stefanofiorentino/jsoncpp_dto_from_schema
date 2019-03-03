/* example.i */
%module ProductModule
%{
#include "../product.hpp"
%}
%include "std_string.i"
%include "typemaps.i"
%include "stl.i"
%include "../product.hpp"