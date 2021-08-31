/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  G. Barrand, R. Ansari , 2016-2017 
  Small utility/wrapper classes for interface with HDF5 library 
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#ifndef HDF5NAMETAG_H_SEEN
#define HDF5NAMETAG_H_SEEN

#include "hdf5inoutfile.h"

namespace SOPHYA {
/*!
   \class HDF5NameTag
   \ingroup HDF5IOServer

   A simple class which can be used in conjuction with stream (<< >>) operators
   to define the name of the next dataset or group which will be created or opened 
   for reading in a HDF5InOutFile object. 

   Pushing a HDF5NameTag(name) to an output (opened in creation or write mode) HDF5InOutFile stream \b hos 
   corresponds to the calling hos.SetNextObjectCreateName(name). Extracting a HDF5NameTag(name) from an 
   input HDF5InOutFile stream (opened for reading) \b his corresponds to his.SetNextObjectOpenName(name).

   \code 
   HDF5InOutFile hos("toto.h5", HDF5InOutFile::CreateOverwrite);
   hos << HDF5NameTag("MyFirstObject");

   HDF5InOutFile hss("toto.h5", HDF5InOutFile::RO);
   his >> HDF5NameTag("MyFirstObject");
   \endcode 
*/

class HDF5NameTag {
public:
  HDF5NameTag() { } 
  HDF5NameTag(std::string const & name) : name_(name) { } 
  HDF5NameTag(const char * name) : name_(name) { } 
  HDF5NameTag(HDF5NameTag const & a) : name_(a.name_) { }
  HDF5NameTag& operator= (HDF5NameTag const & a) { name_=a.name_; return *this; }
  HDF5NameTag& operator= (std::string const & a) { name_=a; return *this; }
  inline std::string const & getName() const { return name_; }
protected:
  std::string name_;
};

/*! \brief operator << overload to  define the name for the next object (dataset or group) 
   which will be created in the HDF5InOutFile stream without an explicitly provided name */ 
inline HDF5InOutFile& operator << (HDF5InOutFile& hos, HDF5NameTag const & hnt) 
{ hos.SetNextObjectCreateName(hnt.getName()); return(hos); }

/*! \brief operator << overload to  define the name for the next object (dataset or group) 
  which will be opened for read in the HDF5InOutFile stream without an explicitly provided name */ 
inline HDF5InOutFile& operator >> (HDF5InOutFile& his, HDF5NameTag const & hnt) 
{ his.SetNextObjectOpenName(hnt.getName()); return(his); }


} // Fin du namespace  SOPHYA

#endif  /* HDF5NAMETAG_H_SEEN */
