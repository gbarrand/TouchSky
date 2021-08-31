#include "h5stream.h"
#include "pexceptions.h"
#include "sopnamsp.h"

//-------------------------------------------------------------
//--- Classe HDF5OutStream : HDF5 output stream for SOPHYA
//-------------------------------------------------------------

HDF5OutStream::HDF5OutStream(string path)
{
  _filename = path;
    /*
     * Create a new file using H5F_ACC_TRUNC access,
     * default file creation properties, and default file
     * access properties.
     */
  _file = H5Fcreate(path.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  _auto_ds_id = 0;
}

HDF5OutStream::~HDF5OutStream()
{
/* Close the HDF5 file */
  H5Fclose(_file);
}

int HDF5OutStream::Write(string dsname, TArray<int_4>& ia)
{
    hid_t       dataset;         /* file and dataset handles */
    hid_t       datatype, dataspace;   /* handles */
    hsize_t     dimsf[10];              /* dataset dimensions */
    herr_t      status;            
    /*
     * Describe the size of the array and create the data space for fixed
     * size dataset. 
     */
    for(int ir=0; ir<ia.Rank(); ir++)     dimsf[ir] = ia.Size(ir);
    dataspace = H5Screate_simple(ia.Rank(), dimsf, NULL); 

    /* 
     * Define datatype for the data in the file.
     * We will store big/little endian INT numbers depending on 
     *  IS_BIG_ENDIAN flag in machdefs
     */
    datatype = H5Tcopy(H5T_NATIVE_INT32);
    if (IS_BIG_ENDIAN) status = H5Tset_order(datatype, H5T_ORDER_BE);
    else status = H5Tset_order(datatype, H5T_ORDER_LE);

    /*
     * Create a new dataset within the file using defined dataspace and
     * datatype and default dataset creation properties.
     */
    dataset = H5Dcreate(_file, dsname.c_str(), datatype, dataspace,
			H5P_DEFAULT,H5P_DEFAULT, H5P_DEFAULT);
    /*
     * Write the data to the dataset using default transfer properties.
     */
    status = H5Dwrite(dataset, H5T_NATIVE_INT32, H5S_ALL, H5S_ALL,
		      H5P_DEFAULT, ia.Data());

    /*
     * Close/release resources.
     */
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);                 
    return status;
}
int HDF5OutStream::Write(string dsname, TArray<r_4>& fa)
{
    hid_t       dataset;         /* file and dataset handles */
    hid_t       datatype, dataspace;   /* handles */
    hsize_t     dimsf[10];              /* dataset dimensions */
    herr_t      status;            
    /*
     * Describe the size of the array and create the data space for fixed
     * size dataset. 
     */
    for(int ir=0; ir<fa.Rank(); ir++)     dimsf[ir] = fa.Size(ir);
    dataspace = H5Screate_simple(fa.Rank(), dimsf, NULL); 

    /* 
     * Define datatype for the data in the file.
     * We will store big/little endian float numbers 
     * depending on  IS_BIG_ENDIAN flag in machdefs
     */
    datatype = H5Tcopy(H5T_NATIVE_FLOAT);
    if (IS_BIG_ENDIAN) status = H5Tset_order(datatype, H5T_ORDER_BE);
    else status = H5Tset_order(datatype, H5T_ORDER_LE);

    /*
     * Create a new dataset within the file using defined dataspace and
     * datatype and default dataset creation properties.
     */
    dataset = H5Dcreate(_file, dsname.c_str(), datatype, dataspace,
			H5P_DEFAULT,H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Write the data to the dataset using default transfer properties.
     */
    status = H5Dwrite(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
		      H5P_DEFAULT, fa.Data());

    /*
     * Close/release resources.
     */
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);                 
    return status;
}

int HDF5OutStream::Write(TArray<int_4>& ia)
{
  _auto_ds_id++;
  char buff[48];
  sprintf(buff, "SOH_Auto_DS_Id_%d", _auto_ds_id);
  string dsn = buff;
  return Write(dsn, ia);
}

int HDF5OutStream::Write(TArray<r_4>& fa)
{
  _auto_ds_id++;
  char buff[48];
  sprintf(buff, "SOH_Auto_DS_Id_%d", _auto_ds_id);
  string dsn = buff;
  return Write(dsn, fa);
}


//-------------------------------------------------------------
//--- Classe HDF5InStream : HDF5 input stream for SOPHYA
//-------------------------------------------------------------

HDF5InStream::HDF5InStream(string path)
{
  _filename = path;
    /*
     * Open the file and the dataset.
     */
  _file = H5Fopen(path.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  _auto_ds_id = 0;
}

HDF5InStream::~HDF5InStream()
{
/* Close the HDF5 file */
  H5Fclose(_file);
}

int HDF5InStream::Read(string dsname, TArray<int_4>& ia)
{
    hid_t       dataset;         /* handles */
    hid_t       datatype, dataspace;   
    hid_t       memspace; 
    H5T_class_t t_class;                 /* data type class */
    H5T_order_t order;                 /* data order */
    size_t      size;                  /*
				        * size of the data element	       
				        * stored in file
				        */
    hsize_t     dimsm[10];              /* memory space dimensions */
    hsize_t     dims_out[10];           /* dataset dimensions */      
    herr_t      status;                             

    /*
     * Open the the dataset.
     */
    dataset = H5Dopen(_file, dsname.c_str(), H5P_DEFAULT);

    /*
     * Get datatype and dataspace handles and then query
     * dataset class, order, size, rank and dimensions.
     */
    datatype  = H5Dget_type(dataset);     /* datatype handle */ 
    t_class     = H5Tget_class(datatype);
    if (t_class == H5T_INTEGER) printf("HDF5InStream::Read(...int_4)/Info: Data set has INTEGER type, ");
    else printf("\n HDF5InStream::Read(...int_4)/Warning: Data set NOT INTEGER type, ");
    order     = H5Tget_order(datatype);
    if (order == H5T_ORDER_LE) printf("Little endian order, ");
    else printf("Big endian order, \n");
    size  = H5Tget_size(datatype);
    printf("Data size is %d \n", (int)size);

    dataspace = H5Dget_space(dataset);    /* dataspace handle */
    int rank      = H5Sget_simple_extent_ndims(dataspace);
    herr_t status_n  = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    printf("HDF5InStream::Read(...int_4)/Info rank %d, dimensions %lu x %lu \n", rank,
	   (unsigned long)(dims_out[0]), (unsigned long)(dims_out[1]));

    sa_size_t asz[5] = {0,0,0,0,0};
    for(int ir=0; ir<rank; ir++) asz[ir] = dims_out[ir];
    ia.SetSize(rank, asz);

    for(int ir=0; ir<ia.Rank(); ir++)     dimsm[ir] = ia.Size(ir);
    memspace = H5Screate_simple(ia.Rank(), dimsm, NULL); 
    status = H5Dread(dataset, H5T_NATIVE_INT32, memspace, dataspace,
		     H5P_DEFAULT, ia.Data());
    /*
     * Close/release resources.
     */
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Sclose(dataspace);
    H5Sclose(memspace);

    return status; 
}

int HDF5InStream::Read(string dsname, TArray<r_4>& fa)
{
    hid_t       dataset;         /* handles */
    hid_t       datatype, dataspace;   
    hid_t       memspace; 
    H5T_class_t t_class;                 /* data type class */
    H5T_order_t order;                 /* data order */
    size_t      size;                  /*
				        * size of the data element	       
				        * stored in file
				        */
    hsize_t     dimsm[10];              /* memory space dimensions */
    hsize_t     dims_out[10];           /* dataset dimensions */      
    herr_t      status;                             

    /*
     * Open the the dataset.
     */
    dataset = H5Dopen(_file, dsname.c_str(), H5P_DEFAULT);

    /*
     * Get datatype and dataspace handles and then query
     * dataset class, order, size, rank and dimensions.
     */
    datatype  = H5Dget_type(dataset);     /* datatype handle */ 
    t_class     = H5Tget_class(datatype);
    if (t_class == H5T_FLOAT) printf("HDF5InStream::Read(...r_4)/Info: Data set has FLOAT type, ");
    else printf("\n HDF5InStream::Read(...r_4)/Warning: Data set NOT FLOAT type \n");
    order     = H5Tget_order(datatype);
    if (order == H5T_ORDER_LE) printf("Little endian order, ");
    else printf("Big endian order, ");
    size  = H5Tget_size(datatype);
    printf("Data size is %d \n", (int)size);

    dataspace = H5Dget_space(dataset);    /* dataspace handle */
    int rank      = H5Sget_simple_extent_ndims(dataspace);
    herr_t status_n  = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    printf("HDF5InStream::Read(...r_4)/Info rank %d, dimensions %lu x %lu \n", rank,
	   (unsigned long)(dims_out[0]), (unsigned long)(dims_out[1]));

    sa_size_t asz[5] = {0,0,0,0,0};
    for(int ir=0; ir<rank; ir++) asz[ir] = dims_out[ir];
    fa.SetSize(rank, asz);
    fa = -99.;
    for(int ir=0; ir<fa.Rank(); ir++)     dimsm[ir] = fa.Size(ir);
    memspace = H5Screate_simple(fa.Rank(), dimsm, NULL); 
    status = H5Dread(dataset, H5T_NATIVE_FLOAT, memspace, dataspace,
		     H5P_DEFAULT, fa.Data());
    /*
     * Close/release resources.
     */
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Sclose(dataspace);
    H5Sclose(memspace);

    return status; 

}

int HDF5InStream::Read(TArray<int_4>& ia)
{
  _auto_ds_id++;
  char buff[48];
  sprintf(buff, "SOH_Auto_DS_Id_%d", _auto_ds_id);
  string dsn = buff;
  return Read(dsn, ia);
}

int HDF5InStream::Read(TArray<r_4>& fa)
{
  _auto_ds_id++;
  char buff[48];
  sprintf(buff, "SOH_Auto_DS_Id_%d", _auto_ds_id);
  string dsn = buff;
  return Read(dsn, fa);
}

// ----------------------------------------------------------------------------
//   Fonctions de tests 
 
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdf.ncsa.uiuc.edu/HDF5/doc/Copyright.html.  If you do not have     *
 * access to either file, you may request a copy from hdfhelp@ncsa.uiuc.edu. *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*  
 *  This example writes data to the HDF5 file.
 *  Data conversion is performed during write operation.  
 */
 

#define H5FILE_NAME        "h5sa.h5"
#define DATASETNAME "SAIntArray" 

int SAWriteToHDF5(TArray<int>& ia)
{
    hid_t       file, dataset;         /* file and dataset handles */
    hid_t       datatype, dataspace;   /* handles */
    hsize_t     dimsf[10];              /* dataset dimensions */
    herr_t      status;                             
    int         i, j;

    /*
     * Create a new file using H5F_ACC_TRUNC access,
     * default file creation properties, and default file
     * access properties.
     */
    file = H5Fcreate(H5FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Describe the size of the array and create the data space for fixed
     * size dataset. 
     */
    for(int ir=0; ir<ia.Rank(); ir++)     dimsf[ir] = ia.Size(ir);
    dataspace = H5Screate_simple(ia.Rank(), dimsf, NULL); 

    /* 
     * Define datatype for the data in the file.
     * We will store little endian INT numbers.
     */
    datatype = H5Tcopy(H5T_NATIVE_INT32);
    status = H5Tset_order(datatype, H5T_ORDER_LE);

    /*
     * Create a new dataset within the file using defined dataspace and
     * datatype and default dataset creation properties.
     */
    dataset = H5Dcreate(file, DATASETNAME, datatype, dataspace,
			H5P_DEFAULT,H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Write the data to the dataset using default transfer properties.
     */
    status = H5Dwrite(dataset, H5T_NATIVE_INT32, H5S_ALL, H5S_ALL,
		      H5P_DEFAULT, ia.Data());

    /*
     * Close/release resources.
     */
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Fclose(file);
 
    return status;
}     

/*  
 *   This example reads hyperslab from the SDS.h5 file 
 *   created by h5_write.c program into two-dimensional
 *   plane of the three-dimensional array. 
 *   Information about dataset in the SDS.h5 file is obtained. 
 */
 

int SAReadFromHDF5(TArray<int_4>& ia)
{
    hid_t       file, dataset;         /* handles */
    hid_t       datatype, dataspace;   
    hid_t       memspace; 
    H5T_class_t t_class;                 /* data type class */
    H5T_order_t order;                 /* data order */
    size_t      size;                  /*
				        * size of the data element	       
				        * stored in file
				        */
    hsize_t     dimsm[10];              /* memory space dimensions */
    hsize_t     dims_out[10];           /* dataset dimensions */      
    herr_t      status;                             

 
    /*
     * Open the file and the dataset.
     */
    file = H5Fopen(H5FILE_NAME, H5F_ACC_RDONLY, H5P_DEFAULT);
    dataset = H5Dopen(file, DATASETNAME, H5P_DEFAULT);

    /*
     * Get datatype and dataspace handles and then query
     * dataset class, order, size, rank and dimensions.
     */
    datatype  = H5Dget_type(dataset);     /* datatype handle */ 
    t_class     = H5Tget_class(datatype);
    if (t_class == H5T_INTEGER) printf("SAReadFromHDF5/Info: Data set has INTEGER type \n");
    else printf("SAReadFromHDF5/Warning: Data set NOT INTEGER type \n");
//    else throw IOExc("SAReadFromHDF5-Bad data type in data set (non integer) " );
    order     = H5Tget_order(datatype);
    if (order == H5T_ORDER_LE) printf("SAReadFromHDF5/Info:Little endian order \n");
    else printf("SAReadFromHDF5/Warning: NOT Little endian order \n");
    size  = H5Tget_size(datatype);
    printf("SAReadFromHDF5/Info: Data size is %d \n", (int)size);

    dataspace = H5Dget_space(dataset);    /* dataspace handle */
    int rank      = H5Sget_simple_extent_ndims(dataspace);
    herr_t status_n  = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
    printf("rank %d, dimensions %lu x %lu \n", rank,
	   (unsigned long)(dims_out[0]), (unsigned long)(dims_out[1]));

    sa_size_t asz[5] = {0,0,0,0,0};
    for(int ir=0; ir<rank; ir++) asz[ir] = dims_out[ir];
    ia.SetSize(rank, asz);

    for(int ir=0; ir<ia.Rank(); ir++)     dimsm[ir] = ia.Size(ir);
    memspace = H5Screate_simple(ia.Rank(), dimsm, NULL); 
    status = H5Dread(dataset, H5T_NATIVE_INT32, memspace, dataspace,
		     H5P_DEFAULT, ia.Data());
    /*
     * Close/release resources.
     */
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Sclose(dataspace);
    H5Sclose(memspace);
    H5Fclose(file);

    return status;
}     
