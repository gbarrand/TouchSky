/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  HDF5 IO Handler for BaseDataTable objects 
   R. Ansari, C. Magneville  2018  
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#ifndef HDF5DTABLEHAND_H_SEEN
#define HDF5DTABLEHAND_H_SEEN

#include "machdefs.h"
#include <string>
#include "hdf5handler.h"
#include "datatable.h"

namespace SOPHYA {

/*! 
  \brief operator << overload to write an BaseDataTable to a HDF5InOutFile file wrapper object 

  Use the HDF5NameTag object (push it to the HDF5InOutFile stream) to define the dataset name in the hdf5 file 
*/

inline HDF5InOutFile& operator << (HDF5InOutFile& os,  BaseDataTable const & dt) 
{ 
  HDF5Handler<BaseDataTable> fio(const_cast<BaseDataTable &>(dt)); 
  string san=os.GetNextObjectCreateName(); 
  fio.Write(os,san.c_str());  
  return os; 
}

/*! 
  \brief operator >> overload to read an BaseDataTable from a HDF5InOutFile file wrapper object 

  Use the HDF5NameTag object (pull it from the HDF5InOutFile stream) to define the dataset name to be read 
*/

inline HDF5InOutFile& operator >> (HDF5InOutFile& is,  BaseDataTable & dt)
{ 
  HDF5Handler<BaseDataTable> fio(dt);  
  string san=is.GetNextObjectOpenName(); 
  fio.Read(is, san.c_str());  
  return(is); 
}

} // Fin du namespace


#endif   /*  HDF5NTUPHAND_H_SEEN */

