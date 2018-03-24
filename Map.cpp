// Map.cpp
// 
// by Adam Keefer, March 24, 2018
//
//-------------------------------------

#include <iostream>
#include <string>
#include <vector>
#include "Exceptions.hpp"
#include "Map.hpp"
#include "/usr/local/include/cairo/cairo.h"
#include "/usr/local/include/cairo/cairo-pdf.h"

namespace GeoStar {

   Map::Map(size_t surfaceSizeX, size_t surfaceSizeY) {
	MapSizeException MapSizeError;
	if (surfaceSizeX <= 0) throw MapSizeError;
	if (surfaceSizeY <= 0) throw MapSizeError;

	sizeX = surfaceSizeX;
	sizeY = surfaceSizeY;
        image = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 
						surfaceSizeX, surfaceSizeY);

   }//end - Map()

   Map::Map(const std::string &PDFName, size_t surfaceSizeX, size_t surfaceSizeY) {
	MapSizeException MapSizeError;
	if (surfaceSizeX <= 0) throw MapSizeError;
	if (surfaceSizeY <= 0) throw MapSizeError;

	sizeX = surfaceSizeX;
	sizeY = surfaceSizeY;
	pdfName = PDFName;
	image = cairo_pdf_surface_create(PDFName.c_str(), surfaceSizeX, surfaceSizeY);

   }//end - Map()


   Map::Map(const std::string &fileName, size_t surfaceSizeX, size_t surfaceSizeY, 
		size_t imageSizeX, size_t imageSizeY) {
	MapSizeException MapSizeError;
	PNGSizeException PNGSizeError;
	if (surfaceSizeX <= 0) throw MapSizeError;
	if (surfaceSizeY <= 0) throw MapSizeError;

	sizeX = surfaceSizeX;
	sizeY = surfaceSizeY;
        image = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 
						surfaceSizeX, surfaceSizeY);

	readPNG(fileName, imageSizeX, imageSizeY);
   }//end - Map()

   void Map::readPNG(const std::string &fileName, size_t imageSizeX, 
		 	size_t imageSizeY) {
	PNGSizeException PNGSizeError;
	if (imageSizeX <= 0) throw PNGSizeError;
	if (imageSizeY <= 0) throw PNGSizeError;
	if (imageSizeX > sizeX) throw PNGSizeError;
	if (imageSizeY > sizeY) throw PNGSizeError;

	cairo_t *cr = cairo_create(image);
	cairo_surface_t *png;
	png = cairo_image_surface_create_from_png(fileName.c_str());
	cairo_set_source_surface(cr, png, (sizeX - imageSizeX) / 2,
					(sizeY - imageSizeY) / 2);
	cairo_paint(cr);
	cairo_surface_destroy(png);
	cairo_destroy(cr);
   }//end - readPNG

   void Map::writePNG(const std::string &fileName) {
	cairo_surface_write_to_png(image, fileName.c_str());
   }//end - writePNG

   

}//end - namespace GeoStar
