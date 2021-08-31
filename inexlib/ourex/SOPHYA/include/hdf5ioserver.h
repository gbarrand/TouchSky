/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  R. Ansari ,  G. Barrand,  C. Magneville   2016-2018
  HDF5 library Wrapper classes and HDF5 I/O handlers for SOPHYA classes 
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

/*!
   \defgroup HDF5IOServer HDF5IOServer  module
   This module contains classes which provides c++ wrapper for a subset of functionalities 
   of HDF5 I/O library and classes handling I/O in this format for SOPHYA objects.  
   This module uses the HDF5 library (see https://www.hdfgroup.org/downloads/hdf5/ ) 
*/

#ifndef HDF5IOSERVER_H_SEEN
#define HDF5IOSERVER_H_SEEN

// List of all necessary include files for this module 
#include "shdf5utils.h"
#include "hdf5inoutfile.h"
#include "hdf5nametag.h"
#include "hdf5handler.h"
#include "hdf5arrhand.h"
#include "hdf5ntuphand.h"
#include "hdf5segdbhand.h"
#include "hdf5dtablehand.h"
#include "hdf5manager.h"
#include "hdf5init.h"

#endif  /* HDF5IOSERVER_H_SEEN */
