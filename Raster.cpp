// Raster.cpp
//
// by Nick Paris, Feb 18, 2017
//
//--------------------------------------------


#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <string.h>
#include <cstdlib>

#include "H5Cpp.h"
#include "Exceptions.hpp"
#include "Image.hpp"
#include "Raster.hpp"
#include "File.hpp"

#include "attributes.hpp"
//#include <opencv2/opencv.hpp>
#include <fftw3.h>
#include <complex>

namespace GeoStar {

  Raster::Raster(Image *image, const std::string &name){
    RasterOpenErrorException RasterOpenError;
    RasterDoesNotExistException RasterDoesNotExist;

    if(!image->datasetExists(name)) throw RasterDoesNotExist;

    // check if its a valid Raster:
    rasterobj = new H5::DataSet(image->openDataset(name));
    if(read_object_type() != "geostar::raster") {
      delete rasterobj;
      throw RasterOpenError;
    }//endif

    // finish setting object-specific data:
    rastername = name;
    rastertype = "geostar::raster";

  }// end-Raster-constructor




  Raster::Raster(Image *image, const std::string &name, const RasterType &type,
           const int &nx, const int &ny){

    RasterCreationErrorException RasterCreationError;
    RasterExistsException RasterExistsError;

    if(image->datasetExists(name)) throw RasterExistsError;

    // create a 2D dataset
    hsize_t dims[2];
    dims[0] = ny;
    dims[1] = nx;
    H5::DataSpace dataspace(2, dims);

    switch(type) {
    case INT8U:
      rasterobj = new H5::DataSet(image->createDataset(name, H5::PredType::NATIVE_UINT8, dataspace));
      break;
    case INT16U:
      rasterobj = new H5::DataSet(image->createDataset(name, H5::PredType::NATIVE_UINT16, dataspace));
      break;
    case REAL32:
      rasterobj = new H5::DataSet(image->createDataset(name, H5::PredType::NATIVE_FLOAT, dataspace));
      break;
    default:
      throw RasterCreationError;
    }// end case

    rastername = name;
    raster_datatype=type;
    rastertype = "geostar::raster";

    // set objtype attribute.
    write_object_type(rastertype);

  }// end-Raster-constructor



 template <>  H5::PredType Raster::getHdf5Type<uint8_t>() {return
H5::PredType::NATIVE_UINT8;}
    template <>  H5::PredType Raster::getHdf5Type<int8_t>() {return
H5::PredType::NATIVE_INT8;}
    template <>  H5::PredType Raster::getHdf5Type<uint16_t>() {return
H5::PredType::NATIVE_UINT16;}
    template <>  H5::PredType Raster::getHdf5Type<int16_t>() {return
H5::PredType::NATIVE_INT16;}
    template <>  H5::PredType Raster::getHdf5Type<uint32_t>() {return
H5::PredType::NATIVE_UINT32;}
    template <>  H5::PredType Raster::getHdf5Type<int32_t>() {return
H5::PredType::NATIVE_INT32;}
    template <>  H5::PredType Raster::getHdf5Type<uint64_t>() {return
H5::PredType::NATIVE_UINT64;}
    template <>  H5::PredType Raster::getHdf5Type<int64_t>() {return
H5::PredType::NATIVE_INT64;}
    template <>  H5::PredType Raster::getHdf5Type<float>() {return
H5::PredType::NATIVE_FLOAT;}
    template <>  H5::PredType Raster::getHdf5Type<double>() {return
H5::PredType::NATIVE_DOUBLE;}




  //returns the actual size of the raster in the x-direction
  long int Raster::get_nx() const {
    H5::DataSpace space=rasterobj->getSpace();
    hsize_t dims[2];
    space.getSimpleExtentDims(dims);
    return dims[1];
  }// end: get_nx

  //returns the actual size of the raster in the y-direction
  long int Raster::get_ny() const {
    H5::DataSpace space=rasterobj->getSpace();
    hsize_t dims[2];
    space.getSimpleExtentDims(dims);
    return dims[0];
  }// end: get_ny


  // in-place simple threshhold
  // < value : set to 0.
  void Raster::thresh(const double &value) {
    long int nx = get_nx();
    long int ny = get_ny();

    vector<long int>slice(4);
    slice[0]=0;
    slice[1]=0;
    slice[2]=nx;
    slice[3]=1;

    vector<float>data(nx);

    for(int line=0;line<ny;++line) {
      slice[1]=line;
      read(slice, data);
      for(int pixel=0; pixel<nx;++pixel) {
        if(data[pixel] < value) data[pixel]=0;
      }// endfor: pixel
      write(slice, data);
    }// endfor: line

  }// end: thresh


  // writes to different/existing channel
  void Raster::scale(Raster *ras_out, const double &offset, const double &mult) const {
    long int nx = get_nx();
    long int ny = get_ny();
    int i;

    vector<long int>slice(4);
    slice[0]=0;
    slice[1]=0;
    slice[2]=nx;
    slice[3]=1;

    vector<float>indata(nx);
    vector<float>outdata(nx);

    for(int line=0;line<ny;++line) {
      slice[1]=line;
      read(slice,indata);
      for(int pixel=0; pixel<nx;++pixel) {
        i = mult*(indata[pixel]-offset);
        if (i<0)   i=0;
        outdata[pixel]= i;
      }// endfor: pixel
      ras_out->write(slice, outdata);
    }// endfor: line

  }// end: scale



  void Raster::copy(const long int *inslice, Raster *ras_out) const {
    SliceSizeException SliceSizeError;

	int i;
  //check and see if the input and output slices fit in their respective rasters
  	long int nx_in = get_nx();
	long int ny_in = get_ny();

	if (nx_in < (inslice[0] + inslice[2])) throw SliceSizeError;
	if (ny_in < (inslice[1] + inslice[3])) throw SliceSizeError;

	long int nx_out = ras_out->get_nx();
	long int ny_out = ras_out->get_ny();

	if (nx_out < (inslice[2])) throw SliceSizeError;
	if (ny_out < (inslice[3])) throw SliceSizeError;


    vector<long int>islice(4);
    islice[0]=inslice[0];
    islice[1]=0;
    islice[2]=inslice[2]; //nx
    islice[3]=1;

    vector<long int>oslice(4);
    oslice[0]=0;
    oslice[1]=0;
    oslice[2]=inslice[2]; //nx
    oslice[3]=1;

    vector<float>data(inslice[2]);

    for(int line=inslice[1];line<inslice[1]+inslice[3];++line) {
      islice[1]=line;
      read(islice,data);

      oslice[1]=line-inslice[1];
      ras_out->write(oslice, data);
    }// endfor: line


  }// end: copy




  void Raster::set(const long int *slice, const int &value) {
    int i;

    SliceSizeException SliceSizeError;

	long int nx = get_nx();
	long int ny = get_ny();

//check if input slice fits in raster
	if (nx < (slice[0] + slice[2])) throw SliceSizeError;
	if (ny < (slice[1] + slice[3])) throw SliceSizeError;

    vector<long int>islice(4);
    islice[0]=slice[0];
    islice[1]=slice[1];
    islice[2]=slice[2];
    islice[3]=slice[3];

    long int num = slice[2]*slice[3];
    vector<float>data(num);

    //assume slices are small, do all at one go:
    for(int i=0;i<num;i++){
      data[i]=value;
    }// endfor

    write(islice, data);

  }// end: set

 void Raster::drawFilledCircle(long int x0, long int y0, const double radius, const double color) {
	RasterSizeErrorException RasterSizeError;
	RadiusSizeException RadiusSizeError;

	if (radius < 0) throw RadiusSizeError;

	//circle must not extend beyond raster
	long int nx = get_nx(), ny = get_ny();
	if ((x0 - radius) < 0 || (x0 + radius) > nx) throw RasterSizeError;
	if ((y0 - radius) < 0 || (y0 + radius) > ny) throw RasterSizeError;

	//slice enclosing circle
	vector<long int>slice(4);
	slice[0] = x0 - radius;
	slice[1] = y0 - radius;
	slice[2] = 2 * radius;
	slice[3] = 2 * radius;

	long int size = slice[2] * slice[3];
	std::vector<float>data(size);
	
	read(slice, data);

	//overwrite pixels within the circle
	double dx = 0, dy = 0;
	double distsq = 0;
	double rsq = radius * radius;
	for (int x = 0; x < slice[2]; ++x) {
	  for (int y = 0; y < slice[3]; ++y) {
	    dx = x - radius;
	    dy = y - radius;
	    distsq = dx * dx + dy * dy;
	     if (distsq <= rsq) {
		data[y * slice[2] + x] = color;
	      }//endif
	   }//endfor
	}//endfor

	write(slice, data);

 }//end drawPoint

 void Raster::drawLine(std::vector<long int> slice, const double radius, const double color) {

	RasterSizeErrorException RasterSizeError;
	SliceSizeException SliceSizeError;
	RadiusSizeException RadiusSizeError;

	if (slice.size() < 4) throw SliceSizeError;

	if (radius < 0) throw RadiusSizeError;

	long int nx = get_nx(), ny = get_ny();
	if ((slice[0] - radius) < 0 || (slice[0] + radius) > nx) throw RasterSizeError;
	if ((slice[1] - radius < 0) || (slice[1] + radius) > ny) throw RasterSizeError;
	if ((slice[0] + slice[2] - radius) < 0 || (slice[0] + slice[2] + radius) > nx) throw RasterSizeError;
	if ((slice[1] + slice[3] - radius) < 0 || (slice[1] + slice[3] + radius) > ny) throw RasterSizeError;

	std::vector<float> data;
	read(slice, data);

	drawFilledCircle(slice[0], slice[1], radius, color);

	drawFilledCircle(slice[0] + slice[2], slice[1] + slice[3], radius, color);

	double slope = slice[3] / slice[2];
	double y = 0;

	double distDivisor = sqrt((slice[2] * slice[2]) + (slice[3] * slice[3]));
	double lineDist = 0;

	//define equation of line as y-y1 = slope(x-x1)
	// -> y = slope*x - slope*x1 + y1
	// but within the slice, x1 and y1 are zero (initial points) so y = slope * x
	for (int i = 0; i < slice[2]; ++i) {
	  y = (slope * i);
	  data[y * slice[3] + i] = color;
	}

	//now sweep through slice and find any points with distance to line less than the radius
	//use (Ax + By + C) / sqrt(A^2 + B^2) for the distance calculation

	for (int i = 0; i < slice[3]; ++i) {
	  for (int j = 0; j < slice[2]; ++j) {
	    lineDist = fabs((slice[2] * i) - (slice[3] * j)) / distDivisor;
	    if (lineDist <= radius) {
		data[i * slice[3] + j] = color;
		}//endif
	  }//endfor
	}//endfor

	write(slice, data);
		
 }//end drawLine

  void Raster::drawRectangle(std::vector<long int>slice, const int radius, const int color) {
	RasterSizeErrorException RasterSizeError;
	SliceSizeException SliceSizeError;
	RadiusSizeException RadiusSizeError;

	if (slice.size() < 4) throw SliceSizeError;

	if (radius < 0) throw RadiusSizeError;

	long int nx = get_nx(), ny = get_ny();
	if (slice[0] < 0 || slice[2] > nx) throw RasterSizeError;
	if (slice[1] < 0 || slice[3] > ny) throw RasterSizeError;

	//draw edges of desired radius one at a time, setting each region to desired color

	//left edge
	long int oslice[4] = {slice[0], slice[1], radius, slice[3]};
	set(oslice, color);
	//top edge
	oslice[2] = slice[2];
	oslice[3] = radius;
	set(oslice, color);
	//bottom edge
	oslice[0] = slice[0];
	oslice[1] = slice [1] + slice[3] - radius;
	oslice[2] = slice[2];
	oslice[3] = radius;
	set(oslice, color);
	//right edge
	oslice[0] = slice[0] + slice[2] - radius;
	oslice[1] = slice[1];
	oslice[2] = radius;
	oslice[3] = slice[3];
	set(oslice, color);

 }//endRectangle

void Raster::drawFilledRectangle(std::vector<long int>slice, const int radius, const int lineColor, const int fillColor) {
	RasterSizeErrorException RasterSizeError;
	SliceSizeException SliceSizeError;
	RadiusSizeException RadiusSizeError;

	if (slice.size() < 4) throw SliceSizeError;
	
	if (radius < 0) throw RadiusSizeError;

	long int nx = get_nx(), ny = get_ny();
	if (slice[0] < 0 || slice[2] > nx) throw RasterSizeError;
	if (slice[1] < 0 || slice[3] > ny) throw RasterSizeError;

	//set whole slice to filled color first
	long int oslice[4] = {slice[0], slice[1], slice[2], slice[3]};
	set(oslice, fillColor);

	//draw edges of desired radius one at a time, setting each region to desired color

	//left edge
	oslice[2] = radius;
	set(oslice, lineColor);
	//top edge
	oslice[2] = slice[2];
	oslice[3] = radius;
	set(oslice, lineColor);
	//bottom edge
	oslice[0] = slice[0];
	oslice[1] = slice [1] + slice[3] - radius;
	oslice[2] = slice[2];
	oslice[3] = radius;
	set(oslice, lineColor);
	//right edge
	oslice[0] = slice[0] + slice[2] - radius;
	oslice[1] = slice[1];
	oslice[2] = radius;
	oslice[3] = slice[3];
	set(oslice, lineColor);		

 }//endFilledRectangle

  void Raster::addSaltPepper(Raster *rasterOut, const double low) {
	RasterSizeErrorException RasterSizeError;
	ProbabilityException ProbabilityError;
	//check if probability parameters are right
	if(low < 0 || low > 0.5) throw ProbabilityError;

	const double high = 1 - low;

	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_out = rasterOut->get_nx();
	long int ny_out = rasterOut->get_ny();
	//check raster bounds
	if (nx != nx_out) throw RasterSizeError;
	if (ny != ny_out) throw RasterSizeError;

	double temp = 0;
	std::vector<long int> slice(4);
	slice[0] = 0;
	slice[1] = 0;
	slice[2] = nx;
	slice[3] = 1;

	//init random seed, to limit pseudorandom results
	srand((unsigned)time(NULL));

	std::vector<double> data(nx);

	//loop through image line by line, calculating random value between 0 and 1		
	for (int i = 0; i < ny; ++i) {
		slice[1] = i;
		read(slice, data);
	 for (int j = 0; j < nx; ++j) {
		temp = ((double)rand() / (double)(RAND_MAX));
		//set values equal to 0 if below low thresh, or higher than high thresh
		if (temp <= low) data[j] = 0;
		else if (temp >= high) data[j] = 15000;
	  }//endfor
	rasterOut->write(slice, data);
	}//endfor
	
	
 }//end--addSaltPepper

 void Raster::bitShift(Raster *rasterOut, int bits, bool direction) {
	RasterSizeErrorException RasterSizeError;
	BitException BitError;

	if (bits < 0) throw BitError;
	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_out = rasterOut->get_nx();
	long int ny_out = rasterOut->get_ny();
	//check raster bounds
	if (nx != nx_out) throw RasterSizeError;
	if (ny != ny_out) throw RasterSizeError;

	std::vector<long int> slice(4);
	slice[0] = 0;
	slice[1] = 0;
	slice[2] = nx;
	slice[3] = 1;
	
	std::vector<float> data(nx);
	
	//loop through image and bitshift right or left, line by line
	if (direction) {
	double inverse = 1 / pow(2, bits);
	  for (int i = 0; i < ny; ++i) {
		slice[1] = i;
		read(slice, data);
	    for (int j = 0; j < nx; ++j) {
		data[j] *= inverse;
	    }//endfor
	    rasterOut->write(slice, data);
	  }//endfor
	}//endif
	
	else {
	for (int i = 0; i < ny; ++i) {
		slice[1] = i;
		read(slice, data);
	 for (int j = 0; j < nx; ++j) {
		data[j] *= pow(2, bits);
	  }//endfor
	rasterOut->write(slice, data);
	}//endfor
	}//endelse


 }//end--bitShift

  void Raster::autoLocalThresh(Raster *rasterOut, const int partitions) {
	PartitionException PartitionError;
	RasterSizeErrorException RasterSizeError;

	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_out = rasterOut->get_nx();
	long int ny_out = rasterOut->get_ny();
	//check raster bounds
	if (nx != nx_out) throw RasterSizeError;
	if (ny != ny_out) throw RasterSizeError;
	//check partition bounds
	if (partitions <= 0) throw PartitionError;
	if (partitions > 150) throw PartitionError;

	double partitionSizeX = nx / partitions;
	double partitionSizeY = ny / partitions;
	
   vector<long int>slice(4);
    slice[0]=0;
    slice[1]=0;
    slice[2]=partitionSizeX;
    slice[3]=1;

    vector<double>data;
    double threshhold = 0;
    double max = 0;
    double min = 0;

//first define the slice loops, x and y which go through the sectors.  reset thresh variables
for (int y = 0; y < partitions; ++y) {
 for (int x = 0; x < partitions; ++x) {
	slice[0] = x * partitionSizeX;
	max = 0;
	min = 100000; //assign large num to min to init - can't reassign variable to a data pt with every read
	threshhold = 0;
//then the interior loops, which go into the actual sectors and do the reading and writing

//first find max and min and compute average
   for (int i = 0; i < partitionSizeY; ++i) {
	slice[1] = i + y * partitionSizeY;
	read(slice, data);
     for (int j = 0; j < partitionSizeX; ++j) {
	if (data[j] < min) min = data[j];
	if (data[j] > max) max = data[j];
	}//endfor - j
    }//endfor - i

	//not sure exactly what factor this should be divided by.  Gets better as partitions grow.
	threshhold = (max + min) / 3;
	

//then perform threshholding operation
   for (int p = 0; p < partitionSizeY; ++p) {
	slice[1] = p + y * partitionSizeY;
	read(slice, data);
     for (int q = 0; q < partitionSizeX; ++q) {
	if (data[q] < threshhold) data[q] = 0;
	}//endfor - q
	rasterOut->write(slice, data);
    }//endfor - p

  }//endfor - x
}//endfor - y

 }//end - autoLocalThresh



  void Raster::FFT_2D(GeoStar::Image *img, Raster *rasOutReal, Raster *rasOutImg) {
	RasterSizeErrorException RasterSizeError;

	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_outReal = rasOutReal->get_nx();
	long int ny_outReal = rasOutReal->get_ny();
	long int nx_outImg = rasOutImg->get_nx();
	long int ny_outImg = rasOutImg->get_ny();
	//check raster bounds
	if (nx != nx_outReal) throw RasterSizeError;
	if (ny != ny_outReal) throw RasterSizeError;
	if (nx != nx_outImg) throw RasterSizeError;
	if (ny != ny_outImg) throw RasterSizeError;

	fftw_complex *in, *out;
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);
	fftw_plan plan;
	plan = fftw_plan_dft_1d(nx, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	std::vector<long int>sliceFFTW(4);
    		sliceFFTW[0]=0;
    		sliceFFTW[1]=0;
    		sliceFFTW[2]=nx;
    		sliceFFTW[3]=1;
	std::vector<double> dataReal(nx);
	std::vector<double> dataImg(nx);
	
	GeoStar::Raster *rasBufferReal = img->create_raster("BufferReal", GeoStar::REAL32, nx, ny);
	GeoStar::Raster *rasBufferImg = img->create_raster("BufferImg", GeoStar::REAL32, nx, ny);

	//transform row by row
	for (int y = 0; y < ny; ++y) {	
	sliceFFTW[1] = y;
	read(sliceFFTW, dataReal);
	for(int i=0;i<nx;i++) { 
	   in[i][0]=dataReal[i]; 
	   in[i][1]=0.0;
	}

	
	fftw_execute(plan);
	
	for (int i = 0; i < nx; i++) {
		dataReal[i] = out[i][0];
		dataImg[i] = out[i][1];
	}
	rasBufferReal->write(sliceFFTW, dataReal);
	rasBufferImg->write(sliceFFTW, dataImg);
	
	}//endfor - row-by-row

	//now delete objects and reinitialize for the ny size - cols transform
	fftw_destroy_plan(plan);
	fftw_free(in); fftw_free(out);

	fftw_complex *inCols, *outCols;
	inCols = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ny);
	outCols = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ny);
	fftw_plan planCols;
	planCols = fftw_plan_dft_1d(ny, inCols, outCols, FFTW_FORWARD, FFTW_ESTIMATE);

		sliceFFTW[0] = 0;
    		sliceFFTW[1] = 0;
    		sliceFFTW[2] = 1;
		sliceFFTW[3] = ny;

	dataReal.reserve(ny);
	dataImg.reserve(ny);
	
	//transform col-by-col	
	for (int x = 0; x < nx; ++x) {
	sliceFFTW[0] = x;
	rasBufferReal->read(sliceFFTW, dataReal);
	rasBufferImg->read(sliceFFTW, dataImg);
	for (int i = 0; i < ny; ++i) {
		inCols[i][0] = dataReal[i];
		inCols[i][1] = dataImg[i];
	}
	
	fftw_execute(planCols);
	
	for (int i = 0; i < ny; ++i) {
		dataReal[i] = outCols[i][0];
		dataImg[i] = outCols[i][1];
	}
	rasOutReal->write(sliceFFTW, dataReal);
	rasOutImg->write(sliceFFTW, dataImg);

	}//endfor - col-by-col

	delete rasBufferReal;
	delete rasBufferImg;
	fftw_destroy_plan(planCols);
	fftw_free(inCols); fftw_free(outCols);

   }//end - FFT_2D

  void Raster::FFT_2D_Inv(GeoStar::Image *img, Raster *rasOut, Raster *rasInImg) {
	RasterSizeErrorException RasterSizeError;

	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_out = rasOut->get_nx();
	long int ny_out = rasOut->get_ny();
	long int nx_img = rasInImg->get_nx();
	long int ny_img = rasInImg->get_ny();
	//check raster bounds
	if (nx != nx_out) throw RasterSizeError;
	if (ny != ny_out) throw RasterSizeError;
	if (nx != nx_img) throw RasterSizeError;
	if (ny != ny_img) throw RasterSizeError;

	fftw_complex *in, *out;
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ny);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ny);
	fftw_plan plan;
	plan = fftw_plan_dft_1d(ny, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);

	std::vector<long int>sliceFFTW(4);
    		sliceFFTW[0]=0;
    		sliceFFTW[1]=0;
    		sliceFFTW[2]=1;
    		sliceFFTW[3]=ny;
	std::vector<double> dataReal(ny);
	std::vector<double> dataImg(ny);
	
	GeoStar::Raster *rasBufferReal = img->create_raster("BufferRealInv", GeoStar::REAL32, nx, ny);
	GeoStar::Raster *rasBufferImg = img->create_raster("BufferImgInv", GeoStar::REAL32, nx, ny);

	//transform col by col
	for (int x = 0; x < nx; ++x) {	
	sliceFFTW[0] = x;
	read(sliceFFTW, dataReal);
	rasInImg->read(sliceFFTW, dataImg);
	for(int i=0;i<ny;i++) { 
	   in[i][0]=dataReal[i]; 
	   in[i][1]=dataImg[i];
	}

	
	fftw_execute(plan);
	
	for (int i = 0; i < ny; i++) {
		dataReal[i] = out[i][0];
		dataImg[i] = out[i][1];
	}
	rasBufferReal->write(sliceFFTW, dataReal);
	rasBufferImg->write(sliceFFTW, dataImg);
	
	}//endfor - col-by-col

	//now delete objects and reinitialize for the nx size - rows transform
	fftw_destroy_plan(plan);
	fftw_free(in); fftw_free(out);

	fftw_complex *inRows, *outRows;
	inRows = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);
	outRows = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);
	fftw_plan planRows;
	planRows = fftw_plan_dft_1d(nx, inRows, outRows, FFTW_BACKWARD, FFTW_ESTIMATE);

		sliceFFTW[0] = 0;
    		sliceFFTW[1] = 0;
    		sliceFFTW[2] = nx;
		sliceFFTW[3] = 1;

	dataReal.reserve(nx);
	dataImg.reserve(nx);
	
	//transform row-by-row	
	for (int y = 0; y < ny; ++y) {
	sliceFFTW[1] = y;
	rasBufferReal->read(sliceFFTW, dataReal);
	rasBufferImg->read(sliceFFTW, dataImg);
	for (int i = 0; i < nx; ++i) {
		inRows[i][0] = dataReal[i];
		inRows[i][1] = dataImg[i];
	}
	
	fftw_execute(planRows);
	
	for (int i = 0; i < nx; ++i) {
		dataReal[i] = outRows[i][0] / 200000; //just a scaling factor, feel free to adjust as needed
	}
	rasOut->write(sliceFFTW, dataReal);

	}//endfor - row-by-row

	delete rasBufferReal;
	delete rasBufferImg;
	fftw_destroy_plan(planRows);
	fftw_free(inRows); fftw_free(outRows);

	}//end - FFT_2D_Inv

 void Raster::lowPassFilter(GeoStar::Image *img, Raster *rasInReal, Raster *rasInImg, Raster *rasOut) {
	RasterSizeErrorException RasterSizeError;
	//check the output raster only, all others are checked in the FFT_2D() function
	long int nx = get_nx();
	long int ny = get_ny();
	if (nx != rasOut->get_nx()) throw RasterSizeError;
	if (ny != rasOut->get_ny()) throw RasterSizeError;

	//transform
	FFT_2D(img, rasInReal, rasInImg);

	//set slice in output rasters to zero
	const long int width = nx / 5;
	const long int setSlice[4] = {width, width, width, width};
	rasInReal->set(setSlice, 0);
	rasInImg->set(setSlice, 0);

	//transform back
	rasInReal->FFT_2D_Inv(img, rasOut, rasInImg);

 } //end - lowPassFilter

 void Raster::downsample(Raster * rasOut) {
  	RasterSizeErrorException RasterSizeError;
	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_out = rasOut->get_nx();
	long int ny_out = rasOut->get_ny();
	if (nx / 2 != nx_out) throw RasterSizeError;
	if (ny / 2 != ny_out) throw RasterSizeError;
	
	//define kernel and a double so no integer division
	//scaling factor
	double mKernel = 400;
	double gaussianKernel[5][5] = {{1/mKernel, 4/mKernel, 6/mKernel, 4/mKernel, 1/mKernel}, 
					{4/mKernel, 16/mKernel, 24/mKernel, 16/mKernel, 4/mKernel},
					{6/mKernel, 24/mKernel, 36/mKernel, 24/mKernel, 6/mKernel}, 
					{4/mKernel, 16/mKernel, 24/mKernel, 16/mKernel, 4/mKernel},
					{1/mKernel, 4/mKernel, 6/mKernel, 4/mKernel, 1/mKernel}};

    vector<long int> slice(4);
    slice[0]=0;
    slice[1]=0;
    slice[2]=nx;
    slice[3]=1;

    vector<double> data(nx);
	
	int mFlipped = 0, nFlipped = 0;
	double temp;

	for (int i = 0; i < ny; ++i) {
	  slice[1] = i;
	  read(slice, data);
	  for (int j = 0; j < nx; ++j) {
		temp = 0;
	     for (int m = 0; m < 5; ++m) {
		mFlipped = 5 - 1 - m;
		for (int n = 0; n < 5; ++n) {
		  nFlipped = 5 - 1 - n;
		  
		  //convolve: multiply and accumulate
		  temp += (data[j] * gaussianKernel[mFlipped][nFlipped]);


		}//endfor - n
	      }//endfor - m
		data[j] = temp;
	   }//endfor - j
	   write(slice, data);
	}//endfor - i


    vector<double> dataOut(nx_out);

	int outIndexSlice = 0, outIndexData = 0;

	//remove all even numbered rows and cols to downsize
	//change size of slice - write won't work unless the same slice is used for read and write
	for (int i = 1; i < ny; i += 2) {
	  slice[1] = i;
	  slice[2] = nx;
	  read(slice, data);	
	  slice[1] = outIndexSlice;
	  slice[2] = nx_out;
	  ++outIndexSlice;
	  outIndexData = 0;

	  for (int j = 1; j < nx; j += 2) {
		dataOut[outIndexData] = data[j];
		++outIndexData;
	  }//endfor - j

	  rasOut->write(slice, dataOut);
	}//endfor - i


 } //end - downsample

  vector<Raster *> Raster::gaussianPyramid(Image *img, int n) {
	//RasterSizeErrorException if needed
	IntegerParameterException integerParameterError;
	if (n < 1) throw integerParameterError;
	long int nx = get_nx();
	long int ny = get_ny();

	//make base vector, fill with base layer and first downsample
	vector<Raster *> output(n);
	output[0] = this;
	output[1] = new Raster(img, "GPyramid1", REAL32, nx / 2, ny / 2);

	downsample(output[1]);
	//then repeat n - 1 times
	for (int i = 2; i <= n; ++i) {
	  output[i] = new Raster(img, "GPyramid" + to_string(i), REAL32, nx / pow(2, i), ny / pow(2, i));
	  output[i - 1]->downsample(output[i]);
	}


	return output;

  }//end - gaussianPyramid


  void Raster::upsample(Raster *rasOut) {
	RasterSizeErrorException RasterSizeError;
	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_out = rasOut->get_nx();
	long int ny_out = rasOut->get_ny();
	if (nx * 2 != nx_out) throw RasterSizeError;
	if (ny * 2 != ny_out) throw RasterSizeError;
	
	//define kernel and a double so no integer division
	//scaling factor
	double mKernel = 400;
	double gaussianKernel[5][5] = {{1/mKernel, 4/mKernel, 6/mKernel, 4/mKernel, 1/mKernel}, 
					{4/mKernel, 16/mKernel, 24/mKernel, 16/mKernel, 4/mKernel},
					{6/mKernel, 24/mKernel, 36/mKernel, 24/mKernel, 6/mKernel}, 
					{4/mKernel, 16/mKernel, 24/mKernel, 16/mKernel, 4/mKernel},
					{1/mKernel, 4/mKernel, 6/mKernel, 4/mKernel, 1/mKernel}};

    vector<long int> slice(4);
    slice[0]=0;
    slice[1]=0;
    slice[2]=nx;
    slice[3]=1;

    vector<double> data(nx);

    vector<double> dataOut(nx_out);

	int outIndexSlice = 1, outIndexData = 1;

	//first read existing data to larger raster to upsize - then convolve after
	// only read in every other row and col in the rasOut

	//change size of slice - write won't work unless the same slice is used for read and write
	for (int i = 0; i < ny; ++i) {
	  slice[1] = i;
	  slice[2] = nx;
	  read(slice, data);	
	  slice[1] = outIndexSlice;
	  slice[2] = nx_out;
	  outIndexSlice += 2;
		//just in case dimensions are even and end data is overshot by 1
		if (outIndexSlice > ny_out) outIndexSlice = ny_out;
	  outIndexData = 1;

	  for (int j = 0; j < nx; ++j) {
		dataOut[outIndexData] = data[j];
		outIndexData += 2;
		if (outIndexData > nx_out) outIndexData = nx_out;
	  }//endfor - j

	  rasOut->write(slice, dataOut);
	}//endfor - i

	
	//now convolve - approximate missing values with neighboring pixels

	int mFlipped = 0, nFlipped = 0;
	double temp;

	slice[0] = 0;
	slice[1] = 0;
	slice[2] = nx_out;
	slice[3] = 1;

	for (int i = 1; i < ny_out; i += 2) {
	  slice[1] = i;
	  rasOut->read(slice, dataOut);
	  for (int j = 1; j < nx_out; j += 2) {
		temp = 0;
	     for (int m = 0; m < 5; ++m) {
		mFlipped = 5 - 1 - m;
		for (int n = 0; n < 5; ++n) {
		  nFlipped = 5 - 1 - n;
		  
		  //convolve: multiply and accumulate
		  temp += (dataOut[j] * gaussianKernel[mFlipped][nFlipped]);


		}//endfor - n
	      }//endfor - m
		dataOut[j] = temp;
		if (j == 1) dataOut[0] = temp;
		if (j + 1 < nx_out) dataOut[j + 1] = temp;
	   }//endfor - j
	   rasOut->write(slice, dataOut);
	  if (i == 1) {
	    slice[1] = 0;
	    rasOut->write(slice, dataOut);
	}
	  if (i + 1 < ny_out) {
	   slice[1] = i + 1;
	   rasOut->write(slice, dataOut);
	  }//endif
	}//endfor - i


 }//end - upsample


  vector<Raster *> Raster::laplacianPyramid(Image *img, int n) {
	//RasterSizeErrorException if needed
	IntegerParameterException integerParameterError;
	if (n < 1) throw integerParameterError;
	long int nx = get_nx();
	long int ny = get_ny();

	//make base vector, fill with base layer and first upsample
	vector<Raster *> output(n);
	output[0] = this;
	output[1] = new Raster(img, "LPyramid1", REAL32, nx * 2, ny * 2);

	upsample(output[1]);
	//then repeat n - 1 times
	for (int i = 2; i <= n; ++i) {
	  output[i] = new Raster(img, "LPyramid" + to_string(i), REAL32, nx * pow(2, i), ny * pow(2, i));
	  output[i - 1]->upsample(output[i]);
	}


	return output;

  }//end - gaussianPyramid

  void Raster::harmonicMean(GeoStar::Raster * rasOut, int n) {
	RasterSizeErrorException RasterSizeError;
	IntegerParameterException IntegerParameterError;

	if (n < 3) throw IntegerParameterError;
	if (n > 11) throw IntegerParameterError;
	if (n % 2 == 0) throw IntegerParameterError;

	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_out = rasOut->get_nx();
	long int ny_out = rasOut->get_ny();
	if (nx != nx_out) throw RasterSizeError;
	if (ny != ny_out) throw RasterSizeError;

	int numRegionsY = ny / n;
	int numRegionsX = nx / n;

	vector<long int> slice(4);
    slice[0]=0;
    slice[1]=0;
    slice[2]=n;
    slice[3]=1;

    vector<double> data(n);
	double temp(0);


	//main outer loops that go across the whole image
  for (int y = 0; y < numRegionsY; ++y) {
   for (int x = 0; x < numRegionsX; ++x) {
     slice[0] = x * n;
	temp = 0;

	//sum reciprocals
    for (int ySmall = 0; ySmall < n; ++ySmall) {
	slice[1] = (y * n) + ySmall;
	read(slice, data);
	for (int xSmall = 0; xSmall < n; ++xSmall)
	temp += 1.0 / data[xSmall];
	
	}//endfor - ysmall and xsmall

	//calc harmonic mean
	temp = (n * n) / temp; 

	//now write to output raster
    for (int ySmall = 0; ySmall < n; ++ySmall) {
	slice[1] = (y * n) + ySmall;
	read(slice, data);
	for (int xSmall = 0; xSmall < n; ++xSmall)
	data[xSmall] = temp;

	rasOut->write(slice, data);
	}//endfor - ysmall and xsmall

    }//endfor - x
  }//endfor - y

 }//end - harmonicMean

  void Raster::midpointFilter(GeoStar::Raster * rasOut, int n) {
	RasterSizeErrorException RasterSizeError;
	IntegerParameterException IntegerParameterError;

	if (n < 3) throw IntegerParameterError;
	if (n > 11) throw IntegerParameterError;
	if (n % 2 == 0) throw IntegerParameterError;

	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_out = rasOut->get_nx();
	long int ny_out = rasOut->get_ny();
	if (nx != nx_out) throw RasterSizeError;
	if (ny != ny_out) throw RasterSizeError;

	int numRegionsY = ny / n;
	int numRegionsX = nx / n;

	vector<long int> slice(4);
    slice[0]=0;
    slice[1]=0;
    slice[2]=n;
    slice[3]=1;

    vector<double> data(n);
	double min(0);
	double max(0);


	//main outer loops that go across the whole image
  for (int y = 0; y < numRegionsY; ++y) {
   for (int x = 0; x < numRegionsX; ++x) {
     slice[0] = x * n;

	//find local min and max
    for (int ySmall = 0; ySmall < n; ++ySmall) {
	slice[1] = (y * n) + ySmall;
	read(slice, data);
	min = data[0];
	max = data[0];
	for (int xSmall = 0; xSmall < n; ++xSmall) {

	  if (data[xSmall] < min) min = data[xSmall];
	  else if (data[xSmall] > max) max = data[xSmall];

	  }//endfor - xsmall
	}//endfor - ysmall

	//calc midpoint
	max = (min + max) / 2; 

	//now write to output raster
    for (int ySmall = 0; ySmall < n; ++ySmall) {
	slice[1] = (y * n) + ySmall;
	read(slice, data);
	for (int xSmall = 0; xSmall < n; ++xSmall)
	data[xSmall] = max;

	rasOut->write(slice, data);
	}//endfor - ysmall and xsmall

    }//endfor - x
  }//endfor - y

 }//end - midpointFilter

  void Raster::rangeFilter(GeoStar::Raster * rasOut, int n) {
	RasterSizeErrorException RasterSizeError;
	IntegerParameterException IntegerParameterError;

	if (n < 3) throw IntegerParameterError;
	if (n > 11) throw IntegerParameterError;
	if (n % 2 == 0) throw IntegerParameterError;

	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_out = rasOut->get_nx();
	long int ny_out = rasOut->get_ny();
	if (nx != nx_out) throw RasterSizeError;
	if (ny != ny_out) throw RasterSizeError;

	int numRegionsY = ny / n;
	int numRegionsX = nx / n;

	vector<long int> slice(4);
    slice[0]=0;
    slice[1]=0;
    slice[2]=n;
    slice[3]=1;

    vector<double> data(n);
	double min(0);
	double max(0);


	//main outer loops that go across the whole image
  for (int y = 0; y < numRegionsY; ++y) {
   for (int x = 0; x < numRegionsX; ++x) {
     slice[0] = x * n;

	//find local min and max
    for (int ySmall = 0; ySmall < n; ++ySmall) {
	slice[1] = (y * n) + ySmall;
	read(slice, data);
	min = data[0];
	max = data[0];
	for (int xSmall = 0; xSmall < n; ++xSmall) {

	  if (data[xSmall] < min) min = data[xSmall];
	  else if (data[xSmall] > max) max = data[xSmall];

	  }//endfor - xsmall
	}//endfor - ysmall

	//calc range
	max = max - min; 

	//now write to output raster
    for (int ySmall = 0; ySmall < n; ++ySmall) {
	slice[1] = (y * n) + ySmall;
	read(slice, data);
	for (int xSmall = 0; xSmall < n; ++xSmall)
	data[xSmall] = max;

	rasOut->write(slice, data);
	}//endfor - ysmall and xsmall

    }//endfor - x
  }//endfor - y

 }//end - rangeFilter

 void Raster::gradientMask(GeoStar::Raster * rasOut, int mask) {
	RasterSizeErrorException RasterSizeError;
	IntegerParameterException IntegerParameterError;

	if (mask < 1) throw IntegerParameterError;
	if (mask > 8) throw IntegerParameterError;

	long int nx = get_nx();
	long int ny = get_ny();
	long int nx_out = rasOut->get_nx();
	long int ny_out = rasOut->get_ny();
	if (nx != nx_out) throw RasterSizeError;
	if (ny != ny_out) throw RasterSizeError;
	
	//define your mask
	
	int kernel[3][3];

	switch (mask) {
	case 1:
	kernel[0][0] = -1;
	kernel[0][1] = -2;
	kernel[0][2] = -1;
	kernel[1][0] = 0;
	kernel[1][1] = 0;
	kernel[1][2] = 0;
	kernel[2][0] = 1;
	kernel[2][1] = 2;
	kernel[2][2] = 1;
	break;
	case 2:
	kernel[0][0] = 1;
	kernel[0][1] = 0;
	kernel[0][2] = -1;
	kernel[1][0] = 2;
	kernel[1][1] = 0;
	kernel[1][2] = -2;
	kernel[2][0] = 1;
	kernel[2][1] = 0;
	kernel[2][2] = -1;
	break;
	case 3:
	kernel[0][0] = 1;
	kernel[0][1] = 2;
	kernel[0][2] = 1;
	kernel[1][0] = 0;
	kernel[1][1] = 0;
	kernel[1][2] = 0;
	kernel[2][0] = -1;
	kernel[2][1] = -2;
	kernel[2][2] = -1;
	break;
	case 4: 
	kernel[0][0] = -1;
	kernel[0][1] = 0;
	kernel[0][2] = 1;
	kernel[1][0] = -2;
	kernel[1][1] = 0;
	kernel[1][2] = 2;
	kernel[2][0] = -1;
	kernel[2][1] = 2;
	kernel[2][2] = 1;
	break;
	case 5: 
	kernel[0][0] = 0;
	kernel[0][1] = -1;
	kernel[0][2] = -2;
	kernel[1][0] = 1;
	kernel[1][1] = 0;
	kernel[1][2] = -1;
	kernel[2][0] = 2;
	kernel[2][1] = 1;
	kernel[2][2] = 0;
	break;
	case 6: 
	kernel[0][0] = -2;
	kernel[0][1] = -1;
	kernel[0][2] = 0;
	kernel[1][0] = -1;
	kernel[1][1] = 0;
	kernel[1][2] = 1;
	kernel[2][0] = 0;
	kernel[2][1] = 1;
	kernel[2][2] = 2;
	break;
	case 7: 
	kernel[0][0] = 2;
	kernel[0][1] = 1;
	kernel[0][2] = 0;
	kernel[1][0] = 1;
	kernel[1][1] = 0;
	kernel[1][2] = -1;
	kernel[2][0] = 0;
	kernel[2][1] = -1;
	kernel[2][2] = -2;
	break;
	case 8: 
	kernel[0][0] = 0;
	kernel[0][1] = 1;
	kernel[0][2] = 2;
	kernel[1][0] = -1;
	kernel[1][1] = 0;
	kernel[1][2] = 1;
	kernel[2][0] = -2;
	kernel[2][1] = -1;
	kernel[2][2] = 0;
	break;
	default:
	break;
	}//end - switch

	double blurKernel[3][3] = {0.0625, 0.125, 0.0625, 0.125, 0.5, 0.125, 0.0625, 0.125, 0.0625};

    vector<long int> slice(4);
    slice[0]=0;
    slice[1]=0;
    slice[2]=nx;
    slice[3]=1;

    vector<double> data(nx);
	
	int mFlipped = 0, nFlipped = 0;
	double temp;

	for (int i = 0; i < ny; ++i) {
	  slice[1] = i;
	  read(slice, data);
	  for (int j = 0; j < nx; ++j) {
		temp = 0;
	     for (int m = 0; m < 3; ++m) {
		mFlipped = 3 - 1 - m;
		for (int n = 0; n < 3; ++n) {
		  nFlipped = 3 - 1 - n;
		  
		  //convolve: multiply and accumulate
		  temp += (data[j] * blurKernel[mFlipped][nFlipped]);
		}//endfor - n
	      }//endfor - m
		data[j] = temp;
	   }//endfor - j
	   rasOut->write(slice, data);
	}//endfor - i


}//end - gradientMask


  GeoStar::Raster * Raster::operator+(const GeoStar::Raster & r2)
  {
    GeoStar::Image * img = Raster::getParent();
    std::string str = rastername + "_PLUS_" + r2.rastername;
    GeoStar::Raster * result = new GeoStar::Raster(img, str, raster_datatype, get_nx(), get_ny());
    this->add(&r2, result);
    return result;
  }
  void Raster::add(const GeoStar::Raster * r2, GeoStar::Raster * ras_out)
  {
    long int nx = get_nx(), ny = get_ny();
    if(nx != r2->get_nx() || ny != r2->get_ny())throw RasterSizeErrorException();
    std::vector<long int> slice(4);
    slice[0] = 0; slice[1] = 0; slice[2] = nx; slice[3] = 1;
    std::vector<float> bufferA(nx);
    std::vector<float> bufferB(nx);
    for(long int i = 0; i < ny; i++)
    {
      slice[1] = i;
      read(slice, bufferA);
      r2->read(slice, bufferB);
      for(int j = 0; j < get_nx(); j++)bufferA[j] += bufferB[j];
      ras_out->write(slice, bufferA);
    }
  }

  GeoStar::Raster * Raster::operator-(const GeoStar::Raster & r2)
  {
    GeoStar::Image * img = Raster::getParent();
    std::string str = rastername + "_MINUS_" + r2.rastername;
    GeoStar::Raster * result= new GeoStar::Raster(img, str, raster_datatype, get_nx(), get_ny());
    this->subtract(&r2, result);
    return result;
  }
  void Raster::subtract(const GeoStar::Raster * r2, GeoStar::Raster * ras_out)
  {
    long int nx = get_nx(), ny = get_ny();
    if(nx != r2->get_nx() || ny != r2->get_ny())throw RasterSizeErrorException();
    std::vector<long int> slice(4);
    slice[0] = 0; slice[1] = 0; slice[2] = nx; slice[3] = 1;
    std::vector<float> bufferA(nx);
    std::vector<float> bufferB(nx);
    for(long int i = 0; i < ny; i++)
    {
      slice[1] = i;
      read(slice, bufferA);
      r2->read(slice, bufferB);
      for(int j = 0; j < get_nx(); j++)bufferA[j] -= bufferB[j];
      ras_out->write(slice, bufferA);
    }
  }

  GeoStar::Raster * Raster::operator*(const GeoStar::Raster & r2)
  {
    GeoStar::Image * img = Raster::getParent();
    std::string str = rastername + "_TIMES_" + r2.rastername;
    GeoStar::Raster * result= new GeoStar::Raster(img, str, raster_datatype, get_nx(), get_ny());
    this->multiply(&r2, result);
    return result;
  }
  void Raster::multiply(const GeoStar::Raster * r2, GeoStar::Raster * ras_out)
  {
    long int nx = get_nx(), ny = get_ny();
    if(nx != r2->get_nx() || ny != r2->get_ny())throw RasterSizeErrorException();
    std::vector<long int> slice(4);
    slice[0] = 0; slice[1] = 0; slice[2] = nx; slice[3] = 1;
    std::vector<float> bufferA(nx);
    std::vector<float> bufferB(nx);
    for(long int i = 0; i < ny; i++)
    {
      slice[1] = i;
      read(slice, bufferA);
      r2->read(slice, bufferB);
      for(int j = 0; j < get_nx(); j++)bufferA[j] *= bufferB[j];
      ras_out->write(slice, bufferA);
    }
  }

  GeoStar::Raster * Raster::operator/(const GeoStar::Raster & r2)
  {
    GeoStar::Image * img = Raster::getParent();
    std::string str = rastername + "_DIVIDEDBY_" + r2.rastername;
    GeoStar::Raster * result= new GeoStar::Raster(img, str, raster_datatype, get_nx(), get_ny());
    this->divide(&r2, result);
    return result;
  }
  void Raster::divide(const GeoStar::Raster * r2, GeoStar::Raster * ras_out)
  {
    long int nx = get_nx(), ny = get_ny();
    if(nx != r2->get_nx() || ny != r2->get_ny())throw RasterSizeErrorException();
    std::vector<long int> slice(4);
    slice[0] = 0; slice[1] = 0; slice[2] = nx; slice[3] = 1;
    std::vector<float> bufferA(nx);
    std::vector<float> bufferB(nx);
    for(long int i = 0; i < ny; i++)
    {
      slice[1] = i;
      read(slice, bufferA);
      r2->read(slice, bufferB);
      for(int j = 0; j < get_nx(); j++)
      {
        if(bufferB[j] == 0)bufferA[j] = 255; // divide by zero goes to max value
        else bufferA[j] /= bufferB[j];
      }
      ras_out->write(slice, bufferA);
    }
  }

  Raster* Raster::resize(Image *img, int resize_width, int resize_height){
    /*long int nx = get_nx(), ny = get_ny();

    if (resize_width < 0 || resize_height < 0){
      throw RasterSizeErrorException();
    }

    if (resize_width == nx && resize_height == ny){
      return this;
    }

    Raster *ras_temp = img->create_raster("temp", GeoStar::INT8U, resize_width, ny);
    Raster *ras2 = img->create_raster("resized", GeoStar::INT8U, resize_width, resize_height);


    //first width, then height
    vector<float> src_width(nx);
    vector<float> src_height(ny);

    vector<float> destination_width(resize_width);
    vector<float> destination_height(resize_height);

    std::vector<long int> slice(4);
    slice[0]=0; slice [1]=0;
    slice[2]=nx; slice[3]=1;

    std::vector<long int> output_slice(4);
    output_slice[0]=0; output_slice [1]=0;
    output_slice[2]=resize_width; output_slice[3]=1;

    for (int i = 0; i < ny; ++i){
      //get new row
      slice[1] = i;
      read(slice, src_width);
      //change each row in ras2
      cv::resize(cv::InputArray(src_width), cv::OutputArray(destination_width), cv::Size(resize_width, 1), resize_width/nx, 1);
      output_slice[1] = i;
      ras_temp->write(output_slice, destination_width);
    }

    slice[3] = ny;
    slice[2] = 1;
    slice[1] = 0;
    output_slice[1] = 0;
    output_slice[2] = 1;
    output_slice[3] = resize_height;

    //now adjust the height
    for (int i = 0; i < resize_width; ++i){
      //get new column
      slice[0] = i;
      ras_temp->read(slice, src_height);
      //change each col in ras2
      cv::resize(cv::InputArray(src_height), cv::OutputArray(destination_height), cv::Size(resize_height, 1), resize_height/ny, 1);

      output_slice[0] = i;
      ras2->write(output_slice, destination_height);
    }
      delete ras2;
      return ras2;
	*/
  }

  GeoStar::Image * Raster::getParent()
  {
    size_t len = H5Iget_name(rasterobj->getId(),NULL,0);
    char buffer[len];
    H5Iget_name(rasterobj->getId(),buffer,len+1);
    std::string imgName = buffer;
    int a = imgName.find_first_of("/");
    imgName = imgName.substr(a,imgName.find_last_of("/")-a);

    std::string fileName = rasterobj->getFileName();
    GeoStar::File * f = new GeoStar::File(fileName, "existing");
    GeoStar::Image * img = new Image(f,imgName);
    delete f;
    return img;
  }

  GeoStar::Raster * GeoStar::Raster::operator+(const float & val)
  {
    GeoStar::Image * img = getParent();
    std::string str = rastername+"_PLUS_val";
    GeoStar::Raster * r2 = new GeoStar::Raster(img, str, raster_datatype, get_nx(),get_ny());
    std::vector<long int> slice(4);
    slice[0] = 0; slice[1] = 0; slice[2] = get_nx(); slice[3] = 1;
    std::vector<float> buffer(get_nx());
    for(int i = 0; i < get_ny(); i++)
    {
      slice[1] = i;
      read(slice, buffer);
      for(int j = 0; j < get_nx(); j++)buffer[j]+=val;
      r2->write(slice, buffer);
    }
    return r2;
  }
  GeoStar::Raster * GeoStar::Raster::operator-(const float & val)
  {
    GeoStar::Image * img = getParent();
    std::string str = rastername+"_MINUS_val";
    GeoStar::Raster * r2 = new GeoStar::Raster(img, str, raster_datatype, get_nx(),get_ny());
    std::vector<long int> slice(4);
    slice[0] = 0; slice[1] = 0; slice[2] = get_nx(); slice[3] = 1;
    std::vector<float> buffer(get_nx());
    for(int i = 0; i < get_ny(); i++)
    {
      slice[1] = i;
      read(slice, buffer);
      for(int j = 0; j < get_nx(); j++)buffer[j]-=val;
      r2->write(slice, buffer);
    }
    return r2;
  }
  GeoStar::Raster * GeoStar::Raster::operator*(const float & val)
  {
    GeoStar::Image * img = getParent();
    std::string str = rastername+"_TIMES_val";
    GeoStar::Raster * r2 = new GeoStar::Raster(img, str, raster_datatype, get_nx(),get_ny());
    std::vector<long int> slice(4);
    slice[0] = 0; slice[1] = 0; slice[2] = get_nx(); slice[3] = 1;
    std::vector<float> buffer(get_nx());
    for(int i = 0; i < get_ny(); i++)
    {
      slice[1] = i;
      read(slice, buffer);
      for(int j = 0; j < get_nx(); j++)buffer[j]*=val;
      r2->write(slice, buffer);
    }
    return r2;
  }
  GeoStar::Raster * GeoStar::Raster::operator/(const float & val)
  {
    GeoStar::Image * img = getParent();
    std::string str = rastername+"_DIVIDEDBY_val";
    GeoStar::Raster * r2 = new GeoStar::Raster(img, str, raster_datatype, get_nx(),get_ny());
    std::vector<long int> slice(4);
    slice[0] = 0; slice[1] = 0; slice[2] = get_nx(); slice[3] = 1;
    std::vector<float> buffer(get_nx());
    if(val == 0) // can't divide by zero
    {
      throw GeoStar::DivideByZeroException();
    }
    else
    {
      for(int i = 0; i < get_ny(); i++)
      {
        slice[1] = i;
        read(slice, buffer);
        for(int j = 0; j < get_nx(); j++)buffer[j]/=val;
        r2->write(slice, buffer);
      }
    }
    return r2;
  }
}// end namespace GeoStar
