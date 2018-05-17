// Copyright (C) 2010, Guy Barrand. All rights reserved.
// See the file TouchSky.license for terms.

#include "../TouchSky/main"

typedef TouchSky::main app_main_t;

#import <exlib/app/Cocoa/main_mm>

int main(int argc,char** argv) {return exlib_main<TouchSky::context,TouchSky::main>("TouchSky",argc,argv);}
