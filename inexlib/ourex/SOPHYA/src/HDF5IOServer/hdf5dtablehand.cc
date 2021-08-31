/* ---------------------------------------------------------------------
  SOPHYA class library - HDF5IOServer module
  HDF5 IO Handler for BaseDataTable objects 
   R. Ansari, C. Magneville  2018  
  (C)  UPS+LAL IN2P3/CNRS  , CEA-Irfu 
  ------------------------------------------------------------------- */

#include "hdf5handler.h"
#include "datatable.h"
#include "hdf5segdbhand.h"
#include "hdf5nametag.h"


/*! 
  \ingroup HDF5IOServer
  \brief HDF5 I/O handler for BaseDataTable objects
*/


namespace SOPHYA {

//--- declaration de fonctions utilitaires - Voir en fin de fichier pour l'implementation
//  Conversion de type de colonne FieldType en string - pour ecriture des types ds le fichier HDF5 
static bool h5_datatable_handler_coltype2str(BaseDataTable::FieldType ftyp, string & ftypstr);
//  Conversion de string en type de colonne FieldType - pour les types lus depuis le fichier HDF5 
static BaseDataTable::FieldType h5_datatable_handler_str2coltype(string const & ftypstr); 


DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
int HDF5Handler<BaseDataTable>::CheckReadability(HDF5InOutFile& is, const char* objpath)  const 
{
  HDF5IdWrapper datatype;
  HDF5IdWrapper dataspace;
  DVList dvl;
  H5O_type_t otype=is.GetObjectType_Info(objpath, dvl, datatype, dataspace);
  if (otype != H5O_TYPE_GROUP)  return 0;
  bool fgok=false;
  string clsname=dvl.GetS("SOPHYA_HDF5Handler_ClassName");
  if (clsname=="SOPHYA::HDF5Handler<BaseDataTable>")   fgok=true;
  if (fgok) return 3;
  return 0;
}

DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void HDF5Handler<BaseDataTable>::Write(HDF5InOutFile& ohs, const char* name)  const 
{
  if (dobj == NULL)  
    throw NullPtrError("HDF5Handler<BaseDataTable>::Write() dobj=NULL ");
  if ((dobj->NColumns()<1)||(dobj->NRows()<1))
    throw HDF5IOException("HDF5Handler<BaseDataTable>::Write()  Empty BaseDataTable");
  // -- save the current group 
  HDF5IdWrapper  savgrp = ohs.CurrentGroup();
  // --- create the group which will contain the BaseDataTable 
  HDF5IdWrapper  dtgrp = ohs.CreateGroup(name);
  ohs.SetCurrentGroup(dtgrp);

  MuTyV mtv=string("SOPHYA::HDF5Handler<BaseDataTable>");
  ohs.WriteAttribute(dtgrp,"SOPHYA_HDF5Handler_ClassName",mtv);
  DataTable const * dtp = dynamic_cast<DataTable const *>(dobj);
  if (dtp) mtv=string("SOPHYA::DataTable");
  else mtv=string("SOPHYA::BaseDataTable");
  ohs.WriteAttribute(dtgrp,"SOPHYA_ClassName",mtv);
  mtv=(int_8)(dobj->NColumns());
  ohs.WriteAttribute(dtgrp,"SOPHYA_BaseDataTable_NColumns",mtv);
  mtv=(int_8)(dobj->NRows());
  ohs.WriteAttribute(dtgrp,"SOPHYA_BaseDataTable_NRows",mtv);
  ohs.WriteAttribute(dtgrp,"NumberOfRows",mtv);
  mtv=(int_8)(dobj->SegmentSize());
  ohs.WriteAttribute(dtgrp,"SOPHYA_BaseDataTable_SegmentSize",mtv);
  mtv=(int_8)(dobj->NbSegments());
  ohs.WriteAttribute(dtgrp,"SOPHYA_BaseDataTable_NbSegments",mtv);

  vector<bool> vfgok(dobj->NColumns());
  size_t ncolok=0;  int_8 i=-1;  
  for(size_t k=0; k<dobj->NColumns(); k++) {
    string coltypstr;
    bool fgok = h5_datatable_handler_coltype2str(dobj->GetColumnType(k), coltypstr);
    vfgok[k]=fgok;
    if (!fgok) { 
      cerr << " HDF5Handler<BaseDataTable>::Write() Not handled column type : " << coltypstr << endl;
      continue;
    }
    i++;   ncolok++;
    char buff[64];
    sprintf(buff,"ColName_%ld",(long)i);
    mtv=dobj->GetColumnName(k);
    ohs.WriteAttribute(dtgrp,buff,mtv);
    sprintf(buff,"ColType_%ld",(long)i);
    mtv=coltypstr;
    ohs.WriteAttribute(dtgrp,buff,mtv);
    sprintf(buff,"ColVSz_%ld",(long)i);
    mtv=(int_8)dobj->GetColumnVecSize(k);
    ohs.WriteAttribute(dtgrp,buff,mtv);
  }
  mtv=(int_8)ncolok;
  ohs.WriteAttribute(dtgrp,"NumberOfColumns",mtv);

  //DBG  cout << "CCCCC*DBG*HDF5Handler<BaseDataTable>::Write()  ohs.DataSetWrite(dataset)  DONE"<<endl;
  if (dobj->Info().NVar()>0)
    ohs.WriteAttributes(dtgrp, dobj->Info());
  // Writing column data 
  i=-1;
  for(size_t k=0; k<dobj->NColumns(); k++) {
    if (!vfgok[k])  continue;
    i++;
    string coldataname="ColData_";
    coldataname+=dobj->GetColumnName(k);
    size_t sk = dobj->mNames[k].ser;
    switch (dobj->GetColumnType(k)) {
    case BaseDataTable::IntegerField :
      ohs << HDF5NameTag(coldataname) << *(dobj->mIColsP[sk]); 
      break;
    case BaseDataTable::LongField :
      ohs << HDF5NameTag(coldataname) << *(dobj->mLColsP[sk]); 
      break;
    case BaseDataTable::FloatField :
      ohs << HDF5NameTag(coldataname) << *(dobj->mFColsP[sk]); 
      break;
    case BaseDataTable::DoubleField :
      ohs << HDF5NameTag(coldataname) << *(dobj->mDColsP[sk]); 
      break;
    case BaseDataTable::ComplexField :
      ohs << HDF5NameTag(coldataname) << *(dobj->mYColsP[sk]); 
      break;
    case BaseDataTable::DoubleComplexField :
      ohs << HDF5NameTag(coldataname) << *(dobj->mZColsP[sk]); 
      break;
      /*   A COMPLETER 
	   case BaseDataTable::StringField :
	   ohs << HDF5NameTag(coldataname) << *(dobj->mSColsP[sk]); 
	   break;  */
    case BaseDataTable::DateTimeField :
      ohs << HDF5NameTag(coldataname) << *(dobj->mTColsP[sk]); 
      break;
      /*  A COMPLETER 
    case BaseDataTable::CharacterField :
      ohs << HDF5NameTag(coldataname) << *(dobj->mCColsP[sk]); 
      break; */
    case BaseDataTable::FMLStr16Field :
      ohs << HDF5NameTag(coldataname) << *(dobj->mS16ColsP[sk]); 
      break;
    case BaseDataTable::FMLStr64Field :
      ohs << HDF5NameTag(coldataname) << *(dobj->mS64ColsP[sk]); 
      break;
      /*  A COMPLETER  
	  case BaseDataTable::FlagVec16Field :
	  ohs << HDF5NameTag(coldataname) << *(dobj->mB16ColsP[sk]); 
	  break;
	  case BaseDataTable::FlagVec64Field :
	  ohs << HDF5NameTag(coldataname) << *(dobj->mB64ColsP[sk]); 
	  break;  */
    default:
      throw HDF5IOException("HDF5Handler<BaseDataTable>::Write() (DataTable) unknown column type ");
      break;
    }
  }    
// On revient au groupe de depart ds le fichier ...
  ohs.SetCurrentGroup(savgrp);
  return;
}


DECL_TEMP_SPEC  /* equivalent a template <> , pour SGI-CC en particulier */
void HDF5Handler<BaseDataTable>::Read(HDF5InOutFile& ihs, const char* objpath)
{
  H5O_type_t otype=ihs.GetObjectType(objpath);
  if (otype != H5O_TYPE_GROUP) 
    throw HDF5IOException("HDF5Handler<BaseDataTable>::Read() ERROR HDF5 object not a group");
  // -- save the current group 
  HDF5IdWrapper  savgrp = ihs.CurrentGroup();
  // --- create the group which will contain the BaseDataTable 
  HDF5IdWrapper  dtgrp = ihs.OpenGroup(objpath);
  ihs.SetCurrentGroup(dtgrp);
  DVList dvl;
  ihs.ReadAttributes(dtgrp, dvl);
  string clsname=dvl.GetS("SOPHYA_HDF5Handler_ClassName");
  if (clsname!="SOPHYA::HDF5Handler<BaseDataTable>") 
    throw HDF5IOException("HDF5Handler<BaseDataTable>::Read() ERROR No SOPHYA handler classname or handlername != HDF5Handler<BaseDataTable>");
  int_8 ncols=dvl.GetI("SOPHYA_BaseDataTable_NColumns");
  int_8 nrows=dvl.GetI("SOPHYA_BaseDataTable_NRows");
  int_8 segsz=dvl.GetI("SOPHYA_BaseDataTable_SegmentSize");
  int_8 nseg=dvl.GetI("SOPHYA_BaseDataTable_NbSegments");
  //DBG  cout << "AAA*DBG*HDF5Handler<BaseDataTable>::Read()  DONE NEntries="<<nent<<endl;

  if (dobj == NULL) {
    dobj = new DataTable(segsz);
    ownobj=true;
  }
  //---- On efface la structure et le contenu de la table pour la refaire 
  dobj->Clear();
  // On met a jour le champ segment-size ...
  dobj->mSegSz=segsz;

  size_t ncolsok=(size_t)dvl.GetI("NumberOfColumns");
  // On ajoute les colonnes 
  for(size_t k=0; k<ncolsok; k++) {
    char buff[64];
    sprintf(buff,"ColName_%ld",(long)k);
    string colname=dvl.GetS(buff);
    dvl.DeleteKey(buff);
    sprintf(buff,"ColType_%ld",(long)k);
    string coltypstr=dvl.GetS(buff);
    dvl.DeleteKey(buff);
    BaseDataTable::FieldType ftyp=h5_datatable_handler_str2coltype(coltypstr);
    sprintf(buff,"ColVSz_%ld",(long)k);
    size_t vsz=(size_t)dvl.GetI(buff,1);
    dvl.DeleteKey(buff);
    dobj->AddColumn(ftyp, colname, vsz);
  }
  dvl.DeleteKey("SOPHYA_HDF5Handler_ClassName");
  dvl.DeleteKey("SOPHYA_ClassName");
  dvl.DeleteKey("SOPHYA_BaseDataTable_NColumns");
  dvl.DeleteKey("SOPHYA_BaseDataTable_NRows");
  dvl.DeleteKey("SOPHYA_BaseDataTable_SegmentSize");
  dvl.DeleteKey("SOPHYA_BaseDataTable_NbSegments");
  dvl.DeleteKey("NumberOfRows");
  dvl.DeleteKey("NumberOfColumns");

  if (dvl.Size()>0)  dobj->Info()=dvl;
  
  //--- Reading column data 
  for(size_t k=0; k<dobj->NColumns(); k++) {
    string coldataname="ColData_";
    coldataname+=dobj->GetColumnName(k);
    size_t sk = dobj->mNames[k].ser;
    switch (dobj->GetColumnType(k)) {
    case BaseDataTable::IntegerField :
      ihs >> HDF5NameTag(coldataname) >> *(dobj->mIColsP[sk]); 
      break;
    case BaseDataTable::LongField :
      ihs >> HDF5NameTag(coldataname) >> *(dobj->mLColsP[sk]); 
      break;
    case BaseDataTable::FloatField :
      ihs >> HDF5NameTag(coldataname) >> *(dobj->mFColsP[sk]); 
      break;
    case BaseDataTable::DoubleField :
      ihs >> HDF5NameTag(coldataname) >> *(dobj->mDColsP[sk]); 
      break;
    case BaseDataTable::ComplexField :
      ihs >> HDF5NameTag(coldataname) >> *(dobj->mYColsP[sk]); 
      break;
    case BaseDataTable::DoubleComplexField :
      ihs >> HDF5NameTag(coldataname) >> *(dobj->mZColsP[sk]); 
      break;
      /*   A COMPLETER 
	   case BaseDataTable::StringField :
	   ihs >> HDF5NameTag(coldataname) >> *(dobj->mSColsP[sk]); 
	   break;
      */
    case BaseDataTable::DateTimeField :
      ihs >> HDF5NameTag(coldataname) >> *(dobj->mTColsP[sk]); 
      break;
      /*  A COMPLETER 
    case BaseDataTable::CharacterField :
      ihs >> HDF5NameTag(coldataname) >> *(dobj->mCColsP[sk]); 
      break; */
    case BaseDataTable::FMLStr16Field :
      ihs >> HDF5NameTag(coldataname) >> *(dobj->mS16ColsP[sk]); 
      break;
    case BaseDataTable::FMLStr64Field :
      ihs >> HDF5NameTag(coldataname) >> *(dobj->mS64ColsP[sk]); 
      break;
      /*  A COMPLETER  
	  case BaseDataTable::FlagVec16Field :
	  ihs >> HDF5NameTag(coldataname) >> *(dobj->mB16ColsP[sk]); 
	  break;
	  case BaseDataTable::FlagVec64Field :
	  ihs >> HDF5NameTag(coldataname) >> *(dobj->mB64ColsP[sk]); 
	  break;
      */
    default:
      throw HDF5IOException("HDF5Handler<BaseDataTable>::Read() (DataTable) unknown column type ");
      break;
      
    }
  }

  // On met a jour les champs nombre d'entrees, nb de segments ...
  dobj->mSegSz=segsz;
  dobj->mNEnt=nrows;
  dobj->mNSeg=nseg;
  // On revient au groupe de depart ds le fichier ...
  ihs.SetCurrentGroup(savgrp);
  return;
}

//  Conversion de type de colonne FieldType en string 
static bool h5_datatable_handler_coltype2str(BaseDataTable::FieldType ftyp, string & ftypstr)
{
  bool fgok=true;
  switch ( ftyp ) {
  case BaseDataTable::IntegerField :
    ftypstr="int";
    break;
  case BaseDataTable::LongField :
    ftypstr="long";
    break;
  case BaseDataTable::FloatField :
    ftypstr="float";
    break;
  case BaseDataTable::DoubleField :
    ftypstr="double";
    break;
  case BaseDataTable::DateTimeField :
    ftypstr="datetime";
    break;  
  case BaseDataTable::ComplexField :
    ftypstr="complex<float>";
    break;  
  case BaseDataTable::DoubleComplexField :
    ftypstr="complex<double>";
    break;  
  case BaseDataTable::StringField :
    ftypstr="string";
    fgok=false;
    break;
  case BaseDataTable::CharacterField :
    ftypstr="char";
    fgok=false;
    break;
  case BaseDataTable::FMLStr16Field :
    ftypstr="string16";
    break;
  case BaseDataTable::FMLStr64Field :
    ftypstr="string64";
    break;
  case BaseDataTable::FlagVec16Field :
    ftypstr="bitvector16";
    fgok=false;
    break;
  case BaseDataTable::FlagVec64Field :
    ftypstr="bitvector64";
    fgok=false;
    break;
  default:
    fgok=false;
    break;
  }
  return fgok;
}

//  Conversion de string en type de colonne FieldType
static BaseDataTable::FieldType h5_datatable_handler_str2coltype(string const & ftypstr) 
{
  BaseDataTable::FieldType ftyp=ftyp=BaseDataTable::IntegerField;
  if (ftypstr == "int")  ftyp=BaseDataTable::IntegerField;
  else if (ftypstr=="long") ftyp=BaseDataTable::LongField;
  else if (ftypstr=="float") ftyp=BaseDataTable::FloatField;
  else if (ftypstr=="double") ftyp=BaseDataTable::DoubleField;
  else if (ftypstr=="datetime") ftyp=BaseDataTable::DateTimeField;
  else if (ftypstr=="complex<float>") ftyp=BaseDataTable::ComplexField;
  else if (ftypstr=="complex<double>") ftyp=BaseDataTable::DoubleComplexField;
  else if (ftypstr=="string") ftyp=BaseDataTable::StringField;
  else if (ftypstr=="char") ftyp=BaseDataTable::CharacterField;
  else if (ftypstr=="string16") ftyp=BaseDataTable::FMLStr16Field;
  else if (ftypstr=="string64") ftyp=BaseDataTable::FMLStr64Field;
  else if (ftypstr=="bitvector16") ftyp=BaseDataTable::FlagVec16Field;
  else if (ftypstr=="bitvector64") ftyp=BaseDataTable::FlagVec64Field;
  else {
    cerr << " h5_datatable_handler_str2coltype(coltypestr="<<ftypstr<<") ERROR unkown column type string"<<endl;
    throw HDF5IOException("h5_datatable_handler_str2coltype() ERROR uknown column type string");
  }
  return ftyp;
}
 

#ifdef __CXX_PRAGMA_TEMPLATES__
#pragma define_template HDF5Handler<BaseDataTable>
#endif

#if defined(ANSI_TEMPLATES) || defined(GNU_TEMPLATES)
template class HDF5Handler<BaseDataTable>;
#endif

} // Fin du namespace SOPHYA 

