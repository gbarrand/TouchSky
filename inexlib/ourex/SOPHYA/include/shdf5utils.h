/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  G. Barrand, R. Ansari , 2016-2018
  Small utility/wrapper classes for interface with HDF5 library 
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#ifndef SHDF5UTILS_H_SEEN
#define SHDF5UTILS_H_SEEN

#include "machdefs.h"
#include <complex>
#include <string>
#include <vector>

//#include "HDF5/hdf5.h"   // HDF5 library declaration include file  
#ifdef EXLIB_USE_NATIVE_HDF5
#include <hdf5.h>
#else
#include <ourex_hdf5.h>
#endif

#include "pexceptions.h"                                                       
#include "thsafeop.h"  //  for ThreadSafe operations (Ref.Count/Share)                                                                        
#include "fmlstr.h"  //  for fixed length strings
#include "timestamp.h"  //  for fixed length strings                                                                      


namespace SOPHYA {

/*! 
  \ingroup HDF5IOServer
  \brief Exception class used by  HDF5 library wrapper classes in HDF5IOserver module
*/
class HDF5IOException : public IOExc {
  public:
  explicit HDF5IOException(const char * m) throw() : IOExc(m) {}
  explicit HDF5IOException(const string& m) throw() : IOExc(m) {}
};

//------ Wrapper class for HDF5 handle/identifiers (hid_t)  ------
class HDF5IdWrapper {
public:
  //! return true if data type identifier correspond to a compound 
  static bool IsCompound(hid_t dt);
  //! return true if the two equal data type identifiers (call H5Tequal) 
  static bool AreEqualDataType(hid_t dt1, hid_t dt2);
  //! return true if the two datatype identifiers refer to the same datatype (same = same data class, size (and offset for compounds - no check on byte ordering) 
  static bool AreSameDataType(hid_t dt1, hid_t dt2);
  //! return true if the two datatype identifiers refer to compatible datatypes (same data class)
  static bool AreCompatibleDataType(hid_t dt1, hid_t dt2);
  //! return true if the two datatype identifiers refer to same (fgsame=true) or compatible datatypes (fgsame=false)
  static bool AreSameOrCompatibleDataType(hid_t dt1, hid_t dt2, bool fgsame);
  //! gives the string representation of an HDF5 type value (H5I_type_t)
  static const char * ConvH5Type2str(H5I_type_t  h5typ);
  //! return a suitable data type identifier for reading compatible type compounds (complex ... ) 
  static hid_t getCompatibleDataTypeForCompound(hid_t memdtyp, hid_t fdtyp);
  //! closes the data type identifier returned by getCompatibleDataTypeForCompound()
  static void closeDataTypeForCompound(hid_t fcdtyp);

  //! default constructor - No Id , NULL H5IDWREF  
  HDF5IdWrapper();
  //! The useful constructor, creates a wrapper from a hopefully valid hid_t
  HDF5IdWrapper(hid_t h5id, H5I_type_t typ);
  //! Copy constructor shares the internal pointer and increments reference count 
  HDF5IdWrapper(HDF5IdWrapper const & a);
  //! destructor : call the close() method (see  below)
  ~HDF5IdWrapper();
  //! Copy (equal) operator - shares the internal pointer and increments reference count 
  HDF5IdWrapper & operator = (HDF5IdWrapper const & a);
  //! return the HDF5 embedded hid_t handle 
  hid_t get_hid() const ;
  //! return the HDF5 embedded handle type 
  H5I_type_t get_type() const ;
  //! return the string representation of the embedded HDF5 handle type 
  const char * get_type_as_str() const; 
  //! return the number of references to the HDF5 Id 
  size_t getNRef() const;

  /*! \brief returns the datatype class, assuming the objects contains a datatype handle
    H5T_INTEGER   H5T_FLOAT H5T_COMPOUND H5I_DATASET H5I_GROUP  ... */
  inline H5T_class_t getDTClass() const
  {  return H5Tget_class(get_hid()); }
  //! returns the datatype size, assuming the objects contains a datatype handle
  inline size_t getDTSize() const
  {  return H5Tget_size(get_hid()); }
  //! returns the dataspace rank, assuming the objects contains a dataspace handle
  inline int getDSRank() const
  { return H5Sget_simple_extent_ndims(get_hid()); }
  //! returns the dataspace sizes along each dimension, assuming the objects contains a dataspace handle
  inline void getDSSizes(hsize_t * sz)  const
  {  H5Sget_simple_extent_dims(get_hid(), sz, NULL); return; }
  //! returns the dataspace sizes assuming the objects contains a dataspace handle
  std::vector<size_t> getDSSizes() const;
  //! returns the dataspace id, assuming that the object contains a dataset handle - the returned dataspace id should be closed after
  inline hid_t getDataSpaceId() const
  {  return  H5Dget_space(get_hid()); }

  //! return true if the datatype handle correspond to a compound (assuming object contains a datatype handle)
  inline bool IsCompound() const 
  { return IsCompound(get_hid()); }
  //! assuming the objects contains a datatype handle return true if they refer to identical datatypes (call H5Tequal)
  inline bool IsEqualDataType(hid_t dtid) const
  { return HDF5IdWrapper::AreEqualDataType(get_hid(), dtid); }
  //! assuming the objects contains a datatype handle return true if the identifier refers to the same datatype as \b dtid 
  inline bool IsSameDataType(hid_t dtid) const
  { return HDF5IdWrapper::AreSameDataType(get_hid(), dtid); }
  //! Assuming the objects contains a datatype handle return true if the identifier refers to a datatype compatible
  inline bool IsCompatibleDataType(hid_t dtid) const
  { return HDF5IdWrapper::AreCompatibleDataType(get_hid(), dtid); }
  //! assuming the objects contains a datatype handle return true if the identifier refers to the same datatype as \b dt
  inline bool IsEqualDataType(HDF5IdWrapper const & dt) const
  { return HDF5IdWrapper::AreEqualDataType(get_hid(), dt.get_hid()); }
  //! assuming the objects contains a datatype handle return true if the identifier refers to the same datatype as \b dt
  inline bool IsSameDataType(HDF5IdWrapper const & dt) const
  { return HDF5IdWrapper::AreSameDataType(get_hid(), dt.get_hid()); }
  /*! \brief Assuming the objects contains a datatype handle return true if the identifier refers to a datatype compatible
    with the one specified by \b dt */
  inline bool IsCompatibleDataType(HDF5IdWrapper const & dt) const
  { return HDF5IdWrapper::AreCompatibleDataType(get_hid(), dt.get_hid()); }
  
  //!  Prints the some information concerning the HDF5 object handle on cout 
  inline  void      Print() const  { Print(cout); }  
  //!  Prints the some information concerning the HDF5 object handle on stream \b os 
  void      Print(ostream& os)  const;
  
  //!  Initialisation - should be called before the first use of this class
  static void doInit();
protected:
  void share(HDF5IdWrapper const & a);
  void close();
  /*! \cond structure for reference sharing/counting to the hid_t handle */
  typedef struct { size_t nref_; hid_t h5id_; H5I_type_t h5typ_; }  H5IDWREF;
  H5IDWREF * mIdRef; 
};  

/*! operator << overloading - Prints some information about the HDF5 handle on stream \b os */
inline ostream& operator << (ostream& os, HDF5IdWrapper const & h5w)
  {  h5w.Print(os);  return(os);  }

//---------   HDF5Types class (converting c/c++ types to HDF5 Datatype -------------
class HDF5Types {
public:
  //! Byte ordering , Auto : ordering selected to correspond to the native format on the host, Little or Big Endian 
  enum ByteOrder { BO_Auto, BO_LittleEndian, BO_BigEndian };
 // Conversion de type en constante datatype HDF5
  static hid_t DataType(int_1 d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
  static hid_t DataType(uint_1 d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
  static hid_t DataType(int_2  d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
  static hid_t DataType(uint_2 d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
  static hid_t DataType(int_4  d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
  static hid_t DataType(uint_4 d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
  static hid_t DataType(int_8  d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto);
  static hid_t DataType(uint_8 d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto);
  static hid_t DataType(r_4    d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto);
  static hid_t DataType(r_8    d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto);
  static hid_t DataType(complex<r_4>  d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto);  
  static hid_t DataType(complex<r_8>  d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
#ifdef  SO_LDBLE128
  static hid_t DataType(r_16    d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto);
  static hid_t DataType(complex<r_16>  d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
#endif

  //--- TimeStamp 
  static hid_t DataType(TimeStamp const & d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 

  //--- Fixedlength strings FMLString<16>  <32>  <64>  <128>
  static hid_t DataType(FMLString<16> const & d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
  static hid_t DataType(FMLString<32> const & d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
  static hid_t DataType(FMLString<64> const & d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
  static hid_t DataType(FMLString<128> const & d, HDF5Types::ByteOrder bo=HDF5Types::BO_Auto); 
  
  //------------------NativeDataType--------------------
  //----------------------------------------------------
  static hid_t NativeDataType(int_1 d); 
  static hid_t NativeDataType(uint_1 d); 
  static hid_t NativeDataType(int_2  d); 
  static hid_t NativeDataType(uint_2 d); 
  static hid_t NativeDataType(int_4  d); 
  static hid_t NativeDataType(uint_4 d); 
  static hid_t NativeDataType(int_8  d);
  static hid_t NativeDataType(uint_8  d);
  static hid_t NativeDataType(r_4    d);
  static hid_t NativeDataType(r_8    d);
  static hid_t NativeDataType(complex<r_4>  d);  
  static hid_t NativeDataType(complex<r_8>  d); 
#ifdef  SO_LDBLE128
  static hid_t NativeDataType(r_16    d);
  static hid_t NativeDataType(complex<r_16>  d); 
#endif

  //--- TimeStamp 
  static hid_t NativeDataType(TimeStamp const & d); 

  //--- Fixedlength strings FMLString<16>  <32>  <64>  <128>
  static hid_t NativeDataType(FMLString<16> const & d); 
  static hid_t NativeDataType(FMLString<32> const & d); 
  static hid_t NativeDataType(FMLString<64> const & d); 
  static hid_t NativeDataType(FMLString<128> const & d); 

  //! return the datatype for a variable length string 
  static hid_t VLStringDataType();

  //! return the data type corresponding to an HDF5 type as a string 
  static string DataTypeToTypeString(hid_t ityp);

  //!  init function - should be called during initialisation 
  static void doInit();   // to create complex types 

protected:
  static void P_doInit();   // to create complex types 
};

/*! 
  \brief Handles data conversion between SOPHYA c++ class objects and corresponding HDF5 structures 
  
  Default implementation peform no conversion and uses directly the class pointers 
  Specific implementation are in file shdf5utils.cc for classes needing data conversion such as TimeStamp 

  For writing :
\code 
  HDF5InOutFile  h5s(...) ;
  SOH5DataConverter<T> converter(sz);
//--- For writing :
  converter.setData(const T * data);
  h5s.DataSetWrite(... converter.toHDF5());
//---  For reading :
  h5s.DataSetRead(... converter.getH5structPtr());
  converter.setData(T * data);
  converter.fromHDF5();
\endcode
\code 

*/
template <class T>
class SOH5DataConverter {
public:
  //! contructor, with specification of array size for conversion
  SOH5DataConverter(size_t sz);
  //  {  cout << "SOH5DataConverter<T>(size_t sz)/DBG"<<endl; }
  virtual ~SOH5DataConverter();
  //  { cout << "SOH5DataConverter<T>() desctructeur / DBG"<<endl; }
  //! return true if conversion is needed to map to/from HDF5 corresponding structures 
  inline bool needsConversion() { return fgneedconversion_; }
  //! define the data pointer for conversion to HDF5-mapped structure (used when toHDF5() is called) 
  inline void toHDF5_setData(const T * data) { cstdata_=data; }
  //! Convert to the HDF5 mapped structure : default implementation returns just the T data pointer as a void pointer 
  virtual const void * toHDF5();
  //! define the data pointer for conversion from HDF5-mapped structure (used when fromHDF5() is called 
  inline void fromHDF5_setData(T * data) { data_=data; }
  //! Fills the T objects from the  HDF5 mapped structure : default implementation does nothing  
  virtual void fromHDF5();
  //! return the pointer to HDF5 mapped structure used to perform conversion if applicable - Default implementation returns NULL
  inline void * getH5structPtr() { return ch5stp_; }
public:
  bool fgneedconversion_;
  size_t sz_; 
  void * ch5stp_;
  T * data_;
  const T * cstdata_;
};



//! return an HDF5 DataType id embedded  in an HDF5IdWrapper object 
template <class T>
inline HDF5IdWrapper WrapDataType(T d)
{
  hid_t dtyp = H5Tcopy(HDF5Types::DataType(d));
  return HDF5IdWrapper(dtyp, H5I_DATATYPE);
}

//! return an HDF5 native (memory) DataType id embedded  in an HDF5IdWrapper object 
template <class T>
inline HDF5IdWrapper WrapNativeDataType(T d)
{
  hid_t dtyp = H5Tcopy(HDF5Types::NativeDataType(d));
  return HDF5IdWrapper(dtyp, H5I_DATATYPE);
}
  
} // Fin du namespace  SOPHYA

#endif  /* SHDF5UTILS_H_SEEN */
