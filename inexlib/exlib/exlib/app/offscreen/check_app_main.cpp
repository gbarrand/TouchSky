// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file EXLIB_APP.license for terms.

#include "../EXLIB_APP/main"

#include <exlib/app/offscreen/main_cpp>

int main(int argc,char** argv) {return exlib_main<EXLIB_APP::main>("EXLIB_APP",argc,argv);}

//exlib_build_use inlib expat
//exlib_build_use exlib png jpeg zlib inlib_glutess freetype kernel
//exlib_build_bigobj
