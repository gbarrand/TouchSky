/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  G. Barrand, R. Ansari , 2016-2018 
  Small utility/wrapper classes for interface with HDF5 library 
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#include "machdefs.h"
#include <iostream>

#include "shdf5utils.h"
#include "timestamp.h"

using namespace std;

namespace SOPHYA {
 
//---------------------------------------------------------
//--------   HDF5IdWrapper class implementation ----------
//---------------------------------------------------------

static ThSafeOp * g_sh5_thsop = NULL;  // global mutex for thread safe operation

/*! 
  \ingroup HDF5IOServer
  \brief Wrapper class for HDF5 handle/identifiers (hid_t) 
  This class handles reference sharing and the handle closing by the object destructor 
*/

//! public initialisation method for the class (to ensure thread safe operation)  
void HDF5IdWrapper::doInit()
{
  if (g_sh5_thsop == NULL)   g_sh5_thsop = new ThSafeOp; // global mutex for thread safe operation
  return;
}


bool HDF5IdWrapper::IsCompound(hid_t dt)
{
  H5T_class_t cl = H5Tget_class(dt);
  if (cl == H5T_COMPOUND)  return true;
  return false;
}

bool HDF5IdWrapper::AreEqualDataType(hid_t dt1, hid_t dt2) 
{
  return (H5Tequal(dt1, dt2)>0);
}

bool HDF5IdWrapper::AreSameDataType(hid_t dt1, hid_t dt2) 
{
  return HDF5IdWrapper::AreSameOrCompatibleDataType(dt1, dt2, true); 
}

bool HDF5IdWrapper::AreCompatibleDataType(hid_t dt1, hid_t dt2)
{
  return HDF5IdWrapper::AreSameOrCompatibleDataType(dt1, dt2, false); 
}

bool HDF5IdWrapper::AreSameOrCompatibleDataType(hid_t dt1, hid_t dt2, bool fgsame)
{
  bool fg1 = (H5Tequal(dt1, dt2)>0);
  if (fg1) return true;
  H5T_class_t cl1 = H5Tget_class(dt1);
  H5T_class_t cl2 = H5Tget_class(dt2);
  size_t sz1=H5Tget_size(dt1);
  size_t sz2=H5Tget_size(dt2);
  H5T_order_t ord1=H5Tget_order(dt1);
  H5T_order_t ord2=H5Tget_order(dt2);
  //DBG  cout << " *DBG*AreSameOrCompatibleDataType(fgsame="<<(fgsame?"TRUE":"FALSE")<<" cl1="<<cl1<<" cl2="<<cl2<<" sz1="<<sz1
  //     <<" sz2="<<sz2<<" H5T_FLOAT="<<H5T_FLOAT<<" H5T_INTEGER="<<H5T_INTEGER<<endl;
  if (fgsame) {
    if ((cl1 == cl2) && ((cl1 == H5T_FLOAT) || (cl1 == H5T_INTEGER) || (cl1 == H5T_STRING)) && (sz1 == sz2))  return true;
  }
  else {
    if ((cl1 == cl2) && ((cl1 == H5T_FLOAT) || (cl1 == H5T_INTEGER)) )  return true;
  }
  // On s'occupe des autres types (structures=compund), complexes en particulier !
  if ((cl1 == H5T_COMPOUND) && (cl2 == H5T_COMPOUND)) {
    int nmemb=H5Tget_nmembers(dt1);
    /* DBG    cout << " *DBG*AreSameOrCompatibleDataType(fgsame="<<(fgsame?"TRUE":"FALSE")<<" Compounds nmemb="<<nmemb
       << " nmemb2="<<H5Tget_nmembers(dt2)<<endl;  */
    if (nmemb != H5Tget_nmembers(dt2)) return false;
    bool fgok=true;
    for(int i=0; i<nmemb; i++) {
      hid_t mtyp1 = H5Tget_member_type(dt1, i);
      hid_t mtyp2 = H5Tget_member_type(dt2, i);
      H5T_class_t mcl1 = H5Tget_class(mtyp1); 
      H5T_class_t mcl2 = H5Tget_class(mtyp2); 
      size_t msz1=H5Tget_size(mtyp1);
      size_t msz2=H5Tget_size(mtyp2);
      size_t moff1=H5Tget_member_offset(dt1, i);
      size_t moff2=H5Tget_member_offset(dt1, i);
      //DBG      cout << " *DBG*AreSameOrCompatibleDataType() i="<<i<<" mcl1="<<mcl1<<" mcl2="<<mcl2<<" msz1="<<msz1<<" msz2="<<msz2<<" moff1="<<moff1<<" moff2="<<moff2<<endl;
      if ((mcl1 != mcl2) || ((mcl1 != H5T_FLOAT) && (mcl1 != H5T_INTEGER) && (mcl1 != H5T_STRING)) || 
	  (fgsame && (msz1 != msz2)) || (fgsame && (moff1 != moff2)) ) 	fgok=false;
      H5Tclose(mtyp1);
      H5Tclose(mtyp2);
      if (!fgok) break;
    }
    if (fgok)  return true;
    else return false;
  }
  return false;
}

hid_t HDF5IdWrapper::getCompatibleDataTypeForCompound(hid_t memdtyp, hid_t fdtyp)
{
  //-- ca peut pas marcher si on ne fait pas de copie 
  //DEL  if (HDF5IdWrapper::AreEqualDataType(memdtyp, fdtyp) || !HDF5IdWrapper::IsCompound(fdtyp))   return memdtyp;
  //DBG  cout << " *DBG*HDF5IdWrapper::getCompatibleDataTypeForCompound() : copying fdtyp="<<fdtyp<<endl;
  hid_t fdtc=H5Tcopy(fdtyp);
  H5Tset_order(fdtc,H5Tget_order(memdtyp));
  return fdtc;
}

void HDF5IdWrapper::closeDataTypeForCompound(hid_t fcdtyp)
{
  if (fcdtyp != H5I_INVALID_HID) H5Tclose(fcdtyp);
}

const char * HDF5IdWrapper::ConvH5Type2str(H5I_type_t  h5typ)
{
  switch ( h5typ ) {
  case H5I_FILE :
    return "H5I_FILE";
    break;
  case H5I_GROUP :
    return "H5I_GROUP";
     break;
  case H5I_DATATYPE :
    return "H5I_DATATYPE";
    break;
  case H5I_DATASPACE :
    return "H5I_DATASPACE";
    break;
  case H5I_DATASET :
    return "H5I_DATASET";
    break;
  case H5I_ATTR :
    return "H5I_ATTR";
    break;
  default:
    return "UNKNOWN_H5Type";
    break;
  }
  return "XXXXX??XX";
}

  
//! default constructor - No Id , NULL H5IDWREF  
HDF5IdWrapper::HDF5IdWrapper()
  : mIdRef(NULL)
{
}

//  The useful constructor, creates a wrapper from a hopefully valid hid_t , 
HDF5IdWrapper::HDF5IdWrapper(hid_t h5id, H5I_type_t  h5typ)
  : mIdRef(NULL)
{
  mIdRef = new H5IDWREF;
  g_sh5_thsop->lock();
  mIdRef->nref_=1;
  mIdRef->h5id_=h5id;
  mIdRef->h5typ_=h5typ;
  g_sh5_thsop->unlock();
}

// copy constructor : shares the identifier 
HDF5IdWrapper::HDF5IdWrapper(HDF5IdWrapper const & a)
  : mIdRef(NULL)
{
  g_sh5_thsop->lock();
  share(a);
  g_sh5_thsop->unlock();
}

// destructor : call the close() method (see  below)
  HDF5IdWrapper::~HDF5IdWrapper()
{
  g_sh5_thsop->lock();
  close();
  g_sh5_thsop->unlock();
}

// equal/copy operator : shares the identifier
HDF5IdWrapper & HDF5IdWrapper::operator = (HDF5IdWrapper const & a)
{
  g_sh5_thsop->lock();
  share(a);
  g_sh5_thsop->unlock();
  return *this;
}

// return the HDF5 embedded hid_t handle 
hid_t HDF5IdWrapper::get_hid() const
{
  if (mIdRef)  return mIdRef->h5id_;
  return H5I_INVALID_HID;
}

// return the HDF5 embedded handle type 
H5I_type_t HDF5IdWrapper::get_type() const
{
  if (mIdRef)  return mIdRef->h5typ_;
  return H5I_UNINIT;
}

  /* return the dataset or group name, if applicable
std::string HDF5IdWrapper::getName()  const
{
  if (mIdRef)  return mIdRef->name_;
  return "";
}
*/

const char * HDF5IdWrapper::get_type_as_str() const
{
  if (mIdRef)  return ConvH5Type2str(mIdRef->h5typ_);
  else return "";
}
  
// return the number of references to the HDF5 Id 
size_t HDF5IdWrapper::getNRef() const
{
  if (mIdRef)  return mIdRef->nref_;
  return 0;
}


/*! 
  returns the dataspace sizes as a vector<size_t>. The returned vector size represents the array rank, 
*/
std::vector<size_t> HDF5IdWrapper::getDSSizes() const
{
  if (get_type() != H5I_DATASPACE)   
    throw  HDF5IOException("HDF5IdWrapper::getDSSizes()/ERROR object not a DataSpace");
  hid_t dsid = get_hid();
  size_t rank = (size_t)H5Sget_simple_extent_ndims(get_hid()); // H5Tget_size(dsid);
  std::vector<size_t> siz(rank);
  hsize_t sz[8];
  hsize_t * szp=sz;
  if (rank > (size_t)8)  szp = new hsize_t[rank];
  H5Sget_simple_extent_dims(dsid, szp, NULL);
  for(size_t i=0; i<rank; i++)  siz[i]=(size_t)szp[i];
  if (sz != szp)  delete[] szp;
  return siz;
}
  
//! call close() and shares the H5IDWREF pointer, incrementing the reference count.
void HDF5IdWrapper::share(HDF5IdWrapper const & a)
{
  close();
  mIdRef = a.mIdRef;
  if (mIdRef) mIdRef->nref_++;
  return;
}

//! decrement the reference count, closes the HDF5 handle and deletes H5IDWREF when count reaches zero
void HDF5IdWrapper::close()  
{
  if (mIdRef == NULL)   return;
  mIdRef->nref_--;
  if (mIdRef->nref_ > 0)  return;
  // Count has reached zero :
  switch ( mIdRef->h5typ_ ) {
  case H5I_FILE :
    H5Fclose( mIdRef->h5id_ );
    break;
  case H5I_GROUP :
    H5Gclose( mIdRef->h5id_ );
    break;
  case H5I_DATATYPE :
    H5Tclose( mIdRef->h5id_ );
    break;
  case H5I_DATASPACE :
    H5Sclose( mIdRef->h5id_ );
    break;
  case H5I_DATASET :
    H5Dclose( mIdRef->h5id_ );
    break;
  case H5I_ATTR :
    H5Aclose( mIdRef->h5id_ );
    break;
  default:
    cerr<<"HDF5IdWrapper::delete()/ERROR: invalid or unhandled hid_t type"<<(int)mIdRef->h5typ_<<endl;
    break;
  }
  delete mIdRef;
}

void HDF5IdWrapper::Print(ostream& os)  const
{
  //  os << "HDF5IdWrapper(hid="<<hex<<get_hid()<<dec<<") type="<<get_type_as_str();
  os << get_type_as_str();
  if (get_type() == H5I_DATASPACE) {
    std::vector<size_t> sz=getDSSizes();
    os << "  Rank="<<getDSRank()<<"  Sizes[1x2x3...->...ZxYxX] = ";
    for(size_t i=0; i<sz.size(); i++) {
      if (i>0) os << " x ";  
      os << sz[i];
    }
  }
  else if (get_type() == H5I_DATATYPE) {
    string dtcls="H5T_???";
    if (getDTClass()==H5T_INTEGER)  dtcls="H5T_INTEGER";
    else if (getDTClass()==H5T_FLOAT)  dtcls="H5T_FLOAT";
    else if (getDTClass()==H5T_COMPOUND)  dtcls="H5T_COMPOUND";
    os << "  Class= " << dtcls << " Size= "<<getDTSize();
  }
  return; 
}


//---------------------------------------------------------
//---------   HDF5Types class implementation -------------
//---------------------------------------------------------

/*! 
  \ingroup HDF5IOServer
  \brief  for converting c/c++ types to HDF5 Datatype

  NativeDataType() method return the data type as mapped in memory, while DataType() return the data type with a specific 
  byte ordering. The byte oredering correspond Some c++ objects might need data conversion. See the class SOH5DataConverter<T>.

  \warning The hid_t returned by call to HDF5IOServer::DataType() or HDF5IOServer::NativeDataType()
  should NOT be passed to H5Tclose(). 

  \sa SOPHYA::SOH5DataConverter
*/

static bool fg_hdf5_init_types_done = false;   // to test for initial creation of custom (complex,TimeStamp...) data types
// Native (in memory) complex data types - for in memory data types, as returned by HDF5Types<T>::NativeDataType()
static hid_t hdf5_cmplx_r4 = H5I_INVALID_HID;
static hid_t hdf5_cmplx_r8 = H5I_INVALID_HID;
static hid_t hdf5_cmplx_r16 = H5I_INVALID_HID;

// long double (16 bytes) real numbers with byte ordering 
static hid_t hdf5_r16_le = H5I_INVALID_HID;
static hid_t hdf5_r16_be = H5I_INVALID_HID;

// complex data types with byte ordering specification - For data types in file, as returned by HDF5Types<T>::DataType()
static hid_t hdf5_cmplx_r4_le = H5I_INVALID_HID;
static hid_t hdf5_cmplx_r4_be = H5I_INVALID_HID;
static hid_t hdf5_cmplx_r8_le = H5I_INVALID_HID;
static hid_t hdf5_cmplx_r8_be = H5I_INVALID_HID;
static hid_t hdf5_cmplx_r16_le = H5I_INVALID_HID;
static hid_t hdf5_cmplx_r16_be = H5I_INVALID_HID;

//  data type corresponding to TimeStamp 
static hid_t hdf5_timestamp_ri8 = H5I_INVALID_HID;
static hid_t hdf5_timestamp_ri8_le = H5I_INVALID_HID;
static hid_t hdf5_timestamp_ri8_be = H5I_INVALID_HID;

// variable length string
static hid_t hdf5_vl_string = H5I_INVALID_HID;
  
// fixed length string (for FMLString<16> <32> <64> <128>
static hid_t hdf5_string_fmlstr16 = H5I_INVALID_HID;
static hid_t hdf5_string_fmlstr32 = H5I_INVALID_HID;
static hid_t hdf5_string_fmlstr64 = H5I_INVALID_HID;
static hid_t hdf5_string_fmlstr128 = H5I_INVALID_HID;

void HDF5Types::doInit()
{
  if ( fg_hdf5_init_types_done ) return;
  return HDF5Types::P_doInit();
}


// we use the fact that c++ complex< > has the same memory layout as the structures defined below 
typedef struct {  r_4 re, im;  } cmplx_r4;
typedef struct {  r_8 re, im;  } cmplx_r8;
#ifdef  SO_LDBLE128
typedef struct {  r_16 re, im;  } cmplx_r16;
#endif
typedef struct {  r_8 seconds; int_8 days;  } timestamp_r8_i8;


void HDF5Types::P_doInit()
{
  herr_t  status;
  //--- complex<r_4>
  // DBG cout << "*DBG*P_doInit()  Creating compound type hdf5_cmplx_r4 "<< endl;
  r_4 x4;
  hdf5_cmplx_r4 = H5Tcreate (H5T_COMPOUND, sizeof ( cmplx_r4 ));
  H5Tinsert (hdf5_cmplx_r4, "complex<float>.real", HOFFSET(cmplx_r4, re), HDF5Types::NativeDataType(x4));
  H5Tinsert (hdf5_cmplx_r4, "complex<float>.imaginary", HOFFSET(cmplx_r4, im), HDF5Types::NativeDataType(x4));

  hdf5_cmplx_r4_le = H5Tcopy(hdf5_cmplx_r4);
  H5Tset_order(hdf5_cmplx_r4_le, H5T_ORDER_LE);
  hdf5_cmplx_r4_be = H5Tcopy(hdf5_cmplx_r4);
  H5Tset_order(hdf5_cmplx_r4_be, H5T_ORDER_BE);

  H5Tlock(hdf5_cmplx_r4);
  H5Tlock(hdf5_cmplx_r4_le);
  H5Tlock(hdf5_cmplx_r4_be);

  //--- complex<r_8>
  //DBG cout << "*DBG*P_doInit()  Creating compound type hdf5_cmplx_r8 "<< endl;
  r_8 x8;
  hdf5_cmplx_r8 = H5Tcreate (H5T_COMPOUND, sizeof ( cmplx_r8 ));
  H5Tinsert (hdf5_cmplx_r8, "complex<double>.real", HOFFSET(cmplx_r8, re), HDF5Types::NativeDataType(x8));
  H5Tinsert (hdf5_cmplx_r8, "complex<double>.imaginary", HOFFSET(cmplx_r8, im), HDF5Types::NativeDataType(x8));

  hdf5_cmplx_r8_le = H5Tcopy(hdf5_cmplx_r8);
  H5Tset_order(hdf5_cmplx_r8_le, H5T_ORDER_LE);
  hdf5_cmplx_r8_be = H5Tcopy(hdf5_cmplx_r8);
  H5Tset_order(hdf5_cmplx_r8_be, H5T_ORDER_BE);

  H5Tlock(hdf5_cmplx_r8);
  H5Tlock(hdf5_cmplx_r8_le);
  H5Tlock(hdf5_cmplx_r8_be);

  //--- complex<r_16>
#ifdef  SO_LDBLE128
  r_16 x16;
  hdf5_r16_le = H5Tcopy(HDF5Types::NativeDataType(x16));
  H5Tset_order(hdf5_r16_le, H5T_ORDER_LE);
  hdf5_r16_be = H5Tcopy(HDF5Types::NativeDataType(x16));
  H5Tset_order(hdf5_r16_be, H5T_ORDER_BE);
  H5Tlock(hdf5_r16_le);
  H5Tlock(hdf5_r16_be);

  hdf5_cmplx_r16 = H5Tcreate (H5T_COMPOUND, sizeof ( cmplx_r16 ));
  H5Tinsert (hdf5_cmplx_r16, "complex<long double>.real", HOFFSET(cmplx_r16, re), HDF5Types::NativeDataType(x16));
  H5Tinsert (hdf5_cmplx_r16, "complex<long double>.imaginary", HOFFSET(cmplx_r16, im), HDF5Types::NativeDataType(x16));

  hdf5_cmplx_r16_le = H5Tcopy(hdf5_cmplx_r16);
  H5Tset_order(hdf5_cmplx_r16_le, H5T_ORDER_LE);
  hdf5_cmplx_r16_be = H5Tcopy(hdf5_cmplx_r16);
  H5Tset_order(hdf5_cmplx_r16_be, H5T_ORDER_BE);

  H5Tlock(hdf5_cmplx_r16);
  H5Tlock(hdf5_cmplx_r16_le);
  H5Tlock(hdf5_cmplx_r16_be);
#endif

  //  datatype for TimeStamp 
  r_8 tx8;
  int_8 ti8;
  hdf5_timestamp_ri8 = H5Tcreate (H5T_COMPOUND, sizeof ( timestamp_r8_i8 ));
  H5Tinsert (hdf5_timestamp_ri8, "timestamp.seconds", HOFFSET(timestamp_r8_i8, seconds), HDF5Types::NativeDataType(tx8));
  H5Tinsert (hdf5_timestamp_ri8, "timestamp.days", HOFFSET(timestamp_r8_i8, days), HDF5Types::NativeDataType(ti8));

  hdf5_timestamp_ri8_le = H5Tcopy(hdf5_timestamp_ri8);
  H5Tset_order(hdf5_timestamp_ri8_le, H5T_ORDER_LE);
  hdf5_timestamp_ri8_be = H5Tcopy(hdf5_timestamp_ri8);
  H5Tset_order(hdf5_timestamp_ri8_be, H5T_ORDER_BE);
  H5Tlock(hdf5_timestamp_ri8);
  H5Tlock(hdf5_timestamp_ri8_le);
  H5Tlock(hdf5_timestamp_ri8_be);

  // Fixed length strings FMLString<16> <32> <64> <128>
  hdf5_string_fmlstr16 = H5Tcopy(H5T_C_S1);
  status = H5Tset_size(hdf5_string_fmlstr16, 16); 
  H5Tlock(hdf5_string_fmlstr16);
  hdf5_string_fmlstr32 = H5Tcopy(H5T_C_S1);
  status = H5Tset_size(hdf5_string_fmlstr32, 32); 
  H5Tlock(hdf5_string_fmlstr32);
  hdf5_string_fmlstr64 = H5Tcopy(H5T_C_S1);
  status = H5Tset_size(hdf5_string_fmlstr64, 64); 
  H5Tlock(hdf5_string_fmlstr64);
  hdf5_string_fmlstr128 = H5Tcopy(H5T_C_S1);
  status = H5Tset_size(hdf5_string_fmlstr128, 128); 
  H5Tlock(hdf5_string_fmlstr128);

  //--- variable length string
  hdf5_vl_string = H5Tcopy(H5T_C_S1);
  status = H5Tset_size(hdf5_vl_string, H5T_VARIABLE); 
  H5Tlock(hdf5_vl_string);
  /*
  hdf5_vl_string = H5Tvlen_create (H5T_C_S1);
  */
  // CHECK / REZA , should we check the status ?

  fg_hdf5_init_types_done= true;
  return;
}

hid_t HDF5Types::DataType(int_1  d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (H5T_STD_I8LE);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (H5T_STD_I8BE);

#ifdef IS_BIG_ENDIAN
  return (H5T_STD_I8BE);
#else
  return (H5T_STD_I8LE);
#endif
}

hid_t HDF5Types::DataType(uint_1  d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (H5T_STD_U8LE);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (H5T_STD_U8BE);

#ifdef IS_BIG_ENDIAN
  return (H5T_STD_U8BE);
#else
  return (H5T_STD_U8LE);
#endif
}

hid_t HDF5Types::DataType(int_2  d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (H5T_STD_I16LE);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (H5T_STD_I16BE);

#ifdef IS_BIG_ENDIAN
  return (H5T_STD_I16BE);
#else
  return (H5T_STD_I16LE);
#endif
}

hid_t HDF5Types::DataType(uint_2  d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (H5T_STD_U16LE);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (H5T_STD_U16BE);

#ifdef IS_BIG_ENDIAN
  return (H5T_STD_U16BE);
#else
  return (H5T_STD_U16LE);
#endif
}

hid_t HDF5Types::DataType(int_4  d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (H5T_STD_I32LE);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (H5T_STD_I32BE);

#ifdef IS_BIG_ENDIAN
  return (H5T_STD_I32BE);
#else
  return (H5T_STD_I32LE);
#endif
}

hid_t HDF5Types::DataType(uint_4  d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (H5T_STD_U32LE);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (H5T_STD_U32BE);

#ifdef IS_BIG_ENDIAN
  return (H5T_STD_U32BE);
#else
  return (H5T_STD_U32LE);
#endif
}

hid_t HDF5Types::DataType(int_8  d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (H5T_STD_I64LE);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (H5T_STD_I64BE);

#ifdef IS_BIG_ENDIAN
  return (H5T_STD_I64BE);
#else
  return (H5T_STD_I64LE);
#endif
}

hid_t HDF5Types::DataType(uint_8  d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (H5T_STD_U64LE);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (H5T_STD_U64BE);

#ifdef IS_BIG_ENDIAN
  return (H5T_STD_U64BE);
#else
  return (H5T_STD_U64LE);
#endif
}

hid_t HDF5Types::DataType(r_4    d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (H5T_IEEE_F32LE);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (H5T_IEEE_F32BE);

#ifdef IS_BIG_ENDIAN
  return (H5T_IEEE_F32BE);
#else
  return (H5T_IEEE_F32LE);
#endif
}

hid_t HDF5Types::DataType(r_8    d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (H5T_IEEE_F64LE);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (H5T_IEEE_F64BE);

#ifdef IS_BIG_ENDIAN
  return (H5T_IEEE_F64BE);
#else
  return (H5T_IEEE_F64LE);
#endif
}

hid_t HDF5Types::DataType(complex<r_4>  d, HDF5Types::ByteOrder bo)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();

  if (bo==HDF5Types::BO_LittleEndian) 
    return (hdf5_cmplx_r4_le);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (hdf5_cmplx_r4_be);

#ifdef IS_BIG_ENDIAN
  return (hdf5_cmplx_r4_be);
#else
  return (hdf5_cmplx_r4_le);
#endif
}

hid_t HDF5Types::DataType(complex<r_8>  d, HDF5Types::ByteOrder bo)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();

  if (bo==HDF5Types::BO_LittleEndian) 
    return (hdf5_cmplx_r8_le);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (hdf5_cmplx_r8_be);

#ifdef IS_BIG_ENDIAN
  return (hdf5_cmplx_r8_be);
#else
  return (hdf5_cmplx_r8_le);
#endif
}

#ifdef  SO_LDBLE128
hid_t HDF5Types::DataType(r_16    d, HDF5Types::ByteOrder bo)
{
  if (bo==HDF5Types::BO_LittleEndian) 
    return (hdf5_r16_le);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (hdf5_r16_be);

#ifdef IS_BIG_ENDIAN
  return (hdf5_r16_be);
#else
  return (hdf5_r16_le);
#endif
}

hid_t HDF5Types::DataType(complex<r_16>  d, HDF5Types::ByteOrder bo)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();

  if (bo==HDF5Types::BO_LittleEndian) 
    return (hdf5_cmplx_r16_le);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (hdf5_cmplx_r16_be);

#ifdef IS_BIG_ENDIAN
  return (hdf5_cmplx_r16_be);
#else
  return (hdf5_cmplx_r16_le);
#endif
}

#endif

hid_t HDF5Types::DataType(TimeStamp const & d, HDF5Types::ByteOrder bo)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();

  if (bo==HDF5Types::BO_LittleEndian) 
    return (hdf5_timestamp_ri8_le);
  else if (bo==HDF5Types::BO_BigEndian) 
    return (hdf5_timestamp_ri8_be);

#ifdef IS_BIG_ENDIAN
  return (hdf5_timestamp_ri8_be);
#else
  return (hdf5_timestamp_ri8_le);
#endif
}

hid_t HDF5Types::DataType(FMLString<16> const & d, HDF5Types::ByteOrder /*bo*/)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_string_fmlstr16);
}
hid_t HDF5Types::DataType(FMLString<32> const & d, HDF5Types::ByteOrder /*bo*/)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_string_fmlstr32);
}
hid_t HDF5Types::DataType(FMLString<64> const & d, HDF5Types::ByteOrder /*bo*/)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_string_fmlstr64);
}
hid_t HDF5Types::DataType(FMLString<128> const & d, HDF5Types::ByteOrder /*bo*/)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_string_fmlstr128);
}

hid_t HDF5Types::NativeDataType(int_1  d)
{
  return (H5T_NATIVE_SCHAR);
}

hid_t HDF5Types::NativeDataType(uint_1  d)
{
  return (H5T_NATIVE_UCHAR);
}

hid_t HDF5Types::NativeDataType(int_2  d)
{
  hid_t dtyp = H5T_NATIVE_SHORT;
  if (sizeof(short int) != 2)  {
    if (sizeof(int) == 2) dtyp = H5T_NATIVE_INT;
  }
  return dtyp;
}

hid_t HDF5Types::NativeDataType(uint_2  d)
{
  hid_t dtyp = H5T_NATIVE_USHORT;
  if (sizeof(unsigned short int) != 2)  {
    if (sizeof(unsigned int) == 2) dtyp = H5T_NATIVE_UINT;
  }
  return dtyp;
}


hid_t HDF5Types::NativeDataType(int_4  d)
{
  hid_t dtyp = H5T_NATIVE_INT;
  if (sizeof(int) != 4)  {
    if (sizeof(short int) == 4) dtyp = H5T_NATIVE_SHORT;
    else if (sizeof(long int) == 4)  dtyp = H5T_NATIVE_LONG;
  }
  return dtyp;
}

hid_t HDF5Types::NativeDataType(uint_4  d)
{
  hid_t dtyp = H5T_NATIVE_UINT;
  if (sizeof(unsigned int) != 4)  {
    if (sizeof(unsigned short int) == 4) dtyp = H5T_NATIVE_USHORT;
    else if (sizeof(unsigned long int) == 4)  dtyp = H5T_NATIVE_ULONG;
  }
  return dtyp;
}

hid_t HDF5Types::NativeDataType(int_8  d)
{
  hid_t dtyp = H5T_NATIVE_LONG;
  if (sizeof(long int) != 8)  {
    if (sizeof(long long int) == 8) dtyp = H5T_NATIVE_LLONG;
  }
  return dtyp;
}

hid_t HDF5Types::NativeDataType(uint_8  d)
{
  hid_t dtyp = H5T_NATIVE_ULONG;
  if (sizeof(unsigned long int) != 8)  {
    if (sizeof(unsigned long long int) == 8) dtyp = H5T_NATIVE_ULLONG;
  }
  return dtyp;
}

hid_t HDF5Types::NativeDataType(r_4  d)
{
  hid_t dtyp = H5T_NATIVE_FLOAT;
  if (sizeof(float) != 4)  {
    if (sizeof(double) == 4) dtyp = H5T_NATIVE_DOUBLE;
  }
  return dtyp;
}

hid_t HDF5Types::NativeDataType(r_8  d)
{
  hid_t dtyp = H5T_NATIVE_DOUBLE;
  if (sizeof(double) != 8)  {
    if (sizeof(float) == 8) dtyp = H5T_NATIVE_FLOAT;
  }
  return dtyp;
}

hid_t HDF5Types::NativeDataType(complex<r_4>  d)
{
  if ( !fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_cmplx_r4);
}

hid_t HDF5Types::NativeDataType(complex<r_8>  d)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_cmplx_r8);
}

#ifdef  SO_LDBLE128
hid_t HDF5Types::NativeDataType(r_16 d)
{
  return (H5T_NATIVE_LDOUBLE);
}

hid_t HDF5Types::NativeDataType(complex<r_16>  d)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_cmplx_r16);
}
#endif

hid_t HDF5Types::NativeDataType(TimeStamp const & d)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_timestamp_ri8);
}

hid_t HDF5Types::NativeDataType(FMLString<16> const & d)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_string_fmlstr16);
}
hid_t HDF5Types::NativeDataType(FMLString<32> const & d)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_string_fmlstr32);
}
hid_t HDF5Types::NativeDataType(FMLString<64> const & d)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_string_fmlstr64);
}
hid_t HDF5Types::NativeDataType(FMLString<128> const & d)
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();
  return (hdf5_string_fmlstr128);
}

hid_t HDF5Types::VLStringDataType()
{
  if ( ! fg_hdf5_init_types_done ) HDF5Types::P_doInit();  
  return  (hdf5_vl_string);
}

  
string HDF5Types::DataTypeToTypeString(hid_t ityp)
{
  /*
  switch (ityp) {
  case TBYTE :
    return "uint_1";
    break;
  case TSHORT :
    return "int_2";
    break;
  case TUSHORT :
    return "uint_2";
    break;
  case TINT :
    if (sizeof(int) == 4)  return "int_4";
    else if (sizeof(int) == 8)  return "int_8";
    else if (sizeof(int) == 2)  return "int_2";
    break;
  case TUINT :
    if (sizeof(int) == 4)  return "uint_4";
    else if (sizeof(int) == 8)  return "uint_8";
    else if (sizeof(int) == 2)  return "uint_2";
    break;
  case TINT32BIT :  
    return "int_4";
    break;
  case TULONG :
    if (sizeof(long) == 4)  return "uint_4";
    else if (sizeof(long) == 8)  return "uint_8";
    else if (sizeof(long) == 2)  return "uint_2";
    break;
#ifdef TLONGLONG
  case TLONGLONG :
    return "int_8";
    break;
#endif
  case TFLOAT :
    return "r_4";
    break;
  case TDOUBLE :
    return "r_8";
    break;
  case TCOMPLEX : 
    return "complex< r_4 >";
    break;
  case TDBLCOMPLEX : 
    return "complex< r_8 >";
    break;
  case TSTRING : 
    return "string";
    break;
  default:
    return "???" ;
    break;
  }
*/
  return "";
}

//----------------------------------------------------------
//------- Classe SOH5DataConverter<T> generique 
//----------------------------------------------------------
template <class T>
SOH5DataConverter<T>::SOH5DataConverter(size_t sz) 
: fgneedconversion_(false), sz_(sz) , ch5stp_(NULL) , data_(NULL), cstdata_(NULL)
{ 
}
template <class T>
SOH5DataConverter<T>::~SOH5DataConverter() 
{ 
} 
template <class T>
const void * SOH5DataConverter<T>::toHDF5()
{
  return (const void *)(cstdata_);
}

template <class T>
void SOH5DataConverter<T>::fromHDF5()
{
  return;
}


//----------------------------------------------------------
//------- Specialisation de SOH5DataConverter<T> 
//----------------------------------------------------------

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
SOH5DataConverter<TimeStamp>::SOH5DataConverter(size_t sz)
: fgneedconversion_(true), sz_(sz) , ch5stp_(NULL) 
{
  sz_=sz;
  ch5stp_ = (void *)(new timestamp_r8_i8[sz_]);  // timestamp_r8_i8 : structure c (paire r_8, int_8) qui correspond a TimeStamp
  //DBG  cout << " ---- DEBUG SOH5DataConverter<TimeStamp>::SOH5DataConverter(sz="<<sz<<")  ch5stp_ = "<<(unsigned long)ch5stp_;
  //DBG  cout << " ... needsConversion() ? = "<<(needsConversion()?"TRUE":"FALSE")<<endl;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
SOH5DataConverter<TimeStamp>::~SOH5DataConverter()
{
  timestamp_r8_i8 * tmp = (timestamp_r8_i8 *)ch5stp_;
  //DBG  cout << " ---- DEBUG SOH5DataConverter<TimeStamp>::~SOH5DataConverter() deleting tmp="<<(unsigned long)tmp<<endl;
  delete[] tmp;
}

/* typedef struct {  r_8 seconds; int_8 days;  } timestamp_r8_i8;
   void Set(int_8 days, r_8 seconds);  DaysPart()    SecondsPart()  */

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
const void * SOH5DataConverter<TimeStamp>::toHDF5()
{
  timestamp_r8_i8 * tmp = (timestamp_r8_i8 *)ch5stp_;
  for(size_t i=0; i<sz_; i++) {
    tmp[i].days=cstdata_[i].DaysPart();  tmp[i].seconds=cstdata_[i].SecondsPart();   
  }
  return (const void *)(ch5stp_);
}



DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void SOH5DataConverter<TimeStamp>::fromHDF5()
{
  timestamp_r8_i8 * tmp = (timestamp_r8_i8 *)ch5stp_;
  for(size_t i=0; i<sz_; i++) data_[i].Set(tmp[i].days, tmp[i].seconds); 
  //DBG  cout << " ---- DEBUG SOH5DataConverter<TimeStamp>::fromHDF5() data[0]=" <<data_[0]<<endl;

  return;
}

///////////////////////////////////////////////////////////////
#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template SOH5DataConverter<int_1>
#pragma define_template SOH5DataConverter<uint_1>
#pragma define_template SOH5DataConverter<int_2>
#pragma define_template SOH5DataConverter<uint_2>
#pragma define_template SOH5DataConverter<int_4>
#pragma define_template SOH5DataConverter<uint_4>
#pragma define_template SOH5DataConverter<int_8>
#pragma define_template SOH5DataConverter<uint_8>
#pragma define_template SOH5DataConverter<r_4>
#pragma define_template SOH5DataConverter<r_8>
#pragma define_template SOH5DataConverter< complex<r_4> >
#pragma define_template SOH5DataConverter< complex<r_8> >
#ifdef  SO_LDBLE128
#pragma define_template SOH5DataConverter<r_16>
#pragma define_template SOH5DataConverter< complex<r_16> >
#endif

#pragma define_template SOH5DataConverter< FMLString<16> >
#pragma define_template SOH5DataConverter< FMLString<32> >
#pragma define_template SOH5DataConverter< FMLString<64> >
#pragma define_template SOH5DataConverter< FMLString<128> >

#pragma define_template SOH5DataConverter<TimeStamp>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class SOH5DataConverter<int_1>; 
template class SOH5DataConverter<uint_1>; 
template class SOH5DataConverter<int_2>; 
template class SOH5DataConverter<uint_2>; 
template class SOH5DataConverter<int_4>; 
template class SOH5DataConverter<uint_4>; 
template class SOH5DataConverter<int_8>; 
template class SOH5DataConverter<uint_8>; 
template class SOH5DataConverter<r_4>; 
template class SOH5DataConverter<r_8>; 
template class SOH5DataConverter< complex<r_4> >; 
template class SOH5DataConverter< complex<r_8> >; 
#ifdef  SO_LDBLE128
template class SOH5DataConverter<r_16>; 
template class SOH5DataConverter< complex<r_16> >; 
#endif

template class SOH5DataConverter< FMLString<16> >; 
template class SOH5DataConverter< FMLString<32> >; 
template class SOH5DataConverter< FMLString<64> >; 
template class SOH5DataConverter< FMLString<128> >; 

template class SOH5DataConverter<TimeStamp>; 
#endif

} // Fin du namespace SOPHYA
