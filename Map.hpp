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
#include "Exceptions.hpp"
#include "/usr/local/include/cairo/cairo.h"
#include "/usr/local/include/cairo/cairo-pdf.h"

namespace GeoStar {

/** \brief Map -- Data structure for creating maps

This class is used to to create Topographical maps, UTM projections, etc given a raw satellite image.
It employs a range of drawing techniques and will output to a PNG or PDF format

\see File, Image, Raster

\par Usage Overview
At the moment, this class operates seperately from the file->image->raster heirarchy of GeoStar
and the HDF5 library.

All operations in this class require the raw map image in png format to create the map

They also require the latitude/longitude of the image so that proper labels can be generated.

This class contains several routines that each add small bits of information to the map - see below for more details.

The final map can be either output to a new pdf, or a new or existing png image.

The map class cannot be used to read from an existing pdf - only to create a new one.

There is no function to write to a pdf - the Map class automatically writes to the pdf
with every operation.  So, using the appropriate pdf constructor will create an empty 
pdf file of the desired size in your current directory and will store that data 
in the Map class for future operations.

\par Details
This class keeps track of the lat/long coordinates of the image,
the size of the current surface, if you are creating a pdf, and the image data itself.

*/	

class Map {
private:
 double lat = 0;
 double longitude = 0;
 size_t sizeX;
 size_t sizeY;
 std::string pdfName = "";
 cairo_surface_t *image;

public:

/** \brief Map Constructor -- allows you to create an empty Map

    This constructor allows you to create a blank Map of predetermined size.
	This map cannot be used to output to a PDF - see the next constructor.

    \see Map(), readPNG()

    \param[in] surfaceSizeX
	The size of your map - x coordinate.

    \param[in] surfaceSizeY
      The size of your map - y coordinate

    \returns
	A valid Map object upon success.

    \Par Exceptions
	MapSizeException

    \Par Example
	Creating an empty map:
	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::Map *map = new GeoStar::Map(432, 648);

	delete map;
	}

	\endcode

    \Par Details

	The surface sizes must be greater than zero, else 
	MapSizeException is thrown.

	After creation, you can imprint the image centered on your map using readPNG().
    */
   Map(size_t surfaceSizeX, size_t surfaceSizeY);


/** \brief Map Constructor -- allows you to create an empty PDF Map

    This constructor allows you to create a blank Map of predetermined size.
	This constructor specifies a pdf output filname, 
	and is the only way to create a PDF Map.

    \see Map(), readPNG()

    \param[in] PDFName
	What you want to name your output pdf file.

    \param[in] surfaceSizeX
	The size of your map - x coordinate.

    \param[in] surfaceSizeY
      The size of your map - y coordinate.

    \returns
	A valid Map object upon success.

    \Par Exceptions
	MapSizeException

    \Par Example
	Creating an empty map for PDF output:
	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::Map *map = new GeoStar::Map("myMap.pdf", 432, 648);

	delete map;
	}

	\endcode

    \Par Details

	The surface sizes must be greater than zero, else 
	MapSizeException is thrown.
	Again, this is the only way to create a map for PDF output.
	Then, you can imprint the image centered on your map using readPNG().

	This constructor cannot be used to read from an existing pdf - only
	to create a new one.

	Remember, there is no function to write to a pdf - it is done automatically.
    */
   Map(const std::string &PDFName, size_t surfaceSizeX, size_t surfaceSizeY);


/** \brief Map Constructor -- allows you to create a Map from an Image

    This constructor allows you to create a new map with a png image (your raw image)
	centered at the center.

    \see Map(), readPNG()

    \param[in] fileName
	The name of the png file you want to read in.

    \param[in] surfaceSizeX
	The size of your map - x coordinate.

    \param[in] surfaceSizeY
      The size of your map - y coordinate

    \param[in] imageSizeX
	The size of your image - x coordinate.

    \param[in] imageSizeY
      The size of your image - y coordinate

    \returns
	A valid Map object upon success.

    \Par Exceptions
	MapSizeException
	PNGSizeException

    \Par Example
	Creating a map with landsat data:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::Map *map = new GeoStar::Map("LC08_L1TP_01_T1_B7.png",
				942, 1024, 745, 850);

	delete map;
	}

	\endcode

	<table>
        <tr>
        \imageSize{readPNGOutput.png,height:400px;width:350px;}
        \image html readPNGOutput.png "New map with .png data"
        </tr>
        </table>

    \Par Details

	The surface sizes and image sizes must be greater than zero, else 
	MapSizeException or PNGSizeException, respectively, are thrown.
	The image sizes must be smaller than their respective surface sizes, 
	else PNGSizeException is thrown.
    */
   Map(const std::string &fileName, size_t surfaceSizeX, size_t surfaceSizeY, 
		size_t imageSizeX, size_t imageSizeY);

   /** \brief Map destructor allows one to delete a map object from memory.

   The Map destructor is automatically called to clean up memory used by the Map object.
   There is a single pointer to the image surface, or data, that must be deleted.

   \see Map()

   \returns
       Nothing.

   \par Exceptions
       None.

   \par Example
       creating a raster named "test":
       \code
       #include "geostar.hpp"
       #include <string>
       using namespace std;
       int main()
       {
         GeoStar::Map *map = new GeoStar::Map("LC08_L1TP_01_T1_B7.png",
				942, 1024, 745, 850);

       }//end-main
       \endcode

       Before main ends, the c++ run-time system automatically destroys the map object
       using the Map object destructor.

  */

   inline ~Map() {
	cairo_surface_destroy(image);
    }//end - destructor


/** \brief readPNG() -- allows you to read image data from a png file

    This function allows you to add an image to the center of 
	an already existing map, reading from a .png file.

    \see Map()

    \param[in] fileName
	The name of the png file you want to read in.

    \param[in] imageSizeX
	The size of your image - x coordinate.

    \param[in] imageSizeY
      The size of your image - y coordinate

    \returns
	nothing

    \Par Exceptions
	PNGSizeException

    \Par Example
	Reading a png to add to a prexisting map for pdf output:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::Map *map = new GeoStar::Map(942, 1024, true);
	map->readPNG("LC08_L1TP_01_T1_B7.png", 745, 850);

	delete map;
	}

	\endcode

	<table>
        <tr>
        \imageSize{readPNGOutput.png,height:400px;width:350px;}
        \image html readPNGOutput.png "New map with .png data"
        </tr>
        </table>

    \Par Details

	The  image sizes must be greater than zero, else 
	PNGSizeException is thrown.
	The image sizes must be smaller than their respective map sizes, 
	else PNGSizeException is thrown.
    */
   void readPNG(const std::string &fileName, size_t imageSizeX, 
		 size_t imageSizeY);


/** \brief writePNG() -- allows you to write a map to a png file.

    This function allows you to write the map data to a png file.
	The file can be existing or not - if not, it will be created.

    \see readPNG()

    \param[in] fileName
	The name of the png file you want to write to.

    \returns
	nothing

    \Par Exceptions
	none

    \Par Example
	Creating a new map with png data, then outputting to a new image:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::Map *map = new GeoStar::Map("LC08_L1TP_01_T1_B7.png", 
					942, 1024, 745, 850);
	map->writePNG("newOutputMap.png");

	delete map;
	}

	\endcode

	<table>
        <tr>
        \imageSize{readPNGOutput.png,height:400px;width:350px;}
        \image html readPNGOutput.png "New map with .png data"
        </tr>
        </table>

    \Par Details

	Note that if you choose to write to an existing PNG, the image
	data will be overwritten.  
    */
   void writePNG(const std::string &fileName);

   




};//endclass - map

}//end - namespace GeoStar

#endif // MAP_HPP_
