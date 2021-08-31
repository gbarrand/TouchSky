/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  R. Ansari , G. Barrand,    2016-2018  
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#ifndef HDF5ARRHAND_H_SEEN
#define HDF5ARRHAND_H_SEEN

#include "machdefs.h"
#include <string>
#include "tarray.h"
#include "tvector.h"
#include "cimage.h"
#include "datatype.h"

#include "hdf5handler.h"

namespace SOPHYA {

/*! 
  \ingroup HDF5IOServer
  \brief HDF5 I/O handler for array objects
*/

template <class T>
class HDF5ArrayHandler : public HDF5HandlerInterface {
public :
  HDF5ArrayHandler() { dobj=NULL; ownobj=true; }
  HDF5ArrayHandler(TArray< T > & obj) { dobj = &obj; ownobj=false; }
  virtual   ~HDF5ArrayHandler() { if (ownobj && dobj) delete dobj; }
  virtual   AnyDataObj* DataObj() { return(dobj); }

  virtual   int         CheckHandling(AnyDataObj const & o) const 
  {  
    if ( (typeid(o) == typeid(TArray<T>)) ||
	 (typeid(o) == typeid(TMatrix<T>)) ||
	 (typeid(o) == typeid(TVector<T>)) )  return 2;
    TArray<T> const * po = dynamic_cast< TArray<T> const * >(& o); 
    if (po == NULL) return 0;
    else return 1;
  }

  virtual   void        SetDataObj(AnyDataObj & o) 
  {  
    TArray<T> * po = dynamic_cast< TArray<T> * >(& o); 
    if (po == NULL)  {
      string msg = "HDF5ArrayHandler<T>::SetDataObj() Wrong object type: " ;
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
    if (rank > BASEARRAY_MAXNDIMS)  {
      cout <<"HDF5ArrayHandler<T>::CheckReadability() dataspace rank="<<rank<<" >BASEARRAY_MAXNDIMS="<<BASEARRAY_MAXNDIMS
	   <<" --> can not read"<<endl;
      return 0;
    }
    hsize_t sz[BASEARRAY_MAXNDIMS]={0,0,0,0,0};
    dataspace.getDSSizes(sz);
    size_t fullsz=1;
    for(int ir=0; ir<rank; ir++) {
      if (sz[ir]<1)  fullsz=0;
      else fullsz *= (size_t)sz[ir];
    }
    if (fullsz<1) {
      cout <<"HDF5ArrayHandler<T>::CheckReadability() dataspace fullsize="<<fullsz<<" <=0 --> can not read"<<endl;
      return 0;
    }
    T x;
    hid_t dtm = HDF5Types::DataType(x);
    bool fgsd = datatype.IsSameDataType(dtm);
    bool fgok=false;
    string clsname=dvl.GetS("SOPHYA_HDF5Handler_ClassName");
    if (clsname=="SOPHYA::HDF5ArrayHandler<T>")  fgok=true;
    else {
      clsname=dvl.GetS("SOPHYA_ClassName");
      if (clsname=="SOPHYA::TArray<T>")  fgok=true;
    }
    if (fgok) {
      if (fgsd) return 3;
      else return 2;
    }
    else if (fgsd) return 2;
    return 1;
  }

  virtual HDF5HandlerInterface* Clone() const 
  { return new HDF5ArrayHandler< T >(); }

  inline operator T&() { return(*dobj); }

  //----- Ecriture
  //! Writes the complete array as an IMAGE HDU
  virtual void     Write(HDF5InOutFile& ohs, const char* name)  const 
  {
    if (( dobj == NULL) || (dobj->Size() < 1))  
      throw NullPtrError("HDF5ArrayHandler<T>::Write() dobj=NULL or dobj->Size()=0");
    if (!(dobj->IsPacked()))
      throw NotAvailableOperation("HDF5ArrayHandler<T>::Write() HDF5 I/O NOT supported for unpacked arrays");

    // -- creating the dataset 
    hsize_t sizes[BASEARRAY_MAXNDIMS] = {0,0,0,0,0};
    /* Note that in HDF5 convention, the LAST dimension is the FASTEST changing, contiguous in memory. 
       int A[N1][N2][N3] --> HDF5 data set with size[0]=N1, size[1]=N2, size[2]=N3 
       correspond to a Sophya array TArray<int>(N3,N2,N1)     */
    int_4 iih=dobj->NbDimensions();
    for(int_4 i=0; i<dobj->NbDimensions(); i++) {
      iih--; sizes[iih] = (hsize_t)dobj->Size(i);
    }
    int rank=dobj->NbDimensions();
    T x;
    HDF5IdWrapper dataset = ohs.CreateDataSet(name, HDF5Types::DataType(x, ohs.getDefaultByteOrdering()), rank, sizes);
    MuTyV mtv=string("SOPHYA::HDF5ArrayHandler<T>");
    ohs.WriteAttribute(dataset,"SOPHYA_HDF5Handler_ClassName",mtv);
    mtv=string("SOPHYA::TArray<T>");
    Image<T> * pimg = dynamic_cast< Image<T> * >(dobj);
    if (pimg) mtv=string("SOPHYA::Image<T>");
    else {
      TVector<T> * pvec = dynamic_cast< TVector<T> * >(dobj); 
      if (pvec) mtv=string("SOPHYA::TVector<T>");
      else {
	TMatrix<T> * pmx = dynamic_cast< TMatrix<T> * >(dobj); 
	mtv=string("SOPHYA::TMatrix<T>");
      }
    }
    ohs.WriteAttribute(dataset,"SOPHYA_ClassName",mtv);
    // writing array data 
    ohs.FullDataSetWrite(dataset, HDF5Types::NativeDataType(x), dobj->Data());
    // writing the DVList Info() object as attributes 
    ohs.WriteAttributes(dataset, dobj->Info());
    return;
  }

  //----- Lecture
  //! Resize the array and reads the complete dataset to the array
  virtual void     Read(HDF5InOutFile& ihs, const char* objpath)
  {
    H5O_type_t otype=ihs.GetObjectType(objpath);
    if (otype != H5O_TYPE_DATASET)  throw HDF5IOException("HDF5ArrayHandler<T>::Read() HDF5 object not a DataSet");
    HDF5IdWrapper datatype;
    HDF5IdWrapper dataspace;
    HDF5IdWrapper dataset = ihs.OpenDataSet(objpath, datatype, dataspace);
    T x;
    hid_t rdmemtyp=HDF5Types::NativeDataType(x);
    hid_t dtm = HDF5Types::DataType(x);
    bool fg1 = datatype.IsSameDataType(dtm);
    bool fg2 = (fg1?true:datatype.IsCompatibleDataType(dtm));
    if (!fg1 && !fg2)
      throw TypeMismatchExc("HDF5ArrayHandler<T>::Read() / Incompatible datatypes between file and object");

    hid_t fcdtyp=H5I_INVALID_HID;
    if (datatype.IsCompound()) {
      if (!fg1) 
	throw TypeMismatchExc("HDF5ArrayHandler<T>::Read()/Compound type in file, NOT SAME as DataType(T)"); 
      fcdtyp=HDF5IdWrapper::getCompatibleDataTypeForCompound(rdmemtyp, datatype.get_hid());
      rdmemtyp=fcdtyp;
    }

    if (!fg1 && fg2) {
      cerr << "HDF5ArrayHandler<T>::Read()/Warning: reading data from file with conversion"<<endl;
      cerr << " dataset name="<<objpath<<" object data type T="<<DataTypeInfo<T>::getTypeName()<<endl;
    }
    // Setting array dimensions
    int rank = dataspace.getDSRank();
    if (rank > BASEARRAY_MAXNDIMS)
      throw HDF5IOException("HDF5ArrayHandler<T>::Read() dataset rank > max SOPHYA BASEARRAY_MAXNDIMS=5");
    hsize_t sz[BASEARRAY_MAXNDIMS]={0,0,0,0,0};
    dataspace.getDSSizes(sz);
    sa_size_t asz[BASEARRAY_MAXNDIMS]={0,0,0,0,0};
    /* Note that in HDF5 convention, the LAST dimension is the FASTEST changing, contiguous in memory. 
       int A[N1][N2][N3] --> HDF5 data set with size[0]=N1, size[1]=N2, size[2]=N3 
       correspond to a Sophya array TArray<int>(N3,N2,N1)     */
    size_t fullsz=1;
    int_4 irs=rank;
    for(int_4 ir=0; ir<rank; ir++) {
      if (sz[ir]<1)  fullsz=0;
      else { irs--; asz[irs] = sz[ir];    fullsz *= (size_t)sz[ir]; }
    }
    if (fullsz < 1)  throw HDF5IOException("HDF5ArrayHandler<T>::Read() dataset full size <= 0");
    if ( dobj == NULL )  {
      dobj = new TArray<T> ;
      ownobj = true;
    }
    dobj->SetSize(rank, asz);
    // Reading array data 
    ihs.FullDataSetRead(dataset, rdmemtyp, dobj->Data());
    // Reading attributes
    ihs.ReadAttributes(dataset, dobj->Info());
    dobj->Info().DeleteKey("SOPHYA_HDF5Handler_ClassName");
    dobj->Info().DeleteKey("SOPHYA_ClassName");
    HDF5IdWrapper::closeDataTypeForCompound(fcdtyp);
    return;
  }
  
protected :
  TArray<T>  * dobj;
  bool ownobj;       // True si dobj obtenu par new
};



/*! 
  \brief operator << overload to write a TArray<T> to a HDF5InOutFile file wrapper object 
  
  Use the HDF5NameTag object (push it to the HDF5InOutFile stream) to define the dataset name in the hdf5 file 
*/
template <class T>
inline HDF5InOutFile& operator << (HDF5InOutFile& os,  TArray<T> const & arr) 
{ 
  HDF5ArrayHandler<T> fio(const_cast<TArray<T> &>(arr));  
  string san=os.GetNextObjectCreateName(); 
  fio.Write(os,san.c_str());  
  return os; 
}

/*! 
  \brief operator >> overload to read a TArray<T> from a HDF5InOutFile file wrapper object 

  Use the HDF5NameTag object (pull it from the HDF5InOutFile stream) to define the dataset name to be read 
*/
template <class T>
inline HDF5InOutFile& operator >> (HDF5InOutFile& is,  TArray<T> & arr)
{ 
  HDF5ArrayHandler<T> fio(arr);  
  string san=is.GetNextObjectOpenName(); 
  fio.Read(is, san.c_str());  
  return(is); 
}


} // Fin du namespace SOPHYA 

#endif   /* HDF5ARRHAND_H_SEEN */

