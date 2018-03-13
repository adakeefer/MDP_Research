// test2.cpp
//
// by Leland Pierce, March 11, 2017
//
// test out our HDF5-based objects (File, Image, Raster)]
// adds calls to read tiff images, landsat stuff...
//
// usage: test2
//
//---------------------------------------------------------
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>
#include <exception>

#include "geostar.hpp"

#include "boost/filesystem.hpp"


main() {

  // delete output file if already exists
  boost::filesystem::path p("a2.h5");
  boost::filesystem::remove(p);

  GeoStar::File *file = new GeoStar::File("a2.h5", "new");

  GeoStar::Image *img = file->create_image("landsat");

  GeoStar::Raster *ras;
	//download your own tiff images from USGS landsat repository and insert filename here
  ras = img->read_file("bear.jpg", "bear", 1);

  long int nx = ras->get_nx();
  long int ny = ras->get_ny();

  GeoStar::Raster *ras2 = img->create_raster("test", GeoStar::REAL32, nx, ny);

  //std::vector<long int> slice(4);
  //slice[0]=1000; slice [1]=1500;
  //slice[2]=4000; slice[3]=4000;

  //long int slice2[4] = {1000, 1500, 4000, 4000};

  //std::vector<double> buffer;

//comment and uncomment tests as needed, check if everything runs
    //ras->read(slice, buffer);
   //ras2->write(slice, buffer);
  //ras->set(slice2, 5000);
  //ras->copy(slice2, ras2);
  //ras->scale(ras2, 100, 0.03);
  //ras->thresh(7000);
  //ras2->thresh(10000);
   //ras->downsample(ras2);

/*
    std::vector<GeoStar::Raster *> outputGaus =
	ras->gaussianPyramid(img, 4);

    std::vector<GeoStar::Raster *> outputLap = 
	outputGaus[4]->laplacianPyramid(img, 4);
*/

  //ras->gradientMask(ras2, 5);
  //ras->harmonicMean(ras2, 5);
	//ras->midpointFilter(ras2, 11);
	ras->rangeFilter(ras2, 11);

  delete ras;
  
  delete ras2; 

  delete img;

  delete file;

}// end-main
