/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  R. Ansari ,     2016-2018  
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#ifndef HDF5SEGDBHAND_H_SEEN
#define HDF5SEGDBHAND_H_SEEN

#include "machdefs.h"
#include <string>
#include "segdatablock.h"
#include "hdf5handler.h"


namespace SOPHYA {

/*! 
  \ingroup HDF5IOServer
  \brief HDF5 I/O handler for segmented data blocks SegDBInterface<T> 

  In memory object SegDataBlock<T> is created when reading object from file when no SegDBInterface<T> is specified 
*/

template <class T>
class HDF5SegDBHandler : public HDF5HandlerInterface {
public :
  HDF5SegDBHandler() { dobj=NULL; ownobj=true; }
  HDF5SegDBHandler(SegDBInterface< T > & obj) { dobj = &obj; ownobj=false; }
  virtual   ~HDF5SegDBHandler() { if (ownobj && dobj) delete dobj; }
  virtual   AnyDataObj* DataObj() { return(dobj); }

  virtual   int         CheckHandling(AnyDataObj const & o) const 
  {  
    if (typeid(o) == typeid(SegDBInterface<T>))  return 2;
    SegDBInterface<T> const * po = dynamic_cast< SegDBInterface<T> const * >(& o); 
    if (po == NULL) return 0;
    else return 1;
  }

  virtual   void        SetDataObj(AnyDataObj & o) 
  {  
    SegDBInterface<T> * po = dynamic_cast< SegDBInterface<T> * >(& o); 
    if (po == NULL)  {
      string msg = "HDF5SegDBHandler<T>::SetDataObj() Wrong object type: " ;
      msg += typeid(o).name(); 
      throw TypeMismatchExc(msg);    
    }
    if (ownobj && dobj) delete dobj;  dobj = po; ownobj = false; 
  }

  virtual int         CheckReadability(HDF5InOutFile& is, const char* objpath)  const 
  {
    HDF5IdWrapper datatype;
    HDF5IdWrapper dataspace;
    DVList dvl;
    H5O_type_t otype=is.GetObjectType_Info(objpath, dvl, datatype, dataspace);
    if (otype != H5O_TYPE_DATASET)  return 0;
    // Setting array dimensions
    int rank = dataspace.getDSRank();
    if (rank != 2)  {
      //      cout <<"HDF5SegDBHandler<T>::CheckReadability() dataspace rank="<<rank<<"!=2 --> can not read"<<endl;
      return 0;
    }
    hsize_t sz[2]={0,0};
    dataspace.getDSSizes(sz);
    size_t fullsz=sz[0]*sz[1];
    if (fullsz<1) {
      //      cout <<"HDF5SegDBHandler<T>::CheckReadability() dataspace fullsize="<<fullsz<<" <=0 --> can not read"<<endl;
      return 0;
    }
    T x;
    hid_t dtm = HDF5Types::DataType(x);
    bool fgsd = datatype.IsSameDataType(dtm);
    bool fgok=false;
    string clsname=dvl.GetS("SOPHYA_HDF5Handler_ClassName");
    if (clsname=="SOPHYA::HDF5SegDBHandler<T>")  fgok=true;
    if (fgok) {
      if (fgsd) return 3;
      else return 2;
    }
    else if (fgsd) return 2;
    return 1;
  }

  virtual HDF5HandlerInterface* Clone() const 
  { return new HDF5SegDBHandler< T >(); }

  inline operator T&() { return(*dobj); }

  //----- Ecriture
  //! Writes the complete array as an IMAGE HDU
  virtual void     Write(HDF5InOutFile& ohs, const char* name)  const 
  {
    if (( dobj == NULL) || (dobj->Size() < 1))  
      throw NullPtrError("HDF5SegDBHandler<T>::Write() dobj=NULL or dobj->Size()=0");

    // --- for converting SOPHYA classes (such as TimeStamp) mapped to some c-structures for HDF5 files 
    SOH5DataConverter<T> converter(dobj->SegmentSize());
    
    /* Note that in HDF5 convention, the LAST dimension is the FASTEST changing, contiguous in memory. 
       int A[N1][N2][N3] --> HDF5 data set with size[0]=N1, size[1]=N2, size[2]=N3 
       correspond to a Sophya array TArray<int>(N3,N2,N1)     */
    // -- creating the dataset
    hsize_t sizes[BASEARRAY_MAXNDIMS] = {0,0};
    sizes[1]=(hsize_t)dobj->SegmentSize();
    sizes[0]=(hsize_t)dobj->NbSegments();
    hsize_t chunksz[2] = {1,1};
    chunksz[1]=dobj->SegmentSize();
    chunksz[0]=1;
    size_t nchunk=dobj->NbSegments();
    while (nchunk>max_nchunk_) {    // In order to avoid too large number of chunks ...
      chunksz[1]*=2;  nchunk/=2;
    }
    T x;
    hid_t dtyp = HDF5Types::DataType(x, ohs.getDefaultByteOrdering());
    HDF5IdWrapper dataset = ohs.CreateChunkedDataSet(name, dtyp, 2, sizes, chunksz);

    MuTyV mtv=string("SOPHYA::HDF5SegDBHandler<T>");
    ohs.WriteAttribute(dataset,"SOPHYA_HDF5Handler_ClassName",mtv);
    SegDataBlock<T> const * sdbp = dynamic_cast< SegDataBlock<T> const * >(dobj);
    if (sdbp)  mtv=string("SOPHYA::SegDataBlock<T>");
    else mtv=string("SOPHYA::SegDBInterface<T>");
    ohs.WriteAttribute(dataset,"SOPHYA_ClassName",mtv);
    if (converter.needsConversion()) {
      string s = "Written with conversion from class T=";
      s += typeid(x).name();
      ohs.WriteAttribute(dataset,"SOPHYA_ConversionFromClass",mtv);
    }
    mtv = (int_8)dobj->SegmentSize();
    ohs.WriteAttribute(dataset,"SOPHYA_SegDB_SegmentSize",mtv);
    mtv = (int_8)dobj->NbSegments();
    ohs.WriteAttribute(dataset,"SOPHYA_SegDB_NbSegments",mtv);
    mtv = (int_8)dobj->NbItems();
    ohs.WriteAttribute(dataset,"SOPHYA_SegDB_NbItems",mtv);

    // writing array data 
    hsize_t segsz[2] = {1,1};
    segsz[1]=dobj->SegmentSize();
    segsz[0]=1;
    hsize_t offset[2] = {0,0};  
    for(size_t k=0; k<dobj->NbSegments(); k++) {
      offset[0]=k;
      converter.toHDF5_setData(dobj->GetCstSegment(k));
      ohs.DataSetWrite(dataset,offset,segsz,HDF5Types::NativeDataType(x),converter.toHDF5());
    } 
    return;
  }

  //----- Lecture
  //! Resize the array and reads the complete IMAGE HDU to the array
  virtual void     Read(HDF5InOutFile& ihs, const char* objpath)
  {
    H5O_type_t otype=ihs.GetObjectType(objpath);
    if (otype != H5O_TYPE_DATASET)  throw HDF5IOException("HDF5SegDBHandler<T>::Read() HDF5 object not a DataSet");
    HDF5IdWrapper datatype;
    HDF5IdWrapper dataspace;
    HDF5IdWrapper dataset = ihs.OpenDataSet(objpath, datatype, dataspace);
    T x;
    hid_t rdmemtyp=HDF5Types::NativeDataType(x);
    hid_t dto = HDF5Types::DataType(x);
    bool fg1 = datatype.IsSameDataType(dto);
    bool fg2 = (fg1?true:datatype.IsCompatibleDataType(dto));
    if (!fg1 && !fg2)
      throw TypeMismatchExc("HDF5SegDBHandler<T>::Read() / Incompatible datatypes between file and object");

    hid_t fcdtyp=H5I_INVALID_HID;
    if (datatype.IsCompound()) {
      if (!fg1) 
	throw TypeMismatchExc("HDF5SegDBHandler<T>::Read()/Compound type in file, NOT SAME as DataType(T)"); 
      //DBG      cout << " *DBG* HDF5SegDBHandler<T> calling HDF5IdWrapper::getCompatibleDataTypeForCompound - T="<<typeid(T).name()<<endl;
      fcdtyp=HDF5IdWrapper::getCompatibleDataTypeForCompound(rdmemtyp, datatype.get_hid());
      rdmemtyp=fcdtyp;
    }

    if (!fg1 && fg2) {
      cerr << "HDF5SegDBHandler<T>::Read()/Warning: reading data from file with conversion for dataset name="
	   <<objpath<<endl;
      //      cerr << " dataset name="<<objpath<<" object data type T="<<DataTypeInfo<T>::getTypeName()<<endl;
    }
    // Setting array dimensions
    /* Note that in HDF5 convention, the LAST dimension is the FASTEST changing, contiguous in memory. 
       int A[N1][N2][N3] --> HDF5 data set with size[0]=N1, size[1]=N2, size[2]=N3 
       correspond to a Sophya array TArray<int>(N3,N2,N1)     */
    int rank = dataspace.getDSRank();
    if (rank != 2)  
      throw HDF5IOException("HDF5SegDBHandler<T>::Read() dataset rank != 2");
    hsize_t sz[2]={0,0};
    dataspace.getDSSizes(sz);
    size_t fullsz = sz[0]*sz[1];
    if (fullsz < 1)  throw HDF5IOException("HDF5SegDBHandler<T>::Read() dataset full size <= 0");
    DVList dvl;
    ihs.ReadAttributes(dataset, dvl);
    size_t nitem=1;
    string clsname=dvl.GetS("SOPHYA_HDF5Handler_ClassName");
    if (clsname == "SOPHYA_HDF5Handler_ClassName") 
      nitem=dvl.GetI("SOPHYA_SegDB_NbItems",1);
    if ( dobj == NULL )  {
      dobj = new SegDataBlock<T>(sz[1],sz[0],nitem) ;
      ownobj = true;
    }
    else dobj->SetSize(sz[1],sz[0],nitem);
    // Reading array data 
    hsize_t segsz[2] = {1,1};
    segsz[1]=dobj->SegmentSize();
    segsz[0]=1;
    hsize_t offset[2] = {0,0};  
    hid_t dtm = HDF5Types::NativeDataType(x);
    // --- for converting SOPHYA classes (such as TimeStamp) mapped to some c-structures for HDF5 files 
    SOH5DataConverter<T> converter(dobj->SegmentSize());
    if (converter.needsConversion())  {  // mapped SOPHYA c++ class : we need to convert to the correspond
      void * memdp = converter.getH5structPtr();
      for(size_t k=0; k<dobj->NbSegments(); k++) {
	offset[0]=k;
	ihs.DataSetRead(dataset,offset,segsz,rdmemtyp,memdp);
	converter.fromHDF5_setData(dobj->GetSegment(k));
	converter.fromHDF5();
      }
    }
    else {
      for(size_t k=0; k<dobj->NbSegments(); k++) {
	void * memdp = dobj->GetSegment(k);
	offset[0]=k;
	ihs.DataSetRead(dataset,offset,segsz,rdmemtyp,memdp);
      } 
    }
    HDF5IdWrapper::closeDataTypeForCompound(fcdtyp);
    return;
  }
  
protected :
  SegDBInterface<T>  * dobj;
  bool ownobj;       // True si dobj obtenu par new
};



/*! 
  \brief operator << overload to write a SegDBInterface<T> to a HDF5InOutFile file wrapper object 
  
  Use the HDF5NameTag object (push it to the HDF5InOutFile stream) to define the dataset name in the hdf5 file 
*/
template <class T>
inline HDF5InOutFile& operator << (HDF5InOutFile& os,  SegDBInterface<T> const & sdb) 
{ 
  HDF5SegDBHandler<T> fio(const_cast<SegDBInterface<T> &>(sdb));  
  string san=os.GetNextObjectCreateName(); 
  fio.Write(os,san.c_str());  
  return os; 
}

/*! 
  \brief operator >> overload to read a SegDBInterface<T> from a HDF5InOutFile file wrapper object 

  Use the HDF5NameTag object (pull it from the HDF5InOutFile stream) to define the dataset name to be read 
*/
template <class T>
inline HDF5InOutFile& operator >> (HDF5InOutFile& is,  SegDBInterface<T> & sdb)
{ 
  HDF5SegDBHandler<T> fio(sdb);  
  string san=is.GetNextObjectOpenName(); 
  fio.Read(is, san.c_str());  
  return(is); 
}


} // Fin du namespace SOPHYA 

#endif   /* HDF5SEGDBHAND_H_SEEN */

