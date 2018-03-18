// Map.hpp
// 
// by Adam Keefer, March 18, 2018
//
//-------------------------------------
#ifndef MAP_HPP_
#define MAP_HPP_

#include <iostream>
#include <string>
#include <vector>
#include "/usr/local/include/cairo/cairo.h"
#include "/usr/local/include/cairo/cairo-pdf.h"

namespace GeoStar {

/** \brief Map -- Data structure for creating maps


This class is used to to create Topographical maps, UTM projections, etc given a raw satellite image.
It employs a range of drawing techniques and will output to a PDF format

\see File, Image, Raster

\par Usage Overview
At the moment, this class operates seperately from the file->image->raster heirarchy of GeoStar
and the HDF5 library.

All operations in this class require the raw map image in either png or pdf format to create the map

They also require the latitude/longitude of the image so that proper labels can be generated.

This class contains several routines that each add small bits of information to the map - see below for more details.

The final map can be either output to a new pdf or existing pdf, or a new or existing png image.

\par Details
This class keeps track of the filenames for reading/writing, the lat/long coordinates of the image,
and the image data itself.

*/	

class Map {
private:
 std::string fileName;
 double lat;
 double long;
 cairo_surface_t *image;

public:






};//endclass - map

}//end - namespace GeoStar

#endif // MAP_HPP_
