// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

#include "../swig/inlib_i"

#define LUA_COMPAT_ALL

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include <cstddef> //ptrdiff_t

#include "inlib_swig_lua.icc"

//exlib_build_use inlib lua kernel socket
//exlib_build_bigobj

// to read pawdemo.root STAFF tree :
//exlib_build_use csz
//exlib_build_cppflags -DINLIB_USE_CSZ
