// This may look like C code, but it is really -*- C++ -*-

#ifndef H5STREAM_H_SEEN
#define H5STREAM_H_SEEN

// Classe de flot pour enrobage de fichiers HDF5
// R. Ansari      LAL IN2P3/CNRS  06/2004

#include "machdefs.h"
#include <string>
#include "tarray.h"

#include "hdf5.h"

namespace SOPHYA {
class HDF5OutStream {
public:
                      HDF5OutStream(string path);
  virtual             ~HDF5OutStream();
  virtual int         Write(string dsname, TArray<int_4>& ia);
  virtual int         Write(string dsname, TArray<r_4>& fa);
  virtual int         Write(TArray<int_4>& ia);
  virtual int         Write(TArray<r_4>& fa);

protected:
  std::string _filename;
  hid_t   _file;
  int     _auto_ds_id;
};

inline HDF5OutStream& operator << (HDF5OutStream& os, TArray<int_4>& ia) 
{ os.Write(ia); return os; }
inline HDF5OutStream& operator << (HDF5OutStream& os, TArray<r_4>& fa) 
{ os.Write(fa); return os; }

class HDF5InStream {
public:
                      HDF5InStream(string path);
  virtual             ~HDF5InStream();
  virtual int         Read(string dsname, TArray<int_4>& ia);
  virtual int         Read(string dsname, TArray<r_4>& fa);
  virtual int         Read(TArray<int_4>& ia);
  virtual int         Read(TArray<r_4>& fa);

protected:
  std::string _filename;
  hid_t   _file;
  int     _auto_ds_id;
};

inline HDF5InStream& operator >> (HDF5InStream& is, TArray<int_4>& ia) 
{ is.Read(ia); return is; }
inline HDF5InStream& operator >> (HDF5InStream& is, TArray<r_4>& fa) 
{ is.Read(fa); return is; }


int SAWriteToHDF5(TArray<int_4>& ia);
int SAReadFromHDF5(TArray<int_4>& ia);

}// namespace SOPHYA

#endif
