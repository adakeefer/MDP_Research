// test4.cpp
//
// by Adam Keefer, November 11th, 2017
//
// tests fftw library as well as some fourier transforms and dft stuff
//
// usage: test4
//
//---------------------------------------------------------
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <complex>
#include "geostar.hpp"
#include <cmath>

#include "boost/filesystem.hpp"

const double PI = 3.1415926535897;

void FFTW_baseTest(const long int nx);

void FFTW_2D_C2C_Test(const long int nx, const long int ny, GeoStar::Image *img, GeoStar::Raster *rasIn,
				GeoStar::Raster *rasOutReal, GeoStar::Raster *rasOutImg);

void FFTW_2D_C2C_Cos_Test(long int nx, long int ny, GeoStar::Image *img, 
			GeoStar::Raster *rasOutReal, GeoStar::Raster *rasOutImg, GeoStar::Raster *rasOutSquared);


main() {

  // delete output file if already exists
  boost::filesystem::path p("a4.h5");
  boost::filesystem::remove(p);

	//define all necessary testing structures
  GeoStar::File *file = new GeoStar::File("a4.h5", "new");

  //GeoStar::Image *img = file->create_image("landsat");
	GeoStar::Image *img = file->create_image("beach");

  GeoStar::Raster *ras;
	
  //ras = img->read_file("LC08_L1TP_027033_20170506_20170515_01_T1_B7.TIF", "B07", 1);
	ras = img->read_file("beach.jpg", "chan1", 1);

  long int nx = ras->get_nx();
  long int ny = ras->get_ny();

  GeoStar:: Raster *ras2 = img->create_raster("test1", GeoStar::REAL32, nx, ny);
  GeoStar:: Raster *ras3 = img->create_raster("test2", GeoStar::REAL32, nx, ny);  
  GeoStar:: Raster *ras4 = img->create_raster("test3", GeoStar::REAL32, nx, ny); 

//begin testing fftw objects

	//FFTW_2D_C2C_Cos_Test(nx, ny, img, ras2, ras3, ras4);
	//ras2->FFT_2D_Inv(img, ras4, ras3);
	//FFTW_2D_C2C_Test(nx, ny, img, ras, ras2, ras3);
	//ras->FFT_2D(img, ras2, ras3);
	//ras2->FFT_2D_Inv(img, ras4, ras3);
	ras->lowPassFilter(img, ras2, ras3, ras4);

  delete ras;
  
  delete ras2; 

  delete ras3;
  
  delete ras4;

  delete img;

  delete file;

}// end-main


	void FFTW_baseTest(const long int nx) {
	//complex data declaration, dynamic memory test
	fftw_complex *test;
	test = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);
	fftw_free(test);

	//plan data structure test, create plan for discrete fourier transform - one dimensional
	fftw_complex *in, *out;
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);

	fftw_plan plan;
	plan = fftw_plan_dft_1d(nx, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	//execute plan
	fftw_execute(plan);
	//delete plan at end
	fftw_destroy_plan(plan);
	fftw_free(in); fftw_free(out);
	}//end - basetest


	//now try to integrate hdf5 objects with fftw objects - 2D complex to complex / real to complex
	void FFTW_2D_C2C_Test(const long int nx, const long int ny, GeoStar::Image *img, GeoStar::Raster *rasIn,
				GeoStar::Raster *rasOutReal, GeoStar::Raster *rasOutImg) {
	fftw_complex *in2, *out2;
	in2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);
	out2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);
	fftw_plan plan2;
	plan2 = fftw_plan_dft_1d(nx, in2, out2, FFTW_FORWARD, FFTW_ESTIMATE);

	std::vector<long int>sliceFFTW(4);
    		sliceFFTW[0]=0;
    		sliceFFTW[1]=0;
    		sliceFFTW[2]=nx;
    		sliceFFTW[3]=1;
	std::vector<double> dataReal(nx);
	std::vector<double> dataImg(nx);
	
	GeoStar:: Raster *rasBufferReal = img->create_raster("BufferReal", GeoStar::REAL32, nx, ny);
	GeoStar:: Raster *rasBufferImg = img->create_raster("BufferImg", GeoStar::REAL32, nx, ny);

	//transform row by row
	for (int y = 0; y < ny; ++y) {	
	sliceFFTW[1] = y;
	rasIn->read(sliceFFTW, dataReal);
	for(int i=0;i<nx;i++) { 
	   in2[i][0]=dataReal[i]; 
	   in2[i][1]=0.0;
	}

	
	fftw_execute(plan2);
	
	for (int i = 0; i < nx; i++) {
		dataReal[i] = out2[i][0];
		dataImg[i] = out2[i][1];
	}
	rasBufferReal->write(sliceFFTW, dataReal);
	rasBufferImg->write(sliceFFTW, dataImg);
	
	}//endfor - row-by-row

	//now delete objects and reinitialize for the ny size - cols transform
	fftw_destroy_plan(plan2);
	fftw_free(in2); fftw_free(out2);

	fftw_complex *inCols, *outCols;
	inCols = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ny);
	outCols = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ny);
	fftw_plan planCols;
	planCols = fftw_plan_dft_1d(ny, inCols, outCols, FFTW_FORWARD, FFTW_ESTIMATE);

		sliceFFTW[0] = 0;
    		sliceFFTW[1] = 0;
    		sliceFFTW[2] = 1;
		sliceFFTW[3] = ny;

	dataReal.resize(ny);
	dataImg.resize(ny);
	cout << "at 1" << endl;
	//transform col-by-col	
	for (int x = 0; x < nx; ++x) {
	cout << " x equals " << x << endl;
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

	cout << "at 3" << endl;
	delete rasBufferReal;
	delete rasBufferImg;
	fftw_destroy_plan(planCols);
	fftw_free(inCols); fftw_free(outCols);
	
	}//end - FFTW_2D_C2C_Test






   void FFTW_2D_C2C_Cos_Test(long int nx, long int ny, GeoStar::Image *img, 
			GeoStar::Raster *rasOutReal, GeoStar::Raster *rasOutImg, GeoStar::Raster *rasOutSquared) {
	fftw_complex *in2, *out2;
	in2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);
	out2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nx);
	fftw_plan plan2;
	plan2 = fftw_plan_dft_1d(nx, in2, out2, FFTW_FORWARD, FFTW_ESTIMATE);

	std::vector<long int>sliceFFTW(4);
    		sliceFFTW[0]=0;
    		sliceFFTW[1]=0;
    		sliceFFTW[2]=nx;
    		sliceFFTW[3]=1;
	std::vector<double> dataReal(nx);
	std::vector<double> dataImg(nx);
	
	GeoStar:: Raster *rasBufferReal = img->create_raster("BufferReal", GeoStar::REAL32, nx, ny);
	GeoStar:: Raster *rasBufferImg = img->create_raster("BufferImg", GeoStar::REAL32, nx, ny);

	//transform row by row
	for (int y = 0; y < ny; ++y) {	
	sliceFFTW[1] = y;
	for(int i=0;i<nx;i++) { 
	   in2[i][0]=cos(10*PI*i / nx) + sin(3.6 * PI * i / nx); 
	   in2[i][1]=0.0;
	}

	/*for (int i = 0; i < nx; ++i) {
	cout << "cos real part " << in2[i][0] << endl;
	cout << "imaginary part " << in2[i][1] << endl;
	}*/

	fftw_execute(plan2);
	
	for (int i = 0; i < nx; i++) {
		dataReal[i] = out2[i][0];
		dataImg[i] = out2[i][1];
	}
	rasBufferReal->write(sliceFFTW, dataReal);
	rasBufferImg->write(sliceFFTW, dataImg);
	
	}//endfor - row-by-row

	//now delete objects and reinitialize for the ny size - cols transform
	fftw_destroy_plan(plan2);
	fftw_free(in2); fftw_free(out2);

	fftw_complex *inCols, *outCols;
	inCols = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ny);
	outCols = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * ny);
	fftw_plan planCols;
	planCols = fftw_plan_dft_1d(ny, inCols, outCols, FFTW_FORWARD, FFTW_ESTIMATE);

		sliceFFTW[0] = 0;
    		sliceFFTW[1] = 0;
    		sliceFFTW[2] = 1;
		sliceFFTW[3] = ny;

	dataReal.resize(ny);
	dataImg.resize(ny);
	std::vector<double>dataSquared(ny);
	cout << "at 1" << endl;
	//transform col-by-col	
	for (int x = 0; x < nx; ++x) {
	cout << " x equals " << x << endl;
	sliceFFTW[0] = x;
	rasBufferReal->read(sliceFFTW, dataReal);
	rasBufferImg->read(sliceFFTW, dataImg);
	for (int i = 0; i < ny; ++i) {
		inCols[i][0] = dataReal[i];
		inCols[i][1] = dataImg[i];
		//cout << " before fft - inCols Real equals " << inCols[i][0] << endl;
		//cout << " before fft - inCols Imaginary equals " << inCols[i][1] << endl;
	}

	fftw_execute(planCols);
	
	for (int i = 0; i < ny; ++i) {
		//cout << " after fft - outCols Real equals " << outCols[i][0] << endl;
		//cout << " after fft - outCols Imaginary equals " << outCols[i][1] << endl;
		dataReal[i] = outCols[i][0];
		dataImg[i] = outCols[i][1];
		dataSquared[i] = pow(outCols[i][0], 2) + pow(outCols[i][1], 2);
	}
	rasOutReal->write(sliceFFTW, dataReal);
	rasOutImg->write(sliceFFTW, dataImg);
	rasOutSquared->write(sliceFFTW, dataSquared);

	}//endfor - col-by-col

	cout << "at 3" << endl;
	delete rasBufferReal;
	delete rasBufferImg;
	fftw_destroy_plan(planCols);
	fftw_free(inCols); fftw_free(outCols);

	}//end - FFTW_2D_C2C_Cos_Test
	
	

