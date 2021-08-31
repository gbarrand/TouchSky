//-----------------------------------------------------------
// Classe DataTable
//    in memory implementation of BaseDataTable  
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari      UPS+LAL IN2P3/CNRS  2005-2013
//-----------------------------------------------------------

// Class DataTable  (Row-Column data table in memory) 
//   R. Ansari -  Avril 2005
//   (C)  LAL-IN2P3/CNRS    CEA-DAPNIA  

#ifndef DATATABLE_H_SEEN
#define DATATABLE_H_SEEN

#include "basedtable.h"
#include "objfio.h"


namespace SOPHYA {

//  Forward class declaration for Fits handler
template <class T>  class FitsHandler;

//! An implementation of BaseDataTable with data (columns) stored in memory.
class DataTable : public BaseDataTable {
public:
  DataTable(size_t segsz=512); 
  DataTable(DataTable const& a, bool share=true); 
  
  virtual size_t AddColumn(FieldType ft, std::string const & cnom, size_t vsz=1, bool fgfcnet=false);

  //! Equal (copy) operator - Copies the data and the structure if necessary from \b a   
  inline DataTable&  operator = (BaseDataTable const& a) 
  { CopyFrom(a) ; return *this ; } 
  //! Equal (copy) operator - Copies the data and the structure if necessary from \b a   
  inline DataTable& operator = (DataTable const& a) 
  { CopyFrom(a) ; return *this ; } 

  //  Pour la gestion de persistance PPF
  friend class ObjFileIO<BaseDataTable> ;
  // pour fichiers FITS
  friend class FitsHandler<BaseDataTable>;

  //! Reset the table content and structure
  virtual void Clear();
protected:
  void Share(DataTable const & a);
  void Clone(DataTable const & a);

  // Donnees en memoire
  std::vector< SegDataBlock<int_4> > mICols; 
  std::vector< SegDataBlock<int_8> > mLCols; 
  std::vector< SegDataBlock<r_4> > mFCols;
  std::vector< SegDataBlock<r_8> > mDCols; 
  std::vector< SegDataBlock< complex<r_4> > > mYCols;
  std::vector< SegDataBlock< complex<r_8> > > mZCols;
  std::vector< SegDataBlock< std::string > > mSCols; 
  std::vector< SegDataBlock< TimeStamp > > mTCols; 
  std::vector< SegDataBlock<char> > mCCols; 
  std::vector< SegDataBlock< String16 > > mS16Cols;
  std::vector< SegDataBlock< String64 > > mS64Cols;  
  std::vector< SegDataBlock< FlagVector16 > > mB16Cols;
  std::vector< SegDataBlock< FlagVector64 > > mB64Cols;                                  
}; 
  
/*! Writes the object in the POutPersist stream \b os */
inline POutPersist& operator << (POutPersist& os, DataTable & obj)
{ ObjFileIO<BaseDataTable> fio(&obj);  fio.Write(os);  return(os); }
/*! Reads the object from the PInPersist stream \b is */
inline PInPersist& operator >> (PInPersist& is, DataTable & obj)
{ ObjFileIO<BaseDataTable> fio(&obj); is.SkipToNextObject(); 
  fio.Read(is); return(is); }
// Classe pour la gestion de persistance
// ObjFileIO<BaseDataTable>
} // namespace SOPHYA

#endif

