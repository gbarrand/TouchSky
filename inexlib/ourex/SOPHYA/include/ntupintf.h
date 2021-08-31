//-----------------------------------------------------------
// Classe NTupleInterface Interface de type NTuple   
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// R. Ansari          (C) UPS+LAL IN2P3/CNRS  1999 
//-----------------------------------------------------------

// Interface de type NTuple           Reza 05/99
// LAL-IN2P3/CNRS               CEA-DAPNIA      

#ifndef NTUPLEINTERFACE_H_SEEN
#define NTUPLEINTERFACE_H_SEEN

#include "machdefs.h"
#include <string>
#include <complex>

namespace SOPHYA {  

//! Structure declaration for NTupleInterface GetLineMTP() method 
struct z_cmplx_f4 {  float real;  float imag; };
struct z_cmplx_f8 {  double real;  double imag; };
union NTupMTPtr {
  const int * i4;    /* int_4 */
  const long long * i8;  /*  int_8 */
  const float * f4;  
  const double * f8;   
  const struct z_cmplx_f4 * z4; 
  const struct z_cmplx_f8 * z8; 
  const char * s; 
  const char * * sp;   
  const void * vp;
};
typedef union NTupMTPtr NTupMTPointer;

//------------------  NTupleInterface class ---------------------
class NTupleInterface {
public:
  			NTupleInterface(); 
  virtual		~NTupleInterface();
// Nombre de lignes du NTuple  (Nb d'entrees)  
  virtual size_t     NbLines() const ;
// Nombre de colonnes (nb de variables)
  virtual size_t     NbColumns() const ;
// La ligne numero n, sous forme d'un tableau de double
  virtual r_8 *         GetLineD(size_t n) const ;
// Cellule de la ligne n, colonne k
  virtual r_8		GetCell(size_t n, size_t k) const ;
// Cellule de la ligne n, colonne nom 
  inline  r_8		GetCell(size_t n, std::string const & nom) const 
  { return GetCell(n, ColumnIndex(nom)); }
// Cellule de la ligne n, colonne k, converti en chaine de caracteres
  virtual std::string	GetCelltoString(size_t n, size_t k) const ;
// Cellule de la ligne n, colonne nom, converti en chaine de caracteres
//! Returns the string representation of a given cell  
  inline  std::string	GetCelltoString(size_t n, std::string const & nom) const 
  { return GetCelltoString(n, ColumnIndex(nom)); }
// Min et Max pour la colonne k
  virtual void		GetMinMax(size_t k, double& min, double& max)   const ; 
// Min et Max pour la colonne nom
//! Return the minimum and maximum values for a given named column 
  inline  void		GetMinMax(std::string const & nom, double& min, double& max)   const 
  { return GetMinMax(ColumnIndex(nom), min, max); }
// Somme_val et Somme_val^2 pour la colonne k
  virtual void		GetSum(size_t k, double& sum, double& sumsq)   const ; 
// Somme_val et Somme_val^2 pour la colonne nom
//! Return the sum of values and values-squared (Sum[v], Sum[v^2]) for a given named column 
  inline  void		GetSum(std::string const & nom, double& sum, double& sumsq)   const 
  { return GetSum(ColumnIndex(nom), sum, sumsq); }
// Numero de colonne pour nom - exception si nom inexistant 
  virtual size_t        ColumnIndex(std::string const & nom)  const ;
// Numero de colonne pour nom - negatif si Nom de colonne inexistant 
  virtual int           ColumnIndexNIU(std::string const & nom)  const ;
// Nom de colonne numero k - exception si k >= NbColumns() 
  virtual std::string   ColumnName(size_t k) const;
// Declaration des variables style C
  virtual std::string	VarList_C(const char* nomx=NULL) const ;
// Return true if the derived class implements methods to return string representation of cells, rows, header ...
  virtual bool          ImplementsContentStringRepresentation() const ;
// Entete et liste de variable, pouvant servir a l'impression
  virtual std::string   LineHeaderToString() const;
// Contenu de la ligne n, pouvant servir a l'impression
  virtual std::string   LineToString(size_t n) const;  

//-- Extended interface 
  virtual bool          ImplementsExtendedInterface() const;
  virtual NTupMTPointer * GetLineMTP(size_t n) const ; 
  virtual std::string	VarListMTP_C(const char* nomx=NULL) const ;
  static  std::string   MTP_C_Declarations();
};

} // namespace SOPHYA

#endif   /*  NTUPLEINTERFACE_H_SEEN  */


