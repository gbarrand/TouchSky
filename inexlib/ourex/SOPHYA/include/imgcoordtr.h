//------------------------------------------------------------------------------------
// SOPHYA class library - (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
// Ancillary classes for  Image<T> class - class ImgCoordTransform
// Jan 2015 - R. Ansari, J.E.Campagne, C. Magneville
//        Updated Jan 2018 
//------------------------------------------------------------------------------------

#ifndef IMGCOORDTR_H_SEEN
#define IMGCOORDTR_H_SEEN

#include "wcsdef.h"
#include "rotation3d.h"
#include "sunitpcst.h"
#include "dvlist.h"
#include "objfio.h"

namespace SOPHYA {

//--------------------------------------------------------------------------------   
//-------------------------  ImgCoordTransform class ----------------------------
//--------------------------------------------------------------------------------   

//! Class representing coordinate transformation between the image coordinates and real world coordinate system
class ImgCoordTransform : public AnyDataObj, public WCSCoordTransformInterface {
public:
  //! Default Constructor 
  ImgCoordTransform();
  //! Copy contrsuctor 
  inline ImgCoordTransform(ImgCoordTransform const& a)
    { CopyFrom(a); }
  //! Destructor 
  virtual ~ImgCoordTransform();
  //! Copy operator 
  inline ImgCoordTransform& operator = (ImgCoordTransform const& a)
    { CopyFrom(a); return *this; }
  //! Initalize the transformation parameters from WCS-FITS keywords 
  void InitFromWCSFITSKeywords(DVList const & dvl); 
  //! Fill \b \dvl object with WCS-FITS keywords 
  void FillWCSFITSKeywords(DVList & dvl) const; 
  //! define the World Coordinate System (WCS) 
  inline void setWCSId(WCSId wcsid, WCS_RS_Id wcsrsid=WCS_RS_Unknown) 
    { wcsid_=wcsid; wcsrsid_=wcsrsid; }
  //! define the reference point on the image plane 
  inline void setImageReferencePoint(double ximg0, double yimg0) 
    { img_org_[0]=ximg0; img_org_[1]=yimg0; }
  //! define the reference point in World Coordinate system (angular position on sphere)  
  void setWCSReferencePoint(LongitudeLatitude const& radec0); 
  //! return true if setWCSReferencePoint() has been called 
  inline bool isWCSRefSet() const { return fgset_wcsref_; }
  //! define the reference point on the image plane and in World Coordinate (angular position on sphere)  
  void setReferencePoint(double ximg0, double yimg0, LongitudeLatitude const& radec0);
  //! define the reference point and the scale on the local coordinate system 
  inline void setLCS(double x0, double y0, double scale, Units scu=Units())
    { lc_org_[0]=x0; lc_org_[1]=y0; lc_scale_=scale; scu_=scu; }
  //! define the angular scale on the image plane/coordinate system (pixel size in radian)
  void setImageScale(Angle xscale, Angle yscale); 
  //! define the angular scale on the image plane/coordinate system (pixel size in radian)
  inline void setImageScale(Angle scale) 
    { setImageScale(scale, scale); }
  //! define the rotation / inversion symmetry matrix 
  void setRotationSymmetryMatrix(Angle alpha, bool fginvx=false, bool fginvy=false);
  //! define explicitly the CD 2x2 imagecoord to angular plane projection matrix
  void setProjectionCDMatrix(double * CDij);
  //! Convert the image coordinates to WCS angular coordinates 
  virtual void ImageCoordToWCS(double ximg, double yimg, LongitudeLatitude& radec) const;
  //! Convert WCS coordinates to image coordinates
  virtual void WCSToImageCoord(LongitudeLatitude const& radec, double & ximg, double & yimg) const;
  //! Convert the image coordinates to the Local Coord system 
  virtual void ImageCoordToLCS(double ximg, double yimg, double& x, double& y) const;
  //! Convert the LCS Local Coord system to image coordinates 
  void LCSToImageCoord(double x, double y, double & ximg, double & yimg) const;

  //! Prints the object on \b cout (return the cout stream object)
  inline std::ostream& Print()  const  
  { return Print(std::cout); }  
  //! Prints the object on stream \b os (return the os stream object)
  virtual std::ostream& Print(std::ostream& os)  const;

  //! copy function 
  void CopyFrom(ImgCoordTransform const& a);

  friend class ObjFileIO<ImgCoordTransform>;
protected:
  WCSId  wcsid_;             //!< identify WCS coordinate system  
  WCS_RS_Id wcsrsid_;        //!< identify the WCS coordinate system convention/reference  
  LongitudeLatitude wc_radecorg_;    //!< world coordinate (in radian) right-ascension (meridian) / declination (latitude)
  bool fgset_wcsref_;        //!< if true,  setWCSReferencePoint() called 
  double img_org_[2];        //!< x,y of the reference point on the image 
  double img_scale_[2];      //!< angular scale (in radian) subtended by a single pixel along x,y 
  double lc_org_[2];         //!< x,y of the reference point on the local coordinate system  
  double lc_scale_;          //!< converting angles to local distances   
  Units  scu_;               //!< local image plane distance units 
  double alpha_;               //!< rotation angle from image (pixel) coordinates to local tangent plane angle coordinates 
  bool fginvx_, fginvy_;       //!< flags , if true, invert x (first image axis) , y (second image axis) 
  double mx_CD_[4];            //!< 2x2 matrix corresponding to the CDi_j FITS WCS convention (arbitray 2x2 matrix) 
  double mx_CD_inv_[4];        //!< Inverse of the 2x2 CDi_j FITS WCS matrix 
  bool fgset_CD_;              //!< if true, the mx_CD_ matrix has been explicitly defined 
  EulerRotation3D rot_;        //!< rotation from the local angular space to WCS 
};

//! operator << overloading - Prints the ImgPhotometricBand object on the stream \b os
inline std::ostream& operator << (std::ostream& s, ImgCoordTransform const & tr)
{  return tr.Print(s); }

//--------------------------------------------------------------------------------   
//------  PPF handler:  ObjFileIO<ImgCoordTransform> ----------------------------
//--------------------------------------------------------------------------------   

//! Writes the ImgCoordTransform object in the POutPersist stream \b os
inline POutPersist& operator << (POutPersist& os, ImgCoordTransform & obj)
{ ObjFileIO<ImgCoordTransform> fio(&obj);  fio.Write(os);  return(os); }
//! Reads the ImgCoordTransform object from the PInPersist stream \b is 
inline PInPersist& operator >> (PInPersist& is, ImgCoordTransform & obj)
{ ObjFileIO<ImgCoordTransform> fio(&obj); is.SkipToNextObject(); fio.Read(is); return(is); }


}  // End of namespace SOPHYA

#endif
