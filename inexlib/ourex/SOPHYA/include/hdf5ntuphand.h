/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  HDF5 IO Handler for NTuple objects 
   R. Ansari, C. Magneville  2018  
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#ifndef HDF5NTUPHAND_H_SEEN
#define HDF5NTUPHAND_H_SEEN

#include "machdefs.h"
#include <string>
#include "hdf5handler.h"
#include "ntuple.h"

namespace SOPHYA {

/*! 
  \brief operator << overload to write an NTuple to a HDF5InOutFile file wrapper object 

  Use the HDF5NameTag object (push it to the HDF5InOutFile stream) to define the dataset name in the hdf5 file 
*/

inline HDF5InOutFile& operator << (HDF5InOutFile& os,  NTuple const & nt) 
{ 
  HDF5Handler<NTuple> fio(const_cast<NTuple &>(nt)); 
  string san=os.GetNextObjectCreateName(); 
  fio.Write(os,san.c_str());  
  return os; 
}

/*! 
  \brief operator >> overload to read an NTuple from a HDF5InOutFile file wrapper object 

  Use the HDF5NameTag object (pull it from the HDF5InOutFile stream) to define the dataset name to be read 
*/

inline HDF5InOutFile& operator >> (HDF5InOutFile& is,  NTuple & nt)
{ 
  HDF5Handler<NTuple> fio(nt);  
  string san=is.GetNextObjectOpenName(); 
  fio.Read(is, san.c_str());  
  return(is); 
}

} // Fin du namespace


#endif   /*  HDF5NTUPHAND_H_SEEN */

