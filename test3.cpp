// test3.cpp
//
// by Adam Keefer, 9/25/17
//
// tests to file, image, and ras Exceptions
// comment and uncomment tests as needed
//
// usage: test3
//
//---------------------------------------------------------
#include <string>
#include <iostream>
#include <cstdint>
#include <vector>

#include "geostar.hpp"

#include "boost/filesystem.hpp"


main() {

  // delete output file if already exists
  boost::filesystem::path p("a3.h5");
  boost::filesystem::remove(p);

  GeoStar::File *file = new GeoStar::File("a3.h5", "new");

	//should throw fileexistsexception
	//GeoStar::File *file2 = new GeoStar::File("a3.h5", "new");

	//should throw filedoesnotexistexception
	//GeoStar::File *file2 = new GeoStar::File("a4.h5", "existing");

	//should throw fileaccessexception
	//GeoStar::File *file2 = new GeoStar::File("a4.h5", "blah");

	/*  should throw imageopenerror
  	GeoStar::Image *img = file->create_image("Example1");
	img->write_object_type("blah");

	delete img;

	GeoStar::Image *img2 = file->open_image("Example1");
	*/

	/* should throw imageopenerror
	GeoStar::Image *img3 = file->create_image("Example2");
	img3->write_object_type("blah");

	delete img3;

	GeoStar::Image *img4 = new GeoStar::Image(file, "Example2");
	*/

	GeoStar::Image *img5 = file->create_image("Example3");

	GeoStar::Raster *ras;

	//should throw fileopenerror
	//ras = img5->read_file("blah.tif", "test", 1);
	
	
	//should throw rasterdoesnotexist
	//ras = new GeoStar::Raster(img5, "blah");

	/*should throw rasteropenerror
	ras = new GeoStar::Raster(img5, "test", GeoStar::REAL32, 20, 20);
	ras->write_object_type("blah");
	delete ras;
	GeoStar::Raster *ras2 = new GeoStar::Raster(img5, "test");
	*/

	//should throw rastercreationerror
	//ras = new GeoStar::Raster(img5, "test", GeoStar::COMPLEX_REAL128, 20, 20);

	/*should throw rasterexistserror
	ras = new GeoStar::Raster(img5, "test", GeoStar::REAL32, 20, 20);
	delete ras;
	GeoStar::Raster *ras2 = new GeoStar::Raster(img5, "test", GeoStar::REAL32, 20, 20);
	*/

	ras = new GeoStar::Raster(img5, "test", GeoStar::REAL32, 20, 20);
	vector<double> buffer;

	/*should throw slicesizeerror
	vector<long int> slice(3);
	ras->write(slice, buffer);
	*/

	/*should throw slicesizeerror
	vector<long int> slice(3);
	ras->read(slice, buffer);
	*/
	

	delete ras;
	delete img5;
	delete file;
}//endmain


