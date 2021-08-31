//-----------------------------------------------------------
// Class BaseDataTable
//    Interface class and common services implementation for 
//    data organised as row-column tables 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  2005, 2013 
//-----------------------------------------------------------

//   R. Ansari -  Avril 2005  - MAJ : 2013 

#include "basedtable.h"
#include <ctype.h>
#include "pexceptions.h"
#include "thsafeop.h"
#include <string.h> 
#include <stdio.h> 

using namespace std;

namespace SOPHYA {

/*!
   \class BaseDataTable
   \ingroup HiStats
   Base class for data tables. Each row (line) represent a record
   and the column contains a given data type. A column may contain 
   a vector of data with a fixed size vsz.
   The following data types are currently supported:
  \verbatim
  ----------------------------------------------------------------------------------------
  Integer : 32 bits signed integer (int_4)
  LongInteger : 64 bits signed integer (int_8)
  Float : single precision float (32 bits, r_4 )
  Double : double precision float (64 bits, r_8 )
  Complex : single precision complex number (2 x 32 bits, complex<r_4> )
  DoubleComplex : double precision complex number (2 x 64 bits, complex<r_8> )
  String : character string, variable unbound length (std::string) 
  DateTime : date/time (SOPHYA::TimeStamp)
  Character : character (8 bits, char)
  String16 : character string with a maximum length of 15 characters (SOPHYA::String16)
  String64 : character string with a maximum length of 63 characters (SOPHYA::String64)
  FlagVector16 : array of 16 flags (SOPHYA::FlagVector16)
  FlagVector64 : array of 64 flags (SOPHYA::FlagVector64)
  ----------------------------------------------------------------------------------------
  \endverbatim

  Data can be appended to the table row by row (AddRow()) and accessed row wise (GetRow()),
  or column wise (GetColumn()). Data can also be copied to the table column wise (FillColumn()).
  Selected rows and or column can be copied from another table (CopySelectedRows() CopySelectedRowColumns()). 
  Row-wise and column-wise data merging operations can be also performed between tables 
  (see CopyFrom() , RowMerge() and ColumnMerge()).
  
   \warning Thread safe fill operation can be activated using the method SetThreadSafe()
   Default mode is NON thread-safe fill.   

   
   \sa SOPHYA::MuTyV
   \sa SOPHYA::DataTableRow
   \sa SOPHYA::DataTableRowPtr
   \sa SOPHYA::BaseDataTable
*/

/*!
  if fgl == true , return LongForm string
  \verbatim
  ----------------------------------------------  
  FieldType         ShortForm     LongForm
  ----------------------------------------------  
  IntegerField          I         Integer
  LongField             L         Long Integer
  FloatField            F         Float
  DoubleField           D         Double
  ComplexField         Zx4        Complex
  DoubleComplexField   Zx8        DoubleComplex
  StringField           S         String
  DateTimeField         T         DateTime
  CharField             C         Character
  FMLStr16Field        S16        String16
  FMLStr64Field        S64        String64
  FlagVec16Field       B16        FlagVector16
  FlagVec64Field       B64        FlagVector64
  ----------------------------------------------  
  \endverbatim
*/
string BaseDataTable::ColTypeToString(FieldType ft, bool fgl)
{
  string rs;
  switch (ft) {
  case IntegerField :
    if (fgl) rs = "Integer";
    else rs = "I";
    break;
  case LongField :
    if (fgl) rs = "Long Integer";
    else rs = "L";
    break;
  case FloatField :
    if (fgl) rs = "Float";
    else rs = "F";
    break;
  case DoubleField :
    if (fgl) rs = "Double";
    else rs = "D";
    break;
  case ComplexField :
    if (fgl) rs = "Complex";
    else rs = "Zx4";    
    break;
  case DoubleComplexField :
    if (fgl) rs = "DoubleComplex";
    else rs = "Zx8";
    break;    
  case StringField :
    if (fgl) rs = "String";
    else rs = "S";
    break;
  case DateTimeField :
    if (fgl) rs = "DateTime";
    else rs = "T";
    break;
  case CharacterField :
    if (fgl) rs = "Character";
    else rs = "C";
    break;
  case FMLStr16Field :
    if (fgl) rs = "String16";
    else rs = "S16";
    break;
  case FMLStr64Field :
    if (fgl) rs = "String64";
    else rs = "S64";
    break;
  case FlagVec16Field :
    if (fgl) rs = "FlagVector16";
    else rs = "B16";
    break;
  case FlagVec64Field :
    if (fgl) rs = "FlagVector64";
    else rs = "B64";
    break;
  default:
    rs = "??";
    break;
  }
  return rs;
}

BaseDataTable::FieldType BaseDataTable::StringToColType(string const & sctyp)
{
  if ( (sctyp == "Integer") || (sctyp == "I") )  return IntegerField;
  else if ( (sctyp == "Long Integer") || (sctyp == "L") )  return LongField;
  else if ( (sctyp == "Float") || (sctyp == "F") )  return FloatField;
  else if ( (sctyp == "Double") || (sctyp == "D") )  return DoubleField;
  else if ( (sctyp == "Complex") || (sctyp == "Zx4") )  return ComplexField;
  else if ( (sctyp == "DoubleComplex") || (sctyp == "Zx8") || (sctyp == "Z") )  
    return DoubleComplexField;
  else if ( (sctyp == "String") || (sctyp == "S") )  return StringField;
  else if ( (sctyp == "DateTime") || (sctyp == "T") )  return DateTimeField;
  else if ( (sctyp == "Character") || (sctyp == "C") )  return CharacterField;
  else if ( (sctyp == "String16") || (sctyp == "S16") )  return FMLStr16Field;
  else if ( (sctyp == "String64") || (sctyp == "S64") )  return FMLStr64Field;
  else if ( (sctyp == "FlagVector16") || (sctyp == "B16") )  return FlagVec16Field;
  else if ( (sctyp == "FlagVector64") || (sctyp == "B64") )  return FlagVec64Field;
  else return DoubleField;
}

DataTableCellType BaseDataTable::Convert2DTCellType(FieldType ft)
{
  DataTableCellType rt;
  switch (ft) {
  case IntegerField :
    rt = DTC_IntegerType;
    break;
  case LongField :
    rt = DTC_LongType;
    break;
  case FloatField :
    rt = DTC_FloatType;
    break;
  case DoubleField :
    rt = DTC_DoubleType;
    break;
  case ComplexField :
    rt = DTC_ComplexType;
    break;
  case DoubleComplexField :
    rt = DTC_DoubleComplexType;
    break;    
  case StringField :
    rt = DTC_StringType;
    break;
  case DateTimeField :
    rt = DTC_DateTimeType;
    break;
  case CharacterField :
    rt = DTC_CharacterType;
    break;
  case FMLStr16Field :
    rt = DTC_FMLStr16Type;
    break;
  case FMLStr64Field :
    rt = DTC_FMLStr64Type;
    break;
  case FlagVec16Field :
    rt = DTC_FlagVec16Type;
    break;
  case FlagVec64Field :
    rt = DTC_FlagVec64Type;
    break;
  default:
    rt = DTC_StringType;
    break;
  }
  return rt;

}

/* Constructeur */
BaseDataTable::BaseDataTable(size_t segsz)
{
  mNEnt = 0;
  mSegSz = (segsz > 0) ? segsz : 16;
  mNSeg = 0;
  mVarD = NULL;
  mVarMTV = NULL;
  mVarMTP = NULL;
  mVarTMS = NULL;
  mVarStrP = NULL;
  mFgVecCol = false;
  mInfo = NULL;
  mThS = NULL;
  SetShowMinMaxFlag();
}

BaseDataTable::~BaseDataTable()
{
  if (mVarD) delete[] mVarD;
  if (mVarMTV) delete[] mVarMTV;
  if (mVarMTP) delete[] mVarMTP;
  if (mVarTMS) delete[] mVarTMS;
  if (mVarStrP) delete[] mVarStrP;
  if (mInfo) delete mInfo;
  if (mThS) delete mThS;
}

/*!
  \brief Activate or deactivate thread-safe \b AddRow() operations

  If fg==true, create an ThSafeOp object in order to insure atomic AddRow()
  and GetRow()/GetColumn() operations. if fg==false, the ThSafeOp object (mThS) 
  of the target DataTable is destroyed.

  When activated, the following operations are thread-safe : 

  - AddRow(const r_8* data)
  - AddRow(const MuTyV* data)
  - AddRow(DataTableRow const& data)
  - FillColumn(size_t k, vector<T> const& v)
  - GetRow(size_t n, DataTableRow& row)
  - GetRow(size_t n, MuTyV* mtvp)
  - GetRowPtr(size_t n, DataTableRowPtr& rowp)
  - GetCstRowPtr(size_t n, DataTableRowPtr& rowp)
  - GetColumn(size_t k, vector<T> & v)
  - GetColumnD(size_t k)

  \warning The default AddRow() operation mode for DataTables is NOT thread-safe. 
  Note also that the thread-safety state is NOT saved to PPF (or FITS) streams.
*/
void BaseDataTable::SetThreadSafe(bool fg) 
{
  if (fg) {
    if (mThS) return;
    else mThS = new ThSafeOp();
  }
  else {
    if (mThS) delete mThS;
    mThS = NULL;
  }
}


/*! \cond
  \class DT_TSOP_SYNC 
  \ingroup HiStats 
  Classe utilitaire pour faciliter la gestion de lock pour 
  operations thread-safe BaseDataTable
*/
class DT_TSOP_SYNC {
 public:
  explicit DT_TSOP_SYNC(ThSafeOp* ths) 
  { 
    ths_ = ths; 
    if (ths_) ths_->lock(); 
  }
  ~DT_TSOP_SYNC() 
  {
    if (ths_) ths_->unlock(); 
  }
  inline ThSafeOp* NOp() { return ths_; }
 protected:
  ThSafeOp* ths_;
};
/*! \endcond */

/*
  If the first table (*this) has already columns, a ParmError exception is generated. Otherwise, 
  the table (*this) structure, is copied from \b a table. The structure corresponds to 
  the table column names, types and vector size, as well as column units where applicable. 
*/
size_t BaseDataTable::CopyStructure(BaseDataTable const & a)
{
  if (NVar() > 0) 
    throw ParmError("BaseDataTable::CopyStructure() Table already has columns");
  if (a.NVar() == 0) {
    cout << "BaseDataTable::CopyStructure(a)/Warning Table a is not initialized" << endl;
    return 0;
  }
  for (size_t kk=0; kk<a.mNames.size(); kk++) {
    AddColumn(a.mNames[kk].type, a.mNames[kk].nom, a.mNames[kk].vecsz);
    bool sdone=false;
    Units un=a.GetUnits(kk,sdone);
    if (sdone) SetUnits(kk,un);
  }

  return NVar();
}

/*!
   No check on column names is performed. return true is same structure
*/
bool BaseDataTable::CompareStructure(BaseDataTable const & a)
{
  if (NVar() != a.NVar())  return false;
  for (size_t kk=0; kk<mNames.size(); kk++) 
    if ( (mNames[kk].type != a.mNames[kk].type) || 
	 (mNames[kk].vecsz != a.mNames[kk].vecsz) ) return false;
  return true;
}

/*!
   return true is same structure. The two tables should have the same structure (column type and vector
   size), and the same column names.
*/
bool BaseDataTable::CompareStructAndColNames(BaseDataTable const & a)
{
  if (NVar() != a.NVar())  return false;
  for (size_t kk=0; kk<mNames.size(); kk++) 
    if ( (mNames[kk].type != a.mNames[kk].type) || 
	 (mNames[kk].nom != a.mNames[kk].nom) ||
	 (mNames[kk].vecsz != a.mNames[kk].vecsz) ) return false;
  return true;
}

// Definition d'unite 
void BaseDataTable::SetUnits(size_t k, const Units& un)
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 

  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::SetUnits() out of range column index k");
  size_t sk = mNames[k].ser;
  switch (mNames[k].type) {
  case IntegerField :
    mIColsP[sk]->SetUnits(un);
    break;
  case LongField :
    mLColsP[sk]->SetUnits(un);
    break;
  case FloatField :
    mFColsP[sk]->SetUnits(un);
    break;
  case DoubleField :
    mDColsP[sk]->SetUnits(un);
    break;
  case ComplexField :
    mYColsP[sk]->SetUnits(un);
    break;
  case DoubleComplexField :
    mZColsP[sk]->SetUnits(un);
    break;
  case StringField :
    mSColsP[sk]->SetUnits(un);
    break;
  case DateTimeField :
    mTColsP[sk]->SetUnits(un);
    break;
  case CharacterField :
    mCColsP[sk]->SetUnits(un);
    break;
  case FMLStr16Field :
    mS16ColsP[sk]->SetUnits(un);
    break;
  case FMLStr64Field :
    mS64ColsP[sk]->SetUnits(un);
    break;
  case FlagVec16Field :
    mB16ColsP[sk]->SetUnits(un);
    break;
  case FlagVec64Field :
    mB64ColsP[sk]->SetUnits(un);
    break;
  default:
    break;
  }
  return; 
}

// Definition d'unite 
Units BaseDataTable::GetUnits(size_t k, bool& sdone)   const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 

  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::SetUnits() out of range column index k");
  size_t sk = mNames[k].ser;
  Units ru;
  switch (mNames[k].type) {
  case IntegerField :
    ru=mIColsP[sk]->GetUnits(sdone);
    break;
  case LongField :
    ru=mLColsP[sk]->GetUnits(sdone);
    break;
  case FloatField :
    ru=mFColsP[sk]->GetUnits(sdone);
    break;
  case DoubleField :
    ru=mDColsP[sk]->GetUnits(sdone);
    break;
  case ComplexField :
    ru=mYColsP[sk]->GetUnits(sdone);
    break;
  case DoubleComplexField :
    ru=mZColsP[sk]->GetUnits(sdone);
    break;
  case StringField :
    ru=mSColsP[sk]->GetUnits(sdone);
    break;
  case DateTimeField :
    ru=mTColsP[sk]->GetUnits(sdone);
    break;
  case CharacterField :
    ru=mCColsP[sk]->GetUnits(sdone);
    break;
  case FMLStr16Field :
    ru=mS16ColsP[sk]->GetUnits(sdone);
    break;
  case FMLStr64Field :
    ru=mS64ColsP[sk]->GetUnits(sdone);
    break;
  case FlagVec16Field :
    ru=mB16ColsP[sk]->GetUnits(sdone);
    break;
  case FlagVec64Field :
    ru=mB64ColsP[sk]->GetUnits(sdone);
    break;
  default:
    break;
  }
  return ru; 
}

//! Check the validity of column name \b cnom - throw exception.
bool BaseDataTable::CheckColName(string const & cnom)
{
  size_t l,k;
  l = cnom.length();
  if (l < 1)  
    throw ParmError("BaseDataTable::CheckColName() zero length column name");
  if (!isalpha(cnom[0]))  
    throw ParmError("BaseDataTable::CheckColName() first character not alphabetical");
  for(k=1; k<l; k++) 
    if ((!isalnum(cnom[k])) && (cnom[k] != '_'))  
      throw ParmError("BaseDataTable::CheckColName() Non alphanumeric char in name");
  for(k=0; k<mNames.size(); k++)
    if (cnom == mNames[k].nom) 
      throw ParmError("BaseDataTable::CheckColName() already existing name");
  return true;
}

/*! 
  \brief Check  the validity of column name \b cnom and correct it if necessary
  return true if \b cnom has been changed
*/
bool BaseDataTable::CheckCorrectColName(string& cnom)
{
  size_t l,k;
  l = cnom.length();
  string ssn;
  MuTyV mvv = (uint_8)(NCols()+1);
  mvv.Convert(ssn);
  bool fgrc = false;
  if (l < 1) { cnom = "C"+ssn;   fgrc = true; }
  if (!isalpha(cnom[0]))  { cnom = "C"+cnom;  fgrc = true; }
  for(k=1; k<l; k++) 
    if ((!isalnum(cnom[k])) && (cnom[k] != '_'))  { 
      cnom[k] = '_';  fgrc = true;
    }
  bool fgdouble = true;
  while (fgdouble) {
    fgdouble = false;
    for(k=0; k<mNames.size(); k++)
      if (cnom == mNames[k].nom) {
	fgdouble = true;  break;
      }
    if (fgdouble) { cnom = cnom+ssn; fgrc = true; } 
  }
  return fgrc;
}

// Retourne le nombre total d'elements dans une ligne 
size_t  BaseDataTable::NItems_in_Row() const
{
  size_t rv=0;
  for(size_t k=0; k<mNames.size(); k++)  rv+=mNames[k].vecsz;
  return rv;
}

void  BaseDataTable::SetNRows(size_t nrows) 
{
  if (nrows < NRows()) {
    cout << "(Warning) BaseDataTable::SetNRows(" << nrows << ") < NRows()=" << NRows() << " NOT changed " << endl;
    return;
  }
  while (mSegSz*mNSeg < nrows)  Extend();
  size_t oldnrows=NRows();
  mNEnt=nrows;
  cout << "(Warning) BaseDataTable::SetNRows() changed " << oldnrows << " -> " << NRows() << endl; 
  return;
}

/*! 
  Extends the table (in the row direction). This method is called automatically when needed.
*/
size_t BaseDataTable::Extend()
{
  for(size_t k=0; k<mIColsP.size(); k++) 
    mIColsP[k]->Extend();
  for(size_t k=0; k<mLColsP.size(); k++) 
    mLColsP[k]->Extend();
  for(size_t k=0; k<mFColsP.size(); k++) 
    mFColsP[k]->Extend();
  for(size_t k=0; k<mDColsP.size(); k++)     
    mDColsP[k]->Extend();
  for(size_t k=0; k<mYColsP.size(); k++) 
    mYColsP[k]->Extend();
  for(size_t k=0; k<mZColsP.size(); k++) 
    mZColsP[k]->Extend();
  for(size_t k=0; k<mSColsP.size(); k++) 
    mSColsP[k]->Extend();
  for(size_t k=0; k<mTColsP.size(); k++) 
    mTColsP[k]->Extend();
  for(size_t k=0; k<mCColsP.size(); k++) 
    mCColsP[k]->Extend();
  for(size_t k=0; k<mS16ColsP.size(); k++) 
    mS16ColsP[k]->Extend();
  for(size_t k=0; k<mS64ColsP.size(); k++) 
    mS64ColsP[k]->Extend();
  for(size_t k=0; k<mB16ColsP.size(); k++) 
    mB16ColsP[k]->Extend();
  for(size_t k=0; k<mB64ColsP.size(); k++) 
    mB64ColsP[k]->Extend();
  mNSeg++;
  return mNSeg;
}

// Retourne une structure
/*!
 The returned DataTableRow object can be used for subsequent call to 
 AddRow() or GetRow() methods.
 Generate an exception if called for a table with no columns
*/
DataTableRow BaseDataTable::EmptyRow()  const 
{
  if (NCols() == 0) 
    throw ParmError("BaseDataTable::EmptyRow() Table has no column !");
  vector<string> nms;
  vector<size_t> cseqp;
  for(size_t k=0; k<NVar(); k++)  {
    nms.push_back(mNames[k].nom);
    cseqp.push_back(mNames[k].vecsz);
  }
  DataTableRow row(nms, cseqp);
  return row;
}

/*!
 The returned DataTablePtr object should be used in subsequent calls  
 GetRowPtr() or NextRowPtr() methods.
 The method generates an exception if called for a table with no columns
 The use of DataTablePtr object with GetRowPtr() / NextRowPtr() is a more efficient 
 way of filling the table or accessing its content, as it avoids unnecessary data copies.

 \warning: the DataTablePtr returned by EmptyRowPtr() can not be used before 
 call to GetRowPtr() or NextRowPtr(), as data pointers are invalid ( = NULL) 
*/
DataTableRowPtr BaseDataTable::EmptyRowPtr()   const 
{
  if (NCols() == 0) 
    throw ParmError("BaseDataTable::EmptyRowPtr() Table has no column !");
  vector<string> nms;
  vector<size_t> csz;
  vector<DataTableCellType> ctyp;
  for(size_t k=0; k<NVar(); k++)  {
    ctyp.push_back(Convert2DTCellType(mNames[k].type));
    nms.push_back(mNames[k].nom);
    csz.push_back(mNames[k].vecsz);
  }
  DataTableRowPtr rowp(ctyp, nms, csz);
  return rowp;
}

// 
// A quel index correspond mon nom ? 
// 
size_t BaseDataTable::IndexNom(char const* nom) const 
{
  for(size_t k=0; k<NVar(); k++) 
    if ( mNames[k].nom == nom )      return k;
  throw NotFoundExc("BaseDataTable::IndexNom(): unknown column name ");
}

string BaseDataTable::NomIndex(size_t k) const 
{
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::NomIndex() out of range column index k");
  return mNames[k].nom ; 
}

//! Adds a row (or line) to the table with r_8* inout data
/*! 
  The data to be added is provided as an array (vector) of double (r_8).
  The necessary data conversion is performed, depending on each 
  column's data typeconverted to the data type. 
  Return the new number of table rows (lines / entries)
  \param data : Data for each cell of the row to be appended 
     data[k] k=0..NColumns() if no column with vector content 
     data[k] k=0..NItems_in_Row() if no column with vector content 
    
*/
size_t BaseDataTable::AddRow(const r_8* data)
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 

  if (NVar() == 0) 
    throw ParmError("BaseDataTable::AddRow(const r_8*) Table has no column !");
  if (NEntry() == SegmentSize()*NbSegments())  Extend();
  size_t n = NEntry();
  size_t bid = n/mSegSz;
  size_t off = n%mSegSz;
  size_t off2 = off;
  if (!mFgVecCol) {   // Aucune colonne de type vecteur 
    for(size_t k=0; k<mIColsP.size(); k++)  
      mIColsP[k]->GetSegment(bid)[off] = (int_4)data[mIColIdx[k]];
    for(size_t k=0; k<mLColsP.size(); k++) 
      mLColsP[k]->GetSegment(bid)[off] = (int_8)data[mLColIdx[k]];
    for(size_t k=0; k<mFColsP.size(); k++) 
      mFColsP[k]->GetSegment(bid)[off] = (r_4)data[mFColIdx[k]];
    for(size_t k=0; k<mDColsP.size(); k++) 
      mDColsP[k]->GetSegment(bid)[off] = data[mDColIdx[k]];
    for(size_t k=0; k<mYColsP.size(); k++) 
      mYColsP[k]->GetSegment(bid)[off] = complex<r_4>(data[mYColIdx[k]],0.);
    for(size_t k=0; k<mZColsP.size(); k++) 
      mZColsP[k]->GetSegment(bid)[off] = complex<r_8>(data[mZColIdx[k]],0.);
    for(size_t k=0; k<mSColsP.size(); k++) 
      mSColsP[k]->GetSegment(bid)[off] = (string)MuTyV(data[mSColIdx[k]]);
    for(size_t k=0; k<mTColsP.size(); k++) 
      mTColsP[k]->GetSegment(bid)[off].Set(data[mTColIdx[k]]);
    for(size_t k=0; k<mCColsP.size(); k++)  
      mCColsP[k]->GetSegment(bid)[off] = '\0';
    for(size_t k=0; k<mS16ColsP.size(); k++) 
      mS16ColsP[k]->GetSegment(bid)[off] = (string)MuTyV(data[mS16ColIdx[k]]);
    for(size_t k=0; k<mS64ColsP.size(); k++) 
      mS64ColsP[k]->GetSegment(bid)[off] = (string)MuTyV(data[mS64ColIdx[k]]);
    for(size_t k=0; k<mB16ColsP.size(); k++) 
      mB16ColsP[k]->GetSegment(bid)[off].clearAll(); 
    for(size_t k=0; k<mB16ColsP.size(); k++) 
      mB64ColsP[k]->GetSegment(bid)[off].clearAll(); 

  }
  else {   // Il y a des colonnes avec des contenus de type vecteur 
    for(size_t k=0; k<mIColsP.size(); k++) {  
      size_t nitems = mIColsP[k]->NbItems();
      if (nitems == 1) 
	mIColsP[k]->GetSegment(bid)[off] = (int_4)data[mNames[mIColIdx[k]].item_in_row];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mIColsP[k]->GetSegment(bid)[off2+i] = (int_4)data[mNames[mIColIdx[k]].item_in_row+i];
      }
    }
    for(size_t k=0; k<mLColsP.size(); k++) {
      size_t nitems = mLColsP[k]->NbItems();
      if (nitems == 1) 
	mLColsP[k]->GetSegment(bid)[off] = (int_8)data[mNames[mLColIdx[k]].item_in_row];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mLColsP[k]->GetSegment(bid)[off2+i] = (int_8)data[mNames[mLColIdx[k]].item_in_row+i];
      }
    }
    for(size_t k=0; k<mFColsP.size(); k++) {
      size_t nitems = mFColsP[k]->NbItems();
      if (nitems == 1) 
	mFColsP[k]->GetSegment(bid)[off] = (r_4)data[mNames[mFColIdx[k]].item_in_row];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mFColsP[k]->GetSegment(bid)[off2+i] = (r_4)data[mNames[mFColIdx[k]].item_in_row+i];
      }
    }
    for(size_t k=0; k<mDColsP.size(); k++) {
      size_t nitems = mDColsP[k]->NbItems();
      if (nitems == 1) 
	mDColsP[k]->GetSegment(bid)[off] = data[mNames[mDColIdx[k]].item_in_row];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mDColsP[k]->GetSegment(bid)[off2+i] = data[mNames[mDColIdx[k]].item_in_row+i];
      }
    }
    for(size_t k=0; k<mYColsP.size(); k++) {
      size_t nitems = mYColsP[k]->NbItems();
      if (nitems == 1) 
	mYColsP[k]->GetSegment(bid)[off] = complex<r_4>(data[mNames[mYColIdx[k]].item_in_row],0.);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mYColsP[k]->GetSegment(bid)[off2+i] = complex<r_4>(data[mNames[mYColIdx[k]].item_in_row+i],0.);
      }
    }
    for(size_t k=0; k<mZColsP.size(); k++) {
      size_t nitems = mZColsP[k]->NbItems();
      if (nitems == 1) 
	mZColsP[k]->GetSegment(bid)[off] = complex<r_8>(data[mNames[mZColIdx[k]].item_in_row],0.);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mZColsP[k]->GetSegment(bid)[off2+i] = complex<r_8>(data[mNames[mZColIdx[k]].item_in_row+i],0.);
      }
    }
    for(size_t k=0; k<mSColsP.size(); k++) {
      size_t nitems = mSColsP[k]->NbItems();
      if (nitems == 1) 
	mSColsP[k]->GetSegment(bid)[off] = (string)MuTyV(data[mNames[mSColIdx[k]].item_in_row]);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mSColsP[k]->GetSegment(bid)[off2+i] = (string)MuTyV(data[mNames[mSColIdx[k]].item_in_row+i]);
      }
    }
    for(size_t k=0; k<mTColsP.size(); k++) {
      size_t nitems = mTColsP[k]->NbItems();
      if (nitems == 1) 
	mTColsP[k]->GetSegment(bid)[off].Set(data[mNames[mTColIdx[k]].item_in_row]);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mTColsP[k]->GetSegment(bid)[off2+i].Set(data[mNames[mTColIdx[k]].item_in_row+i]);
      }
    }
    for(size_t k=0; k<mCColsP.size(); k++) {  
      size_t nitems = mCColsP[k]->NbItems();
      if (nitems == 1) 
	mCColsP[k]->GetSegment(bid)[off] = '\0';
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mCColsP[k]->GetSegment(bid)[off2+i] = '\0';
      }
    }
    for(size_t k=0; k<mS16ColsP.size(); k++) {
      size_t nitems = mS16ColsP[k]->NbItems();
      if (nitems == 1) 
	mS16ColsP[k]->GetSegment(bid)[off] = (string)MuTyV(data[mNames[mS16ColIdx[k]].item_in_row]);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mS16ColsP[k]->GetSegment(bid)[off2+i] = (string)MuTyV(data[mNames[mS16ColIdx[k]].item_in_row+i]);
      }
    }
    for(size_t k=0; k<mS64ColsP.size(); k++) {
      size_t nitems = mS64ColsP[k]->NbItems();
      if (nitems == 1) 
	mS64ColsP[k]->GetSegment(bid)[off] = (string)MuTyV(data[mNames[mS64ColIdx[k]].item_in_row]);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mS64ColsP[k]->GetSegment(bid)[off2+i] = (string)MuTyV(data[mNames[mS64ColIdx[k]].item_in_row+i]);
      }
    }
    for(size_t k=0; k<mB16ColsP.size(); k++) {
      size_t nitems = mB16ColsP[k]->NbItems();
      if (nitems == 1) 
	mB16ColsP[k]->GetSegment(bid)[off].clearAll();
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mB16ColsP[k]->GetSegment(bid)[off2+i].clearAll();
      }
    }
    for(size_t k=0; k<mB64ColsP.size(); k++) {
      size_t nitems = mB64ColsP[k]->NbItems();
      if (nitems == 1) 
	mB64ColsP[k]->GetSegment(bid)[off].clearAll();
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mB64ColsP[k]->GetSegment(bid)[off2+i].clearAll();
      }
    }

  }
  mNEnt++;
  return mNEnt;
}

//! Adds a row (or line) to the table with input data as an array of MuTyV
/*! 
  The data to be added is provided as an array (vector) of MuTyV.
  The MuTyV class conversion operators are used to match against each 
  cell data type.
  Return the new number of table rows (lines / entries)
  \param data : Data (MuTyV*) for each cell of the row to be appended 
  (data[k] k=0..NbColumns()) 
*/
size_t BaseDataTable::AddRow(const MuTyV* data)
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 

  if (NVar() == 0) 
    throw ParmError("BaseDataTable::AddRow(const MuTyV*) Table has no column !");
  if (NEntry() == SegmentSize()*NbSegments())  Extend();
  size_t n = NEntry();
  size_t bid = n/mSegSz;
  size_t off = n%mSegSz;
  size_t off2 = off;
  complex<r_4> z4;   complex<r_8> z8;
  string str;   TimeStamp tms;
  int_8 i8dum;
  if (!mFgVecCol) {   // Aucune colonne de type vecteur 
    for(size_t k=0; k<mIColsP.size(); k++) 
      mIColsP[k]->GetSegment(bid)[off] = (int_4)data[mIColIdx[k]];
    for(size_t k=0; k<mLColsP.size(); k++) 
      mLColsP[k]->GetSegment(bid)[off] = (int_8)data[mLColIdx[k]];
    for(size_t k=0; k<mFColsP.size(); k++) 
      mFColsP[k]->GetSegment(bid)[off] = (r_4)data[mFColIdx[k]];
    for(size_t k=0; k<mDColsP.size(); k++) 
      mDColsP[k]->GetSegment(bid)[off] = (r_8)data[mDColIdx[k]];
    for(size_t k=0; k<mYColsP.size(); k++) 
      mYColsP[k]->GetSegment(bid)[off] = 
	complex<r_4>(data[mYColIdx[k]].GetRealPart(), data[mYColIdx[k]].GetImagPart());
    for(size_t k=0; k<mZColsP.size(); k++) 
      mZColsP[k]->GetSegment(bid)[off] = 
	complex<r_8>(data[mZColIdx[k]].GetRealPart(), data[mZColIdx[k]].GetImagPart());
    for(size_t k=0; k<mSColsP.size(); k++) 
      mSColsP[k]->GetSegment(bid)[off] = (string)data[mSColIdx[k]];
    for(size_t k=0; k<mTColsP.size(); k++) 
      mTColsP[k]->GetSegment(bid)[off] = data[mTColIdx[k]].Convert(tms);
    for(size_t k=0; k<mCColsP.size(); k++) 
      mCColsP[k]->GetSegment(bid)[off] = (char)data[mCColIdx[k]];
    for(size_t k=0; k<mS16ColsP.size(); k++) 
      mS16ColsP[k]->GetSegment(bid)[off] = (string)(data[mS16ColIdx[k]]);
    for(size_t k=0; k<mS64ColsP.size(); k++) 
      mS64ColsP[k]->GetSegment(bid)[off] = (string)(data[mS64ColIdx[k]]);
    for(size_t k=0; k<mB16ColsP.size(); k++) 
      mB16ColsP[k]->GetSegment(bid)[off].decodeInteger(data[mB16ColIdx[k]].Convert(i8dum));
    for(size_t k=0; k<mB64ColsP.size(); k++) 
      mB64ColsP[k]->GetSegment(bid)[off].decodeInteger(data[mB64ColIdx[k]].Convert(i8dum));

  }
  else {   // Il y a des colonnes avec des contenus de type vecteur 
   for(size_t k=0; k<mIColsP.size(); k++) {  
      size_t nitems = mIColsP[k]->NbItems();
      if (nitems == 1) 
	mIColsP[k]->GetSegment(bid)[off] = (int_4)data[mNames[mIColIdx[k]].item_in_row];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mIColsP[k]->GetSegment(bid)[off2+i] = (int_4)data[mNames[mIColIdx[k]].item_in_row+i];
      }
    }
    for(size_t k=0; k<mLColsP.size(); k++) {
      size_t nitems = mLColsP[k]->NbItems();
      if (nitems == 1) 
	mLColsP[k]->GetSegment(bid)[off] = (int_8)data[mNames[mLColIdx[k]].item_in_row];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mLColsP[k]->GetSegment(bid)[off2+i] = (int_8)data[mNames[mLColIdx[k]].item_in_row+i];
      }
    }
    for(size_t k=0; k<mFColsP.size(); k++) {
      size_t nitems = mFColsP[k]->NbItems();
      if (nitems == 1) 
	mFColsP[k]->GetSegment(bid)[off] = (r_4)data[mNames[mFColIdx[k]].item_in_row];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mFColsP[k]->GetSegment(bid)[off2+i] = (r_4)data[mNames[mFColIdx[k]].item_in_row+i];
      }
    }
    for(size_t k=0; k<mDColsP.size(); k++) {
      size_t nitems = mDColsP[k]->NbItems();
      if (nitems == 1) 
	mDColsP[k]->GetSegment(bid)[off] = (r_8)data[mNames[mDColIdx[k]].item_in_row];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mDColsP[k]->GetSegment(bid)[off2+i] = (r_8)data[mNames[mDColIdx[k]].item_in_row+i];
      }
    }
    for(size_t k=0; k<mYColsP.size(); k++) {
      size_t nitems = mYColsP[k]->NbItems();
      if (nitems == 1) 
	mYColsP[k]->GetSegment(bid)[off] = data[mNames[mYColIdx[k]].item_in_row].Convert(z4);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mYColsP[k]->GetSegment(bid)[off2+i] = data[mNames[mYColIdx[k]].item_in_row+i].Convert(z4);
      }
    }
    for(size_t k=0; k<mZColsP.size(); k++) {
      size_t nitems = mZColsP[k]->NbItems();
      if (nitems == 1) 
	mZColsP[k]->GetSegment(bid)[off] = data[mNames[mZColIdx[k]].item_in_row].Convert(z8);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mZColsP[k]->GetSegment(bid)[off2+i] = data[mNames[mZColIdx[k]].item_in_row+i].Convert(z8);
      }
    }
    for(size_t k=0; k<mSColsP.size(); k++) {
      size_t nitems = mSColsP[k]->NbItems();
      if (nitems == 1) 
	mSColsP[k]->GetSegment(bid)[off] = data[mNames[mSColIdx[k]].item_in_row].Convert(str);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mSColsP[k]->GetSegment(bid)[off2+i] = data[mNames[mSColIdx[k]].item_in_row+i].Convert(str);
      }
    }
    for(size_t k=0; k<mTColsP.size(); k++) {
      size_t nitems = mTColsP[k]->NbItems();
      if (nitems == 1) 
	mTColsP[k]->GetSegment(bid)[off] = data[mNames[mTColIdx[k]].item_in_row].Convert(tms);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mTColsP[k]->GetSegment(bid)[off2+i] = data[mNames[mTColIdx[k]].item_in_row+i].Convert(tms);
      }
    }
   for(size_t k=0; k<mCColsP.size(); k++) {  
      size_t nitems = mCColsP[k]->NbItems();
      if (nitems == 1) 
	mCColsP[k]->GetSegment(bid)[off] = (char)data[mNames[mCColIdx[k]].item_in_row];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mCColsP[k]->GetSegment(bid)[off2+i] = (char)data[mNames[mCColIdx[k]].item_in_row+i];
      }
    }
    for(size_t k=0; k<mS16ColsP.size(); k++) {
      size_t nitems = mS16ColsP[k]->NbItems();
      if (nitems == 1) 
	mS16ColsP[k]->GetSegment(bid)[off] = data[mNames[mS16ColIdx[k]].item_in_row].Convert(str);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mS16ColsP[k]->GetSegment(bid)[off2+i] = data[mNames[mS16ColIdx[k]].item_in_row+i].Convert(str);
      }
    }
    for(size_t k=0; k<mS64ColsP.size(); k++) {
      size_t nitems = mS64ColsP[k]->NbItems();
      if (nitems == 1) 
	mS64ColsP[k]->GetSegment(bid)[off] = data[mNames[mS64ColIdx[k]].item_in_row].Convert(str);
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mS64ColsP[k]->GetSegment(bid)[off2+i] = data[mNames[mS64ColIdx[k]].item_in_row+i].Convert(str);
      }
    }
    for(size_t k=0; k<mB16ColsP.size(); k++) {
      size_t nitems = mB16ColsP[k]->NbItems();
      if (nitems == 1) 
	mB16ColsP[k]->GetSegment(bid)[off].decodeInteger(data[mNames[mB16ColIdx[k]].item_in_row].Convert(i8dum));
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mB16ColsP[k]->GetSegment(bid)[off2+i].decodeInteger(data[mNames[mB16ColIdx[k]].item_in_row+i].Convert(i8dum));
      }
    }
    for(size_t k=0; k<mB64ColsP.size(); k++) {
      size_t nitems = mB64ColsP[k]->NbItems();
      if (nitems == 1) 
	mB64ColsP[k]->GetSegment(bid)[off].decodeInteger(data[mNames[mB64ColIdx[k]].item_in_row].Convert(i8dum));
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mB64ColsP[k]->GetSegment(bid)[off2+i].decodeInteger(data[mNames[mB64ColIdx[k]].item_in_row+i].Convert(i8dum));
      }
    }

  }
  mNEnt++;
  return mNEnt;
}
//! Adds a row (or line) to the table with input data as DataTableRow object
/*! 
  The internal MuTyV array of the object contains the data and the 
  MuTyV class conversion operators are used to match against each 
  cell data type.
  Only the size of the input data object is checked.
  Return the new number of table rows (lines / entries)
  \param data : Data  for each cell of the row to be appended 
  (data[k] k=0..NbColumns()) 
*/
size_t BaseDataTable::AddRow(DataTableRow const& data)
{
  if ( (data.NCols() != NCols()) || (data.Size() != NItems_in_Row()) ) 
    throw SzMismatchError(" BaseDataTable::AddRow(DataTableRow& data) - data.NCols() != NCols() OR  data.Size() != NItems_in_Row() ");
  return AddRow(data.MTVPtr());
}


//--------------- Methodes de remplissage des colonnes ------------------
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector<int_4> const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector<int_4> v) out of range column index k");
  if (mNames[k].type!=IntegerField) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector<int_4> v) NOT IntegerField type column");
  size_t sk = mNames[k].ser;
  mIColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector<int_8> const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector<int_8> v) out of range column index k");
  if (mNames[k].type!=LongField) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector<int_8> v) NOT LongField type column");
  size_t sk = mNames[k].ser;
  mLColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector<r_4> const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector<r_4> v) out of range column index k");
  if (mNames[k].type!=FloatField) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector<r_4> v) NOT FloatField type column");
  size_t sk = mNames[k].ser;
  mFColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector<r_8> const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector<r_8> v) out of range column index k");
  if (mNames[k].type!=DoubleField) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector<r_8> v) NOT DoubleField type column");
  size_t sk = mNames[k].ser;
  mDColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector< complex<r_4> > const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector< complex<r_4> > v) out of range column index k");
  if (mNames[k].type!=ComplexField) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector< complex<r_4> > v) NOT ComplexField type column");
  size_t sk = mNames[k].ser;
  mYColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector< complex<r_8> > const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector< complex<r_8> > v) out of range column index k");
  if (mNames[k].type!=DoubleComplexField) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector< complex<r_8> > v) NOT DoubleComplexField type column");
  size_t sk = mNames[k].ser;
  mZColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector< string > const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector< string > v) out of range column index k");
  if (mNames[k].type!=StringField) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector< string > v) NOT StringField type column");
  size_t sk = mNames[k].ser;
  mSColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector< TimeStamp > const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector< TimeStamp > v) out of range column index k");
  if (mNames[k].type!=DateTimeField) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector< TimeStamp > v) NOT DateTimeField type column");
  size_t sk = mNames[k].ser;
  mTColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector<char> const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector<char> v) out of range column index k");
  if (mNames[k].type!=CharacterField) 
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector<char> v) NOT CharacterField type column");
  size_t sk = mNames[k].ser;
  mCColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector< String16 > const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector< String16 > v) out of range column index k");
  if (mNames[k].type!=FMLStr16Field) 
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector< String16 > v) NOT FMLStr16Field type column");
  size_t sk = mNames[k].ser;
  mS16ColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector< String64 > const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector< String16 > v) out of range column index k");
  if (mNames[k].type!=FMLStr64Field) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector< String16 > v) NOT FMLStr64Field type column");
  size_t sk = mNames[k].ser;
  mS64ColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector< FlagVector16 > const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector< FlagVector16 > v) out of range column index k");
  if (mNames[k].type!=FlagVec16Field) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector< FlagVector16 > v) NOT FlagVec16Field type column");
  size_t sk = mNames[k].ser;
  mB16ColsP[sk]->CopyFrom(v);
  return;
}
/* --Methode-- */
void BaseDataTable::FillColumn(size_t k, vector< FlagVector64 > const & v) 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::FillColumn(k, vector< FlagVector16 > v) out of range column index k");
  if (mNames[k].type!=FlagVec64Field) 
    throw TypeMismatchExc("BaseDataTable::FillColumn(k, vector< FlagVector64 > v) NOT FlagVec16Field type column");
  size_t sk = mNames[k].ser;
  mB64ColsP[sk]->CopyFrom(v);
  return;
}

/* --Methode-- */
/*!
  \param k : the column index (starting from zero) in the target table (*this)
  \param src : the source table from which the data is copied 
  \param ks : the column index in the source \b src table

  \warning the number of data items copied is equal to the minimum of the number of data items in the 
  source and target tables 

  \warning a TypeMismatchExc exception is generated if the two columns do not have the same data type or 
  not same vector size
*/
void BaseDataTable::FillColumn(size_t k, BaseDataTable const& src, size_t ks)
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if ((k >= NVar())||(ks >= src.NVar())) 
    throw RangeCheckError("BaseDataTable::FillColumn(k, BaseDataTable, ks) out of range column index k or ks");
  if ((mNames[k].type!=src.mNames[ks].type)||(mNames[k].vecsz!=src.mNames[ks].vecsz))
    throw RangeCheckError("BaseDataTable::FillColumn(k, BaseDataTable, ks) different data type or vector size for the two columns");
  size_t sk = mNames[k].ser;
  size_t sks = src.mNames[ks].ser;

  Units ru;
  switch (mNames[k].type) {
  case IntegerField :
    mIColsP[sk]->CopyFrom(*(src.mIColsP[sks]));
    break;
  case LongField :
    mLColsP[sk]->CopyFrom(*(src.mLColsP[sks]));
    break;
  case FloatField :
    mFColsP[sk]->CopyFrom(*(src.mFColsP[sks]));
    break;
  case DoubleField :
    mDColsP[sk]->CopyFrom(*(src.mDColsP[sks]));
    break;
  case ComplexField :
    mYColsP[sk]->CopyFrom(*(src.mYColsP[sks]));
    break;
  case DoubleComplexField :
    mZColsP[sk]->CopyFrom(*(src.mZColsP[sks]));
    break;
  case StringField :
    mSColsP[sk]->CopyFrom(*(src.mSColsP[sks]));
    break;
  case DateTimeField :
    mTColsP[sk]->CopyFrom(*(src.mTColsP[sks]));
    break;
  case CharacterField :
    mCColsP[sk]->CopyFrom(*(src.mCColsP[sks]));
    break;
  case FMLStr16Field :
    mS16ColsP[sk]->CopyFrom(*(src.mS16ColsP[sks]));
    break;
  case FMLStr64Field :
    mS64ColsP[sk]->CopyFrom(*(src.mS64ColsP[sks]));
    break;
  case FlagVec16Field :
    mB16ColsP[sk]->CopyFrom(*(src.mB16ColsP[sks]));
    break;
  case FlagVec64Field :
    mB64ColsP[sk]->CopyFrom(*(src.mB64ColsP[sks]));
    break;
  default:
    break;
  }
  return;
}
/*!
  Fills the input \b row object with the content of row \b n.
  Return a reference to the input \b row object. 
  Generate an exception if the input \b row object has the wrong size.
*/
DataTableRow& BaseDataTable::GetRow(size_t n, DataTableRow& row) const
{
  if ( (row.NCols() != NCols()) || (row.Size() != NItems_in_Row()) ) 
    throw SzMismatchError(" BaseDataTable::GetRow(n, DataTableRow& data) - row.NCols() != NCols() OR  row.Size() != NItems_in_Row() ");
  GetRow(n, row.MTVPtr());
  return row;
}

/*!
  For thread-safe operation, specify a valid \b mtvp  pointer (!= NULL)  
*/
MuTyV* BaseDataTable::GetRow(size_t n, MuTyV* mtvp) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 

  if (n >= NEntry())
    throw RangeCheckError("BaseDataTable::GetRow() out of range line index n");
  if (mtvp == NULL) {
    if (mVarMTV == NULL) mVarMTV = new MuTyV[NItems_in_Row()];
    mtvp = mVarMTV;
  }
  size_t bid = n/mSegSz;
  size_t off = n%mSegSz;
  size_t off2 = off;
  if (!mFgVecCol) {   // Aucune colonne de type vecteur 
    for(size_t k=0; k<mIColsP.size(); k++) 
      mtvp[mIColIdx[k]] = mIColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mLColsP.size(); k++) 
      mtvp[mLColIdx[k]] = mLColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mFColsP.size(); k++) 
      mtvp[mFColIdx[k]] = mFColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mDColsP.size(); k++) 
      mtvp[mDColIdx[k]] = mDColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mYColsP.size(); k++) 
      mtvp[mYColIdx[k]] = mYColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mZColsP.size(); k++) 
      mtvp[mZColIdx[k]] = mZColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mSColsP.size(); k++) 
      mtvp[mSColIdx[k]] = mSColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mTColsP.size(); k++)
      mtvp[mTColIdx[k]] = mTColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mCColsP.size(); k++) 
      mtvp[mCColIdx[k]] = mCColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mS16ColsP.size(); k++) 
      mtvp[mS16ColIdx[k]] = mS16ColsP[k]->GetCstSegment(bid)[off].cbuff_ptr();
    for(size_t k=0; k<mS64ColsP.size(); k++) 
      mtvp[mS64ColIdx[k]] = mS64ColsP[k]->GetCstSegment(bid)[off].cbuff_ptr();
    for(size_t k=0; k<mB16ColsP.size(); k++) 
      mtvp[mB16ColIdx[k]] = mB16ColsP[k]->GetCstSegment(bid)[off].convertToInteger();
    for(size_t k=0; k<mB64ColsP.size(); k++) 
      mtvp[mB64ColIdx[k]] = mB64ColsP[k]->GetCstSegment(bid)[off].convertToInteger();
  }
  else {  // Il y a des colonnes avec contenu vecteur (plusieurs elements)
    for(size_t k=0; k<mIColsP.size(); k++) {
      size_t nitems = mIColsP[k]->NbItems();
      if (nitems == 1) 	
	mtvp[mNames[mIColIdx[k]].item_in_row] = mIColsP[k]->GetCstSegment(bid)[off];
      else {
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mIColIdx[k]].item_in_row+i] = mIColsP[k]->GetCstSegment(bid)[off2+i];	  
      }
    }
    for(size_t k=0; k<mLColsP.size(); k++) {
      size_t nitems = mLColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mLColIdx[k]].item_in_row] = mLColsP[k]->GetCstSegment(bid)[off];
      else {
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mLColIdx[k]].item_in_row+i] = mLColsP[k]->GetCstSegment(bid)[off2+i];
      }
    }
    for(size_t k=0; k<mFColsP.size(); k++) {
      size_t nitems = mFColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mFColIdx[k]].item_in_row] = mFColsP[k]->GetCstSegment(bid)[off];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mFColIdx[k]].item_in_row+i] = mFColsP[k]->GetCstSegment(bid)[off2+i];
      }
    }
    for(size_t k=0; k<mDColsP.size(); k++) {
      size_t nitems = mDColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mDColIdx[k]].item_in_row] = mDColsP[k]->GetCstSegment(bid)[off];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mDColIdx[k]].item_in_row+i] = mDColsP[k]->GetCstSegment(bid)[off2+i];
      }
    }
    for(size_t k=0; k<mYColsP.size(); k++) {
      size_t nitems = mYColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mYColIdx[k]].item_in_row] = mYColsP[k]->GetCstSegment(bid)[off];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mYColIdx[k]].item_in_row+i] = mYColsP[k]->GetCstSegment(bid)[off2+i];
      }
    }
    for(size_t k=0; k<mZColsP.size(); k++) {
      size_t nitems = mZColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mZColIdx[k]].item_in_row] = mZColsP[k]->GetCstSegment(bid)[off];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mZColIdx[k]].item_in_row+i] = mZColsP[k]->GetCstSegment(bid)[off2+i];
      }
    }
    for(size_t k=0; k<mSColsP.size(); k++) {
      size_t nitems = mSColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mSColIdx[k]].item_in_row] = mSColsP[k]->GetCstSegment(bid)[off];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mSColIdx[k]].item_in_row+i] = mSColsP[k]->GetCstSegment(bid)[off2+i];
      }
    }
    for(size_t k=0; k<mTColsP.size(); k++) {
      size_t nitems = mTColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mTColIdx[k]].item_in_row] = mTColsP[k]->GetCstSegment(bid)[off];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mTColIdx[k]].item_in_row+i] = mTColsP[k]->GetCstSegment(bid)[off2+i];
      }
    }
    for(size_t k=0; k<mCColsP.size(); k++) {
      size_t nitems = mCColsP[k]->NbItems();
      if (nitems == 1) 	
	mtvp[mNames[mCColIdx[k]].item_in_row] = mCColsP[k]->GetCstSegment(bid)[off];
      else {
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mCColIdx[k]].item_in_row+i] = mCColsP[k]->GetCstSegment(bid)[off2+i];	  
      }
    }
    for(size_t k=0; k<mS16ColsP.size(); k++) {
      size_t nitems = mS16ColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mS16ColIdx[k]].item_in_row] = mS16ColsP[k]->GetCstSegment(bid)[off].cbuff_ptr();
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mS16ColIdx[k]].item_in_row+i] = mS16ColsP[k]->GetCstSegment(bid)[off2+i].cbuff_ptr();
      }
    }
    for(size_t k=0; k<mS64ColsP.size(); k++) {
      size_t nitems = mS64ColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mS64ColIdx[k]].item_in_row] = mS64ColsP[k]->GetCstSegment(bid)[off].cbuff_ptr();
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mS64ColIdx[k]].item_in_row+i] = mS64ColsP[k]->GetCstSegment(bid)[off2+i].cbuff_ptr();
      }
    }
    for(size_t k=0; k<mB16ColsP.size(); k++) {
      size_t nitems = mB16ColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mB16ColIdx[k]].item_in_row] = mB16ColsP[k]->GetCstSegment(bid)[off].convertToInteger();
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mB16ColIdx[k]].item_in_row+i] = mB16ColsP[k]->GetCstSegment(bid)[off2+i].convertToInteger();
      }
    }
    for(size_t k=0; k<mB64ColsP.size(); k++) {
      size_t nitems = mB64ColsP[k]->NbItems();
      if (nitems == 1) 
	mtvp[mNames[mB64ColIdx[k]].item_in_row] = mB64ColsP[k]->GetCstSegment(bid)[off].convertToInteger();
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mtvp[mNames[mB64ColIdx[k]].item_in_row+i] = mB64ColsP[k]->GetCstSegment(bid)[off2+i].convertToInteger();
      }
    }

  }
  return mtvp;
}

/*!
    This method should be used for write access to the data table rows. Use the GetCstRowPtr() method for reading.
    No check on the DataTableRowPtr \b rowp object is performed, which should be created by 
    a call to the EmptyRowPtr() method. return the DataTableRowPtr provided as the argument.

    \warning This method is NOT thread-safe 
*/
DataTableRowPtr& BaseDataTable::GetRowPtr(size_t n, DataTableRowPtr& rowp)
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (n >= NEntry())
    throw RangeCheckError("BaseDataTable::GetRowPtr() out of range line index n");
  size_t bid = n/mSegSz;
  size_t off0 = n%mSegSz;
  for(size_t k=0; k<mNames.size(); k++) {
    size_t sk = mNames[k].ser;
    size_t off = off0*mNames[k].vecsz;
    switch (mNames[k].type) {
    case IntegerField :
      rowp[k].i4 = mIColsP[sk]->GetSegment(bid)+off;
      break;
    case LongField :
      rowp[k].i8 = mLColsP[sk]->GetSegment(bid)+off;
      break;
    case FloatField :
      rowp[k].f4 = mFColsP[sk]->GetSegment(bid)+off;
    break;
    case DoubleField :
      rowp[k].f8 = mDColsP[sk]->GetSegment(bid)+off;
      break;
    case ComplexField :
      rowp[k].z4 = mYColsP[sk]->GetSegment(bid)+off;
      break;
    case DoubleComplexField :
      rowp[k].z8 = mZColsP[sk]->GetSegment(bid)+off;
      break;
    case StringField :
      rowp[k].s = mSColsP[sk]->GetSegment(bid)+off;
      break;
    case DateTimeField :
      rowp[k].tms = mTColsP[sk]->GetSegment(bid)+off;
      break;
    case CharacterField :
      rowp[k].cp = mCColsP[sk]->GetSegment(bid)+off;
      break;
    case FMLStr16Field :
      rowp[k].s16 = mS16ColsP[sk]->GetSegment(bid)+off;
      break;
    case FMLStr64Field :
      rowp[k].s64 = mS64ColsP[sk]->GetSegment(bid)+off;
      break;
    case FlagVec16Field :
      rowp[k].b16 = mB16ColsP[sk]->GetSegment(bid)+off;
      break;
    case FlagVec64Field :
      rowp[k].b64 = mB64ColsP[sk]->GetSegment(bid)+off;
      break;
    default:
      rowp[k].p = NULL;
      break;
    }
  }
  return rowp ; 
}

/*!
    This method should be used for read access to the data table rows. Use the GetRowPtr() method for read/write access.
    No check on the DataTableRowPtr \b rowp object is performed, which should be created by 
    a call to the EmptyRowPtr() method. return the DataTableRowPtr provided as the argument.

    \warning This method is NOT thread-safe 
*/
DataTableRowPtr& BaseDataTable::GetCstRowPtr(size_t n, DataTableRowPtr& rowp)  const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (n >= NEntry())
    throw RangeCheckError("BaseDataTable::GetCstRowPtr() out of range line index n");
  size_t bid = n/mSegSz;
  size_t off0 = n%mSegSz;
  for(size_t k=0; k<mNames.size(); k++) {
    size_t sk = mNames[k].ser;
    size_t off = off0*mNames[k].vecsz;
    switch (mNames[k].type) {
    case IntegerField :
      rowp[k].i4 = const_cast<int_4 *>(mIColsP[sk]->GetCstSegment(bid)+off);
      break;
    case LongField :
      rowp[k].i8 = const_cast<int_8 *>(mLColsP[sk]->GetCstSegment(bid)+off);
      break;
    case FloatField :
      rowp[k].f4 = const_cast<r_4 *>(mFColsP[sk]->GetCstSegment(bid)+off);
    break;
    case DoubleField :
      rowp[k].f8 = const_cast<r_8 *>(mDColsP[sk]->GetCstSegment(bid)+off);
      break;
    case ComplexField :
      rowp[k].z4 = const_cast< complex<r_4> *>(mYColsP[sk]->GetCstSegment(bid)+off);
      break;
    case DoubleComplexField :
      rowp[k].z8 = const_cast< complex<r_8> *>(mZColsP[sk]->GetCstSegment(bid)+off);
      break;
    case StringField :
      rowp[k].s = const_cast<std::string *>(mSColsP[sk]->GetCstSegment(bid)+off);
      break;
    case DateTimeField :
      rowp[k].tms = const_cast<TimeStamp *>(mTColsP[sk]->GetCstSegment(bid)+off);
      break;
    case CharacterField :
      rowp[k].cp = const_cast<char *>(mCColsP[sk]->GetCstSegment(bid)+off);
      break;
    case FMLStr16Field :
      rowp[k].s16 = const_cast<String16 *>(mS16ColsP[sk]->GetCstSegment(bid)+off);
      break;
    case FMLStr64Field :
      rowp[k].s64 = const_cast<String64 *>(mS64ColsP[sk]->GetCstSegment(bid)+off);
      break;
    case FlagVec16Field :
      rowp[k].b16 = const_cast<FlagVector16 *>(mB16ColsP[sk]->GetCstSegment(bid)+off);
      break;
    case FlagVec64Field :
      rowp[k].b64 = const_cast<FlagVector64 *>(mB64ColsP[sk]->GetCstSegment(bid)+off);
      break;
    default:
      rowp[k].p = NULL;
      break;
    }
  }
  return rowp ; 
}

/*!
  Adds a row to the table set the data pointers of the DataTableRowPtr \b rowp to the data cells of 
  row \b n of the table. 
  No check on the DataTableRowPtr \b rowp object is performed, which should be created by 
  a call to the EmptyRowPtr() method. return the DataTableRowPtr provided as the argument.
 */
DataTableRowPtr&   BaseDataTable::NextRowPtr(DataTableRowPtr& rowp)
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (mNEnt == mSegSz*mNSeg)  Extend();
  size_t n = mNEnt;   mNEnt++;
  return GetRowPtr(n, rowp);
}

/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector<int_4> & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector<int_4> v) out of range column index k");
  if (mNames[k].type!=IntegerField) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector<int_4> v) NOT IntegerField type column");
  size_t sk = mNames[k].ser;
  mIColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector<int_8> & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector<int_8> v) out of range column index k");
  if (mNames[k].type!=LongField) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector<int_8> v) NOT LongField type column");
  size_t sk = mNames[k].ser;
  mLColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector<r_4> & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector<r_4> v) out of range column index k");
  if (mNames[k].type!=FloatField) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector<r_4> v) NOT FloatField type column");
  size_t sk = mNames[k].ser;
  mFColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector<r_8> & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector<r_8> v) out of range column index k");
  if (mNames[k].type!=DoubleField) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector<r_8> v) NOT DoubleField type column");
  size_t sk = mNames[k].ser;
  mDColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector< complex<r_4> > & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< complex<r_4> > v) out of range column index k");
  if (mNames[k].type!=ComplexField) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< complex<r_4> > v) NOT ComplexField type column");
  size_t sk = mNames[k].ser;
  mYColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector< complex<r_8> > & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< complex<r_8> > v) out of range column index k");
  if (mNames[k].type!=DoubleComplexField) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< complex<r_8> > v) NOT DoubleComplexField type column");
  size_t sk = mNames[k].ser;
  mZColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector< string > & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< string > v) out of range column index k");
  if (mNames[k].type!=StringField) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< string > v) NOT StringField type column");
  size_t sk = mNames[k].ser;
  mSColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector< TimeStamp > & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< TimeStamp > v) out of range column index k");
  if (mNames[k].type!=DateTimeField) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< TimeStamp > v) NOT DateTimeField type column");
  size_t sk = mNames[k].ser;
  mTColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector<char> & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector<char> v) out of range column index k");
  if (mNames[k].type!=CharacterField) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector<char> v) NOT CharacterField type column");
  size_t sk = mNames[k].ser;
  mCColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector< String16 > & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< String16 > v) out of range column index k");
  if (mNames[k].type!=FMLStr16Field) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< String16 > v) NOT FMLStr16Field type column");
  size_t sk = mNames[k].ser;
  mS16ColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector< String64 > & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< String16 > v) out of range column index k");
  if (mNames[k].type!=FMLStr64Field) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< String16 > v) NOT FMLStr64Field type column");
  size_t sk = mNames[k].ser;
  mS64ColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector< FlagVector16 > & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< FlagVector16 > v) out of range column index k");
  if (mNames[k].type!=FlagVec16Field) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< FlagVector16 > v) NOT FlagVec16Field type column");
  size_t sk = mNames[k].ser;
  mB16ColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}
/* --Methode-- */
/*!
  For columns with vector content of size vsz, the vector \b v would be resized to NRows()*vsz, otherwise to NRows()
*/
void BaseDataTable::GetColumn(size_t k, vector< FlagVector64 > & v) const 
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< FlagVector64 > v) out of range column index k");
  if (mNames[k].type!=FlagVec64Field) 
    throw RangeCheckError("BaseDataTable::GetColumn(k, vector< FlagVector64 > v) NOT FlagVec64Field type column");
  size_t sk = mNames[k].ser;
  mB64ColsP[sk]->CopyTo(v);
  v.resize(NRows()*mNames[k].vecsz);
  return;
}

#define BADVAL -1.e39

TVector<r_8> BaseDataTable::GetColumnD(size_t k) const
{
  DT_TSOP_SYNC dttss(mThS); dttss.NOp();  // Thread-safe operation 

  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetColumnD() out of range column index k");
  if (mNames[k].vecsz > 1) 
    throw ParmError("BaseDataTable::GetColumnD() operation not supported for column with vector content (vsz>1)");
    
  size_t sk = mNames[k].ser;
  size_t nelts = NEntry()*mNames[k].vecsz;
  size_t i = 0; 
  TVector<r_8> rv(NEntry());

  for (size_t is=0; is<NbSegments(); is++) {
    switch (mNames[k].type) {
    case IntegerField :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv(i) = mIColsP[sk]->GetCstSegment(is)[j];
      break;
    case LongField :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv(i) = mLColsP[sk]->GetCstSegment(is)[j];
      break;
    case FloatField :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv(i) = mFColsP[sk]->GetCstSegment(is)[j];
      break;
    case DoubleField :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv(i) = mDColsP[sk]->GetCstSegment(is)[j];
      break;
    case ComplexField :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv(i) = mYColsP[sk]->GetCstSegment(is)[j].real();
      break;
    case DoubleComplexField :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv(i) = mZColsP[sk]->GetCstSegment(is)[j].real();
      break;
    case StringField :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv = atof(mSColsP[sk]->GetCstSegment(is)[j].c_str());
      break;
    case DateTimeField :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv(i) = mTColsP[sk]->GetCstSegment(is)[j].ToDays();
      break;
    case CharacterField :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv(i) = (double)(mCColsP[sk]->GetCstSegment(is)[j]);
      break;
    case FMLStr16Field :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv = atof(mS16ColsP[sk]->GetCstSegment(is)[j].cbuff_ptr());
      break;
    case FMLStr64Field :
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)
	rv = atof(mS64ColsP[sk]->GetCstSegment(is)[j].cbuff_ptr());
      break;
      //---- FlagVec16Field  et  FlagVec64Field ne peuvent etre convertis en double 
    default:
      for(size_t j=0; (j<SegmentSize())&&(i<nelts); j++,i++)  
	rv(i) = BADVAL;
      break;
    }
  }
  return rv ; 
}


/*!
  \param src : the source table from which structure and/or data is copied to the target table (*this)
  \param ckcp : if true, generates an exception if the target table has already entries 
  \param pbm : ProgressBarMode  (displays or not a progress bar) 

  if the table is empty, the structure and content of the table \b src are copied to it.
  Otherwise, an exception is generated if the two tables have incompatible structures. 
  Data from table \b src is appended to the target table (*this).  

  \warning: this method is NOT thread-safe
*/
void BaseDataTable::RowMerge(BaseDataTable const& src, bool ckcp, ProgressBarMode pbm)
{
  if (ckcp && (NEntry() > 0) )
    throw ParmError("BaseDataTable::RowMerge(src) Target Table (*this) has already entries ");
  if (src.NVar() == 0)  throw ParmError("BaseDataTable::RowMerge(a) Source Table a has no column");
  if (NVar() == 0) CopyStructure(src);
  else if (!CompareStructure(src)) 
    throw ParmError("BaseDataTable::RowMerge(src) (this,a) have different table structure");
  if (src.NEntry() == 0) {
    cout << " BaseDataTable::RowMerge(a)/Warning : table a has zero (0) entry ! " << endl;
    return;
  }
  DataTableRowPtr  rdest = EmptyRowPtr();
  DataTableRowPtr  rsrc = src.EmptyRowPtr();
  if (pbm!=ProgBarM_None) 
    cout << "BaseDataTable::RowMerge() copying "<<src.NEntry()<<" rows ... "<<endl;
  ProgressBar prgbar(src.NEntry(), pbm);
  for(size_t kk=0; kk<src.NEntry(); kk++) {
    NextRowPtr(rdest);
    src.GetCstRowPtr(kk,rsrc);
    rdest.CopyContentFrom(rsrc);
    prgbar.update(kk);
  }
  return;
}

/*!
  If the table has no columns, the table structure is copied from \b src, otherwise 
  the two tables should have the same structure (column types and order) 
  The selection of rows identified by the row numbers in the \b rows vector is 
  copied from table \b src. If the first table (the one on which SelectFrom() is called)
  has entries (NRows()>0), the data from the \b src table is appended to the first table.

  \warning: this method is NOT thread-safe
*/
void BaseDataTable::CopySelectedRows(BaseDataTable const& src, std::vector<size_t> const & rows, ProgressBarMode pbm)
{
  if ((src.NVar() == 0)||(src.NRows() == 0)) {
    cout << "BaseDataTable::CopySelectedRows(src,rows)/Warning: src table not initialized or empty" << endl;
    return;
  }
  if (NCols() > 0) {
    if (!CompareStructure(src))  
      throw ParmError("BaseDataTable::CopySelectedRows(src,rows) (this,src) have different table structure");
  }
  else CopyStructure(src);
  DataTableRowPtr  rdest = EmptyRowPtr();
  DataTableRowPtr  rsrc = src.EmptyRowPtr();

  if (pbm!=ProgBarM_None) 
    cout << "BaseDataTable::CopySelectedRows(src,rows) "<<rows.size()<<" rows from a total of "<<src.NRows()<<endl;
  ProgressBar prgbar(rows.size(), pbm);

  for(size_t k=0; k<rows.size(); k++) {
    if (rows[k]>=src.NRows())  continue;
    NextRowPtr(rdest);
    src.GetCstRowPtr(rows[k],rsrc);
    rdest.CopyContentFrom(rsrc);
    prgbar.update(k);
  }
  return;
}

/*!
  If the table has no columns, the table structure (columns) is initialized with the 
  list of columns of the \b src table identified by the column indexes \b cols[i], otherwise the 
  compatibility of the table structure with the selected columns of table \b src 
  is checked. An exception is generated if the two structures are different. 
  The selection of rows identified by the row indexes in the \b rows vector 
  from table \b src is then copied or appended to the target table (*this). 

  \warning: this method is NOT thread-safe
*/
void BaseDataTable::CopySelectedRowColumns(BaseDataTable const& src, std::vector<size_t> const & rows,  std::vector<size_t> const & cols, ProgressBarMode pbm)
{
  if ((src.NVar() == 0)||(src.NRows() == 0)) {
    cout << "BaseDataTable::CopySelectedRowColumns(src,rows,cols)/Warning: src table not initialized or empty" << endl;
    return;
  }
  if (NCols()>0) {
    bool fgds=false;
    if (NCols()!=cols.size())  fgds=true;
    for (size_t ii=0; ii<cols.size(); ii++)
      if ( (mNames[ii].type != src.mNames[cols[ii]].type) || 
	   (mNames[ii].vecsz != src.mNames[cols[ii]].vecsz) ) { fgds=true; break; }
    if (fgds) 
      throw ParmError("BaseDataTable::CopySelectedRowColumns(src,rows,cols) incompatible table structure with selected columns");
  }
  else {
    for (size_t ii=0; ii<cols.size(); ii++) {
      size_t i = cols[ii];
      if (i>=src.NVar())  continue;
      AddColumn(src.mNames[i].type, src.mNames[i].nom, src.mNames[i].vecsz);
      bool sdone=false;
      Units un=src.GetUnits(i,sdone);
      if (sdone) SetUnits(ii,un);
    }
  }
  DataTableRowPtr  rdest = EmptyRowPtr();
  DataTableRowPtr  rsrc = src.EmptyRowPtr();
  vector< pair<size_t, size_t> > cid;
  size_t jd=0;
  for (size_t i=0; i<cols.size(); i++)  { 
    if (cols[i]>=src.NVar())  continue;
    cid.push_back( pair<size_t, size_t> (cols[i],jd) );
    jd++;
  }

  if (pbm!=ProgBarM_None) 
    cout << "BaseDataTable::CopySelectedRowColumns(src,rows,cols) "<<rows.size()<<" rows from a total of "<<src.NRows()<<endl;
  ProgressBar prgbar(rows.size(), pbm);

  for(size_t k=0; k<rows.size(); k++) {
    //DBG cout << " *DBG*Select()- k=" << k << " rows[k]= " << rows[k] << endl;
    if (rows[k]>=src.NRows())  continue;
    NextRowPtr(rdest);
    src.GetCstRowPtr(rows[k],rsrc);
    rdest.CopyContentFrom(rsrc,cid);
    prgbar.update(k);
  }
  return;
}

/* --Methode-- */
//! \sa BaseDataTable::ColumnMerge(BaseDataTable const& src, std::vector<size_t> const & cols, const char* prefix) 
void BaseDataTable::ColumnMerge(BaseDataTable const& src, const char* prefix)
{
  if (src.NVar() == 0) {
    cout << "BaseDataTable::ColumnMerge(src)/Warning: src table not initialized" << endl;
    return;
  }  
  std::vector<size_t> cols(src.NVar());
  for(size_t i=0; i<cols.size(); i++)  cols[i]=i;
  return ColumnMerge(src, cols, prefix);
}

/*!
  \param src : Table from which column structure and data is copied to the target object (*this)
  \param cols : list of column indices in table \b src to be appended (merged) to  the target table (*this)
  \param prefix : prefix string, prepended to the \b src column names for columns added to the target table
  \warning: this method is NOT thread-safe
*/

void BaseDataTable::ColumnMerge(BaseDataTable const& src, std::vector<size_t> const & cols, const char* prefix)
{
  if (src.NVar() == 0) {
    cout << "BaseDataTable::ColumnMerge(src)/Warning: src table not initialized" << endl;
    return;
  }
  for (size_t ii=0; ii<cols.size(); ii++) {
    size_t i = cols[ii];
    if (i>=src.NVar())  {
      cout << "BaseDataTable::ColumnMerge(src, cols...)/Warning: column index cols["<<ii<<"]="<<i
	   <<" out of range (>src.NVar())"<<endl;
      continue;
    }
    string cname = prefix+src.mNames[i].nom;
    //DBG    cout << " BaseDataTable::ColumnMerge()/DBG - Calling AddColumn(..,name="<<cname<<" , ...)"<<endl;
    AddColumn(src.mNames[i].type, cname, src.mNames[i].vecsz, true);
    bool sdone=false;
    Units un=src.GetUnits(i,sdone);
    size_t i0=NVar()-1;
    if (sdone) SetUnits(i0,un);
    //DBG    cout << " BaseDataTable::ColumnMerge()/DBG-2 - Calling FillColumn("<<i0<<"...,"<<i<<")"<<endl;
    FillColumn(i0, src, i);
  }
  return;
}

/* --Methode-- */
//! \sa BaseDataTable::ColumnMerge(BaseDataTable const& src, std::vector<size_t> const & cols, const char* prefix) 
void BaseDataTable::ColumnMerge(BaseDataTable const& src, std::vector<std::string> const & colsnm, const char* prefix)
{
  if (src.NVar() == 0) {
    cout << "BaseDataTable::ColumnMerge(src, vector<string> colsnm)/Warning: src table not initialized" << endl;
    return;
  }  
  std::vector<size_t> cols;
  for(size_t i=0; i<colsnm.size(); i++) {  
    int idx = ColumnIndexNIU(colsnm[i]);
    if (idx < 0)  {
      cout << "BaseDataTable::ColumnMerge(src, colsnm...)/Warning: unknown column name colsnm["<<i<<"]="
	   <<colsnm[i]<<endl;
      continue;
    }
    cols.push_back((size_t)idx);
  }
  return ColumnMerge(src, cols, prefix);
}


/*!
   Merge columns from the \b src table, adding data from selected columns to the target table (*this)

  \param src : Table from which column structure and data is copied to the target object (*this)
  \param rows : selection of the rows (row indexes) in the \b src table 
  \param cols : selection of the columns (column indexes) in the \b src  added to the target table (*this)
  \param prefix : prefix string, prepended to the \b src column names for columns added to the  target table
  \param defval : default values for missing data (number of selected rows in \b src < this->NRows())
  \param defstr : default string value for missing data
  \param pbm : ProgressBarMode flag 

  \warning: this method is NOT thread-safe
*/
void BaseDataTable::ColumnMerge(BaseDataTable const& src, std::vector<size_t> const & rows,  std::vector<size_t> const & cols,
				const char* prefix, int_8 defval, const char* defstr, ProgressBarMode pbm)
{
  if ((src.NVar() == 0)||(src.NRows() == 0)) {
    cout << "BaseDataTable::ColumnMerge(src,rows,cols)/Warning: src table not initialized or empty" << endl;
    return;
  }
  size_t nvar0=NVar();
  for (size_t ii=0; ii<cols.size(); ii++) {
    size_t i = cols[ii];
    if (i>=src.NVar())  continue;
    string cname = prefix+src.mNames[i].nom;
    AddColumn(src.mNames[i].type, cname, src.mNames[i].vecsz, true);
    bool sdone=false;
    Units un=src.GetUnits(i,sdone);
    if (sdone) SetUnits(ii+nvar0,un);
  }
  DataTableRowPtr  rdest = EmptyRowPtr();
  DataTableRowPtr  rsrc = src.EmptyRowPtr();
  vector< pair<size_t, size_t> > cid;
  size_t jd=nvar0;
  for (size_t i=0; i<cols.size(); i++)  { 
    if (cols[i]>=src.NVar())  continue;
    cid.push_back( pair<size_t, size_t> (cols[i],jd) );
    jd++;
  }

  if (pbm!=ProgBarM_None) 
    cout << "BaseDataTable::ColumnMerge(src,rows,cols) "<<rows.size()<<" rows from a total of "<<src.NRows()<<endl;
  ProgressBar prgbar(rows.size(), pbm);

  string defs = defstr;
  complex<r_8> defz(0.,0.); 
  TimeStamp defts;
  for(size_t k=0; k<rows.size(); k++) {
    GetRowPtr(k,rdest);
    if (rows[k]<src.NRows()) {
      src.GetCstRowPtr(rows[k],rsrc);
      rdest.CopyContentFrom(rsrc,cid);
    }
    else {
      for(size_t j=nvar0; j<NVar(); j++) {
	for(size_t l=0; l<GetColumnVecSize(j); l++)  {
	  rdest(j,l)=defts; rdest(j,l)=defz; 
	  rdest(j,l)=defs;  rdest(j,l)=defval;
	}
      }
    }
    prgbar.update(k);
  }
  return;
}


//! Returns the associated DVList object
DVList&  BaseDataTable::Info() const
{
  if (mInfo == NULL)  mInfo = new DVList;
  return(*mInfo);
}

/*!
  Formatted (text) output of the table, for lines lstart <= l_index < lend , with step lstep
  \param os : output stream (formatted output)
  \param lstart : start row (line) index
  \param lend : end row (line) index 
  \param lstep : row index increment
  \param qstr : if true, cells with string content will be enclosed in quotes \t 'string_content'  
  \param sep : character string used to separate fields (cells/column) on each line 
  \param clm : character string used to mark comment lines (beginning of the line) 
*/
ostream& BaseDataTable::Print(ostream& os, size_t lstart, size_t lend, size_t lstep, bool qstr, const char* sep, const char* clm) const
{
  os << clm << "#### BaseDataTable::Print() - Table(NRow=" << NEntry() << " , NCol=" 
     << NVar() << ") ##### " << endl;
  os << clm << "! " ;
  for (size_t i=0; i<NVar(); i++) {
    string nom = mNames[i].nom;
    nom += ':'; nom += ColTypeToString(mNames[i].type);
    os << setw(12) << nom << " ";
  }
  os << endl;
  os << clm << "##########################################################################" << endl;
  if (lend>NRows()) lend=NRows();
  if (lstep<1) lstep=1;
  for (size_t l=lstart; l<lend; l+=lstep)
    os << TableRowToString(l, qstr, sep) << endl;
  return os;
}

/*! In addition to printing the number of entries and column names, 
  this method prints also minimum/maximum value for each column. 
  This information might be computed when the Show() method is called.
  This may take some time for tables with large number of entries (>~ 10^6)
*/
void   BaseDataTable::Show(ostream& os) const 
{
  os << "BaseDataTable: NVar= " << NVar() << " NEnt= " << NEntry() 
     << " ( SegSize= " << SegmentSize() << "  NbSegments= " 
     << NbSegments() << " )" << endl;
  if (!mShowMinMaxFg) os << " Warning - min/max values not get/computed ! " << endl;
  os << "------------------------------------------------------------------------------" << endl;
  os << setw(3) << "i" << ":" << setw(15) << " Name" << " [Sz] (Typ) | "  
     << setw(12) << " Min " << " | " << setw(12) << " Max " << " | " 
     << setw(12) << " Units" << endl;
  os << "------------------------------------------------------------------------------" << endl;
  string smin, smax; 
  smin=smax="-";

  for(size_t i = 0 ; i < NVar() ; i++) {    
    string const& nom=mNames[i].nom;
    string vnom="";
    char tampon_vsz[32];
    bool fglongname=false;
    if (nom.length() > 15)  fglongname=true;
    if (mNames[i].vecsz>1)  sprintf(tampon_vsz,"[%2d]",(int)mNames[i].vecsz);
    else strcpy(tampon_vsz,"    ");
    string sunits;
    bool sdone;
    Units un = GetUnits(i, sdone);
    if (sdone) sunits=un.ShortName();
    if ( (mShowMinMaxFg)&&(mNames[i].type!=StringField)&&(mNames[i].type!=FMLStr16Field)&&(mNames[i].type!=FMLStr64Field)&&
	 (mNames[i].type!=FlagVec16Field)&&(mNames[i].type!=FlagVec64Field) )   { 
      MuTyV mtvmin(0.), mtvmax(0.);
      if (mNames[i].type==CharacterField)  {
	char min, max;
	GetMinMax(i, min, max) ;
	mtvmin=min;  mtvmax=max;
	//DBG	cout<<"\n **DBG**CharacterField min="<<min<<" max="<<max<<" ->int:"<<(int)min<<","<<(int)max<<endl;
      }
      else if ((mNames[i].type==IntegerField)||(mNames[i].type==LongField)) {
	int_8 min, max ;
	GetMinMax(i, min, max) ;
	mtvmin=min;  mtvmax=max; 
	//DBG	cout<<"\n **DBG**Int/LongField min="<<min<<" max="<<max<<endl;
      }
      else {
	r_8 min, max ;
	GetMinMax(i, min, max) ;
	mtvmin=min;  mtvmax=max; 
	//DBG	cout<<"\n **DBG**OtherField min="<<min<<" max="<<max<<endl;
      }
      mtvmin.Convert(smin);  mtvmax.Convert(smax);
    }
    else {
      smin=smax="-";
    }

    os << setw(3) << i << ": " << setw(15) << (fglongname?nom.substr(0,15):nom) <<  tampon_vsz
       << " (" << setw(3) << ColTypeToString(mNames[i].type) << ") | " 
       << setw(12) << smin << " | " << setw(12) << smax << " | " << setw(14) << sunits 
       << (fglongname?nom:vnom) << endl;
    
  }
  os << "------------------------------------------------------------------------------" << endl;
  return;
}

//! Fills table from an ascii (text) file
/*
  Return number of non empty lines (rows added to table)
  \param is : input ascii (text) stream
  \param sepc : character(s) separator that delimit the different fields (cells/columns) in each line of the file 
  if no separator specified (sepc=NULL, default) the tab and space characters are used as separators. Up to three 
  different character separator can be specified. 
  \param com : comment line marker. Lines starting with one of the comment characters (up to three) are  
  considered as comment lines and skipped. if no com specified (com=NULL, default) # is the marker of  
  comment lines. 

  \warning Provide a null terminated string if you specify \t sepc and/or \t comc 
*/
size_t  BaseDataTable::FillFromASCIIStream(istream& is, double defval, const char* sepc, const char* com)
{
  // defining default separator
  const char * sep=" \t";
  if ((sepc!=NULL)&&(sepc[0]!='\0'))  sep=sepc;
  // the comment line marker
  char clm[4]={'\0','\0','\0','\0'};  // comment characters 
  if ((com==NULL)||(com[0]=='\0'))   clm[0]='#';  
  else {
    clm[0]=com[0]; 
    if (com[1]!='\0')  { 
      clm[1]=com[1];
      if (com[2]!='\0')  clm[2]=com[2];
    }
  }

  string str;
  if (mVarMTV == NULL) mVarMTV = new MuTyV[NVar()];
  size_t iv, nl;
  nl = 0;
  while (!is.eof()) {
    str = "";
    getline(is, str);
    if (is.good() || is.eof()) {
      size_t l = str.length();
      if ((l==0)||(str[0]==clm[0])||(str[1]==clm[1])||(str[2]==clm[2])) continue;
      for(iv=0; iv<NVar(); iv++) mVarMTV[iv] = defval;
      iv = 0;
      size_t q = 0;
      size_t p = 0;
      while ( (q < l) && (iv < NVar()) ) {
	p = str.find_first_not_of(sep,q);
	if (p >= l)  break;
	if (str[p] == '\'')    {  // Decodage d'un string
	  q = str.find('\'',p+1);
	  if (q < l)  { 
	    mVarMTV[iv] = str.substr(p+1,q-p-1);
	    q++;
	  }
	  else mVarMTV[iv] = str.substr(p+1,l-p-1);
	  iv++;
	}
	else {
	  q = str.find_first_of(sep,p);
	  if (q > l) q = l;
	  mVarMTV[iv] = str.substr(p,q-p);
	  iv++;
	}
	if (mNames[iv-1].type == DateTimeField) {
	  string tts = (string)mVarMTV[iv-1];
	  mVarMTV[iv-1] = TimeStamp(tts);
	}
      }
      AddRow(mVarMTV);
      nl++;
    }
  }  // Fin boucle lignes fichier
  cout << "BaseDataTable::FillFromASCIIFile()/Info: " << nl << " lines decoded from stream " << endl; 
  return(nl);
}

/*!
  - fgforce == true, force recomputing the values, 
  - fgforce == false, does not recompute if number of entry has not changed since last call to ComputeMinMaxSum()
*/
void BaseDataTable::ComputeMinMaxSum(size_t k, bool fgforce)  const
{
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::ComputeMinMaxSum() out of range column index k");
  r_8 min=9.e39;
  r_8 max=-9.e39;
  r_8 sum=0., sumsq=0.;
  char minc='\0', maxc='\0';
  int_4 mini=0, maxi=0;
  int_8 minl=0, maxl=0;

  if (mMinMaxNEnt.size() < NVar()) {
    mMin.clear();
    mMax.clear();
    mSum.clear();
    mSumSq.clear();
    mMinMaxNEnt.clear();
    for(size_t kk=0; kk<NVar(); kk++) {
      mMin.push_back(9.e39);
      mMax.push_back(-9.e39);
      mSum.push_back(0.);
      mSumSq.push_back(0.);
      mMinMaxNEnt.push_back(0);
    }
  }
  if (mMinC.size() < mCColsP.size()) {
    mMinC.clear(); mMaxC.clear(); 
    for(size_t kk=0; kk<mCColsP.size(); kk++)  {
      mMinC.push_back('\0');  mMaxC.push_back('\0');  
    } 
  }
  if (mMinI.size() < mIColsP.size()) {
    mMinI.clear(); mMaxI.clear(); 
    for(size_t kk=0; kk<mIColsP.size(); kk++)  {
      mMinI.push_back(0);  mMaxI.push_back(0);  
    } 
  }
  if (mMinL.size() < mLColsP.size()) {
    mMinL.clear(); mMaxL.clear(); 
    for(size_t kk=0; kk<mLColsP.size(); kk++)  {
      mMinL.push_back(0);  mMaxL.push_back(0);  
    } 
  }
  if (NRows() < 1)  return;
  if ((mMinMaxNEnt[k] == NRows()) && !fgforce)  return;
  // We recompute values 
  size_t sk = mNames[k].ser;
  size_t nitem = mNames[k].vecsz*NEntry();
  
  size_t cnt = 0;
  switch (mNames[k].type) {  
  case IntegerField :
    mini=maxi=0;
    for(size_t is=0; is<mIColsP[sk]->NbSegments(); is++) {
      const int_4* sp = mIColsP[sk]->GetCstSegment(is);
      for(size_t n=0; n<mIColsP[sk]->SegmentSize(); n++) {
	int_4 ci=sp[n];
	if (cnt==0)  { mini=maxi=ci; }
	if (cnt >= nitem)  break;
	if (ci > maxi) maxi = ci;
	if (ci < mini) mini = ci;
	sum += (double)ci;  sumsq += (double)ci*(double)ci;
	cnt++;
      }
    }
    min = (double)mini;   max=(double)maxi; 
    mMinI[sk]=mini;  mMaxI[sk]=maxi;
    break;
  case LongField :
    minl=maxl=0L;
    for(size_t is=0; is<mLColsP[sk]->NbSegments(); is++) {
      const int_8* sp = mLColsP[sk]->GetCstSegment(is);
      for(size_t n=0; n<mLColsP[sk]->SegmentSize(); n++) {
	int_8 cl=sp[n];
	if (cnt==0)  { minl=maxl=cl; }
	if (cnt >= nitem)  break;
	if (cl > maxl) maxl = cl;
	if (cl < minl) minl = cl;
	sum += (double)cl;  sumsq += (double)cl*(double)cl;
	cnt++;
      }
    }
    min = (double)mini;   max=(double)maxi; 
    mMinL[sk]=minl;  mMaxL[sk]=maxl;
    break;
  case FloatField :
    for(size_t is=0; is<mFColsP[sk]->NbSegments(); is++) {
      const r_4* sp = mFColsP[sk]->GetCstSegment(is);
      for(size_t n=0; n<mFColsP[sk]->SegmentSize(); n++) {
	if (cnt >= nitem)  break;
	if (sp[n] > max) max = sp[n];
	if (sp[n] < min) min = sp[n];
	sum += sp[n];  sumsq += sp[n]*sp[n];
	cnt++;
      }
    }
    break;
  case DoubleField :
    for(size_t is=0; is<mDColsP[sk]->NbSegments(); is++) {
      const r_8* sp = mDColsP[sk]->GetCstSegment(is);
      for(size_t n=0; n<mDColsP[sk]->SegmentSize(); n++) {
	if (cnt >= nitem)  break;
	if (sp[n] > max) max = sp[n];
	if (sp[n] < min) min = sp[n];
	sum += sp[n];  sumsq += sp[n]*sp[n];
	cnt++;
      }
    }
    break;
  case ComplexField :
    for(size_t is=0; is<mYColsP[sk]->NbSegments(); is++) {
      const complex<r_4> * sp = mYColsP[sk]->GetCstSegment(is);
      for(size_t n=0; n<mYColsP[sk]->SegmentSize(); n++) {
	if (cnt >= nitem)  break;
	double xr=sp[n].real();
	if (xr > max) max = xr;
	if (xr < min) min = xr;
	sum += xr;  sumsq += xr*xr;
	cnt++;
      }
    }
    break;
  case DoubleComplexField :
    for(size_t is=0; is<mZColsP[sk]->NbSegments(); is++) {
      const complex<r_8> * sp = mZColsP[sk]->GetCstSegment(is);
      for(size_t n=0; n<mZColsP[sk]->SegmentSize(); n++) {
	if (cnt >= nitem)  break;
	double xr=sp[n].real();
	if (xr > max) max = xr;
	if (xr < min) min = xr;
	sum += xr;  sumsq += xr*xr;
	cnt++;
      }
    }
    break;
  case DateTimeField :
    for(size_t is=0; is<mTColsP[sk]->NbSegments(); is++) {
      const TimeStamp* sp = mTColsP[sk]->GetCstSegment(is);
      for(size_t n=0; n<mTColsP[sk]->SegmentSize(); n++) {
	double days = sp[n].ToDays();
	if (cnt >= nitem)  break;
	if (days > max) max = days;
	if (days < min) min = days;
	sum += sp[n];  sumsq += sp[n]*sp[n];
	cnt++;
      }
    }
    break;
  case CharacterField :
    minc=maxc='\0';
    for(size_t is=0; is<mCColsP[sk]->NbSegments(); is++) {
      const char * cp = mCColsP[sk]->GetCstSegment(is);
      for(size_t n=0; n<mCColsP[sk]->SegmentSize(); n++) {
	char cc = cp[n];
	if (cnt==0)  { minc=maxc=cc; }
	if (cnt >= nitem)  break;
	if (cc > maxc) maxc = cc;
	if (cc < minc) minc = cc;
	sum += (double)cc;  sumsq += (double)cc*(double)cc;
	cnt++;
      }
    }
    min = (double)minc;   max=(double)maxc;
    mMinC[sk]=minc;  mMaxC[sk]=maxc;
    break;

  case StringField :
  case FMLStr16Field :
  case FMLStr64Field :
  case FlagVec16Field :
  case FlagVec64Field :
    return;
    break;
  default:
    return;
    break;
  }  

  mMinMaxNEnt[k] = NEntry(); 
  mMin[k] = min;
  mMax[k] = max;
  mSum[k] = sum;
  mSumSq[k] = sumsq;
  return ; 
}

/*!
  - fgforce == true, force recomputing the values, 
  - fgforce == false, does not recompute if number of entry has not changed since last call to ComputeMinMaxSum()
*/
void BaseDataTable::ComputeMinMaxSumAll(bool fgforce)  const
{
  for(size_t k=0; k<NVar(); k++) ComputeMinMaxSum(k, fgforce);
  return;
}

void BaseDataTable::GetMinMax(size_t k, int_8& min, int_8& max)  const
{
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetMinMax(k, int_8&, int_8&) out of range column index k");
  if ((mNames[k].type!=IntegerField) && (mNames[k].type!=LongField) && (mNames[k].type!=CharacterField) ) 
    throw RangeCheckError("BaseDataTable::GetMinMaxC(k, int_8&, int_8&) NOT Integer/Long/Character Field type column");
  min=max=0L;
  ComputeMinMaxSum(k, false);
  size_t sk = mNames[k].ser;
  if (mNames[k].type==IntegerField) {
    min = (int_8)mMinI[sk];  max = (int_8)mMaxI[sk];
  }
  else if (mNames[k].type==LongField) {
    min = mMinL[sk];  max = mMaxL[sk];
  }
  else if (mNames[k].type==CharacterField) {
    min = (int_8)mMinC[sk];  max = (int_8)mMaxC[sk];
  }
  return;

}

void BaseDataTable::GetMinMax(size_t k, char& minc, char& maxc)  const
{
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetMinMax(k, char&, char&) out of range column index k");
  if (mNames[k].type!=CharacterField) 
    throw RangeCheckError("BaseDataTable::GetMinMaxC(k, char&, char&) NOT CharacterField type column");
  minc=maxc='\0';
  ComputeMinMaxSum(k, false);
  size_t sk = mNames[k].ser;
  minc = mMinC[sk];  maxc = mMaxC[sk];
  return;

}

/*!
  \param n : table row index ( 0 ... NEntry()-1) 
  \param qstr : if true , enclose strings in quotes ''
  \param sep : separates fields using \b sep
  \param fw : minimum field width
 */
string BaseDataTable::TableRowToString(size_t n, bool qstr, 
				       const char* sep, int fw) const 
{
  if (n >= NEntry())
    throw RangeCheckError("BaseDataTable::TableRowToString() out of range line index n");
  string rs;
  MuTyV rv;;
  size_t bid = n/mSegSz;
  for(size_t k=0; k<NVar(); k++) {
    size_t off = n%mSegSz;
    if (mNames[k].vecsz>1) off = off*mNames[k].vecsz;
    size_t sk = mNames[k].ser;
    switch (mNames[k].type) {
    case IntegerField :
      rv = mIColsP[sk]->GetCstSegment(bid)[off];
      break;
    case LongField :
      rv = mLColsP[sk]->GetCstSegment(bid)[off];
      break;
    case FloatField :
      rv = mFColsP[sk]->GetCstSegment(bid)[off];
      break;
    case DoubleField :
      rv = mDColsP[sk]->GetCstSegment(bid)[off];
      break;
    case ComplexField :
      rv = mYColsP[sk]->GetCstSegment(bid)[off];
      break;
    case DoubleComplexField :
      rv = mZColsP[sk]->GetCstSegment(bid)[off];
      break;
    case StringField :
      rv = mSColsP[sk]->GetCstSegment(bid)[off];
      break;
    case DateTimeField :
      rv = TimeStamp(mTColsP[sk]->GetCstSegment(bid)[off]);
      break;
    case CharacterField :
      rv = mCColsP[sk]->GetCstSegment(bid)[off];
      break;
    case FMLStr16Field : 
      rv = mS16ColsP[sk]->GetCstSegment(bid)[off].cbuff_ptr();
      break;
    case FMLStr64Field :
      rv = mS64ColsP[sk]->GetCstSegment(bid)[off].cbuff_ptr();
      break;
    case FlagVec16Field : 
      rv = mB16ColsP[sk]->GetCstSegment(bid)[off].convertToString();
      break;
    case FlagVec64Field : 
      rv = mB64ColsP[sk]->GetCstSegment(bid)[off].convertToString();
      break;
    default:
      rv = " ";
      break;
    }
    string s;
    if ( (mNames[k].type == StringField) && (qstr) ) {
      s = '\'';  s += (string)rv; s += '\''; 
    }
    else  s= (string)rv;
    size_t l = s.length();
    for(size_t ii=l; ii<fw; ii++) s += ' ';
    if (k > 0) rs += sep;
    rs += s;
  }
  return rs;
}

// 
// ------------------------------------
// ------- Interface NTuple -----------
// ------------------------------------
// 
size_t BaseDataTable::NbLines() const
{
  return(NEntry());
}

size_t BaseDataTable::NbColumns() const
{
  return(NVar());
}

r_8* BaseDataTable::GetLineD(size_t n) const 
{
  if (n >= NEntry()) 
    throw RangeCheckError("BaseDataTable::GetLineD() out of range line index n");
  if (mVarD == NULL) mVarD = new r_8[NItems_in_Row()];
  for(size_t i=0; i<NItems_in_Row(); i++) mVarD[i]=BADVAL;

  size_t bid = n/mSegSz;
  size_t off = n%mSegSz;
  size_t off2 = off;
  if (!mFgVecCol) {   // Aucune colonne de type vecteur
    for(size_t k=0; k<mIColsP.size(); k++) 
      mVarD[mIColIdx[k]] = mIColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mLColsP.size(); k++) {
      //DBG     cout << " *DBG*BaseDataTable::GetLineD() k=" << k << " bid= " << bid << " off= " << off << endl;
      mVarD[mLColIdx[k]] = mLColsP[k]->GetCstSegment(bid)[off];
    }
    for(size_t k=0; k<mFColsP.size(); k++) 
      mVarD[mFColIdx[k]] = mFColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mDColsP.size(); k++) 
      mVarD[mDColIdx[k]] = mDColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mYColsP.size(); k++) 
      mVarD[mYColIdx[k]] = mYColsP[k]->GetCstSegment(bid)[off].real();
    for(size_t k=0; k<mZColsP.size(); k++) 
      mVarD[mZColIdx[k]] = mZColsP[k]->GetCstSegment(bid)[off].real();
    for(size_t k=0; k<mSColsP.size(); k++) 
      mVarD[mSColIdx[k]] = atof(mSColsP[k]->GetCstSegment(bid)[off].c_str());
    for(size_t k=0; k<mTColsP.size(); k++) 
      mVarD[mTColIdx[k]] = mTColsP[k]->GetCstSegment(bid)[off].ToDays();
    for(size_t k=0; k<mCColsP.size(); k++) 
      mVarD[mCColIdx[k]] = (double)mCColsP[k]->GetCstSegment(bid)[off];
    for(size_t k=0; k<mS16ColsP.size(); k++) 
      mVarD[mS16ColIdx[k]] = atof(mS16ColsP[k]->GetCstSegment(bid)[off].cbuff_ptr());
    for(size_t k=0; k<mS64ColsP.size(); k++) 
      mVarD[mS64ColIdx[k]] = atof(mS64ColsP[k]->GetCstSegment(bid)[off].cbuff_ptr());
  }
  else {    // il y a des colonnes avec contenu vecteur 
    for(size_t k=0; k<mIColsP.size(); k++) {
      size_t nitems = mIColsP[k]->NbItems();
      if (nitems == 1) 	
	mVarD[mNames[mIColIdx[k]].item_in_row] = mIColsP[k]->GetCstSegment(bid)[off];
      else {
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mIColIdx[k]].item_in_row+i] = mIColsP[k]->GetCstSegment(bid)[off2+i];	  
      }
    }
    for(size_t k=0; k<mLColsP.size(); k++) {
      size_t nitems = mLColsP[k]->NbItems();
      if (nitems == 1) 
	mVarD[mNames[mLColIdx[k]].item_in_row] = mLColsP[k]->GetCstSegment(bid)[off];
      else {
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mLColIdx[k]].item_in_row+i] = mLColsP[k]->GetCstSegment(bid)[off2+i];
      }
    }
    for(size_t k=0; k<mFColsP.size(); k++) {
      size_t nitems = mFColsP[k]->NbItems();
      if (nitems == 1) 
	mVarD[mNames[mFColIdx[k]].item_in_row] = mFColsP[k]->GetCstSegment(bid)[off];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mFColIdx[k]].item_in_row+i] = mFColsP[k]->GetCstSegment(bid)[off2+i];
      }
    }
    for(size_t k=0; k<mDColsP.size(); k++) {
      size_t nitems = mDColsP[k]->NbItems();
      if (nitems == 1) 
	mVarD[mNames[mDColIdx[k]].item_in_row] = mDColsP[k]->GetCstSegment(bid)[off];
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mDColIdx[k]].item_in_row+i] = mDColsP[k]->GetCstSegment(bid)[off2+i];
      }
    }
    for(size_t k=0; k<mYColsP.size(); k++) {
      size_t nitems = mYColsP[k]->NbItems();
      if (nitems == 1) 
	mVarD[mNames[mYColIdx[k]].item_in_row] = mYColsP[k]->GetCstSegment(bid)[off].real();
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mYColIdx[k]].item_in_row+i] = mYColsP[k]->GetCstSegment(bid)[off2+i].real();
      }
    }
    for(size_t k=0; k<mZColsP.size(); k++) {
      size_t nitems = mZColsP[k]->NbItems();
      if (nitems == 1) 
	mVarD[mNames[mZColIdx[k]].item_in_row] = mZColsP[k]->GetCstSegment(bid)[off].real();
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mZColIdx[k]].item_in_row+i] = mZColsP[k]->GetCstSegment(bid)[off2+i].real();
      }
    }
    for(size_t k=0; k<mSColsP.size(); k++) {
      size_t nitems = mSColsP[k]->NbItems();
      if (nitems == 1) 
	mVarD[mNames[mSColIdx[k]].item_in_row] = atof(mSColsP[k]->GetCstSegment(bid)[off].c_str());
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mSColIdx[k]].item_in_row+i] = atof(mSColsP[k]->GetCstSegment(bid)[off2+i].c_str());
      }
    }
    for(size_t k=0; k<mTColsP.size(); k++) {
      size_t nitems = mTColsP[k]->NbItems();
      if (nitems == 1) 
	mVarD[mNames[mTColIdx[k]].item_in_row] = mTColsP[k]->GetCstSegment(bid)[off].ToDays();
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mTColIdx[k]].item_in_row+i] = mTColsP[k]->GetCstSegment(bid)[off2+i].ToDays();
      }
    }
    for(size_t k=0; k<mCColsP.size(); k++) {
      size_t nitems = mCColsP[k]->NbItems();
      if (nitems == 1) 	
	mVarD[mNames[mCColIdx[k]].item_in_row] = (double)mCColsP[k]->GetCstSegment(bid)[off];
      else {
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mCColIdx[k]].item_in_row+i] = (double)mCColsP[k]->GetCstSegment(bid)[off2+i];	  
      }
    }
    for(size_t k=0; k<mS16ColsP.size(); k++) {
      size_t nitems = mS16ColsP[k]->NbItems();
      if (nitems == 1) 
	mVarD[mNames[mS16ColIdx[k]].item_in_row] = atof(mS16ColsP[k]->GetCstSegment(bid)[off].c_str());
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mS16ColIdx[k]].item_in_row+i] = atof(mS16ColsP[k]->GetCstSegment(bid)[off2+i].c_str());
      }
    }
    for(size_t k=0; k<mS64ColsP.size(); k++) {
      size_t nitems = mS64ColsP[k]->NbItems();
      if (nitems == 1) 
	mVarD[mNames[mS64ColIdx[k]].item_in_row] = atof(mS64ColsP[k]->GetCstSegment(bid)[off].c_str());
      else { 
	off2 = off*nitems;
	for(size_t i=0; i<nitems; i++) 
	  mVarD[mNames[mS64ColIdx[k]].item_in_row+i] = atof(mS64ColsP[k]->GetCstSegment(bid)[off2+i].c_str());
      }
    }
  }
  return mVarD;
}


r_8  BaseDataTable::GetCell(size_t n, size_t k) const 
{
  if (n >= NEntry())
    throw RangeCheckError("BaseDataTable::GetCell() out of range line index n");
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetCell() out of range column index k");
  double rv = BADVAL;
  size_t sk = mNames[k].ser;
  size_t bid = n/mSegSz;
  size_t off = n%mSegSz;
  if (mNames[k].vecsz>1) off = off*mNames[k].vecsz;
  switch (mNames[k].type) {
  case IntegerField :
    rv = mIColsP[sk]->GetCstSegment(bid)[off];
    break;
  case LongField :
    rv = mLColsP[sk]->GetCstSegment(bid)[off];
    break;
  case FloatField :
    rv = mFColsP[sk]->GetCstSegment(bid)[off];
    break;
  case DoubleField :
    rv = mDColsP[sk]->GetCstSegment(bid)[off];
    break;
  case ComplexField :
    rv = mYColsP[sk]->GetCstSegment(bid)[off].real();
    break;
  case DoubleComplexField :
    rv = mZColsP[sk]->GetCstSegment(bid)[off].real();
    break;
  case StringField :
    rv = atof(mSColsP[sk]->GetCstSegment(bid)[off].c_str());
    break;
  case DateTimeField :
    rv = mTColsP[sk]->GetCstSegment(bid)[off].ToDays();
    break;
  case CharacterField :
    rv = (double)mCColsP[sk]->GetCstSegment(bid)[off];
    break;
  case FMLStr16Field :
    rv = atof(mS16ColsP[sk]->GetCstSegment(bid)[off].cbuff_ptr());
    break;
  case FMLStr64Field :
    rv = atof(mS64ColsP[sk]->GetCstSegment(bid)[off].cbuff_ptr());
    break;
  default:
    rv = BADVAL;
    break;
  }
  return rv ; 
}


string   BaseDataTable::GetCelltoString(size_t n, size_t k) const
{
  if (n >= NEntry())
    throw RangeCheckError("BaseDataTable::GetCell() out of range line index n");
  if (k >= NVar())
    throw RangeCheckError("BaseDataTable::GetCell() out of range column index k");
  MuTyV rv;;
  size_t sk = mNames[k].ser;
  size_t bid = n/mSegSz;
  size_t off = n%mSegSz;
  if (mNames[k].vecsz>1) off = off*mNames[k].vecsz;
  switch (mNames[k].type) {
  case IntegerField :
    rv = mIColsP[sk]->GetCstSegment(bid)[off];
    break;
  case LongField :
    rv = mLColsP[sk]->GetCstSegment(bid)[off];
    break;
  case FloatField :
    rv = mFColsP[sk]->GetCstSegment(bid)[off];
    break;
  case DoubleField :
    rv = mDColsP[sk]->GetCstSegment(bid)[off];
    break;
  case ComplexField :
    rv = mYColsP[sk]->GetCstSegment(bid)[off];
    break;
  case DoubleComplexField :
    rv = mZColsP[sk]->GetCstSegment(bid)[off];
    break;
  case StringField :
    rv = mSColsP[sk]->GetCstSegment(bid)[off];
    break;
  case DateTimeField :
    rv = TimeStamp(mTColsP[sk]->GetCstSegment(bid)[off]);
    break;
  case CharacterField :
    rv = mCColsP[sk]->GetCstSegment(bid)[off];
    break;
  case FMLStr16Field :
    rv = mS16ColsP[sk]->GetCstSegment(bid)[off].c_str();
    break;
  case FMLStr64Field :
    rv = mS64ColsP[sk]->GetCstSegment(bid)[off].c_str();
    break;
  case FlagVec16Field :
    rv = mB16ColsP[sk]->GetCstSegment(bid)[off].convertToString();
    break;
  case FlagVec64Field :
    rv = mB64ColsP[sk]->GetCstSegment(bid)[off].convertToString();
    break;
  default:
    rv = " ";
    break;
  }
  return (string)rv ; 
}

void BaseDataTable::GetMinMax(size_t k, double& min, double& max)  const
{
  min = 9E39 ; max = -9E39 ; 
  ComputeMinMaxSum(k, false);
  min = mMin[k];
  max = mMax[k];
  return;
}

void BaseDataTable::GetSum(size_t k, double& sum, double& sumsq)  const
{
  sum=sumsq=0.; 
  ComputeMinMaxSum(k, false);
  sum = mSum[k];
  sumsq = mSumSq[k];
  return;
}


size_t BaseDataTable::ColumnIndex(string const& nom) const 
{
  return IndexNom(nom) ; 
}

int BaseDataTable::ColumnIndexNIU(string const & nom) const 
{
  for(size_t k=0; k<NVar(); k++) 
    if ( mNames[k].nom == nom )      return (int)k;
  // Reza:Avril 2005 : PINtuple se base sur le renvoi de -1 et pas d'une exception
  return -1;
}

string BaseDataTable::ColumnName(size_t k) const 
{
  return NomIndex(k) ; 
}


string BaseDataTable::VarList_C(const char* nomx) const 
{
  string rets="";
  size_t i;
  for(i=0; i<NVar(); i++) {
    if ( (i%5 == 0) && (i > 0) )  rets += ";";  
    if (i%5 == 0)   rets += "\ndouble "; 
    else rets += ",";
    rets += mNames[i].nom;
  }
  rets += "; \n";
  if (nomx) { 
    char buff[256];
    for(i=0; i<NVar(); i++) {
      rets += mNames[i].nom;
      rets += '=';
      
      sprintf(buff,"%s[%ld]; ",  nomx, (long)i);
      rets += buff;
      if ( (i%3 == 0) && (i > 0) )  rets += "\n"; 
    }
  }
  return(rets);
}


string BaseDataTable::LineHeaderToString() const 
{
  string rets,s;
  
  for(int i=0; i<NVar(); i++) {
    s = mNames[i].nom;
    size_t l = s.length();
    for(size_t ii=l; ii<12; ii++) s += ' ';
    if (i > 0) rets += ' ';   
    rets += s;
  }
  return(rets);  
}

/* --Methode-- */
/*!
  redefined the base class NTupleInterface method to return true, as this class implements 
  the methods returning table content and definition as string ( LineHeaderToString() LineToString() ... )
*/
bool BaseDataTable::ImplementsContentStringRepresentation() const 
{
  return true;
}

/*!
  Return a table row (line) as a string
  \sa TableRowToString()
*/
string BaseDataTable::LineToString(size_t n) const
{
  return TableRowToString(n, false);
}

bool BaseDataTable::ImplementsExtendedInterface() const
{
  return true;
}

//! Return a pointer to an array of NTupMTPointer structure, initialized with pointers to cell contents of row \b n of the table 
NTupMTPointer * BaseDataTable::GetLineMTP(size_t n) const
{
  if (n >= NEntry())
    throw RangeCheckError("BaseDataTable::GetLineMTP() out of range line index n");

  if (mVarMTP==NULL) {    // si les tableaux necessaires n'ont pas ete alloue 
    mVarMTP = new NTupMTPointer[NVar()];
    size_t nsp=0;  size_t ntm=0;
    for(size_t i=0; i<NVar(); i++) {
      FieldType ft=mNames[i].type;
      if (ft==DateTimeField)  ntm += mNames[i].vecsz;
      if ((ft==StringField)||(ft==FMLStr16Field)||(FMLStr64Field)) nsp += mNames[i].vecsz;
    }
    if (ntm > 0) mVarTMS = new r_8[ntm];
    if (nsp > 0) mVarStrP = new const char *[nsp];
  }

  size_t bid = n/mSegSz;
  size_t off0 = n%mSegSz;
  size_t idxtms=0;
  size_t idxstr=0;
  for(size_t k=0; k<mNames.size(); k++) {
    size_t sk = mNames[k].ser;
    size_t off = off0*mNames[k].vecsz;
    switch (mNames[k].type) {
    case IntegerField :
      mVarMTP[k].i4 = (mIColsP[sk]->GetCstSegment(bid)+off);
      break;
    case LongField :
      mVarMTP[k].i8 = (mLColsP[sk]->GetCstSegment(bid)+off);
      break;
    case FloatField :
      mVarMTP[k].f4 = (mFColsP[sk]->GetCstSegment(bid)+off);
    break;
    case DoubleField :
      mVarMTP[k].f8 = (mDColsP[sk]->GetCstSegment(bid)+off);
      break;
    case ComplexField :
      mVarMTP[k].z4 = (const z_cmplx_f4 *)(mYColsP[sk]->GetCstSegment(bid)+off);
      break;
    case DoubleComplexField :
      mVarMTP[k].z8 = (const z_cmplx_f8 *)(mZColsP[sk]->GetCstSegment(bid)+off);
      break;
    case StringField :
      for(size_t i=0; i<mNames[k].vecsz; i++) 
	mVarStrP[idxstr+i]=mSColsP[sk]->GetCstSegment(bid)[off+i].c_str(); 
	mVarMTP[k].sp = mVarStrP+idxstr;
	idxstr += mNames[k].vecsz;
      break;
    case DateTimeField :
      for(size_t i=0; i<mNames[k].vecsz; i++) 
	mVarTMS[idxtms+i]=mTColsP[sk]->GetCstSegment(bid)[off+i].ToDays(); 
	mVarMTP[k].f8 = mVarTMS+idxtms;
	idxtms += mNames[k].vecsz;
      break;
    case CharacterField :
      mVarMTP[k].s = (mCColsP[sk]->GetCstSegment(bid)+off);
      break;
    case FMLStr16Field :
      for(size_t i=0; i<mNames[k].vecsz; i++) 
	mVarStrP[idxstr+i]=mS16ColsP[sk]->GetCstSegment(bid)[off+i].c_str(); 
      mVarMTP[k].sp = mVarStrP+idxstr;
      idxstr += mNames[k].vecsz;
      break;
    case FMLStr64Field :
      for(size_t i=0; i<mNames[k].vecsz; i++) 
	mVarStrP[idxstr+i]=mS64ColsP[sk]->GetCstSegment(bid)[off+i].c_str(); 
      mVarMTP[k].sp = mVarStrP+idxstr;
      idxstr += mNames[k].vecsz;
      break;
    case FlagVec16Field :
      mVarMTP[k].vp = (mB16ColsP[sk]->GetCstSegment(bid)+off)->bit_array();
      break;
    case FlagVec64Field :
      mVarMTP[k].vp = (mB64ColsP[sk]->GetCstSegment(bid)+off)->bit_array();
      break;
    default:
      mVarMTP[k].vp = NULL;
      break;
    }
  }
  return mVarMTP ; 
}

/*! 
  \brief C language declaration of variables for accessing the data of a row of table
  
  Return a string with C language style declaration of variables with column names and corresponding 
  data types, initialzed from an array of NTupMTPointer, as the one that is returned by GetLineMTP()
*/ 
string BaseDataTable::VarListMTP_C(const char* nomx) const
{
  string rets="";
  char tampon[256];
  char s1[32], s2[16];
  for(size_t k=0; k<NVar(); k++) {   // boucle sur les colonnes  
    switch (mNames[k].type) {
    case IntegerField :
      strcpy(s1,"const int ");
      strcpy(s2,"i4");
      break;
    case LongField :
      strcpy(s1,"const long long ");
      strcpy(s2,"i8");
      break;
    case FloatField :
      strcpy(s1,"const float ");
      strcpy(s2,"f4");
      break;
    case DoubleField :
      strcpy(s1,"const double ");
      strcpy(s2,"f8");
      break;
    case ComplexField :
      strcpy(s1,"const struct z_cmplx_f4 ");
      strcpy(s2,"z4");
      break;
    case DoubleComplexField :
      strcpy(s1,"const struct z_cmplx_f8 ");
      strcpy(s2,"z8");
      break;
    case StringField :
    case FMLStr16Field :
    case FMLStr64Field :
      strcpy(s1,"const char * ");
      strcpy(s2,"sp");
      break;
    case DateTimeField :
      strcpy(s1,"const double ");
      strcpy(s2,"f8");
      break;
    case CharacterField :
      strcpy(s1,"const char ");
      strcpy(s2,"s");
      break;
    case FlagVec16Field :
    case FlagVec64Field :
      strcpy(s1,"const char * ");
      strcpy(s2,"vp");
    default:
      strcpy(s1,"???? ");
      strcpy(s2,"?? ");
      break;
    }

    if (mNames[k].vecsz<2) {   // colonne simple - pas de type vecteur 
      rets += s1;  rets += ' ';  rets += mNames[k].nom;
      if (nomx)  sprintf(tampon," = %s[%d].%s[0] ; \n",nomx,(int)k,s2);
      else strcpy(tampon," ; \n");
      rets += tampon;
    }
    else { // colonne avec contenu vecteur 
      rets += s1;  rets += "* ";  rets += mNames[k].nom;
      if (nomx)  sprintf(tampon," = %s[%d].%s ; \n",nomx,(int)k,s2);
      else strcpy(tampon," ; \n");
      rets += tampon;
    }
  }    
  return(rets);
}
// 
// -------------------------------------
// ------- Protected methods -----------
// -------------------------------------
// 

void  BaseDataTable::ClearP()
{
  if ( (NVar() == 0) && (NEntry() == 0)) return;
  mNEnt = 0;
  mNSeg = 0;
  if (mVarD) delete[] mVarD;
  mVarD = NULL;
  if (mVarMTV) delete[] mVarMTV;
  mVarMTV = NULL;
  if (mVarMTP) delete[] mVarMTP;
  mVarMTP = NULL;
  if (mVarTMS) delete[] mVarTMS;
  mVarTMS = NULL;
  if (mVarStrP) delete[] mVarStrP;
  mVarStrP = NULL;

  mNames.clear();
  mFgVecCol = false;
  if (mInfo) delete mInfo;
  mInfo = NULL;
  if (mThS) delete mThS;
  mThS = NULL;
  mMin.clear();
  mMax.clear();
  mMinC.clear();
  mMaxC.clear();
  mMinI.clear();
  mMaxI.clear();
  mMinL.clear();
  mMaxL.clear();
  mSum.clear();
  mSumSq.clear();  
  mMinMaxNEnt.clear();
  mIColsP.clear();
  mLColsP.clear();
  mFColsP.clear();
  mDColsP.clear();
  mYColsP.clear();
  mZColsP.clear();
  mSColsP.clear();
  mTColsP.clear();
  mCColsP.clear();
  mS16ColsP.clear();
  mS64ColsP.clear();
  mB16ColsP.clear();
  mB64ColsP.clear();

  mIColIdx.clear();
  mLColIdx.clear();
  mFColIdx.clear();
  mDColIdx.clear();
  mYColIdx.clear();
  mZColIdx.clear();
  mSColIdx.clear();
  mTColIdx.clear();
  mCColIdx.clear();
  mS16ColIdx.clear();
  mS64ColIdx.clear();
  mB16ColIdx.clear();
  mB64ColIdx.clear();
}

size_t  BaseDataTable::AddColumnBase(FieldType ft, string const & cnom, size_t vsz, size_t ser)
{
  colst col;
  col.nom = cnom;
  col.type = ft;
  col.vecsz = vsz;
  col.ser = ser;
  col.item_in_row = NItems_in_Row();
  //  cout << " *DBG* AddColumnBase(" << (int)ft << "," << cnom << " vsz=" << vsz << "  item_in_row=" << col.item_in_row << endl;
  mNames.push_back(col);
  if (col.vecsz>1) mFgVecCol = true;
  // Tableaux pour les min,max 
  mMin.push_back(9.E39);
  mMax.push_back(-9.E39);
  mSum.push_back(0.);
  mSumSq.push_back(0.);
  mMinMaxNEnt.push_back(0);

  if (NRows() == 0)  return NVar();
  // If the table is not empty, the SegDataBlock of the newly created column should be resized 
  size_t k = NVar()-1;
  size_t sk = mNames[k].ser;
  switch (mNames[k].type) {
  case IntegerField :
    for(size_t j=0; j<NbSegments(); j++)  mIColsP[sk]->Extend();
    mMinI.push_back(0);
    mMaxI.push_back(0);
    break;
  case LongField :
    for(size_t j=0; j<NbSegments(); j++)  mLColsP[sk]->Extend();
    mMinL.push_back(0L);
    mMaxL.push_back(0L);
    break;
  case FloatField :
    for(size_t j=0; j<NbSegments(); j++)  mFColsP[sk]->Extend();
    break;
  case DoubleField :
    for(size_t j=0; j<NbSegments(); j++)  mDColsP[sk]->Extend();
    break;
  case ComplexField :
    for(size_t j=0; j<NbSegments(); j++)  mYColsP[sk]->Extend();
    break;
  case DoubleComplexField :
    for(size_t j=0; j<NbSegments(); j++)  mZColsP[sk]->Extend();
    break;
  case StringField :
    for(size_t j=0; j<NbSegments(); j++)  mSColsP[sk]->Extend();
    break;
  case DateTimeField :
    for(size_t j=0; j<NbSegments(); j++)  mTColsP[sk]->Extend();
    break;
  case CharacterField :
    for(size_t j=0; j<NbSegments(); j++)  mCColsP[sk]->Extend();
    mMinC.push_back('\0');
    mMaxC.push_back('\0');
    break;
  case FMLStr16Field :
    for(size_t j=0; j<NbSegments(); j++)  mS16ColsP[sk]->Extend();
    break;
  case FMLStr64Field :
    for(size_t j=0; j<NbSegments(); j++)  mS64ColsP[sk]->Extend();
    break;
  case FlagVec16Field :
    for(size_t j=0; j<NbSegments(); j++)  mB16ColsP[sk]->Extend();
    break;
  case FlagVec64Field :
    for(size_t j=0; j<NbSegments(); j++)  mB64ColsP[sk]->Extend();
    break;
  }
  return NVar();
}

} // FIN namespace SOPHYA 

