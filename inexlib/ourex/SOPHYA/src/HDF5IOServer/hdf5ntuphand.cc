/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  HDF5 IO Handler for NTuple objects 
  Ansari , 2016-2018
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#include "hdf5handler.h"
#include "ntuple.h"


/*! 
  \ingroup HDF5IOServer
  \brief HDF5 I/O handler for NTuple objects
*/


namespace SOPHYA {

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
int HDF5Handler<NTuple>::CheckReadability(HDF5InOutFile& is, const char* objpath)  const 
{
  HDF5IdWrapper datatype;
  HDF5IdWrapper dataspace;
  DVList dvl;
  H5O_type_t otype=is.GetObjectType_Info(objpath, dvl, datatype, dataspace);
  if (otype != H5O_TYPE_DATASET)  return 0;
  bool fgok=false;
  string clsname=dvl.GetS("SOPHYA_HDF5Handler_ClassName");
  if (clsname=="SOPHYA::HDF5Handler<NTuple>")   fgok=true;
  else {
    clsname=dvl.GetS("SOPHYA_ClassName");
    if (clsname=="SOPHYA::NTuple")   fgok=true;
  }
  if (fgok) return 3;
  return 0;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void HDF5Handler<NTuple>::Write(HDF5InOutFile& ohs, const char* name)  const 
{
  if (dobj == NULL)  
    throw NullPtrError("HDF5Handler<NTuple>::Write() dobj=NULL ");
  if ((dobj->NCols()<1)||(dobj->NRows()<1))
    throw HDF5IOException("HDF5Handler<NTuple>::Write()  Empty NTuple");
  // -- creating the dataset 
  hsize_t sizes[2] = {1,1};
  sizes[1]=dobj->NCols();
  sizes[0]=dobj->BlockSize()*dobj->mNBlk;
  hsize_t chunksz[2] = {1,1};
  chunksz[1]=dobj->NCols();
  chunksz[0]=dobj->BlockSize();
  size_t nchunk=dobj->mNBlk;
  while (nchunk>max_nchunk_) {    // In order to avoid too large number of chunks ...
    chunksz[0]*=2;  nchunk/=2;
  }
  r_4 x4;  r_8 x8;
  hid_t dtyp=HDF5Types::DataType(x4, ohs.getDefaultByteOrdering());
  if (dobj->IsDoublePrecision())  dtyp=HDF5Types::DataType(x8, ohs.getDefaultByteOrdering());
  HDF5IdWrapper dataset = ohs.CreateChunkedDataSet(name, dtyp, 2, sizes, chunksz);
  //DBG  cout << "AAA*DBG*HDF5Handler<NTuple>::Write()  ohs.CreateChunkedDataSet() DONE chunksz[1]="<<chunksz[1]<<endl;
  MuTyV mtv=string("SOPHYA::HDF5Handler<NTuple>");
  ohs.WriteAttribute(dataset,"SOPHYA_HDF5Handler_ClassName",mtv);
  mtv=string("SOPHYA::NTuple");
  ohs.WriteAttribute(dataset,"SOPHYA_ClassName",mtv);
  mtv=(int_4)(dobj->mNVar);
  ohs.WriteAttribute(dataset,"SOPHYA_NTuple_NVAR",mtv);
  mtv=(int_4)(dobj->mNEnt);
  ohs.WriteAttribute(dataset,"SOPHYA_NTuple_NEnt",mtv);
  mtv=(int_4)(dobj->mBlk);
  ohs.WriteAttribute(dataset,"SOPHYA_NTuple_Blk",mtv);
  mtv=(int_4)(dobj->mNBlk);
  ohs.WriteAttribute(dataset,"SOPHYA_NTuple_NBlk",mtv);
  for(int i=0; i<dobj->NCols(); i++) {
    char buff[64];
    sprintf(buff,"SOPHYA_NTuple_ColName_%d",i);
    mtv=dobj->mNames[i];
    ohs.WriteAttribute(dataset,buff,mtv);  
  }
  //DBG  cout << "BBB*DBG*HDF5Handler<NTuple>::Write()  ohs.WriteAttribute()  DONE"<<endl;

  hid_t mtyp=HDF5Types::NativeDataType(x4);
  if (dobj->IsDoublePrecision())  mtyp=HDF5Types::NativeDataType(x8);

  hsize_t blksz[2] = {1,1};
  blksz[1]=dobj->NCols();
  blksz[0]=dobj->BlockSize();
  hsize_t offset[2] = {0,0};  
  for(int jb=0; jb<dobj->mNBlk; jb++) {
    void * memdp = ((dobj->mFgDouble)?(void *)dobj->mPtrD[jb]:(void *)dobj->mPtr[jb]);
    offset[0]=jb*dobj->mBlk;
    ohs.DataSetWrite(dataset,offset,blksz,mtyp,memdp);
  } 
  //DBG  cout << "CCCCC*DBG*HDF5Handler<NTuple>::Write()  ohs.DataSetWrite(dataset)  DONE"<<endl;
  if (dobj->mInfo) 
    ohs.WriteAttributes(dataset, *(dobj->mInfo));
  return;
}


DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void HDF5Handler<NTuple>::Read(HDF5InOutFile& ihs, const char* objpath)
{
  H5O_type_t otype=ihs.GetObjectType(objpath);
  if (otype != H5O_TYPE_DATASET) 
    throw HDF5IOException("HDF5Handler<NTuple>::Read() ERROR HDF5 object not a DataSet");
  HDF5IdWrapper datatype;
  HDF5IdWrapper dataspace;
  HDF5IdWrapper dataset = ihs.OpenDataSet(objpath, datatype, dataspace);
  DVList dvl;
  ihs.ReadAttributes(dataset, dvl);
  string clsname=dvl.GetS("SOPHYA_ClassName");
  if (clsname!="SOPHYA::NTuple") 
    throw HDF5IOException("HDF5Handler<NTuple>::Read() ERROR No SOPHYA classname or classname != NTuple");
  int rank = dataspace.getDSRank();
  if (rank != 2)  
    throw HDF5IOException("HDF5Handler<NTuple>::Read() ERROR dataset rank != 2");
  hsize_t sizes[2] = {0,0};
  dataspace.getDSSizes(sizes);
  int_4 nvar=dvl.GetI("SOPHYA_NTuple_NVAR");
  int_4 nent=dvl.GetI("SOPHYA_NTuple_NEnt");
  int_4 blk=dvl.GetI("SOPHYA_NTuple_Blk");
  int_4 nblk=dvl.GetI("SOPHYA_NTuple_NBlk");
  //DBG  cout << "AAA*DBG*HDF5Handler<NTuple>::Read()  DONE NEntries="<<nent<<endl;

  size_t sz2 = blk*nblk;
  if ( (nvar != (int_4)sizes[1])|| (sz2 != (size_t)sizes[0]) )
    throw HDF5IOException("HDF5Handler<NTuple>::Read() ERROR dataset sizes incompatible with nvar / blk*nblk");

  r_4 x4;  r_8 x8;
  hid_t dt4=HDF5Types::DataType(x4);
  hid_t dt8=HDF5Types::DataType(x8);
  bool fg4 = datatype.IsSameDataType(dt4);
  bool fg8 = datatype.IsSameDataType(dt8);
  if (!fg4 && !fg8) 
    throw HDF5IOException("HDF5Handler<NTuple>::Read() ERROR dataset type not r_4 or r_8 ");
  bool fgdouble=false;
  if (fg8)  fgdouble=true;
  if ( dobj == NULL )  {
    dobj = new NTuple ;
    ownobj = true;
  }
  dobj->Clean();
  dobj->Initialize(nvar,blk,fgdouble);
  for(int i=0; i<nvar; i++) {
    char buff[64];
    sprintf(buff,"SOPHYA_NTuple_ColName_%d",i);
    dobj->mNames.push_back(dvl.GetS(buff));
  }

  hid_t mt4=HDF5Types::NativeDataType(x4);
  hid_t mt8=HDF5Types::NativeDataType(x8);

  dobj->mNEnt=nent;  dobj->mNBlk=nblk;
  hsize_t offset[2] = {0,0};  
  hsize_t blksz[2] = {0,0};
  blksz[1]=sizes[1];  blksz[0]=blk;
  if (fgdouble) {
    for(int jb=0; jb<dobj->mNBlk; jb++) {
      if (jb>0) dobj->mPtrD.push_back(new r_8[nvar*blk]);
      void * memdp = (void *)dobj->mPtrD[jb];
      offset[0]=jb*blk;
      ihs.DataSetRead(dataset,offset,blksz,mt8,memdp);
    }
  } 
  else {
    for(int jb=0; jb<dobj->mNBlk; jb++) {
      if (jb>0) dobj->mPtr.push_back(new r_4[nvar*blk]);
      void * memdp = (void *)dobj->mPtr[jb];
      offset[0]=jb*blk;
      ihs.DataSetRead(dataset,offset,blksz,mt4,memdp);
    }
  }
  dvl.DeleteKey("SOPHYA_HDF5Handler_ClassName");
  dvl.DeleteKey("SOPHYA_ClassName");
  dvl.DeleteKey("SOPHYA_NTuple_NVAR");
  dvl.DeleteKey("SOPHYA_NTuple_NEnt");
  dvl.DeleteKey("SOPHYA_NTuple_Blk");
  dvl.DeleteKey("SOPHYA_NTuple_NBlk");
  if (dvl.Size()>0)  dobj->Info()=dvl;
  return;
}

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template HDF5Handler<NTuple>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class HDF5Handler<NTuple>;
#endif

} // Fin du namespace SOPHYA 

