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
	mapSizeX = imageSizeX;
	mapSizeY = imageSizeY;
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
	mapSizeX = imageSizeX;
	mapSizeY = imageSizeY;
	

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


  void Map::addLatLongGrid(double latTop, double longTop, 
			double latBottom, double longBottom) {
	cairo_t *cr = cairo_create(image);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 1);

	cairo_select_font_face(cr, "Times New Roman", 
	CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 10);

	cairo_text_extents_t latExtents;
	cairo_text_extents_t longExtents;

	std::string coordString;

	double latIncrement = (latTop - latBottom) / 5;
	double longIncrement = (longTop - longBottom) / 5;

	size_t widthIncrement = mapSizeX / 5;
	size_t heightIncrement = mapSizeY / 5;
	size_t mapBeginX = (sizeX - mapSizeX) / 2;
	size_t mapBeginY = (sizeY - mapSizeY) / 2;
	size_t tickSize = ((sizeX < sizeY) ? sizeX / 100 : sizeY / 100);
	if (mapBeginX > tickSize) mapBeginX -= tickSize;
	else mapBeginX -= (tickSize / 3);
	if (mapBeginY > tickSize) mapBeginY -= tickSize;
	else mapBeginY -= (tickSize / 3);

	for (int i = 1; i < 5; ++i) {
	 cairo_move_to(cr, mapBeginX, mapBeginY + (heightIncrement * i));
	 cairo_line_to(cr, sizeX - mapBeginX, mapBeginY + (heightIncrement * i));

	 coordString = to_string(latTop - (latIncrement * i));
	 cairo_text_extents(cr, coordString.c_str(), &latExtents);
	 cairo_move_to(cr, mapBeginX - latExtents.x_bearing - latExtents.width / 2, 
			 (mapBeginY + (heightIncrement * i))
			 - latExtents.y_bearing - latExtents.height / 2);
	 cairo_show_text(cr, coordString.c_str());	 
	 cairo_move_to(cr, (sizeX - mapBeginX) - latExtents.x_bearing - latExtents.width / 2, 
			 (mapBeginY + (heightIncrement * i))
			 - latExtents.y_bearing - latExtents.height / 2);
	 cairo_show_text(cr, coordString.c_str());

	 cairo_move_to(cr, mapBeginX + (widthIncrement * i), mapBeginY);
	 cairo_line_to(cr, mapBeginX + (widthIncrement * i), sizeY - mapBeginY);

	 coordString = to_string(longTop - (longIncrement * i));
	 cairo_text_extents(cr, coordString.c_str(), &longExtents);
	 cairo_move_to(cr, (mapBeginX + (widthIncrement * i)) - longExtents.x_bearing - longExtents.width / 2, 
			 mapBeginY - longExtents.y_bearing - longExtents.height / 2);
	 cairo_show_text(cr, coordString.c_str());	 
	 cairo_move_to(cr, (mapBeginX + (widthIncrement * i)) - longExtents.x_bearing - longExtents.width / 2, 
			 (sizeY - mapBeginY) - longExtents.y_bearing - longExtents.height / 2);
	 cairo_show_text(cr, coordString.c_str());
	
	}//endfor - lines and labels

	cairo_stroke(cr);
	cairo_destroy(cr);

  }//end - addLatLongGrid

   

}//end - namespace GeoStar
