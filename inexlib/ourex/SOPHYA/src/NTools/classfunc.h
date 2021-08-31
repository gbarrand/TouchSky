//-----------------------------------------------------------
// GenericFunction --- ClassFunc1D/2D/3D ...
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Abstract definition for functions viewed as classes
// Original version:   S.Plaszczynski 29/11/02
// updated/extended : R. Ansari  June 2013, Sep 2019 
//-----------------------------------------------------------
////////////////////////////////////////////////////////////////
//this class provides an alernative to functions which can be viewed as classes
//                          S.Plaszczynski 29/11/02
//////////////////////////////////////////////////////////////////////


#ifndef CLASSFUNC_H_SEEN
#define CLASSFUNC_H_SEEN 

#include "pexceptions.h"
#include "anydataobj.h"
#include <vector>
#include <string>

namespace SOPHYA {

/*
  \class GenericFunction 
  \ingroup NTools 
  \brief Base class definition for objects representing real valued functions. 

   GenericFunction are function objects which canbe used as functions, with one or more 
   arguments (nomber of variables). They might optionaly have parameters. 
   GenericFunction are sub-classed for object with one, two or three variables 
*/
class GenericFunction {
public:
  //! Constructor definining the number of function arguments and optionaly number of function parameters 
  GenericFunction(size_t narg, size_t npar=0) : narg_(narg), npar_(npar) { }
  virtual ~GenericFunction() { }
  //! return the number of arguments of the function  f(x,y,z ...) 
  inline size_t NbArg() const { return narg_; }
  //! return the number of function parameters 
  inline size_t NbParam() const { return npar_; }
  //! Clone the function object - default implementation throws an exception - method useful for piapp
  virtual GenericFunction* CloneGenFunc() const
  {
    throw NotAvailableOperation("GenericFunction::CloneGenFunc() - not implemented !");
  }
  //! Clone the function object - default implementation throws an exception - method useful for piapp
  virtual void SetParam(std::string const & pname, double val) const
  {
    throw NotAvailableOperation("GenericFunction::SetParam() - not implemented !");
  }
protected:
  size_t narg_, npar_;
};

/*! 
  \class ClassFunc1D 
  \ingroup NTools   
  \brief Abstract interface definition for 1 D real functions  viewed as classes

  This class represents real function of a single real argument : double f(double x)
  Inheriting classes should define the double operator()(double x)
  For convenience, ClassFunc has been defined using a typedef as an alias - So ClassFunc corresponds to ClassFunc1D 
*/
class ClassFunc1D : public GenericFunction {
public:
  ClassFunc1D() : GenericFunction(1) { } 
  //! return the function value for argument x : f(x) 
  virtual double operator()(double x) const =0;
  //! Clone the 1D function object - default implementation throws an exception - method useful for piapp
  virtual ClassFunc1D* Clone() const
  {
    throw NotAvailableOperation("ClassFunc1D::Clone() - not implemented !");
  }
  virtual GenericFunction* CloneGenFunc() const 
  {
    return Clone();
  }

};

//! typedef definition of ClassFunc as ClassFunc1D for convenience
  typedef ClassFunc1D ClassFunc ;

/*! 
  \class ClassFunc2D 
  \ingroup NTools   
  \brief Abstract interface definition for 2 D real functions  viewed as classes
  Inheriting classes should define the double operator()(double x, double y)
  This class represents real function of a two real argument : double f(double x, double y)
*/
class ClassFunc2D : public GenericFunction {
public:
  ClassFunc2D() : GenericFunction(2) { } 
  //! return the function value for the 2D argument (x,y) : f(x,y) 
  virtual double operator()(double x, double y) const =0;
  //! Clone the 2D function object - default implementation throws an exception - method useful for piapp
  virtual ClassFunc2D* Clone() const
  {
    throw NotAvailableOperation("ClassFunc2D::Clone() - not implemented !");
  }
  virtual GenericFunction* CloneGenFunc() const 
  {
    return Clone();
  }
};

/*! 
  \class ClassFunc3D 
  \ingroup NTools   
  \brief Abstract interface definition for 3 D real functions  viewed as classes
  Inheriting classes should define the double operator()(double x, double y, double z)
  This class represents real function of a three real argument : double f(double x, double y, double z)
*/
class ClassFunc3D : public GenericFunction {
public:
  ClassFunc3D() : GenericFunction(3) { } 
  //! return the function value for the 3D argument (x,y,z) : f(x,y,z) 
  virtual double operator()(double x, double y, double z) const =0;
  //! Clone the 3D function object - default implementation throws an exception - method useful for piapp
  virtual ClassFunc3D* Clone() const
  {
    throw NotAvailableOperation("ClassFunc3D::Clone() - not implemented !");
  }
  virtual GenericFunction* CloneGenFunc() const 
  {
    return Clone();
  }
};

/*! 
  \class ClassFuncND 
  \ingroup NTools   
  \brief Abstract interface definition for n-D real functions  viewed as classes

  This class represents real function of a n real argument : double f(double * x)
*/
class ClassFuncND : public GenericFunction {
public:
  //! constructor,  nd defines the dimension of the function vector argument 
  ClassFuncND(size_t nd) : GenericFunction(nd) { myx_ = new double[nd]; }
  virtual ~ClassFuncND() { delete[] myx_; }
  //! return the function value for the NDim argument provided as a pointer : f(x[]) 
  virtual double operator()(double * x) const =0;
  //! return the function value for the NDim argument provided as a std::vector : f(xvec) 
  virtual double operator()(std::vector<double> const & x) const
  {
    if (x.size() != NbArg())  throw ParmError("ClassFuncND::operator(vector<double> x) Wrong x size (!= NbArg()"); 
    for(size_t i=0; i<x.size(); i++)  myx_[i]=x[i];  
    return ClassFuncND::operator()(myx_); 
  }
  //! Clone the NDim function object - default implementation throws an exception - method useful for piapp
  virtual ClassFuncND* Clone() const
  {
    throw NotAvailableOperation("ClassFuncND::Clone() - not implemented !");
  }
  virtual GenericFunction* CloneGenFunc() const 
  {
    return Clone();
  }
protected:
  double * myx_;
};

/*! 
  \class Function1D 
  \ingroup NTools   
  \brief ClassFunc constructed from a a function:simple forward to function
  For convenience, ClassFunc has been defined using a typedef as an alias
*/
//ClassFunc constructed from a a function:simple forward to function
class Function1D : public ClassFunc1D {
 private:
  double (*f_)(double);
 public:
  //! constructor from a c-function pointer 
  Function1D(double (*g)(double)):f_(g){}
  //! copy constructor 
  Function1D(Function1D const & a):f_(a.f_){}
  //! return function value at x 
  virtual double operator()(double x) const { return f_(x);}
  //! Implementation of base class Clone() method - return a pointer to the copy of the object 
  virtual ClassFunc1D* Clone() const { return new Function1D(*this); }
};

typedef Function1D Function; 

/*! 
  \class Function2D 
  \ingroup NTools   
  \brief ClassFunc2D constructed from a function:simple forward to function
*/
class Function2D : public ClassFunc2D {
 private:
  double (*f_)(double,double);
 public:
  //! constructor from a c-function pointer 
  Function2D(double (*g)(double,double)):f_(g){}
  //! copy constructor 
  Function2D(Function2D const & a):f_(a.f_){}
  //! return function value at (x,y) 
  virtual  double operator()(double x, double y) const { return f_(x,y);}
  //! Implementation of base class Clone() method - return a pointer to the copy of the object 
  virtual ClassFunc2D* Clone() const { return new Function2D(*this); }
};

/*! 
  \class Function3D 
  \ingroup NTools   
  \brief ClassFunc3D constructed from a function:simple forward to function
*/
class Function3D : public ClassFunc3D {
 private:
  double (*f_)(double,double,double);
 public:
  //! constructor from a c-function pointer 
  Function3D(double (*g)(double,double,double)):f_(g){}
  //! copy constructor 
  Function3D(Function3D const & a):f_(a.f_){}
  //! return function value at (x,y,z) 
  virtual  double operator()(double x, double y, double z) const { return f_(x,y,z);}
  //! Implementation of base class Clone() method - return a pointer to the copy of the object 
  virtual ClassFunc3D* Clone() const { return new Function3D(*this); }
};

/*! 
  \class GFuncWrapper 
  \ingroup NTools   
  \brief A wrapper class to cast a GenericFunction into a AnyDataObj object - usefull for piapp 
*/

class GFuncWrapper : public AnyDataObj {
public:
  //! constructor /default constructor : the wrapper holds the provided GenericFunction pointer 
  GFuncWrapper(GenericFunction * gf=NULL, bool adel=true);
  //! constructor from a GenericFunction reference - tries to clone gf by calling CloneGenFunc(), will hold f_Zero is CloneGenFunc() fails 
  GFuncWrapper(GenericFunction const & gf);
  //! destructor - deletes the fonction pointer of adel_ flag is set to true 
  virtual ~GFuncWrapper();
  //! return a reference to the Generic function 
  GenericFunction & getGFunc() { return *gfp_; }
  //! return the number of arguments of the generic function 
  inline size_t NbArg() const { return gfp_->NbArg(); }
  //! return a reference to a 1D function object if the GenericFunction is a ClassFunc1D, returns reference to f_Zero if not 
  ClassFunc1D & getFunc1D() { return *f1dp_; }
  //! return a reference to a 2D function object if the GenericFunction is a ClassFunc2D, returns reference to f_Zero if not 
  ClassFunc2D & getFunc2D() { return *f2dp_; }
  //! return a reference to a 3D function object if the GenericFunction is a ClassFunc3D, returns reference to f_Zero if not 
  ClassFunc3D & getFunc3D() { return *f3dp_; }
protected:
  void Init(GenericFunction * gf);
  GenericFunction* gfp_;
  bool adel_;  // true -> destructor deletes gfp_
  ClassFunc1D* f1dp_; 
  bool adel1_;  // true -> destructor deletes f1dp_
  ClassFunc2D* f2dp_; 
  bool adel2_;  // true -> destructor deletes f2dp_
  ClassFunc3D* f3dp_; 
  bool adel3_;  // true -> destructor deletes f3dp_
};

}    /* --Fin de namespace SOPHYA-- */ 

#endif   /* CLASSFUNC_H_SEEN */
