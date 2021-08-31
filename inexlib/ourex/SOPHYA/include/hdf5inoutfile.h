/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  G. Barrand, R. Ansari , 2016-2017 
  Base classes for interface with HDF5 library  
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#ifndef HDF5INOUTFILE_H_SEEN
#define HDF5INOUTFILE_H_SEEN

#include "machdefs.h"
#include "pexceptions.h"
#include "dvlist.h"

#include <complex>
#include <string>

#include "shdf5utils.h"

namespace SOPHYA {


//--------------------------------------------------------
//------ Wrapper class for operations on HDF5 files ------
//--------------------------------------------------------
  
class HDF5InOutFile {
public :
  /*! \brief File access mode (RO: ReadOnly, RW: ReadWrite, Create, CreateOverwrite) 
    In CreateOverwrite, the file is overwritten if it exists */
  enum IOMode { RO, RW, Create, CreateOverwrite };

  //! Default constructor - The file should be opened subsequently using Open  
  HDF5InOutFile(); 
  //! Constructor with specification of file name and access mode
  HDF5InOutFile(std::string const & name, HDF5InOutFile::IOMode mode);
  //! Constructor with specification of file name and access mode
  HDF5InOutFile(const char* name, HDF5InOutFile::IOMode mode);
  //! Copy constructor 
  HDF5InOutFile(HDF5InOutFile const& fios); 
  //! Destructor - Closes the HDF5 file if necessary 
  virtual           ~HDF5InOutFile();
  //! Copy (equal) operator 
  HDF5InOutFile&   operator= (HDF5InOutFile const& fios);
  
  //! Open the specified file
  void              Open(const char* name, HDF5InOutFile::IOMode mode);
  //! Closes the underlying HDF5 file 
  void              Close();
  //! return the HDF5 file handle   
  inline hid_t      getH5FileHandle() const { return h5file_.get_hid(); }
  //! return the HDF5 library version as a double precision floating point number  
  static double     HDF5LibraryVersion();
  //! return the HDF5 library version as a string  
  static std::string  HDF5LibraryVersionStr();

  //! Return the SOPHYA HDF5IOServer version 
  static double     Version();

  //! Return the file name as specified in the constructor (or Open) 
  inline std::string   FileName() const { return fname_; }

  //! define the default byte oredring for creating datasets
  inline void setDefaultByteOrdering(HDF5Types::ByteOrder bo=HDF5Types::BO_Auto)
  { byteorder_=bo; }
  //! return the default byte oredring for creating datasets
  inline HDF5Types::ByteOrder getDefaultByteOrdering() const 
  { return  byteorder_; }

  //! Return the root group ("/") identifier 
  inline HDF5IdWrapper RootGroup() const { return root_grp_; }

  //! Return current group identifier 
  inline HDF5IdWrapper  CurrentGroup() const { return cur_grp_; }
  //! Define the current group
  inline void           SetCurrentGroup(HDF5IdWrapper const& grp)  { cur_grp_=grp; return; }
  
  /*! \brief Opens the specified group and returns the corresponding identifier 
    the path can be specified as an abolute path (starting with / or relative to the current group */
  HDF5IdWrapper         OpenGroup(const char* grp);
  /*! \brief Opens the specified group and returns the corresponding identifier 
    the path can be specified as an abolute path (starting with / or relative to the current group */
  inline HDF5IdWrapper  OpenGroup(std::string const& grp)
  { return OpenGroup(grp.c_str()); }

  //------------------------------------------------------------------------------------------

  /*! \brief Creates the specified group and returns the corresponding identifier
    the path can be specified as an abolute path (starting with / or relative to the current group */
  HDF5IdWrapper         CreateGroup(const char* grp);
  /*! \brief Creates the specified group and returns the corresponding identifier
    the path can be specified as an abolute path (starting with / or relative to the current group */
  inline HDF5IdWrapper  CreateGroup(std::string const& grp)
  { return CreateGroup(grp.c_str()); }

  /*! \brief Creates a Data Set and returns the corresponding identifier
    the dsname can be specified as an abolute path (starting with / or relative to the current group */
  inline HDF5IdWrapper    CreateDataSet(const char* dsname, hid_t datatype, int rank, hsize_t * sizes)
  {
    return P_CreateDataSet(dsname, datatype, rank, sizes, NULL);
  }
  /*! \brief Creates a Data Set and returns the corresponding identifier
    the dsname can be specified as an abolute path (starting with / or relative to the current group */
  inline HDF5IdWrapper  CreateDataSet(std::string const & dsname, hid_t datatype, int rank, hsize_t * sizes)
  {
    return P_CreateDataSet(dsname.c_str(), datatype, rank, sizes, NULL);
  }
  /*! \brief Creates a Data Set and returns the corresponding identifier
    the dsname can be specified as an abolute path (starting with / or relative to the current group */
  HDF5IdWrapper  CreateDataSet(std::string const & dsname, hid_t datatype, std::vector<size_t> & siz);

  /*! \brief Creates a dataset with auto-assigned name SOH_AUTO_DS_jj where correspond to the value of a counter
    which which is incremented for each dataset created without explicit name */ 
  inline HDF5IdWrapper  CreateDataSet(hid_t datatype, int rank, hsize_t * sizes)
  {
    return P_CreateDataSet(NULL, datatype, rank, sizes, NULL);
  }
  //----------------------------------- Chunked dataset --------------------------------------
  /*! \brief Creates a Chunked Data Set, with unlimited maximum dimensions and returns the corresponding identifier
    the dsname can be specified as an abolute path (starting with / or relative to the current group */
  inline HDF5IdWrapper  CreateChunkedDataSet(const char* dsname, hid_t datatype, int rank, hsize_t * sizes, hsize_t * chunck_sizes)
  {
    return P_CreateDataSet(dsname, datatype, rank, sizes, chunck_sizes);
  }
  /*! \brief Creates a Chunked Data Set, with unlimited maximum dimensions and returns the corresponding identifier
    the dsname can be specified as an abolute path (starting with / or relative to the current group */
  inline HDF5IdWrapper  CreateChunkedDataSet(std::string const & dsname, hid_t datatype, int rank, hsize_t * sizes, hsize_t * chunck_sizes)
  {
    return P_CreateDataSet(dsname.c_str(), datatype, rank, sizes, chunck_sizes);
  }
  //! Creates a Chunked Data Set with the specified initial size and chunck size, and unlimited maximum sizes
  HDF5IdWrapper  CreateChunkedDataSet(std::string const & dsname, hid_t datatype, std::vector<size_t> & siz, std::vector<size_t> & chunck_siz);

  //------------------------------------------------------------------------------------------

  /*! \brief Opens the specified Data Set and returns the corresponding identifier
    the dsname can be specified as an abolute path (starting with / or relative to the current group */
  HDF5IdWrapper    OpenDataSet(const char* dsname, HDF5IdWrapper & datatype, HDF5IdWrapper & dataspace); 
  /*! \brief Opens the specified Data Set and returns the corresponding identifier
    the dsname can be specified as an abolute path (starting with / or relative to the current group */
  inline HDF5IdWrapper  OpenDataSet(std::string const & dsname, HDF5IdWrapper & datatype, HDF5IdWrapper & dataspace)
  {
    return OpenDataSet(dsname.c_str(), datatype, dataspace);
  }
  //! Reads the data from the specified dataset 
  herr_t  FullDataSetRead(HDF5IdWrapper const & dataset, hid_t memdtype, void * memdatap);
  //! Writes the data to the specified dataset 
  herr_t  FullDataSetWrite(HDF5IdWrapper const & dataset, hid_t memdtype, const void * memdatap);
  //! Reads the the partial data set, specified by the offsets and element count from the specified dataset 
  herr_t  DataSetRead(HDF5IdWrapper const & dataset, hsize_t * offset, hsize_t * count, hid_t memdtype, void * memdatap);
  //! Writes the data to the specified dataset 
  herr_t  DataSetWrite(HDF5IdWrapper const & dataset, hsize_t * offset, hsize_t * count, hid_t memdtype, const void * memdatap);

  //! \brief Extends the specified data set (changes its sizes), applicable only to Chunked data sets 
  herr_t  ExtendDataSet(HDF5IdWrapper const & dataset, hsize_t * sizes);

  //------------------------------------------------------------------------------------------
  
  //! Writes the pair (key, val) as an attribute attached to the HDF object (group or dataset) specified by oid  
  static herr_t  WriteAttribute(HDF5IdWrapper const & oid, const char * key, MuTyV const & val, const char *comment=NULL);

  //! Writes the DVList objects as a set of attributes attached to the HDF object (group or dataset) specified by oid  
  static herr_t  WriteAttributes(HDF5IdWrapper const & oid, DVList const & dvl);

  //! Read the attributes named \b key attached to the HDF object (group or dataset) specified by oid, and returns it as a MuTyV   
  inline static MuTyV   ReadAttribute(HDF5IdWrapper const & oid, const char * key)
  {  return HDF5InOutFile::ReadAttribute(oid.get_hid(), key); }
  //! Read the attributes named \b key attached to the HDF object (group or dataset) specified by oid, and returns it as a MuTyV   
  static MuTyV   ReadAttribute(hid_t oid, const char * key);
  //! Read the attributes attached to the HDF object (group or dataset) specified by oid.   
  static int     ReadAttributes(HDF5IdWrapper const & oid, DVList & dvl);

  //------------------------------------------------------------------------------------------
  //! \brief return the type of the object specified by its relative or absolute path 
  H5O_type_t     GetObjectType(const char* objpath); 
  //! \brief return the type of the object specified by its relative or absolute path and associated attributes and information 
  //   return value : H5O_TYPE_DATASET  or H5O_TYPE_GROUP 
  H5O_type_t     GetObjectType_Info(const char* objpath, DVList & dvl, HDF5IdWrapper & datatype, HDF5IdWrapper & dataspace); 
  //! \brief return the type of the object specified by its relative or absolute path and associated attributes and information  
  inline H5O_type_t GetObjectType_Info(std::string const & objpath, DVList & dvl, HDF5IdWrapper & datatype, HDF5IdWrapper & dataspace)
  {
    return GetObjectType_Info(objpath.c_str(), dvl, datatype, dataspace);
  }
  //! return the  number of links (items), the list of of dataset names (\b dslist) and group names (\b grplist) in the current group
  inline int     GetCurrentGroupMemberList(std::vector<std::string> & dslist, std::vector<std::string> & grplist)
  { 
    return GetGroupMemberList(cur_grp_, dslist, grplist);
  }
  //! return total number of links (items) in a group, and the list of dataset names (\b dslist) and group names (\b grplist)
  static int     GetGroupMemberList(HDF5IdWrapper const & gid, std::vector<std::string> & dslist, std::vector<std::string> & grplist);
  //------------------------------------------------------------------------------------------
  
  //! Defines the name for the next created object (dataset or group), if no name is specified  
  inline void    SetNextObjectCreateName(string const & name)
   { next_obj_create_name_ = name; }
  //! Return the name which will be used for the next created object (dataset or group), if no name is specified 
  std::string    GetNextObjectCreateName();
  //! Defines the name for the next object to be opened (dataset or group name), if no name is specified  
  inline void    SetNextObjectOpenName(string const & name)
   { next_obj_open_name_ = name; }
  //! Return the name which will be used for the next object to be opened (dataset or group name), if no name is specified  
  std::string    GetNextObjectOpenName();
  
 //! Prints information about the HDF5 file on standard output stream (cout)
 inline  void      Print(int lev=0) const  { Print(cout, lev); }  
 //! Prints information about the HDF5 file on stream os
 virtual void      Print(ostream& os, int lev=0)  const;

protected:
  //! Chunked or unchunked data set creation method - Chuncked dataset if chunck_sizes != NULL
  HDF5IdWrapper P_CreateDataSet(const char* dsname, hid_t datatype, int rank, hsize_t * sizes, hsize_t * chunck_sizes);

  HDF5IdWrapper h5file_;    //  HDF5 library file handle
  std::string fname_;        //  File name as passed to creator
  HDF5InOutFile::IOMode mode_;    //  file open mode
  HDF5Types::ByteOrder byteorder_;  // default byte oredring for the created data sets in file 
  HDF5IdWrapper root_grp_;  //  The file root group "/"
  HDF5IdWrapper cur_grp_;   //  current group
  int_4 acrcnt_;            //  number of created datasets/groups with auto-attributed / sequencial names
  int_4 aopidx_;            //  index of the next object name to be opened from list_curgrp_ 
  std::vector<std::string>  list_curgrp_;   // list of datasets and groups in the current group (for read with auto-names) 
  std::string next_obj_create_name_;        // next object create name 
  std::string next_obj_open_name_;          // next object open name 
};

/*! Prints HDF5 file information on stream \b s ( fio.Print(s) ) */

inline ostream& operator << (ostream& s, HDF5InOutFile const & fio)
  {  fio.Print(s);  return(s);  }

} // Fin du namespace SOPHYA

#endif  /* HDF5INOUTFILE_H_SEEN */




