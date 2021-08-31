//-----------------------------------------------------------
// class MathArray<T> , ComplexMathArray<T>
//  Usuall mathematical functions and operations on arrays
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
//                     R. Ansari, C.Magneville   03/2000
//-----------------------------------------------------------

#ifndef MathArray_SEEN
#define MathArray_SEEN

#include "machdefs.h"
#include <math.h>
#include "tarray.h"
#include "tmatrix.h"

namespace SOPHYA {

//! Class for simple mathematical operation on arrays
template <class T>
class MathArray {
public:
  virtual ~MathArray(void) {};

// Applying a function  
  // Replaces the input array content with the result f(x)
  static TArray<T>&  ApplyFunctionInPlace(TArray<T> & a, Arr_DoubleFunctionOfX f);
  static TArray<T>&  ApplyFunctionInPlaceF(TArray<T> & a, Arr_FloatFunctionOfX f);
  // Creates a new array and fills it with f(x)
  static TArray<T>  ApplyFunction(TArray<T> const & a, Arr_DoubleFunctionOfX f, bool pack=true);
  static TArray<T>  ApplyFunctionF(TArray<T> const & a, Arr_FloatFunctionOfX f, bool pack=true);
  // Computing Mean-Sigma 
  static double     MeanSigma(TArray<T> const & a, double & mean, double & sig);
  // Tous les elements avec abs(el) <= thr sont mis a val - retourne le nb d'elements modifies
  static size_t     ZeroThresholdInPlace(TArray<T> & a, T thr, T val=0.);
  // Tous les elements avec el <= thr sont mis a val - retourne le nb d'elements modifies
  static size_t     LowThresholdInPlace(TArray<T> & a, T thr, T val);
  // Tous les elements avec el >= thr sont mis a val - retourne le nb d'elements modifies
  static size_t     HighThresholdInPlace(TArray<T> & a, T thr, T val);

};

////////////////////////////////////////////////
//  Calcul de fonctions usuelles 

// see below for g++   
/*! \ingroup TArray \fn Fabs(const TArray<T>& a)
  \brief return a packed array with the absolute value function (fabs()) applied on each array element */
template <class T>
inline TArray<T> Fabs(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, fabs) ); }

/*! \ingroup TArray \fn Sqrt(const TArray<T>& a)
  \brief return a packed array with the square root function (sqrt()) applied on each array element */
template <class T>
inline TArray<T> Sqrt(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, sqrt) ); }

/*! \ingroup TArray \fn Sin(const TArray<T>& a)
  \brief return a packed array with the sinus function (sin()) applied on each array element */
template <class T>
inline TArray<T> Sin(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, sin) ); }

/*! \ingroup TArray \fn Cos(const TArray<T>& a)
  \brief return a packed array with the cosine function (cos()) applied on each array element */
template <class T>
inline TArray<T> Cos(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, cos) ); }

/*! \ingroup TArray \fn tan(const TArray<T>& a)
  \brief return a packed array with the tangente function (tan()) applied on each array element */
template <class T>
inline TArray<T> Tan(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, tan) ); }

/*! \ingroup TArray \fn Asin(const TArray<T>& a)
  \brief return a packed array with the arcsinus function (asin()) applied on each array element */
template <class T>
inline TArray<T> Asin(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, asin) ); }

/*! \ingroup TArray \fn Acos(const TArray<T>& a)
  \brief return a packed array with the arccosine function (acos()) applied on each array element */
template <class T>
inline TArray<T> Acos(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, acos) ); }

/*! \ingroup TArray \fn Atan(const TArray<T>& a)
  \brief return a packed array with the arctangent function (atan()) applied on each array element */
template <class T>
inline TArray<T> Atan(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, atan) ); }

/*! \ingroup TArray \fn Exp(const TArray<T>& a)
  \brief return a packed array with the exponential function (exp()) applied on each array element */
template <class T>
inline TArray<T> Exp(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, exp) ); }

/*! \ingroup TArray \fn Log(const TArray<T>& a)
  \brief return a packed array with the natural logarithm (base e) function (log()) applied on each array element */
template <class T>
inline TArray<T> Log(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, log) ); }

/*! \ingroup TArray \fn Log10(const TArray<T>& a)
  \brief return a packed array with the base 10 logarithm  function (log10()) applied on each array element */
template <class T>
inline TArray<T> Log10(const TArray<T>& a)
  { MathArray<T> ma;   return( ma.ApplyFunction(a, log10) ); }


/*! \ingroup TArray \fn MeanSigma(const TArray<T>&,double&,double&)
  \brief Return \b mean and \b sigma of elements of array \b a */
template <class T>
inline double MeanSigma(const TArray<T>& a, double & mean, double & sig)
  { MathArray<T> ma;   return( ma.MeanSigma(a, mean, sig) ); }

/*! \ingroup TArray \fn Mean(const TArray<T>&)
  \brief Return \b mean of elements of array \b a */
template <class T>
inline double Mean(const TArray<T>& a)
  { MathArray<T> ma;  double mean, sig;  return( ma.MeanSigma(a, mean, sig) ); }


//! Class for simple mathematical operation on complex arrays 
template <class T>
class ComplexMathArray {
public:
  virtual ~ComplexMathArray(void) {};

// Applying a function 
  // Replaces the input array content with the result f(x)
  static TArray< std::complex<T> >& ApplyFunctionInPlace(TArray< std::complex<T> >& a, 
						     Arr_ComplexDoubleFunctionOfX f);
  // Creates a new array and fills it with f(x)
  static TArray< std::complex<T> > ApplyFunction(TArray< std::complex<T> > const & a, 
					     Arr_ComplexDoubleFunctionOfX f, bool pack=true);

  // Creates a new array and fills it with f(x)
  static TArray< std::complex<T> > FillFrom(TArray<T> const & p_real,
					TArray<T> const & p_imag, bool pack=true);

  // Returns real-imaginary part of the complex array
  static TArray<T> real(TArray< std::complex<T> >  const & a, bool pack=true);
  static TArray<T> imag(TArray< std::complex<T> >  const & a, bool pack=true);
  
  // Return norm=|z|^2 of the complex input array 
  static TArray<T> norm(TArray< std::complex<T> >  const & a, bool pack=true);
  // Return module=|z| of the complex input array 
  static TArray<T> abs(TArray< std::complex<T> >  const & a, bool pack=true);
  //! Return the module (sqrt(real^2+imag^2) = |z|) of the complex array elements - alias for abs()
  static inline TArray<T> module(TArray< std::complex<T> >  const & a, bool pack=true)  { return abs(a,pack); }
  //! Return the phase (phi , z = |z| exp (i phi)) of the array complex elements - alias for phase()
  static inline  TArray<T> arg(TArray< std::complex<T> >  const & a, bool pack=true)  { return phase(a,pack); }
  // Return phase(z)=phi , z=|z|exp(i phi) of the complex input array 
  static TArray<T> phase(TArray< std::complex<T> >  const & a, bool pack=true);  
  // complex-conjugate 
  static TArray< std::complex<T> > & conj_self(TArray< std::complex<T> > & a);
  static TArray< std::complex<T> > conj(TArray< std::complex<T> >  const & a, bool pack=true);
  static TMatrix< std::complex<T> >& hermitian_conj(TMatrix< std::complex<T> >  const & a, TMatrix< std::complex<T> > & hca);
  static TMatrix< std::complex<T> > hermitian_conj(TMatrix< std::complex<T> >  const & a);
};

/*! \ingroup TArray \fn real(const TArray< complex<T> >&)
  \brief Return the \b real part of the input complex array \b a */
template <class T>
inline TArray<T> real(const TArray< std::complex<T> >& a)
  { ComplexMathArray<T> cma;  return( cma.real(a) ); }

/*! \ingroup TArray \fn imag(const TArray< complex<T> >&)
  \brief Return the \b imaginary part of the input complex array \b a */
template <class T>
inline TArray<T> imag(const TArray< std::complex<T> >& a)
  { ComplexMathArray<T> cma;  return( cma.imag(a) ); }

/*! \ingroup TArray \fn norm(const TArray< complex<T> >&)
  \brief Return the module squared (real^2+imag^2) of the input complex array \b a */
template <class T>
inline TArray<T> norm(const TArray< std::complex<T> >& a)
  { ComplexMathArray<T> cma;  return( cma.norm(a) ); }

/*! \ingroup TArray \fn abs(const TArray< complex<T> >&)
  \brief Return the module (sqrt(real^2+imag^2)) of the input complex array \b a */
template <class T>
inline TArray<T> abs(const TArray< std::complex<T> >& a)
  { ComplexMathArray<T> cma;  return( cma.abs(a) ); }

/*! \ingroup TArray \fn conj(const TArray< complex<T> >&)
  \brief Return the  complex conjugate of the input complex array \b a */
template <class T>
inline TArray< std::complex<T> > conj(const TArray< std::complex<T> >& a)
  { ComplexMathArray<T> cma;  return( cma.conj(a) ); }

/*! \ingroup TArray \fn phase(const TArray< complex<T> >&)
  \brief Return the \b phase of the input complex array \b a */
template <class T>
inline TArray<T> phase(const TArray< std::complex<T> >& a)
  { ComplexMathArray<T> cma;  return( cma.phase(a) ); }
/*! \ingroup TArray \fn arg(const TArray< complex<T> >&)
  \brief Return the \b phase of the input complex array \b a */
template <class T>
inline TArray<T> arg(const TArray< std::complex<T> >& a)
  { ComplexMathArray<T> cma;  return( cma.phase(a) ); }

/*! \ingroup TArray \fn hermitian_conj(const TMatrix< complex<T> >&)
  \brief Return the  hermitian conjugate of the input complex matrix \b a */
template <class T>
inline TMatrix< std::complex<T> > hermitian_conj(const TMatrix< std::complex<T> >& a)
  { ComplexMathArray<T> cma;  TMatrix< std::complex<T> > hca; cma.hermitian_conj(a,hca); return hca; }

/*! \ingroup TArray \fn ComplexArray(const TArray<T> &, const TArray<T> &)
  \brief Return a complex array, with real and imaginary parts filled from the arguments  */
template <class T>
inline TArray< std::complex<T> > ComplexArray(const TArray<T> & p_real,
					 const TArray<T> & p_imag)
  { ComplexMathArray<T> cma;  return( cma.FillFrom(p_real, p_imag) ); }


} // Fin du namespace

#endif
