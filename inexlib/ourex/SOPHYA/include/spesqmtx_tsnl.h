#if defined ( TSNMLUPG4 ) || defined ( TSNMLUPG4_V1 ) 
/* This is a specific declaration file to cope with two level 
   name look-up scheme , introduced by gcc >= 3.4 
   THIS FILE SHOULD NOT NORMALLY BE INCLUDED IN USER PROGRAMS 
*/

using SOPHYA::SpecialSquareMatrix<T>::mType  ;
using SOPHYA::SpecialSquareMatrix<T>::mNrows  ;
using SOPHYA::SpecialSquareMatrix<T>::mElems  ;
using SOPHYA::SpecialSquareMatrix<T>::mInfo  ;

using SOPHYA::SpecialSquareMatrix<T>::SetSize  ;
using SOPHYA::SpecialSquareMatrix<T>::ReSize  ;
using SOPHYA::SpecialSquareMatrix<T>::ReSizeRow  ;

using SOPHYA::SpecialSquareMatrix<T>::Info  ;

using SOPHYA::SpecialSquareMatrix<T>::MtxType  ;
using SOPHYA::SpecialSquareMatrix<T>::NRows  ;
using SOPHYA::SpecialSquareMatrix<T>::NCols  ;
using SOPHYA::SpecialSquareMatrix<T>::Size  ;
using SOPHYA::SpecialSquareMatrix<T>::SetSeq  ;
using SOPHYA::SpecialSquareMatrix<T>::SetCst  ;
using SOPHYA::SpecialSquareMatrix<T>::Set  ;

/*
using SOPHYA::SpecialSquareMatrix<T> ;
*/

#endif  /* TSNMLUPG4 */
