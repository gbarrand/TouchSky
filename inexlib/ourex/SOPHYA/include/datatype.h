// This may look like C code, but it is really -*- C++ -*-
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Classe pour nom, taille, ... de types de donnees
//                         C.Magneville         10/2000
// LAL (Orsay) / IN2P3-CNRS  DAPNIA/SPP (Saclay) / CEA

#ifndef DATATYPE_H
#define DATATYPE_H

#include "machdefs.h"

#include <string>

namespace SOPHYA {

// class returning type name and size for usual numerical types (int_2,int_4,r_4,r_8 ...)
template <class T>
class DataTypeInfo {
public:
  // return the base type (int_4 int_8 r_4 ...) as a string 
  static std::string  getTypeName();
  // return the base type size (int_8 -> 4 , r_4 -> 4 , complex<r_8> -> 16 ...) 
  static size_t  getTypeSize();
  // return an integer identifier for each of the base type size (int_4 int_8 r_4 ...) 
  static int     getTypeId();
  DataTypeInfo();
};

std::string DecodeGCCTypeName(std::string gcctype);

} // Fin du namespace

#endif
