//------------------------------------------------------------------------------------
// Class  CExpressionEvaluator and associated classes 
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Evaluateur d'expression C   -   R. Ansari 03/2004 , Last update Sep 2019 
//------------------------------------------------------------------------------------

#ifndef CEXPREVAL_SEEN
#define CEXPREVAL_SEEN

#include "machdefs.h"
#include <iostream>
#include <string>
#include "pexceptions.h"
#include "classfunc.h"

namespace SOPHYA {

/*! 
  \ingroup SysTools
  \brief Exception class used by  CExpressionEvaluator
*/
class CExprException : public PException {
  public:
  explicit CExprException(const char * m) throw() : PException(m) {}
  explicit CExprException(const std::string& m) throw() : PException(m) {}
};

//--------------------------------------------------------------------
/*! 
  \ingroup SysTools
  \brief Base class for arithmetic expressions used by CExpressionEvaluator
  \sa CExpressionEvaluator
*/
class CExprBase {
public:
  explicit CExprBase(); 
  virtual ~CExprBase();
  virtual double Evaluate() const = 0;
  virtual bool   CheckE(std::string& errmsg) const { return true; }
  virtual void   CheckThrow(const char * msg) const;
  inline  void   CheckThrow(std::string const & msg) const { CheckThrow(msg.c_str()); }
  virtual void   Print(std::ostream& os) const = 0;
  static long    NbCreate() { return totnexp_create; }
  static long    NbDelete() { return totnexp_delete; }
protected:
  static long totnexp_create;
  static long totnexp_delete;
};


/*! 
  \ingroup SysTools
  \brief For formatted write (print) of expressions in a stream
*/
inline std::ostream& operator << (std::ostream& s, CExprBase const & ex)
  {  ex.Print(s);  return(s);  }

/*! 
  \ingroup SysTools
  \brief Interface class used by the CExpressionEvaluator class to represent a list of variables
  or named values accessible trough a pointer.

  Inherited class should define the \b GetVarPointer() method, and can optionaly redefine 
  the \b Update() method. 
  \sa CExpressionEvaluator
*/
class CE_VarListInterface {
public:
  //! To be called to update values corresponding to the pointers. Default implementation does nothing.
  virtual void Update() { return; } 
  //! Return a pointer to a double containing the value associated with \t name.  should return NULL pointer for an invalid name. 
  virtual double* GetVarPointer(std::string const& name) = 0;
};


class CE_BinExp;
class CE_FuncExp;
//---------------------------------------------------------
class CExpressionEvaluator : public CExprBase {
public:
  CExpressionEvaluator(const char* sexp, CE_VarListInterface* pvlist=NULL);
  CExpressionEvaluator(std::string const & sex, CE_VarListInterface* pvlist=NULL);

  virtual ~CExpressionEvaluator();
//! Evaluate the expression and returns the corresponding value.
  virtual double Evaluate() const;
//! Alias for Evaluate()
  inline double Value() const { return Evaluate(); }
//! Formatted output on a stream
  virtual void  Print(std::ostream& os) const;

protected:
//! Parse variable names or constants and return CE_NumberExp object.
  CExprBase* VarNameOrNumber(std::string const & s); 
//! Does the parsing and builds an CExprBase object.
  CExprBase* ParseString(int extype, std::string fname, std::string const & sex, 
		    size_t off, size_t& stop, std::string& errmsg);

  CExprBase * _exp;
  CE_VarListInterface* _varlist; 
};


/*! 
  \ingroup SysTools
  \brief A single argument function object f(x), created from the parsing of an expression using CExpressionEvaluator 
  for the variable named vnamex (intended for use in piapp)
*/
//  forward class declaration for  CE_VarListInterface to be used in CExpFunc1D , 2D, 3D 
class P_CE_VarList_4_Func;
  
class CExpFunc1D : public ClassFunc1D {
public:
  CExpFunc1D(const char * sex, const char* vnamex);
  CExpFunc1D(std::string const & sex, std::string const & vnamex);
  CExpFunc1D(CExpFunc1D const & a);
  virtual ~CExpFunc1D();
  virtual double operator()(double x) const;
  virtual ClassFunc1D* Clone() const { return new CExpFunc1D(*this); }
protected:
  void CreateFrom(const char * sex, const char* vnamex);
  std::string sex_, vnamex_;
  CExpressionEvaluator* cexpv_;
  P_CE_VarList_4_Func* vlist_;
};

/*! 
  \ingroup SysTools
  \brief A two argument function object f(x,y), created from the parsing of an expression using CExpressionEvaluator 
  for the two variables named vnamex, vnamey  (intended for use in piapp)
*/

class CExpFunc2D : public ClassFunc2D {
public:
  CExpFunc2D(const char * sex, const char* vnamex, const char* vnamey);
  CExpFunc2D(std::string const & sex, std::string const & vnamex,  std::string const & vnamey);
  CExpFunc2D(CExpFunc2D const & a);
  virtual ~CExpFunc2D();
  virtual double operator()(double x, double y) const;
  virtual ClassFunc2D* Clone() const { return new CExpFunc2D(*this); }
protected:
  void CreateFrom(const char * sex, const char* vnamex, const char* vnamey);
  std::string sex_, vnamex_, vnamey_;
  CExpressionEvaluator* cexpv_;
  P_CE_VarList_4_Func* vlist_;
};
  

/*! 
  \ingroup SysTools
  \brief A three argument function object f(x,y,z), created from the parsing of an expression using CExpressionEvaluator 
  for the three variables named vnamex, vnamey, vnamez  (intended for use in piapp)
*/

class CExpFunc3D : public ClassFunc3D {
public:
  CExpFunc3D(const char * sex, const char* vnamex, const char* vnamey, const char* vnamez);
  CExpFunc3D(std::string const & sex, std::string const & vnamex,  std::string const & vnamey, std::string const & vnamez);
  CExpFunc3D(CExpFunc3D const & a);
  virtual ~CExpFunc3D();
  virtual double operator()(double x, double y, double z) const;
  virtual ClassFunc3D* Clone() const { return new CExpFunc3D(*this); }
protected:
  void CreateFrom(const char * sex, const char* vnamex, const char* vnamey, const char* vnamez);
  std::string sex_, vnamex_, vnamey_, vnamez_;
  CExpressionEvaluator* cexpv_;
  P_CE_VarList_4_Func* vlist_;
};
  
}   // End of namespace SOPHYA

/* end of ifdef CEXPREVAL_SEEN */
#endif
