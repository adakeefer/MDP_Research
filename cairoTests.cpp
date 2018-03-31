// cairoTests.cpp
//
// by Adam Keefer, March 18th, 2018
//
// tests cairo libraries and text/drawing, as well as possible hdf5 integration
//
// usage: cairoTests
//
//---------------------------------------------------------
#include <string>
#include <iostream>
#include <vector>
#include "geostar.hpp"

#include "/usr/local/include/cairo/cairo.h"
#include "/usr/local/include/cairo/cairo-pdf.h"

void baseTests();
void gridTests();

int main() {

//begin testing cairo structures
/*
	GeoStar::Map *map = new GeoStar::Map(240, 200);
	map->readPNG("beforeImg.png", 148, 151);
	map->writePNG("MapOutput.png");
	delete map;
	GeoStar::Map *map2 = new GeoStar::Map("MapOutputPDF.pdf", 180, 250);
	map2->readPNG("beforeImg.png", 148, 151);
	delete map2;
	GeoStar::Map *map3 = new GeoStar::Map("beforeImg.png", 350, 480, 
						148, 151);
	map3->writePNG("MapOutputCtor.png");
	delete map3;
*/
	GeoStar::Map *map = new GeoStar::Map(750, 630);
	map->readPNG("michigan.png", 675, 560);
	map->addLatLongGrid(45.4542, -87.6992, 41.3869, -81.7298);
	map->writePNG("michiganGrid.png");
	delete map;

return 0;
}//end - main

  void baseTests() {
	cairo_surface_t *surface =
            cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 240, 80);
        cairo_t *cr =
            cairo_create (surface);

        cairo_select_font_face (cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size (cr, 32.0);
        cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
        cairo_move_to (cr, 10.0, 50.0);
        cairo_show_text (cr, "Hello, world");

        cairo_destroy (cr);
        cairo_surface_write_to_png (surface, "hello.png");
        cairo_surface_destroy (surface);

	cairo_surface_t *canvas =
            cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 220, 240);
        cairo_t *obj =
            cairo_create (canvas);
	//cairo_paint_with_alpha(obj, 0.5);

	cairo_surface_t *image;
	image = cairo_image_surface_create_from_png("beforeImg.png");
	cairo_set_source_surface(obj, image, 40, 30);
	cairo_paint(obj);
	cairo_surface_destroy(image);
	cairo_surface_write_to_png(canvas, "readPNGOutput.png");
	cairo_destroy(obj);
	cairo_surface_destroy(canvas);
	

	cairo_set_source_rgb(obj, 0.9, 0.9, 1);
	cairo_select_font_face(obj, "Georgia", 
	CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(obj, 16);
	cairo_move_to(obj, 65, 200);
	cairo_show_text(obj, "My first map");
	cairo_select_font_face(obj, "Georgia", 
	CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(obj, 12);
	cairo_move_to(obj, 66, 219);
	cairo_show_text(obj, "by Adam Keefer");

	cairo_surface_write_to_png(canvas, "possibleMap.png");
	cairo_destroy(obj);
	cairo_surface_destroy(canvas);


	
	cairo_surface_t *pdf;
	pdf = cairo_pdf_surface_create("output.pdf", 432, 648);
	cairo_t *thing = cairo_create(pdf);
	cairo_set_source_rgb(thing, 1, 1, 1);
	cairo_paint(thing);
	cairo_surface_t *imagePNG;
	imagePNG = cairo_image_surface_create_from_png("beforeImg.png");
	cairo_set_source_surface(thing, imagePNG, 200, 330);
	cairo_paint(thing);
	cairo_surface_destroy(imagePNG);
	cairo_destroy(thing);
	cairo_surface_destroy(pdf);
	}//end - baseTests

 void gridTests() {
//675x560
	cairo_surface_t *surface =
            cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 750, 630);
        cairo_t *cr =
            cairo_create (surface);
	cairo_surface_t *image;
	image = cairo_image_surface_create_from_png("michigan.png");
	cairo_set_source_surface(cr, image, 38, 35);
	cairo_paint(cr);
	cairo_surface_destroy(image);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 1);

	cairo_select_font_face(cr, "Georgia", 
	CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 10);

	double lat = 6.23494, longit = -6.02394;
	string latString = to_string(lat);
	string longString = to_string(longit);
	cairo_text_extents_t latExtents;
	cairo_text_extents_t longExtents;

	for (int i = 1; i < 9; i += 2) {
	cairo_move_to(cr, 28, 80 * i);
	cairo_line_to(cr, 723, 80 * i);
	lat += i / 5.0;
	latString = to_string(lat);
	cairo_text_extents(cr, latString.c_str(), &latExtents);
	cairo_move_to(cr, 27 - latExtents.x_bearing - latExtents.width / 2, 
			(80 * i) - latExtents.y_bearing - latExtents.height / 2);
	cairo_show_text(cr, latString.c_str());
	cairo_move_to(cr, 90 * i, 25);
	cairo_line_to(cr, 90 * i, 605);
	longit += i / 5.0;
	longString = to_string(longit);
	cairo_text_extents(cr, longString.c_str(), &longExtents);
	cairo_move_to(cr, (90 * i) - longExtents.x_bearing - longExtents.width / 2, 
			25 - longExtents.y_bearing - longExtents.height / 2);
	cairo_show_text(cr, longString.c_str());
	}

	cairo_stroke(cr);
	cairo_surface_write_to_png(surface, "michigantest.png");

	cairo_destroy(cr);
	cairo_surface_destroy(surface);
 }

