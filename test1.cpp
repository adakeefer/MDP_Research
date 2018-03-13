// test1.cpp
//
// by Leland Pierce, Feb 18, 2017
//
// test out our HDF5-based objects (File, Image, Raster)
//
// usage: test1
//
//---------------------------------------------------------
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>

#include "geostar.hpp"

#include "boost/filesystem.hpp"


main() {

  std::vector<uint8_t> data(100*100);

  for(int j=0;j<100;++j) for(int i=0;i<100;++i) data[j*100+i]=(j*100+i)%255;

  // delete output file if already exists
  boost::filesystem::path p("a1.h5");
  boost::filesystem::remove(p);

  GeoStar::File *file = new GeoStar::File("a1.h5","new");

  GeoStar::Image *img = file->create_image("ers1");

  delete img;

  img = file->open_image("ers1");
  
  GeoStar:: Raster *ras = img->create_raster("chan1", GeoStar::INT8U, 512, 1024);

  delete ras;

  ras = img->open_raster("chan1");


  std::vector<long int> slice(4);
  slice[0]=10; slice [1]=15;
  slice[2]=100; slice[3]=100;
  ras->write(slice, data);




  slice[0]=0;  slice[1]=0;
  slice[2]=20; slice[3]=20;
  ras->read(slice,data);
  for(int j=0;j<20;++j) {
    for(int i=0;i<20;++i) {
      std::cout << (int)data[j*20+i]<<" ";
    }//endfor: i
    std::cout <<std::endl;
  }//endfor: j

  ras->thresh(50.);

  std::cout<<"after thresh:"<<std::endl;
  ras->read(slice,data);
  for(int j=0;j<20;++j) {
    for(int i=0;i<20;++i) {
      std::cout << (int)data[j*20+i]<<" ";
    }//endfor: i
    std::cout <<std::endl;
  }//endfor: j


  delete ras;

  delete img;

  delete file;

}// end-main
