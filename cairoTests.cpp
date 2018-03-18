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

#include "boost/filesystem.hpp"
#include "/usr/local/include/cairo/cairo.h"
#include "/usr/local/include/cairo/cairo-pdf.h"

int main() {


  // delete output file if already exists
  boost::filesystem::path p("cairo.h5");
  boost::filesystem::remove(p);

	//define all necessary testing structures
  GeoStar::File *file = new GeoStar::File("cairo.h5", "new");

  GeoStar::Image *img = file->create_image("bearImg");

  GeoStar::Raster *ras;
	
  ras = img->read_file("bear.jpg", "bear", 1);

  long int nx = ras->get_nx();
  long int ny = ras->get_ny();

  GeoStar:: Raster *ras2 = img->create_raster("test1", GeoStar::REAL32, nx, ny);

//begin testing cairo structures

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
	cairo_paint_with_alpha(obj, 0.5);

	cairo_surface_t *image;
	image = cairo_image_surface_create_from_png("beforeImg.png");
	cairo_set_source_surface(obj, image, 40, 30);
	cairo_paint(obj);
	cairo_surface_destroy(image);
	
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

 delete ras2;
 delete ras;
 delete img;
 delete file;

return 0;
}//end - main
