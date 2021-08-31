/*    
   --- SOPHYA software - FitsIOServer module ---
   R. Ansari , 2005-2008 
   (C) UPS+LAL IN2P3/CNRS     (C) DAPNIA-SPP/CEA 
*/
#ifndef FITSBLKRW_H
#define FITSBLKRW_H

/* On a un strncpy dans le .h  -> le include qui va avec */
#include <string.h>

#include "fitsinoutfile.h"
#include "fmlstr.h"
#include "flagvector.h"

namespace SOPHYA {

/*! 
  \ingroup FitsIOServer
  \brief Template class with static methods for handling bloc data 
  read from / write to fits filesz
*/

template <class T>
class FitsBlockRW {
public:
//! Write image HDU data to a fits file 
static void WriteImageData(FitsInOutFile& fios, const T * d, size_t sz, 
			   LONGLONG * fpixel=NULL)
{
  int status = 0;
  LONGLONG fpix[15] = {1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1};
  if (fpixel == NULL) fpixel = fpix;
  T * ncd = const_cast<T *>(d);
  fits_write_pixll(fios.FitsPtr(), FitsTypes::DataType(d[0]), fpixel,
		 sz, ncd, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    char buff[32];
    fits_get_errstatus(status, buff);
    std::string msg = "FitsBlockRW<T>::WriteImageData() Error: " ;
    msg += buff;
    throw FitsIOException(msg);
  }
}

//! Read image HDU data from a fits file 
static void ReadImageData(FitsInOutFile& fios,  T * d, size_t sz, 
			  LONGLONG * fpixel=NULL)
{
  int status = 0;
  LONGLONG fpix[15] = {1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1};
  if (fpixel == NULL) fpixel = fpix;
  int anynul = 0;
  fits_read_pixll(fios.FitsPtr(), FitsTypes::DataType(d[0]), fpixel,
		 sz, NULL, d, &anynul, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    char buff[32];
    fits_get_errstatus(status, buff);
    std::string msg = "FitsBlockRW<T>::ReadImageData() Error: " ;
    msg += buff;
    throw FitsIOException(msg);
  }

}


/*! 
  Write binary/ascii HDU data to a fits file. 
  See cfitsio function fits_write_col() for more detail.
  \param colnum : table column number (starting from 1)
  \param firstrow : the write operation starting row (starting from 1)
  \param firstelem : the firstelem (for vector type columns)
  \param d : pointer to data to be written
  \param sz : number of data elements to be written
*/
static void WriteColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			    LONGLONG firstelem, const T * d, size_t sz)
{
  int status = 0;
  T * ncd = const_cast<T *>(d);
  fits_write_col(fios.FitsPtr(), FitsTypes::DataType(d[0]), colnum,
		 firstrow, firstelem, sz, ncd, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    char buff[32];
    fits_get_errstatus(status, buff);
    std::string msg = "FitsBlockRW<T>::WriteColumnData() Error: " ;
    msg += buff;
    throw FitsIOException(msg);
  }
  return;
}

/*! 
  Read binary/ascii HDU data from a fits file. 
  See cfitsio function fits_read_col() for more detail.
  \param colnum : table column number (starting from 1)
  \param firstrow : the read operation starting point (row) (starting from 1)
  \param firstelem : the firstelem (for vector type columns)
  \param d : pointer to data to be written
  \param sz : number of data elements to be read
*/
static void ReadColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			   LONGLONG firstelem, T * d, size_t sz)
{
  int status = 0;
  int anynul = 0;
  fits_read_col(fios.FitsPtr(), FitsTypes::DataType(d[0]), colnum,
		 firstrow, firstelem, sz, NULL, d, &anynul, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    char buff[32];
    fits_get_errstatus(status, buff);
    std::string msg = "FitsBlockRW<T>::ReadColumnData() Error: " ;
    msg += buff;
    throw FitsIOException(msg);
  }
  return;
}


};

//-------------------------------------------------
//------- Specialization pour std::string ---------

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class FitsBlockRW<std::string> {
public:
//! Write image HDU with string data type not supported (throws exception) 
static void WriteImageData(FitsInOutFile& fios, const std::string * d, size_t sz, 
			   LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<std::string>::WriteImageData() string data type Unsupported for image HDU");
}
//! Read image HDU with string data type not supported (throws exception) 
static void ReadImageData(FitsInOutFile& fios,  std::string * d, size_t sz, 
			  LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<std::string>::ReadImageData() string data type Unsupported for image HDU");
}

//! Write character string data to binary/ascii HDU data in a fits file. 
static void WriteColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			    LONGLONG firstelem, const std::string * d, size_t sz)
{
  int status = 0;

  long width, repeat;
  int typecode;
  char errormsgbuff[64];
  fits_get_coltype(fios.FitsPtr(), colnum, &typecode, &repeat, &width, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    fits_get_errstatus(status, errormsgbuff);
    std::string msg = "FitsBlockRW<std::string>::WriteColumnData() Error in fits_get_coltype() ";
    msg += errormsgbuff;
    throw FitsIOException(msg);
  }

  char sbuff[256];
  char * cp[4] = {sbuff, sbuff+64, sbuff+128, sbuff+192};
  if (repeat>255)  cp[0] = new char[repeat+1];
  //cout << " --- Getting in WriteColumnData<string>() colnum=" << colnum << endl;
  for(size_t kk=0; kk<sz; kk++) {
    strncpy(sbuff, d[kk].c_str(), repeat);
    //    char * cp = const_cast<char *>(d[kk].c_str());
    sbuff[repeat] = '\0';
    status = 0;
    //cout <<"DBG-Write2Fits : appel a fits_write_col() kk=" << kk << " / sz=" << sz << endl;
    fits_write_col(fios.FitsPtr(), FitsTypes::DataType(sbuff), colnum,
		   firstrow+kk, firstelem, 1, cp, &status);
    if ( status ) {
      fits_report_error(stderr, status);
      char buff[32];
      fits_get_errstatus(status, buff);
      std::string msg = "FitsBlockRW<std::string>::WriteColumnData() Error: " ;
      msg += buff;
      sprintf(buff," kk=%ld",(long)kk);  msg += buff;
      throw FitsIOException(msg);
    }
  }
  if (repeat>255)  delete[] cp[0];
  return;
}


//! Read character string data to binary/ascii HDU data in a fits file. 
static void ReadColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			   LONGLONG firstelem, std::string * d, size_t sz)
{
  int status = 0;
  int anynul = 0;

  long width, repeat;
  int typecode;
  char errormsgbuff[64];
  fits_get_coltype(fios.FitsPtr(), colnum, &typecode, &repeat, &width, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    fits_get_errstatus(status, errormsgbuff);
    std::string msg = "FitsBlockRW<std::string>::ReadColumnData() Error in fits_get_coltype() ";
    msg += errormsgbuff;
    throw FitsIOException(msg);
  }

  char sbuff[256];
  char * cp[4] = {sbuff, sbuff+64, sbuff+128, sbuff+192};
  if (repeat>255)  cp[0] = new char[repeat+1];
  //  cout << " --- Getting in ReadColumnData<string>() colnum=" << colnum << endl;
  for(size_t kk=0; kk<sz; kk++) {
    //    cout <<"DBG-ReadFrFits : appel a fits_read_col() kk=" << kk << " / sz=" << sz << endl;
    fits_read_col(fios.FitsPtr(), FitsTypes::DataType(sbuff), colnum,
		 firstrow+kk, firstelem, 1, NULL, cp, &anynul, &status);
    sbuff[repeat] = '\0'; d[kk] = sbuff;
    if ( status ) {
      fits_report_error(stderr, status);
      char buff[32];
      fits_get_errstatus(status, buff);
      std::string msg = "FitsBlockRW<std::string>::ReadColumnData() Error: " ;
      msg += buff;
      sprintf(buff," kk=%ld",(long)kk);  msg += buff;
      throw FitsIOException(msg);
    }
  }
  if (repeat>255)  delete[] cp[0];
  return;
}

};  // Fin classe   FitsBlockRW<std::string>

//-------------------------------------------------
//--------- Specialization pour String16 ----------

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class FitsBlockRW<String16> {
public:
//! Write image HDU with string data type not supported (throws exception) 
static void WriteImageData(FitsInOutFile& fios, const String16 * d, size_t sz, 
			   LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<String16>::WriteImageData() String16 data type Unsupported for image HDU");
}
//! Read image HDU with string data type not supported (throws exception) 
static void ReadImageData(FitsInOutFile& fios, String16 * d, size_t sz, 
			  LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<String16>::ReadImageData() String16 data type Unsupported for image HDU");
}

//! Write character string data to binary/ascii HDU data in a fits file. 
static void WriteColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			    LONGLONG firstelem, const String16 * d, size_t sz)
{
  int status = 0;
  char sbuff[256];
  const char * cp[4] = {sbuff, sbuff+16, sbuff+32, sbuff+48};
  // cout << " --- Getting in WriteColumnData<String16>() colnum=" << colnum << endl;
  for(size_t kk=0; kk<sz; kk++) {
    cp[0]=cp[1] = const_cast<char *>(d[kk].c_str());
    //Avec copy: strncpy(sbuff, d[kk].c_str(), 16);  sbuff[255] = '\0';
    status = 0;
    // cout <<"DBG-Write2Fits : appel a fits_write_col() kk=" << kk << " / sz=" << sz << "-> " << sbuff <<endl;
    fits_write_col(fios.FitsPtr(), FitsTypes::DataType(sbuff), colnum,
		   firstrow+kk, firstelem, 1, cp, &status);
    if ( status ) {
      fits_report_error(stderr, status);
      char buff[32];
      fits_get_errstatus(status, buff);
      std::string msg = "FitsBlockRW<std::String16>::WriteColumnData() Error: " ;
      msg += buff;
      sprintf(buff," kk=%ld",(long)kk);  msg += buff;
      throw FitsIOException(msg);
    }
  }
  return;
}


//! Read character string data to binary/ascii HDU data in a fits file. 
static void ReadColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			   LONGLONG firstelem, String16 * d, size_t sz)
{
  int status = 0;
  int anynul = 0;

  long width, repeat;
  int typecode;
  char errormsgbuff[64];
  fits_get_coltype(fios.FitsPtr(), colnum, &typecode, &repeat, &width, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    fits_get_errstatus(status, errormsgbuff);
    std::string msg = "FitsBlockRW<String16>::WriteColumnData() Error in fits_get_coltype() ";
    msg += errormsgbuff;
    throw FitsIOException(msg);
  }

  char sbuff[256];
  char * cp[4] = {sbuff, sbuff+16, sbuff+32, sbuff+48};
  if (repeat>255)  cp[0] = new char[repeat+1];
  //  cout << " --- Getting in ReadColumnData<String16>() colnum=" << colnum << endl;
  for(size_t kk=0; kk<sz; kk++) {
    //    cout <<"DBG-ReadFrFits : appel a fits_read_col() kk=" << kk << " / sz=" << sz << endl;
    fits_read_col(fios.FitsPtr(), FitsTypes::DataType(sbuff), colnum,
		 firstrow+kk, firstelem, 1, NULL, cp, &anynul, &status);
    sbuff[repeat] = '\0'; d[kk] = sbuff;
    if ( status ) {
      fits_report_error(stderr, status);
      char buff[32];
      fits_get_errstatus(status, buff);
      std::string msg = "FitsBlockRW<String16>::ReadColumnData() Error: " ;
      msg += buff;
      sprintf(buff," kk=%ld",(long)kk);  msg += buff;
      throw FitsIOException(msg);
    }
  }
  if (repeat>255)  delete[] cp[0];
  return;
}

};  // Fin classe   FitsBlockRW<String16>

//-------------------------------------------------
//--------- Specialization pour String64 ----------

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class FitsBlockRW<String64> {
public:
//! Write image HDU with string data type not supported (throws exception) 
static void WriteImageData(FitsInOutFile& fios, const String64 * d, size_t sz, 
			   LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<String64>::WriteImageData() String64 data type Unsupported for image HDU");
}
//! Read image HDU with string data type not supported (throws exception) 
static void ReadImageData(FitsInOutFile& fios, String64 * d, size_t sz, 
			  LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<String64>::ReadImageData() String64 data type Unsupported for image HDU");
}

//! Write character string data to binary/ascii HDU data in a fits file. 
static void WriteColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			    LONGLONG firstelem, const String64 * d, size_t sz)
{
  int status = 0;
  char sbuff[256];
  char * cp[4] = {sbuff, sbuff+16, sbuff+32, sbuff+48};
  //cout << " --- Getting in WriteColumnData<String64>() colnum=" << colnum << endl;
  for(size_t kk=0; kk<sz; kk++) {
    cp[0]=cp[1] = const_cast<char *>(d[kk].c_str());
    //Avec copy strncpy(sbuff, d[kk].c_str(), 64);  sbuff[255] = '\0';
    status = 0;
    //cout <<"DBG-Write2Fits : appel a fits_write_col() kk=" << kk << " / sz=" << sz << endl;
    fits_write_col(fios.FitsPtr(), FitsTypes::DataType(sbuff), colnum,
		   firstrow+kk, firstelem, 1, cp, &status);
    if ( status ) {
      fits_report_error(stderr, status);
      char buff[32];
      fits_get_errstatus(status, buff);
      std::string msg = "FitsBlockRW<std::String64>::WriteColumnData() Error: " ;
      msg += buff;
      sprintf(buff," kk=%ld",(long)kk);  msg += buff;
      throw FitsIOException(msg);
    }
  }
  return;
}


//! Read character string data to binary/ascii HDU data in a fits file. 
static void ReadColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			   LONGLONG firstelem, String64 * d, size_t sz)
{
  int status = 0;
  int anynul = 0;

  long width, repeat;
  int typecode;
  char errormsgbuff[64];
  fits_get_coltype(fios.FitsPtr(), colnum, &typecode, &repeat, &width, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    fits_get_errstatus(status, errormsgbuff);
    std::string msg = "FitsBlockRW<String64>::WriteColumnData() Error in fits_get_coltype() ";
    msg += errormsgbuff;
    throw FitsIOException(msg);
  }

  char sbuff[256];
  char * cp[4] = {sbuff, sbuff+64, sbuff+128, sbuff+192};
  if (repeat>255)  cp[0] = new char[repeat+1];
  //  cout << " --- Getting in ReadColumnData<String64>() colnum=" << colnum << endl;
  for(size_t kk=0; kk<sz; kk++) {
    //    cout <<"DBG-ReadFrFits : appel a fits_read_col() kk=" << kk << " / sz=" << sz << endl;
    fits_read_col(fios.FitsPtr(), FitsTypes::DataType(sbuff), colnum,
		 firstrow+kk, firstelem, 1, NULL, cp, &anynul, &status);
    sbuff[repeat] = '\0'; d[kk] = sbuff;
    if ( status ) {
      fits_report_error(stderr, status);
      char buff[32];
      fits_get_errstatus(status, buff);
      std::string msg = "FitsBlockRW<String64>::ReadColumnData() Error: " ;
      msg += buff;
      sprintf(buff," kk=%ld",(long)kk);  msg += buff;
      throw FitsIOException(msg);
    }
  }
  if (repeat>255)  delete[] cp[0];
  return;
}

};  // Fin classe   FitsBlockRW<String64>


//-------------------------------------------------
//--------- Specialization pour TimeStamp ----------

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class FitsBlockRW<TimeStamp> {
public:
//! Write image HDU with string data type not supported (throws exception) 
static void WriteImageData(FitsInOutFile& fios, const TimeStamp * d, size_t sz, 
			   LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<TimeStamp>::WriteImageData() TimeStamp data type Unsupported for image HDU");
}
//! Read image HDU with string data type not supported (throws exception) 
static void ReadImageData(FitsInOutFile& fios, TimeStamp * d, size_t sz, 
			  LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<TimeStamp>::ReadImageData() TimeStamp data type Unsupported for image HDU");
}

//! Write character string data to binary/ascii HDU data in a fits file. 
static void WriteColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			    LONGLONG firstelem, const TimeStamp * d, size_t sz)
{
  r_8 * dbuff = new r_8[sz];
  for(size_t k=0; k<sz; k++) dbuff[k] = d[k].ToDays();
  FitsBlockRW<r_8>::WriteColumnData(fios, colnum, firstrow, firstelem, dbuff, sz);
  delete[] dbuff;
  return;
}


//! Read character string data to binary/ascii HDU data in a fits file. 
static void ReadColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			   LONGLONG firstelem, TimeStamp * d, size_t sz)
{
  r_8 * dbuff = new r_8[sz];
  FitsBlockRW<r_8>::ReadColumnData(fios, colnum, firstrow, firstelem, dbuff, sz);
  for(size_t k=0; k<sz; k++) d[k].Set(dbuff[k]); 
  delete[] dbuff;
  return;
}

};  // Fin classe   FitsBlockRW<TimeStamp>

/*
  Pour FlagVector16 et FlagVector64 , il n'y a pas de correspondence direct 
  entre le numero de ligne (row) du fichier FITS et les numeros des elements.
  en effet, repcount = 16 (64) * ColumnVectorSize 
*/

//-----------------------------------------------------
//--------- Specialization pour FlagVector16 ----------

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class FitsBlockRW<FlagVector16> {
public:
//! Write image HDU with FlagVector (BitArray) not supported (throws exception) 
static void WriteImageData(FitsInOutFile& fios, const FlagVector16 * d, size_t sz, 
			   LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<FlagVector16>::WriteImageData() FlagVector16 data type Unsupported for image HDU");
}
//! Read image HDU with FlagVector (BitArray) not supported (throws exception) 
static void ReadImageData(FitsInOutFile& fios, FlagVector16 * d, size_t sz, 
			  LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<FlagVector16>::ReadImageData() FlagVector16 data type Unsupported for image HDU");
}

//! Write bit array (flags) to binary HDU data in a fits file - column should be of type X or B
static void WriteColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			    LONGLONG firstelem, const FlagVector16 * d, size_t sz)
{
  //  cout << " --- Getting in WriteColumnData<FlagVector16>() colnum=" << colnum << endl;
  int status = 0;
  long width, repeat;
  int typecode;
  char errormsgbuff[64];

  fits_get_coltype(fios.FitsPtr(), colnum, &typecode, &repeat, &width, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    fits_get_errstatus(status, errormsgbuff);
    std::string msg = "FitsBlockRW<FlagVector16>::WriteColumnData() Error in fits_get_coltype() ";
    msg += errormsgbuff;
    throw FitsIOException(msg);
  }
  if ((typecode!=TBIT)&&(typecode!=TBYTE)&&(typecode!=TLOGICAL))  {
    sprintf(errormsgbuff,"col=%d typcod=%d",(int)colnum,(int)typecode);
    std::string msg = "FitsBlockRW<FlagVector16>::WriteColumnData() incompatible type ";
    msg += errormsgbuff;
    throw FitsIOException(msg);    
  }

  char larray16[16];
  size_t kkstep=(repeat/16)+((repeat%16)==0?0:1);  
  char * larray = larray16;
  if (repeat>16) larray=new char[kkstep*16];
  /*--- DBG
  long nrows=0; fits_get_num_rows(fios.FitsPtr(), &nrows, &status);  
  cout<<"*DBG*FitsBlockRW<FlagVector16>::WriteColumnData() - repeat="<<repeat<<" sz="<<sz
  <<" kkstep="<<kkstep<<" firstrow="<<firstrow<<" -> nrows="<<nrows<<endl;   ---*/
  for(size_t kk=0; kk<sz; kk+=kkstep) {
    for(size_t k=0; k<kkstep; k++) {
      for(int i=0; i<16; i++) {
	larray[k*16+i]=0;
	if ((d[kk+k])[i])  larray[k*16+i]=1;
      }
    }
    fits_write_col_bit(fios.FitsPtr(), colnum, firstrow+kk/kkstep, firstelem, repeat, larray, &status);
    if ( status ) {
      fits_report_error(stderr, status);
      char buff[32];
      fits_get_errstatus(status, buff);
      std::string msg = "FitsBlockRW<std::FlagVector16>::WriteColumnData() Error: " ;
      msg += buff;
      sprintf(buff," kk=%ld",(long)kk);  msg += buff;
      throw FitsIOException(msg);
    }
  }
  if (repeat>16) delete[] larray;
  return;
}

//! Read character string data to binary/ascii HDU data in a fits file. 
static void ReadColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			   LONGLONG firstelem, FlagVector16 * d, size_t sz)
{
  //  cout << " --- Getting in ReadColumnData<FlagVector16>() colnum=" << colnum << endl;
  int status = 0;
  long width, repeat;
  int typecode;
  char errormsgbuff[64];

  fits_get_coltype(fios.FitsPtr(), colnum, &typecode, &repeat, &width, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    fits_get_errstatus(status, errormsgbuff);
    std::string msg = "FitsBlockRW<FlagVector16>::ReadColumnData() Error in fits_get_coltype() ";
    msg += errormsgbuff;
    throw FitsIOException(msg);
  }
  if ((typecode!=TBIT)&&(typecode!=TBYTE)&&(typecode!=TLOGICAL))  {
    sprintf(errormsgbuff,"col=%d typcod=%d",(int)colnum,(int)typecode);
    std::string msg = "FitsBlockRW<FlagVector16>::ReadColumnData() incompatible type ";
    msg += errormsgbuff;
    throw FitsIOException(msg);    
  }

  char larray16[16];
  size_t kkstep=(repeat/16)+((repeat%16)==0?0:1);  
  char * larray = larray16;
  if (repeat>16) larray=new char[kkstep*16];
  //DBG  cout<<"*DBG*FitsBlockRW<FlagVector16>::ReadColumnData() - repeat="<<repeat<<" sz="<<sz
  //DBG    <<" kkstep="<<kkstep<<" firstrow="<<firstrow<<endl;

  for(size_t kk=0; kk<sz; kk+=kkstep) {
    fits_read_col_bit(fios.FitsPtr(), colnum, firstrow+kk/kkstep, firstelem, repeat, larray, &status);
    if ( status ) {
      fits_report_error(stderr, status);
      char buff[32];
      fits_get_errstatus(status, buff);
      std::string msg = "FitsBlockRW<std::FlagVector16>::ReadColumnData() Error: " ;
      msg += buff;
      sprintf(buff," kk=%ld",(long)kk);  msg += buff;
      throw FitsIOException(msg);
    }
    for(size_t k=0; k<kkstep; k++) {
      d[kk+k].clearAll(); 
      for(int i=0; i<16; i++) {
	if (larray[k*16+i])  d[kk+k].set(i);
      }
    }
  }
  if (repeat>16) delete[] larray;
  return;
}

};  // Fin classe   FitsBlockRW<FlagVector16>


//-------------------------------------------------
//--------- Specialization pour FlagVector64 ----------

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
class FitsBlockRW<FlagVector64> {
public:
//! Write image HDU with FlagVector (BitArray) not supported (throws exception) 
static void WriteImageData(FitsInOutFile& fios, const FlagVector64 * d, size_t sz, 
			   LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<FlagVector64>::WriteImageData() FlagVector64 data type Unsupported for image HDU");
}
//! Read image HDU with FlagVector (BitArray) not supported (throws exception) 
static void ReadImageData(FitsInOutFile& fios, FlagVector64 * d, size_t sz, 
			  LONGLONG * fpixel=NULL)
{
  throw FitsIOException(
    "FitsBlockRW<FlagVector64>::ReadImageData() FlagVector64 data type Unsupported for image HDU");
}

//! Write bit array (flags) to binary HDU data in a fits file - column should be of type X or B
static void WriteColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			    LONGLONG firstelem, const FlagVector64 * d, size_t sz)
{
  //  cout << " --- Getting in WriteColumnData<FlagVector64>() colnum=" << colnum << endl;
  int status = 0;
  long width, repeat;
  int typecode;
  char errormsgbuff[64];

  fits_get_coltype(fios.FitsPtr(), colnum, &typecode, &repeat, &width, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    fits_get_errstatus(status, errormsgbuff);
    std::string msg = "FitsBlockRW<FlagVector64>::WriteColumnData() Error in fits_get_coltype() ";
    msg += errormsgbuff;
    throw FitsIOException(msg);
  }
  if ((typecode!=TBIT)&&(typecode!=TBYTE)&&(typecode!=TLOGICAL))  {
    sprintf(errormsgbuff,"col=%d typcod=%d",(int)colnum,(int)typecode);
    std::string msg = "FitsBlockRW<FlagVector64>::WriteColumnData() incompatible type ";
    msg += errormsgbuff;
    throw FitsIOException(msg);    
  }

  char larray64[64];
  size_t kkstep=(repeat/64)+((repeat%64)==0?0:1);  
  char * larray = larray64;
  if (repeat>64) larray=new char[kkstep*64];
  /*--- DBG
  long nrows=0; fits_get_num_rows(fios.FitsPtr(), &nrows, &status); 
  cout<<"*DBG*FitsBlockRW<FlagVector64>::WriteColumnData() - repeat="<<repeat<<" sz="<<sz
  <<" kkstep="<<kkstep<<" firstrow="<<firstrow<<" -> nrows="<<nrows<<endl; ---*/

  for(size_t kk=0; kk<sz; kk+=kkstep) {
    for(size_t k=0; k<kkstep; k++) {
      for(int i=0; i<64; i++) {
	larray[k*64+i]=0;
	if ((d[kk+k])[i])  larray[k*64+i]=1;
      }
    }

    fits_write_col_bit(fios.FitsPtr(), colnum, firstrow+kk/kkstep, firstelem, repeat, larray, &status);
    if ( status ) {
      fits_report_error(stderr, status);
      char buff[32];
      fits_get_errstatus(status, buff);
      std::string msg = "FitsBlockRW<FlagVector64>::WriteColumnData() Error: " ;
      msg += buff;
      sprintf(buff," kk=%ld",(long)kk);  msg += buff;
      throw FitsIOException(msg);
    }
  }
  if (repeat>64) delete[] larray;
  return;
}

//! Read character string data to binary/ascii HDU data in a fits file. 
static void ReadColumnData(FitsInOutFile& fios, int colnum, LONGLONG firstrow,
			   LONGLONG firstelem, FlagVector64 * d, size_t sz)
{
  //  cout << " --- Getting in ReadColumnData<FlagVector64>() colnum=" << colnum << endl;
  int status = 0;
  long width, repeat;
  int typecode;
  char errormsgbuff[64];

  fits_get_coltype(fios.FitsPtr(), colnum, &typecode, &repeat, &width, &status);
  if ( status ) {
    fits_report_error(stderr, status);
    fits_get_errstatus(status, errormsgbuff);
    std::string msg = "FitsBlockRW<FlagVector64>::ReadColumnData() Error in fits_get_coltype() ";
    msg += errormsgbuff;
    throw FitsIOException(msg);
  }
  if ((typecode!=TBIT)&&(typecode!=TBYTE)&&(typecode!=TLOGICAL))  {
    sprintf(errormsgbuff,"col=%d typcod=%d",(int)colnum,(int)typecode);
    std::string msg = "FitsBlockRW<FlagVector64>::ReadColumnData() incompatible type ";
    msg += errormsgbuff;
    throw FitsIOException(msg);    
  }

  char larray64[64];
  size_t kkstep=(repeat/64)+((repeat%64)==0?0:1);  
  char * larray = larray64;
  if (repeat>64) larray=new char[kkstep*64];
  //DBG  cout<<"*DBG*FitsBlockRW<FlagVector64>::ReadColumnData() - repeat="<<repeat<<" sz="<<sz
  //DBG    <<" kkstep="<<kkstep<<" firstrow="<<firstrow<<endl;

  for(size_t kk=0; kk<sz; kk+=kkstep) {
   fits_read_col_bit(fios.FitsPtr(), colnum, firstrow+kk/kkstep, firstelem, repeat, larray, &status);
    if ( status ) {
      fits_report_error(stderr, status);
      char buff[32];
      fits_get_errstatus(status, buff);
      std::string msg = "FitsBlockRW<FlagVector64>::ReadColumnData() Error: " ;
      msg += buff;
      sprintf(buff," kk=%ld",(long)kk);  msg += buff;
      throw FitsIOException(msg);
    }
    for(size_t k=0; k<kkstep; k++) {
      d[kk+k].clearAll(); 
      for(int i=0; i<64; i++) {
	if (larray[k*64+i])  d[kk+k].set(i);
      }
    }
  }
  if (repeat>64) delete[] larray;
  return;
}

};  // Fin classe   FitsBlockRW<FlagVector64>

} // Fin du namespace

#endif
