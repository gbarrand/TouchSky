/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  G. Barrand, R. Ansari , 2016-2017 
  Base classes for interface with HDF5 library  
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#include "machdefs.h"
#include "segdatablock.h"
#include "hdf5inoutfile.h"
#include "strutil.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> /*atoll*/
#include <iostream>

using namespace std;

namespace SOPHYA {

/*! 
  \class HDF5InOutFile
  \ingroup HDF5IOServer
  \brief  Wrapper class for HDF5 library functions 

  \code
  //-- Try to read the attributes for a specific data set from an HDF5 file 
  string h5filename="toto.h5";
  string dsname="mydata";
  // 1- Open the file 
  HDF5InOutFile h5is(h5filename, HDF5InOutFile::RO);
  // 2- Open the data set and print the associated attributes 
  HDF5IdWrapper datatype, dataspace;
  HDF5IdWrapper dataset = h5is.OpenDataSet(dsname, datatype, dataspace);
  cout << datatype << dataset;
  // 3- Read and print the associated attributes  
  DVList att;
  h5is.ReadAttributes(dataset, att);
  cout << att;
  \endcode
*/

/* -- Fonction utilitaire pour verifier la validite d'un hid_t - retourne true si OK */
static inline bool HDF5_check_hid_valid(hid_t h5id)
{
  if ((h5id == H5I_INVALID_HID) || (h5id < 0) )  return false;
  return true;
}

/* -- Fonction utilitaire conversion enum mode ouverture en chaine de caracteres */
inline const char * convert_hdf5_access_mode(  HDF5InOutFile::IOMode mode )
{
  if (mode == HDF5InOutFile::RO) return "HDF5InOutFile::RO";
  else if (mode == HDF5InOutFile::RW) return "HDF5InOutFile::RW";
  else if (mode == HDF5InOutFile::Create) return "HDF5InOutFile::Create";
  else if (mode == HDF5InOutFile::CreateOverwrite) return "HDF5InOutFile::CreateOverwrite";
  return "HDF5InOutFile::Mode????";
}

//-- Default constructor - The file should be opened subsequently using Open
HDF5InOutFile::HDF5InOutFile()
  : mode_(HDF5InOutFile::RO), acrcnt_(0), aopidx_(-1)
{
}

/*!
  \param name : name (full path) of the HDF5 file to be opened 
  \param mode : HDF5InOutFile::RO  HDF5InOutFile::RW   HDF5InOutFile::Create   HDF5InOutFile::CreateOverwrite 
*/
HDF5InOutFile::HDF5InOutFile(string const & name, HDF5InOutFile::IOMode mode)
  : mode_(HDF5InOutFile::RO), byteorder_(HDF5Types::BO_Auto), acrcnt_(0), aopidx_(-1)
{
  //  cout << " DBG - HDF5InOutFile(string name= " << name << ")" << endl;
  Open(name.c_str(), mode);
}

/*!
  \param name : name (full path) of the HDF5 file to be opened 
  \param mode : HDF5InOutFile::RO  HDF5InOutFile::RW   HDF5InOutFile::Create   HDF5InOutFile::CreateOverwrite 
*/
HDF5InOutFile::HDF5InOutFile(const char * name, HDF5InOutFile::IOMode mode)
  : mode_(HDF5InOutFile::RO), byteorder_(HDF5Types::BO_Auto), acrcnt_(0), aopidx_(-1)
{
  Open(name, mode);
}

//-- Copy constructor 
HDF5InOutFile::HDF5InOutFile(HDF5InOutFile const& hios)
  : h5file_(hios.h5file_), fname_(hios.fname_), mode_(hios.mode_), byteorder_(HDF5Types::BO_Auto),
  root_grp_(hios.root_grp_), cur_grp_(hios.cur_grp_), 
  acrcnt_(hios.acrcnt_), aopidx_(hios.aopidx_), list_curgrp_(hios.list_curgrp_),
  next_obj_create_name_(hios.next_obj_create_name_), next_obj_open_name_(hios.next_obj_open_name_)
{
}

//--- Destructor 
HDF5InOutFile::~HDF5InOutFile()
{
  if ( HDF5_check_hid_valid(h5file_.get_hid()) )  Close();
}

HDF5InOutFile& HDF5InOutFile::operator= (HDF5InOutFile const& a)
{
  h5file_ = a.h5file_;
  fname_ = a.fname_;
  mode_ = a.mode_;
  byteorder_ = a.byteorder_;
  root_grp_ = a.root_grp_;
  cur_grp_ = a.cur_grp_;
  acrcnt_ = a.acrcnt_;
  aopidx_ = a.aopidx_;
  list_curgrp_ = a.list_curgrp_;
  next_obj_create_name_ = a.next_obj_create_name_;
  next_obj_open_name_ = a.next_obj_open_name_;
  return *this;
}
  
/*-- Methode --*/
/*!
  \warning throws an exception if the object corresponds to an already opened file
 */
void HDF5InOutFile::Open(const char* name, HDF5InOutFile::IOMode mode)
{
  if ( HDF5_check_hid_valid(h5file_.get_hid()) )
    throw HDF5IOException("HDF5InOutFile::Open()/ERROR: file already opened");
       
  int status = 0;
  hid_t h5fid = H5I_INVALID_HID;
  switch ( mode ) {
  case HDF5InOutFile::RO :
    h5fid = H5Fopen(name, H5F_ACC_RDONLY, H5P_DEFAULT);  
    break;
  case HDF5InOutFile::RW :
    h5fid = H5Fopen(name, H5F_ACC_RDWR, H5P_DEFAULT);
    break;
  case HDF5InOutFile::Create :
    h5fid = H5Fcreate(name, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);  
    break;
  case HDF5InOutFile::CreateOverwrite :
    // The file is overwritten if it exists 
    h5fid = H5Fcreate(name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);  
    break;
  default:
    throw HDF5IOException("HDF5InOutFile::Open() - invalid file open mode ");
    break;
  }

  if ( ! HDF5_check_hid_valid(h5fid) ) {
    cerr<<"HDF5InOutFile::Open()/ERROR opening file:"<<name<<" with mode="<<convert_hdf5_access_mode(mode);
    throw HDF5IOException("HDF5InOutFile::Open()/ERROR opening file");
  }
  h5file_ = HDF5IdWrapper(h5fid, H5I_FILE);
  fname_ = name;
  mode_ = mode;
  hid_t gid = H5Gopen2(h5fid, "/", H5P_DEFAULT);
  if ( ! HDF5_check_hid_valid(gid) )
    throw HDF5IOException("HDF5InOutFile::Open()/ERROR failed opening root group");
  root_grp_ = HDF5IdWrapper(gid, H5I_GROUP);
  cur_grp_ = root_grp_;
  acrcnt_ = 0;
  aopidx_ = -1;
  list_curgrp_.clear();
  next_obj_create_name_=next_obj_open_name_="";
  return;
}

/*-- Methode --*/
//! Opens the named fits file (see cfitsio routines hdf5_open_file and hdf5_create_file) 
void HDF5InOutFile::Close()
{
  if ( ! HDF5_check_hid_valid(h5file_.get_hid()) ) {
    cerr << "HDF5InOutFile::Close()/ERROR  Close() called on a non open  HDF5InOutFile object"<<endl;
    return;
  }
  int status = 0;
  if (((mode_ == HDF5InOutFile::Create)||(mode_ == HDF5InOutFile::CreateOverwrite))&&(h5file_.getNRef()==1))  {
    MuTyV mtv;
    mtv=string("--- SOPHYA (http://www.sophya.org) HDF5IOServer module ---");
    WriteAttribute(h5file_,"SOPHYA_LIBRARY_TAG",mtv);
    mtv=Version();
    WriteAttribute(h5file_,"SOPHYA_HDF5IOServer_Version",mtv);
    mtv=HDF5LibraryVersionStr();
    WriteAttribute(h5file_,"HDF5Library_Version",mtv);
  }
  fname_="";
  mode_=HDF5InOutFile::RO;
  root_grp_ = cur_grp_ = HDF5IdWrapper();
  h5file_ = HDF5IdWrapper();
  acrcnt_=0;  aopidx_=-1;
  list_curgrp_.clear();
  next_obj_create_name_=next_obj_open_name_="";
  return;
}


/*-- Methode --*/
/*! returned value = MajorVersionNumber+0.01*MinorVersionNumber+0.0001*ReleaseNumber */
double HDF5InOutFile::HDF5LibraryVersion()
{
  unsigned int majnum, minnum, relnum;
  herr_t st=H5get_libversion(&majnum,  &minnum,  &relnum);
  double ver = (double)majnum+0.01*(double)minnum+0.0001*(double)relnum;
  return ver;
}
/*-- Methode --*/
/*! returned string = MajorVersionNumber.MinorVersionNumber.ReleaseNumber */
std::string HDF5InOutFile::HDF5LibraryVersionStr()
{
  unsigned int majnum, minnum, relnum;
  herr_t st=H5get_libversion(&majnum,  &minnum,  &relnum);
  char str[64];
  sprintf(str,"%d.%d.%d",(int)majnum,(int)minnum,(int)relnum);
  return string(str);
}

//--- Numero de version du module - declaree et implementee ds hdf5init.h .cc 
double HDF5IOServer_Module_Version();

/*-- Methode --*/
double HDF5InOutFile::Version()  
{
  return HDF5IOServer_Module_Version();
}


/*-- Methode --*/
HDF5IdWrapper HDF5InOutFile::OpenGroup(const char * grp)
{
  char buff[64];
  bool fgusenext=false;
  string san;
  if ((grp==NULL)||(strlen(grp)<1)) {
    san = GetNextObjectOpenName();
    grp = san.c_str();
  }

  hid_t gid=-1;
  if (grp[0]=='/')   // absolute path
    gid = H5Gopen2(h5file_.get_hid(), grp, H5P_DEFAULT);
  else  // relative path to the current group
    gid = H5Gopen2(cur_grp_.get_hid(), grp, H5P_DEFAULT);
  if ( ! HDF5_check_hid_valid(gid) )
    throw HDF5IOException("HDF5InOutFile::OpenGroup()/ERROR failed opening group");
  if (fgusenext)  next_obj_open_name_="";
  return HDF5IdWrapper(gid, H5I_GROUP);
}

/*-- Methode --*/
HDF5IdWrapper HDF5InOutFile::CreateGroup(const char * grp)
{
  char buff[64];
  bool fgusenext=false;
  string san;
  if ((grp==NULL)||(strlen(grp)<1)) {
    san = GetNextObjectCreateName();
    grp = san.c_str();
  }
  hid_t gid=-1;
  if (grp[0]=='/')   // absolute path
    gid = H5Gcreate(h5file_.get_hid(), grp, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  else  // relative path to the current group
    gid = H5Gcreate(cur_grp_.get_hid(), grp, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if ( ! HDF5_check_hid_valid(gid) )
    throw HDF5IOException("HDF5InOutFile::CreateGroup()/ERROR failed creating group");
  if (fgusenext)  next_obj_create_name_="";
  return HDF5IdWrapper(gid, H5I_GROUP);
}

#define MAXRANK 8

/*-- Methode --*/
/*!
  \param dsname : Dataset name to be created in the file - absolute or relative path 
  \param datatype : data type of the dataset to be created 
  \param siz : dataset sizes along each dimension - the dataset rank is defined by siz.size()

  \warning: Note that in HDF5 convention, the LAST dimension is the FASTEST changing, contiguous in memory
  int A[3][10] in C would correspond to a dataset with siz[0]=3 and siz[1]=10 (remember that A[0],A[1] ... 
  points each to a set of 10 consecutive integers in memory. In Sophya, such an array would be declared as 
  TArray<int>(10, 3)  with SizeX=10 and SizeY=3 
*/
HDF5IdWrapper HDF5InOutFile::CreateDataSet(std::string const & dsname, hid_t datatype, std::vector<size_t> & siz)
{
  int rank = (int)siz.size();
  hsize_t sz[8];
  hsize_t * szp=sz;
  if (rank > 8)  szp = new hsize_t[(size_t)rank];
  for(size_t i=0; i<siz.size(); i++)  szp[i]=(hsize_t)siz[i];
  HDF5IdWrapper dswid = P_CreateDataSet(dsname.c_str(), datatype, rank, szp, NULL);
  if (sz != szp)  delete[] szp;
  return dswid;
}

/*-- Methode --*/
/*!
  \param dsname : Dataset name to be created in the file - absolute or relative path 
  \param datatype : data type of the dataset to be created 
  \param siz : dataset sizes along each dimension (or initial sizes if chunked) 
  \param chunk_siz : dataset chunk sizes (along each dimension) - Will not be chunked if this argument is set to NULL

  \warning : the dataset rank is defined by siz.size() which should be equal to chunk_siz.size()

  \warning: Note that in HDF5 convention, the LAST dimension is the FASTEST changing, contiguous in memory. 
  See HDF5InOutFile::CreateDataSet() 
 */
HDF5IdWrapper HDF5InOutFile::CreateChunkedDataSet(std::string const & dsname, hid_t datatype, std::vector<size_t> & siz, std::vector<size_t> & chunck_siz)
{
  if (siz.size() != chunck_siz.size())  
    throw SzMismatchError("HDF5InOutFile::CreateChunkedDataSet()/ERROR (siz.size() != chunck_siz.size()");
  int rank = (int)siz.size();
  hsize_t sz[8], csz[8];
  hsize_t * szp=sz;
  hsize_t * cszp=csz;
  if (rank > 8) {
    szp = new hsize_t[(size_t)rank];
    cszp = new hsize_t[(size_t)rank];
  }
  for(size_t i=0; i<siz.size(); i++)  {
    szp[i]=(hsize_t)siz[i];  cszp[i]=(hsize_t)chunck_siz[i]; 
  }  
  HDF5IdWrapper dswid = P_CreateDataSet(dsname.c_str(), datatype, rank, szp, cszp);
  if (sz != szp)  {
    delete[] szp;   delete[] cszp;
  }
  return dswid;
}

/*-- Methode --*/
/*!
  \param dsname : Dataset name to be created in the file - can be an absolute path in the file (/ as first char) or relative to the current group
  \param datatype : data type of the dataset to be created ( H5T_STD_I32LE H5T_STD_I32BE H5T_IEEE_F64BE H5T_IEEE_F64LE ...)
  \param rank : dataset rank (number of dimensions) 
  \param sizes : dataset sizes along each dimension (or initial sizes if chunked) 
  \param chunk_sizes : dataset chunk sizes (along each dimension) - Will not be chunked if this argument is set to NULL

  \warning: Note that in HDF5 convention, the LAST dimension is the FASTEST changing, contiguous in memory. 
  See HDF5InOutFile::CreateDataSet() 

*/

HDF5IdWrapper HDF5InOutFile::P_CreateDataSet(const char* dsname, hid_t datatype, int rank, hsize_t * sizes, hsize_t * chunk_sizes)
{
  char buff[64];
  bool fgusenext=false;
  string san;
  if ((dsname==NULL)||(strlen(dsname)<1)) {
    san=GetNextObjectCreateName();
    dsname=san.c_str();
  }
  
  hid_t cgpid = cur_grp_.get_hid(); // relative to the current group 
  if (dsname[0]=='/') cgpid = h5file_.get_hid(); // absolute path 

  hid_t dataspace=-1;
  hid_t dchunkid=H5P_DEFAULT;
  if (chunk_sizes != NULL) {  // creating chunked dataset with unlimited size
    hsize_t maxsiz[MAXRANK];
    for(int kk=0; kk<MAXRANK; kk++)  maxsiz[kk]=H5S_UNLIMITED;
    dchunkid=H5Pcreate(H5P_DATASET_CREATE);
    H5Pset_chunk(dchunkid, rank, chunk_sizes);
    dataspace = H5Screate_simple(rank, sizes , maxsiz);
  } 
  else dataspace = H5Screate_simple(rank, sizes , NULL);
  hid_t dataset = H5Dcreate(cgpid, dsname, datatype, dataspace, H5P_DEFAULT, dchunkid, H5P_DEFAULT);
  H5Sclose(dataspace);
  if ( ! HDF5_check_hid_valid(dataset) )
    throw HDF5IOException("HDF5InOutFile::CreateDataSet()/ERROR failed creating dataset");
  if (fgusenext)  next_obj_create_name_="";
  return (HDF5IdWrapper(dataset, H5I_DATASET));
}


/*-- Methode --*/
HDF5IdWrapper  HDF5InOutFile::OpenDataSet(const char* dsname, HDF5IdWrapper & datatype, HDF5IdWrapper & dataspace)
{
  char buff[64];
  bool fgusenext=false;
  string san;
  if ((dsname==NULL)||(strlen(dsname)<1)) {
    san=GetNextObjectOpenName();
    dsname=san.c_str();
  }
  hid_t dataset=-1;
  if (dsname[0]=='/')   // absolute path 
    dataset = H5Dopen(h5file_.get_hid(), dsname, H5P_DEFAULT);
  else  // relative to the current group 
    dataset = H5Dopen(cur_grp_.get_hid(), dsname, H5P_DEFAULT);
  if ( ! HDF5_check_hid_valid(dataset) )
    throw HDF5IOException("HDF5InOutFile::OpenDataSet()/ERROR failed opening dataset");
  datatype = HDF5IdWrapper(H5Dget_type(dataset), H5I_DATATYPE);
  dataspace = HDF5IdWrapper(H5Dget_space(dataset), H5I_DATASPACE);
  return HDF5IdWrapper(dataset, H5I_DATASET);
}

/*-- Methode --*/
herr_t HDF5InOutFile::FullDataSetRead(HDF5IdWrapper const & dataset, hid_t memdtype, void * memdatap)
{
  if (dataset.get_type() != H5I_DATASET) 
    throw  HDF5IOException("HDF5InOutFile::FullDataSetRead()/ERROR HDF5IdWrapper object not a DataSet");
  //DBG  cout << "*DBG*HDF5InOutFile::FullDataSetRead() dataset="<<dataset<<" memdtype="<<memdtype
  //       <<" memdatap="<<hex<<memdatap<<dec<<endl;
  return H5Dread(dataset.get_hid(), memdtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, memdatap);
}
/*-- Methode --*/
herr_t HDF5InOutFile::FullDataSetWrite(HDF5IdWrapper const & dataset, hid_t memdtype, const void * memdatap)
{
  if (dataset.get_type() != H5I_DATASET) 
    throw  HDF5IOException("HDF5InOutFile::FullDataSetWrite()/ERROR HDF5IdWrapper object not a DataSet");
  //DBG  cout << "*DBG*HDF5InOutFile::FullDataSetWrite() dataset="<<dataset<<" memdtype="<<memdtype
  //     <<" memdatap="<<hex<<memdatap<<dec<<endl;
  return H5Dwrite(dataset.get_hid(), memdtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, memdatap);
}

/*-- Methode --*/
/*! 
  \param dataset : Dataset Id 
  \param offset : read  position offset along each dimension. 
  \param count : read count along each dimension
  \param memdtype : in memory data type (in memdatap[]) 
  \param memdatap : pointer to the memory location holding or receiving data, considered as a contiguous array 
  with at least as many elements as the total number of elements to be read/written count[0]*count[1]*count[2]...
*/
herr_t  HDF5InOutFile::DataSetRead(HDF5IdWrapper const & dataset, hsize_t * offset, hsize_t * count, hid_t memdtype, void * memdatap)
{
  if (dataset.get_type() != H5I_DATASET) 
    throw  HDF5IOException("HDF5InOutFile::DataSetRead()/ERROR HDF5IdWrapper object not a DataSet");
  hid_t datasetid=dataset.get_hid();
  hid_t dataspaceid=H5Dget_space(datasetid); 
  int rank = H5Sget_simple_extent_ndims(dataspaceid);
  herr_t status = H5Sselect_hyperslab(dataspaceid, H5S_SELECT_SET, offset, NULL /*stride*/, count, NULL /*block*/);
  hsize_t memcnt[1]={1};
  for(int i=0; i<rank; i++) memcnt[0]*=count[i];
  hid_t mspace_id = H5Screate_simple(1, memcnt , NULL);
  status=H5Dread(datasetid, memdtype, mspace_id, dataspaceid, H5P_DEFAULT, memdatap);
  H5Sclose(dataspaceid);
  H5Sclose(mspace_id);
  return status;
}

/*-- Methode --*/
/*! 
  \param dataset : Dataset Id 
  \param offset :  write position offset along each dimension. 
  \param count :   write count along each dimension
  \param memdtype : in memory data type (in memdatap[]) 
  \param memdatap : pointer to the memory location holding or receiving data, considered as a contiguous array 
  with at least as many elements as the total number of elements to be read/written count[0]*count[1]*count[2]...
*/
herr_t  HDF5InOutFile::DataSetWrite(HDF5IdWrapper const & dataset, hsize_t * offset, hsize_t * count, hid_t memdtype, const void * memdatap)
{
  if (dataset.get_type() != H5I_DATASET) 
    throw  HDF5IOException("HDF5InOutFile::DataSetWrite()/ERROR HDF5IdWrapper object not a DataSet");
  hid_t datasetid=dataset.get_hid();
  hid_t dataspaceid=H5Dget_space(datasetid); 
  int rank = H5Sget_simple_extent_ndims(dataspaceid);
  herr_t status = H5Sselect_hyperslab(dataspaceid, H5S_SELECT_SET, offset, NULL /*stride*/, count, NULL /*block*/);
  hsize_t memcnt[1]={1};
  for(int i=0; i<rank; i++) memcnt[0]*=count[i];
  hid_t mspace_id = H5Screate_simple(1, memcnt , NULL);
  status=H5Dwrite(datasetid, memdtype, mspace_id, dataspaceid, H5P_DEFAULT, memdatap);
  H5Sclose(dataspaceid);
  H5Sclose(mspace_id);
  return status;
}

/*-- Methode --*/
herr_t HDF5InOutFile::ExtendDataSet(HDF5IdWrapper const & dataset, hsize_t * sizes)
{
  if (dataset.get_type() != H5I_DATASET) 
    throw  HDF5IOException("HDF5InOutFile::ExtendDataSet()/ERROR HDF5IdWrapper object not a DataSet");
  return H5Dset_extent(dataset.get_hid(), sizes);
}



/*-- Methode --*/
herr_t HDF5InOutFile::WriteAttribute(HDF5IdWrapper const & oid, const char * key, MuTyV const & val, const char *comment)
{
  //DBG  cout << " WriteAttribute*DBG: Writing Att Name="<<attname<<" ->Val="<<mtv<<endl;
  herr_t rc = -1;
  hid_t attdtyp, attmemdtyp;

  int_1 bval;
  int_8 lval;
  r_8 dval;
  complex<r_8> zval;
  string strval;

  void * attdatap=NULL;
  const char * strp[2]={NULL, NULL};
  hvl_t h5vlstr;
  
  switch (val.Type()) {
  case MuTyV::MTVBoolean :
    attdtyp = HDF5Types::DataType(bval);
    attmemdtyp = HDF5Types::NativeDataType(bval);
    val.Convert(bval);  bval=(bval?1:0);
    attdatap = (void *)(&bval);
    break;
  case MuTyV::MTVInteger :
    attdtyp = HDF5Types::DataType(lval);
    attmemdtyp = HDF5Types::NativeDataType(lval);
    val.Convert(lval);
    attdatap = (void *)(&lval);
    break;
  case MuTyV::MTVFloat : 
    attdtyp = HDF5Types::DataType(dval);
    attmemdtyp = HDF5Types::NativeDataType(dval);
    val.Convert(dval);
    attdatap = (void *)(&dval);
    break;
  case MuTyV::MTVComplex : 
    attdtyp = HDF5Types::DataType(zval);
    attmemdtyp = HDF5Types::NativeDataType(zval);
    val.Convert(zval);
    attdatap = (void *)(&zval);
    break;
  case MuTyV::MTVString :
    attdtyp = HDF5Types::VLStringDataType();
    attmemdtyp = HDF5Types::VLStringDataType();
    val.Convert(strval);
    /*
    h5vlstr.len = strval.length();
    h5vlstr.p = (void *)(strval.c_str());
    attdatap = (void *)(&h5vlstr);
    */
    strp[0] = strval.c_str();
    attdatap = strp;
    break;
    /*
  case MuTyV::MTVTimeStamp : 
    break;
    */
  default:
    attdatap = NULL;
    break;
  }
  if (attdatap == NULL)   return rc;
  /* Create dataspace - scalar  */
  hsize_t sz = 1;
  hid_t dsid = H5Screate_simple(1, &sz, NULL);
  /* Create a dataset attribute. */
  hid_t attid = H5Acreate2 (oid.get_hid(), key, attdtyp, dsid, H5P_DEFAULT, H5P_DEFAULT);
  /* Write the attribute data. */
  rc = H5Awrite(attid, attmemdtyp, attdatap);
  /* Close the attribute and dataspace */
  H5Aclose(attid);
  H5Sclose(dsid);
  return rc;
}

/*-- Methode --*/
herr_t HDF5InOutFile::WriteAttributes(HDF5IdWrapper const & oid, DVList const & dvl)
{
  herr_t rc = 0;
  DVList::ValList::const_iterator it;
  for(it = dvl.Begin(); it != dvl.End(); it++)  {
    rc += WriteAttribute(oid, (*it).first.c_str(), (*it).second.elval, (*it).second.elcomm.c_str());
  }
  return rc;
}

/*-- Methode --*/
MuTyV   HDF5InOutFile::ReadAttribute(hid_t oid, const char * key)
{
  MuTyV mtv;
  herr_t rc = -1;
  hid_t attmemdtyp;

  int_1 bval;
  int_8 lval;
  r_8 dval;
  complex<r_8> zval;
  string strval;

  void * attdatap=NULL;
#define _S_BUF_SZ_ 1024 
  char cbuff[_S_BUF_SZ_];
  char * abuff=NULL;
  bool fgbuffalloc=false;
  char * strp[2]={NULL, NULL};
  hvl_t h5vlstr;

  hid_t aid = H5Aopen(oid, key, H5P_DEFAULT);
  hid_t adtype = H5Aget_type(aid);
  H5T_class_t cl = H5Tget_class(adtype);
  size_t sz = H5Tget_size(adtype);
  hid_t aspace = H5Aget_space(aid);
  int arank = H5Sget_simple_extent_ndims(aspace);
  hsize_t adims[5]={0,0,0,0,0};
  if (arank<5) rc = H5Sget_simple_extent_dims(aspace, adims, NULL);

  if ((arank>1)||(adims[0]>1))  {
    cout<<"HDF5InOutFile::ReadAttribute()/Warning: Can NOT read NonScalar attribute - key="<<key<<" class="<<cl<<" size="<<sz
	<<" arank="<<arank<<" adims[0]="<<adims[0]<<endl;
    H5Aclose(aid);
    return mtv; 
  }
  switch (cl) {
  case H5T_INTEGER :
    if (sz == 1) {
      attmemdtyp = HDF5Types::NativeDataType(bval);
      attdatap = (void *)(&bval);
      rc = H5Aread(aid, attmemdtyp, attdatap);
      bool fgb = (bval > 0);
      mtv = fgb;
    }
    else {
      attmemdtyp = HDF5Types::NativeDataType(lval);
      attdatap = (void *)(&lval);
      rc = H5Aread(aid, attmemdtyp, attdatap);
      mtv = lval;
    }
    break;
  case H5T_FLOAT :
    attmemdtyp = HDF5Types::NativeDataType(dval);
    attdatap = (void *)(&dval);
    rc = H5Aread(aid, attmemdtyp, attdatap);
    mtv = dval;
    break;
  case H5T_STRING :
    //DBG    cout<<"*DBG*HDF5InOutFile::ReadAttribute() - reading H5T_STRING class="<<cl<<" size="<<sz<<" Is VLString? "
    //DBG	<< H5Tis_variable_str( adtype )<<endl;
    if (H5Tis_variable_str(adtype)>0) {   // Reading a variable length string 
      attmemdtyp = HDF5Types::VLStringDataType();
      attdatap = strp;
      rc = H5Aread(aid, attmemdtyp, attdatap);
      mtv = string(strp[0]);
      {
	hsize_t sz = 1;
	hid_t dsid = H5Screate_simple(1, &sz, NULL);
	H5Dvlen_reclaim(attmemdtyp, dsid, H5P_DEFAULT, attdatap);
	H5Sclose(dsid);
      }
    }
    else {   // Reading a fixed length string 
      abuff=cbuff;  fgbuffalloc=false;
      if (sz>=_S_BUF_SZ_)  { abuff=new char[sz+1]; fgbuffalloc=true; }
      //DBG      cout<<"*DBG*ReadAttribute() - reading fixed length string in abuf="<<hex<<abuff<<dec<<endl;
      attdatap = abuff;
      rc = H5Aread(aid, adtype, attdatap);
      abuff[sz]='\0'; 
      mtv = string(abuff);
      if (fgbuffalloc) delete[] abuff;
    }
    break;
  case H5T_COMPOUND :
    if ( ( H5Tget_nmembers( adtype ) == 2 ) && (H5Tget_member_class( adtype, 0) == H5T_FLOAT) &&
	 (H5Tget_member_class( adtype, 1) == H5T_FLOAT) ) {
      if (sz == sizeof(complex<r_8>) ) {
	complex<r_8> z8;
	attmemdtyp = HDF5Types::NativeDataType(z8);
	attdatap = &z8;
	rc = H5Aread(aid, attmemdtyp, attdatap);
	mtv = z8;
      }
      else if (sz == sizeof(complex<r_4>) ) {
	complex<r_4> z4;
	attmemdtyp = HDF5Types::NativeDataType(z4);
	attdatap = &z4;
	rc = H5Aread(aid, attmemdtyp, attdatap);
	mtv = z4;
      }
    }
    break;
  default:
    break;
  }
  /* Close the attribute  */
  H5Aclose(aid);
  return mtv;
}
  
int hdf5_att_liste(hid_t location_id, const char * key, const H5A_info_t * ainfo, void * usd)
{
  hid_t aid = H5Aopen(location_id, key, H5P_DEFAULT);
  if (!HDF5_check_hid_valid(aid))  { 
    cerr<<"File hdf5inoutfile.cc: hdf5_att_liste()/ERROR invalid Id returned by H5Aopen() !"<<endl;
    return 0;
  }
  hid_t adtype = H5Aget_type(aid);
  H5T_class_t cl = H5Tget_class(adtype);
  size_t sz = H5Tget_size(adtype);
  //DBG  cout << " *DBG*hdf5_att_liste() key="<<key<<" datasize="<<ainfo->data_size<<" cl="<<cl<<" sz="<<sz<<endl;
  H5Tclose(adtype);
  H5Aclose(aid);
  MuTyV mtv = HDF5InOutFile::ReadAttribute(location_id, key);
  //DBG  cout << " *DBG*hdf5_att_liste() Apres ReadAttribute() key="<<key<<" val="<<mtv<<endl;
  DVList * dvlp = (DVList * )(usd);
  if (dvlp)   (*dvlp)[key] = mtv;
  
  return 0;
}

/*-- Methode --*/
int HDF5InOutFile::ReadAttributes(HDF5IdWrapper const & oid, DVList & dvl)
{
  hsize_t nidx = 0;
  herr_t status = H5Aiterate2(oid.get_hid(), H5_INDEX_NAME, H5_ITER_NATIVE, &nidx, hdf5_att_liste, &dvl); // H5_ITER_INC
  return 0;
}

//--- classe utilitaire pour utilisation par hdf5_grp_liste() 
class Vec_StrH5Type { 
public:
  Vec_StrH5Type(std::vector<std::string> & dslist, std::vector<std::string> & grplist)
    : dslist_(dslist), grplist_(grplist), cnt_(0)
  { }
  std::vector<std::string> & dslist_;
  std::vector<std::string> & grplist_;
  int cnt_;
};

/*-- Methode --*/
H5O_type_t HDF5InOutFile::GetObjectType(const char* objpath)
{
  if ((objpath==NULL) || (objpath[0]=='\0'))  
    throw ParmError("HDF5InOutFile::GetObjectType() NULL or 0-length objpath");
  hid_t oid=-1;
  if (objpath[0]=='/')   // absolute path
    oid = H5Oopen(h5file_.get_hid(), objpath, H5P_DEFAULT);
  else 
    oid = H5Oopen(cur_grp_.get_hid(), objpath, H5P_DEFAULT);

  if (!HDF5_check_hid_valid(oid)) 
    throw HDF5IOException("HDF5InOutFile::GetObjectType()/ERROR invalid Id returned by H5Oopen");
    //    cerr<<"HDF5InOutFile::GetObjectTypeAttribute()/ERROR Invalid Id returned by H5Oopen() for objpath="<<objpath<<endl;

  H5O_info_t obj_info;
  herr_t st = H5Oget_info(oid, &obj_info);
  H5O_type_t otype=obj_info.type;
  H5Oclose(oid);
  return otype;
}

/*-- Methode --*/
/*!   
  The objet type (enum defined by the HDF5 library) is returned with return value \b H5O_TYPE_DATASET for a dataset and 
  \b H5O_TYPE_GROUP for a group.
 
  For a group or a dataset the attributes are read and returned in the DVList object dvl 
  For a dataset, the datatype and dataspace are also returned 
*/
H5O_type_t HDF5InOutFile::GetObjectType_Info(const char* objpath, DVList & dvl, HDF5IdWrapper & datatype, HDF5IdWrapper & dataspace)
{
  if ((objpath==NULL) || (objpath[0]=='\0'))  
    throw ParmError("HDF5InOutFile::GetObjectType_Info() NULL or 0-length objpath");
  hid_t oid=-1;
  if (objpath[0]=='/')   // absolute path
    oid = H5Oopen(h5file_.get_hid(), objpath, H5P_DEFAULT);
  else 
    oid = H5Oopen(cur_grp_.get_hid(), objpath, H5P_DEFAULT);

  if (!HDF5_check_hid_valid(oid)) 
    throw HDF5IOException("HDF5InOutFile::GetObjectType_Info()/ERROR invalid Id returned by H5Oopen");
    //    cerr<<"HDF5InOutFile::GetObjectTypeAttribute()/ERROR Invalid Id returned by H5Oopen() for objpath="<<objpath<<endl;

  H5O_info_t obj_info;
  herr_t st = H5Oget_info(oid, &obj_info);
  H5O_type_t ltype=obj_info.type;
  H5Oclose(oid);

  HDF5IdWrapper objid;
  if (ltype == H5O_TYPE_DATASET) {
    objid=OpenDataSet(objpath,datatype,dataspace);
  }
  else if (ltype == H5O_TYPE_GROUP) {
    objid=OpenGroup(objpath);
  }
  ReadAttributes(objid, dvl);
  
  return ltype;
}

herr_t hdf5_grp_liste(hid_t g_id, const char * name, const H5L_info_t * info, void * usd)
{
  hid_t oid = H5Oopen(g_id, name, H5P_DEFAULT);
  if (!HDF5_check_hid_valid(oid))  { 
    cerr<<"File hdf5inoutfile.cc: hdf5_grp_liste()/ERROR invalid Id returned by H5Oopen() !"<<endl;
    return 0;
  }
  Vec_StrH5Type * pvstrh5t = (Vec_StrH5Type * )(usd);
  pvstrh5t->cnt_++;
  H5O_info_t obj_info;
  herr_t st = H5Oget_info(oid, &obj_info);
  H5O_type_t ltype=obj_info.type;
  H5Oclose(oid);
  /*DBG cout << "*DBG*hdf5_grp_liste() cnt="<<pvstrh5t->cnt_<<" name="<<name<<" info->type="<<info->type
    <<"type="<<ltype<<"  (DS: "<<H5I_DATASET<<" ;GRP: "<<H5I_GROUP<<" )"<<endl; */
  if (ltype == H5O_TYPE_DATASET) pvstrh5t->dslist_.push_back(std::string(name));
  else if (ltype == H5O_TYPE_GROUP) pvstrh5t->grplist_.push_back(std::string(name));
  return 0;
}

/*-- Methode --*/
int HDF5InOutFile::GetGroupMemberList(HDF5IdWrapper const & gid, std::vector<std::string> & dslist, std::vector<std::string> & grplist)
{
  if (gid.get_type() != H5I_GROUP)  
    throw HDF5IOException("HDF5InOutFile::GetGroupMemberList(gid) gid is NOT a group");
  hsize_t idx=0;
  Vec_StrH5Type vstrh5t(dslist, grplist);
  herr_t status = H5Literate(gid.get_hid(), H5_INDEX_NAME, H5_ITER_NATIVE, &idx, hdf5_grp_liste, (void *)&vstrh5t);
  return vstrh5t.cnt_;
}
/*-- Methode --*/
std::string HDF5InOutFile::GetNextObjectCreateName()
{
  if (next_obj_create_name_.length()>0) {
    string rs=next_obj_create_name_;
    next_obj_create_name_="";
    return rs;
  }
  else {
    acrcnt_++;
    char buff[32];
    sprintf(buff,"soh5a%d",(int)acrcnt_);
    return string(buff);
  }
  return "";
}

/*-- Methode --*/
/*! 
  will return the object (dataset or group) name if defined by a previous call to SetNextObjectReadName() 
  or will provide sequentially all object names present in the current group 

  \warning : will throw an HDF5IOException if no dataset or group name present in the current directory or if all have been sequentially read
 */
std::string HDF5InOutFile::GetNextObjectOpenName()
{
  if (next_obj_open_name_.length()>0) {
    string rs=next_obj_open_name_;
    next_obj_open_name_="";
    return rs;
  }
  else {
    if (aopidx_ < 0)  {
      list_curgrp_.clear();
      std::vector<std::string> l_gr;
      GetCurrentGroupMemberList(list_curgrp_, l_gr);
      for(size_t i=0; i<l_gr.size(); i++) list_curgrp_.push_back(l_gr[i]);
      aopidx_=0;
    }
    if (aopidx_<list_curgrp_.size()) {
      size_t ci=aopidx_;  aopidx_++;
      return list_curgrp_[ci];
    }
    else throw HDF5IOException("HDF5InOutFile::GetNextObjectReadName() No name available for sequential read");
  }
  return "";
}

/*-- Methode --*/
void HDF5InOutFile::Print(ostream& os, int lev) const
{
  string mode = convert_hdf5_access_mode(mode_);
  os << " HDF5InOutFile(FileName= " << fname_  << " Mode=" 
     << mode  << ") HDF5LibraryVersion= " << HDF5LibraryVersionStr() << endl;

  return;
}

} // Fin du namespace SOPHYA

