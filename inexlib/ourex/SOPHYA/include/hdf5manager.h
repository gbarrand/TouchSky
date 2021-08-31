/*    
   --- SOPHYA software - HDF5IOServer module ---
   R. Ansari,   2018
   (C) UPS+LAL IN2P3/CNRS     (C) IRFU-SPP/CEA 
*/

#ifndef HDF5MANAGER_H_SEEN
#define HDF5MANAGER_H_SEEN 

#include "machdefs.h"
#include <string>
#include <iostream>
#include "hdf5inoutfile.h"

namespace SOPHYA {

class HDF5HandlerInterface;

//-------------------------------------------------------------
//  Top level HDF5 I/O hanldler manager  
//-------------------------------------------------------------

class HDF5Manager {
 public:
  //! Called once for each handler during init phase.
  static int  RegisterHandler(HDF5HandlerInterface * fhi, int glev=0, string desc="");
  //! Print list of handlers on cout 
  static int  ListHandlers();
  //! Finds the appropriate handler and writes the object \b o to HDF5 file \b os with dataset or group path \b opath 
  static void Write(HDF5InOutFile& os, string const& path, AnyDataObj const & o);
  //! Finds the appropriate handler and reads the object \b o from HDF5 file \b is , from dataset or group \b path 
  static void Read(HDF5InOutFile& is, string const& path, AnyDataObj & o);
  //! Finds the appropriate reader and reads the data from the dataset or group \b path in file \b is
  static HDF5HandlerInterface * Read(HDF5InOutFile& is,  string const& path);
  //! Scans the HDF5 file \b filename and prints information about each dataset or group in the file on formatted stream \b os
  static int ScanFile(string filename, ostream& os, bool fgpatt=false);
  //! Scans the HDF5 file \b filename and prints information about each dataset or group in the file on \b cout
  static inline int ScanFile(string filename, bool fgpatt=false)
  { return ScanFile(filename, cout, fgpatt); }

 protected:
  //! Finds the appropriate handler for the object \b o in the list of registered handlers.
  static HDF5HandlerInterface* FindHandler(AnyDataObj const & o);
  //! Finds the appropriate reader for the current HDU in the list of registered handlers.
  static HDF5HandlerInterface* FindReader(HDF5InOutFile& is, string const& path);
  //! Scans the datasets in a given group 
  static int ScanGroup(HDF5InOutFile &is, string const & grp, ostream& os, bool fgpatt);
};

} // Fin du namespace

#endif   /* HDF5MANAGER_H_SEEN */

