// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file exlib.license for terms.

#include "../swig/inlib_i"

#ifdef EXLIB_USE_NATIVE_FREETYPE
#include <ft2build.h>
#else
#include <ourex_ft2build.h>
#endif
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

#include "../sg/gl2ps_action"
#include "../fits_image"

#ifdef _WIN32
#include "../Windows/plotter"
#include "../Windows/sg_viewer"
#define EXLIB_SWIG_XANY Windows
#else
#include "../X11/plotter"
#include "../X11/sg_viewer"
#define EXLIB_SWIG_XANY X11
#endif

#define LUA_COMPAT_ALL

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

#include "exlib_swig_lua.icc"

//exlib_build_use inlib lua freetype gl2ps GL csz kernel
//exlib_build_use cfitsio


