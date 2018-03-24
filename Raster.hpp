// Raster.hpp
//
// Header and documentation for most
// Image processing routines in Geostar
//----------------------------------------
#ifndef RASTER_HPP_
#define RASTER_HPP_


#include <string>
#include <vector>

#include "H5Cpp.h"
#include "Exceptions.hpp"
#include "RasterType.hpp"
#include "attributes.hpp"

//#include <opencv2/opencv.hpp>
#include <fftw3.h>

namespace GeoStar {
  class Image;
  class File;



  /** \brief Raster -- Class to implement image and channel manipulation functions for HDF5-Raster Files

  This class is used to deal with image objects that have been converted into the HDF5 file format, and is the lowest level
  at which image processing operations happen.  Distinct channels of an image are read and stored as Rasters within an HDF5 image.
  Contains functions to read, write, open, and close rasters, as well as a variety of image manipulation tools.

 \see File, Image, Raster, RasterType

 \Par Usage Overview

  This class is meant to be used in conjunction with other HDF5 classes, namely File and Image.

  You must create or open a file and image before you can create a new raster or open an existing one.

  The Raster must be of one of the raster types defined in RasterType.hpp (ex. REAL32)

  To close, delete the Raster and then delete the Image, then the File.

 \Par Details

  This class has two constructors, to create a new raster or open an existing one.  It also has framework to handle reading
  and writing the object type attribute (Geostar).

  Read and Write for Rasters are type dynamic, i.e. the function will automatically read the data type necessary based on your buffer.

  */

  class Raster {

  private:
    std::string rastername;
    std::string rastertype;
    RasterType  raster_datatype;

  public:
    H5::DataSet *rasterobj;

    /** \brief Raster Constructor -- allows you to open an existing raster

    This constructor allows you to open an existing raster from within a preexisting HDF5-image object

    \see Read, Write

    \param[in] image
      This is the image object that holds the existing raster.  This is a string set by the user

    \param[in] name
      This is a string that holds the name of the Raster

    \returns
	A valid raster object upon success

    \Par Exceptions
	Exceptions that may be raised:

	RasterOpenError
	RasterDoesNotExist

    \Par Example
	Opening a raster named "test":
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "existing");

  	GeoStar::Image *img = file->open_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test");

	delete ras;
	delete img;
	delete file;
	}

	\endcode

    \Par Details
	The HDF5 Attribute named "object_type" must exist with the value "Geostar::HDF5"
	or it is not a Geostar file and an exception is thrown
    */

    Raster(Image *image, const std::string &name);

    /** \brief Raster Constructor -- allows you to create a new raster

    This constructor allows you to create a new raster from within a preexisting HDF5-image object

    \see Read, Write

    \param[in] image
      This is the image object that holds the raster.  This is a string set by the user

    \param[in] name
      This is a string that holds the name of the Raster

    \param[in] type
	Specifies, out of the existing Raster Types, what data type you want your new raster to be.
	Only accepts INT8U, INT16U, REAL32

    \param[in] nx
	specifies x-size (horizontal size) of new raster

    \param[in] ny
	specifies y-size (vertical size) of new raster

    \returns
	A valid raster object upon success

    \Par Exceptions
	Exceptions that may be raised:

	RasterCreationError
	RasterExistsError

    \Par Example
	creating a raster named "test":
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);

	delete ras;
	delete img;
	delete file;
	}

	\endcode

    \Par Details
	The HDF5 Attribute named "object_type" will be created with the value "Geostar::HDF5"
	so it is considered a Geostar file, else an exception is thrown
    */
    Raster(Image *image, const std::string &name, const RasterType &type,
           const int &nx, const int &ny);

/** \brief write_object_type -- allows you to write the type attribute of the raster

    Can change or implement the attribute of a raster, type needs to be "Geostar::HDF5"

    \see read_object_type

    \param[in] type
      This string is the name of the attribute you want to write to the raster.

    \returns
	Nothing

    \Par Exceptions
	None

    \Par Example
	Adding an attribute to a raster named "Test":
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);
	ras->write_object_type("Test");

	delete ras;
	delete img;
	delete file;

	}

	\endcode

    \Par Details
	Most of the time, this function will not even need to be called - the raster
	recieves the "Geostar::HDF5" attribute during creation.

	Implementation in attributes.cpp for any object type.
       The implementation in this class makes it easier to use in the Raster context.
    */

    inline void write_object_type(const std::string &value) {
      GeoStar::write_object_type((H5::H5Location *)rasterobj,value);
    }

/** \brief read_object_type -- allows you to read the type attribute of the raster

    Can see what attribute the raster has, usually to check if type is "Geostar::HDF5"

    \see write_object_type

    \returns
	A string containing the Object attribute.

    \Par Exceptions
	None

    \Par Example
	Reading the attribute of a raster:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);
	string attribute = ras->read_object_type();
	delete ras;
	delete img;
	delete file;

	}

	\endcode

    \Par Details
	Almost all the time this function should return "Geostar::HDF5".
	Implementation in attributes.cpp for any object type.
       The implementation in this class makes it easier to use in the Raster context.
    */

    std::string read_object_type() const {
      return GeoStar::read_object_type((H5::H5Location *)rasterobj);
    }

    /** \brief Raster destructor allows one to delete a Raster object from memory.

   The Raster destructor is automatically called to clean up memory used by the Raster object.
   There is a single pointer to the HDF5 Raster object that must be deleted.

   \see open, close

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
         GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);

       }//end-main
       \endcode

       Before main ends, the c++ run-time system automatically destroys the raster variable
       using the Raster object destructor.

  */

    inline ~Raster() {
      delete rasterobj;
    }

     template <typename T> static H5::PredType getHdf5Type() {return H5::PredType::NATIVE_UINT8;}

/** \brief write -- allows you to write data to a raster

    write data from a vector to a slice in the raster.  The slice is defined by the first four dimensions
	of the slice vector, namely x0, y0, dx, dy in that order.

    \see read

    \param[in] slice
      This vector is the area you define to write data to.  The dimensions are as follows:
	[0] - x-offset
	[1] - y-offset
	[2] - x-size of slice
	[3] - y-size of slice
	slice should not be bigger than the raster you are writing to

    \param[in] buffer
	This vector contains the data that will be written to the raster.
	The type of the vector does not have to be specified, but be sure it is consistient
	with the rasterType you are writing to

    \returns
	Nothing

    \Par Exceptions
      Exceptions that could be raised:
	RasterWriteError
	SliceSizeError

    \Par Example
	Writing to a raster:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras;
	ras = img->read_file("LC08_L1TP_027033_20170506_20170515_01_T1_B7.TIF", "B07", 1);
	GeoStar:: Raster *ras2 = img->create_raster("test_write", GeoStar::REAL32, 200, 200);

	vector<long int> slice(4);
	slice[0] = 0;
	slice[1] = 0;
	slice[2] = 40;
	slice[3] = 60;

	vector<double> data;
	ras->read(slice, data);
	ras2->write(slice,data);

	delete ras;
	delete ras2;
	delete img;
	delete file;

	}

	\endcode

    \Par Details
	You can call 'writenew(slice, bufr);don't need to say writenew<int>(slice, bufr); for example
	Make sure your slice vector is of size 4 or greater, otherwise an error will be thrown.
    */

      template<typename T>
      void write(const std::vector<long int> slice, std::vector<T> buffer) const {

          RasterWriteErrorException RasterWriteError;
          SliceSizeException SliceSizeError;

          // slice needs to have: x0, y0, dx, dy
          if(slice.size() < 4) throw SliceSizeError;

          // size of the slice of data is the SAME as the size of the slice in the file:
          hsize_t memdims[2];
          memdims[0]=slice[3];
          memdims[1]=slice[2];
          H5::DataSpace memspace(2,memdims);

          H5::DataSpace dataspace = rasterobj->getSpace();

          // set the slice within the file's dataset we want to write to:
          hsize_t count[2];
          hsize_t start[2];
          start[0]=slice[1];
          start[1]=slice[0];
          count[0]=slice[3];
          count[1]=slice[2];
          dataspace.selectHyperslab(H5S_SELECT_SET, count, start);

          //int totalSize = (count[0] - start[0]) * (count[1] - start[1]);
          int totalSize = count[0] * count[1];
          if (buffer.size() < totalSize) throw SliceSizeError;

          H5::PredType h5Type = Raster::getHdf5Type<T>();
          rasterobj->write( (void *)&buffer[0], h5Type, memspace, dataspace );
      }



/** \brief read -- allows you to read data from a raster

    read data from a slice in the raster to a vector.  The slice is defined by the first four dimensions
	of the slice vector, namely x0, y0, dx, dy in that order.

    \see write

    \param[in] slice
      This vector is the area you define to read data from.  The dimensions are as follows:
	[0] - x-offset
	[1] - y-offset
	[2] - x-size of slice
	[3] - y-size of slice
	slice should not be bigger than the raster you are reading from

    \param[in] buffer
	This vector contains the data that will be read from the raster.
	The type of the vector does not have to be specified, but be sure it is consistient
	with the rasterType you are reading from

    \returns
	Nothing

    \Par Exceptions
      Exceptions that could be raised:
	RasterReadError
	SliceSizeError

    \Par Example
	Reading to a raster:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras;
	ras = img->read_file("LC08_L1TP_027033_20170506_20170515_01_T1_B7.TIF", "B07", 1);

	GeoStar:: Raster *ras2 = img->create_raster("test_write", GeoStar::REAL32, 200, 200);

	vector<long int> slice(4);
	slice[0] = 0;
	slice[1] = 0;
	slice[2] = 40;
	slice[3] = 60;

	vector<double> data;
	ras->read(slice, data);
	ras2->write(slice,data);

	delete ras;
	delete ras2;
	delete img;
	delete file;

	}

	\endcode

    \Par Details
	Make sure your slice vector is of size 4, otherwise an error will be thrown.
    */

      template<typename T>
      void read(const std::vector<long int> slice, std::vector<T> &buffer) const {

          RasterReadErrorException RasterReadError;
          SliceSizeException SliceSizeError;

          // slice needs to have: x0, y0, dx, dy
          if(slice.size() < 4) throw SliceSizeError;

          // size of the slice of data is the SAME as the size of the slice in the file:
          hsize_t memdims[2];
          memdims[0]=slice[3];
          memdims[1]=slice[2];
          H5::DataSpace memspace(2,memdims);

          H5::DataSpace dataspace = rasterobj->getSpace();

          // set the slice within the file's dataset we want to write to:
          // we want an offset of (50,50), and a size of 100X100
          hsize_t count[2];
          hsize_t start[2];
          start[0]=slice[1];
          start[1]=slice[0];
          count[0]=slice[3];
          count[1]=slice[2];
          dataspace.selectHyperslab(H5S_SELECT_SET, count, start);

          //int totalSize = (count[0] - start[0]) * (count[1] - start[1]);
          int totalSize = count[0] * count[1];
          if (buffer.size() < totalSize) buffer.resize(totalSize);

          H5::PredType h5Type = Raster::getHdf5Type<T>();
          rasterobj->read( (void *)&buffer[0], h5Type, memspace, dataspace);
      } // end: read


    /** \brief get_nx -- allows you to get the x-size of the raster

    returns the actual size of the raster in the x-direction

    \see get_ny

    \returns
	A long int containing the x-size of the raster

    \Par Exceptions
	None

    \Par Example
	Getting the x-size of a raster:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);
	long int nx = ras->get_nx();

	delete ras;
	delete img;
	delete file;

	}

	\endcode

    */
    long int get_nx() const;

    /** \brief get_ny -- allows you to get the y-size of the raster

    returns the actual size of the raster in the y-direction

    \see get_nx

    \returns
	A long int containing the y-size of the raster

    \Par Exceptions
	None

    \Par Example
	Getting the y-size of a raster:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);
	long int ny = ras->get_ny();

	delete ras;
	delete img;
	delete file;

	}

	\endcode

    */
    long int get_ny() const;

    /** \brief thresh -- sets all values under a threshhold to zero

    This function loops through a raster and reads all values.  If any values are lower than a user-defined
	threshhold, all those pixels are set to zero.

    \see set, copy, scale

    \param[in] value
      User defined threshhold - all values less than this in the image are set to zero

    \returns
	nothing

    \Par Exceptions
	none

    \Par Example
	threshholding a raster to 50:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "existing");

  	GeoStar::Image *img = file->open_image("landsat");

  	GeoStar::Raster *ras;
	ras = img->read_file("LC08_L1TP_027033_20170506_20170515_01_T1_B7.TIF", "B07", 1);

	ras->thresh(200.0);

	delete ras;
	delete img;
	delete file;

	}

	\endcode

    \Par Details
	If it looks like nothing is happening, try threshholding at a higher value for better results or consider
	scaling the image down before the threshholding operation.
    */
    void thresh(const double &value);


/** \brief scale -- scales image values within parameters

    Using the multiplier and the offset, this function corrects images so that their values are more easily manipulated.
	All values in the image are multiplied by the mult and shifted right or left by the offset.  The resulting data is
	written to a new raster.

    \see set, copy, thresh

    \param[out] ras_out
      raster that the data is being written to.  The raster must exist and must be the same size as the
	original raster.

    \param[in] mult
	multiplier that all values in the image are multiplied by.  Should be >0.

    \param[in] offset
	Value added to all points in the image.

    \returns
	nothing

    \Par Exceptions
	none

    \Par Example
	scaling a raster:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "existing");

  	GeoStar::Image *img = file->open_image("landsat");

  	GeoStar::Raster *ras;
	ras = img->read_file("LC08_L1TP_027033_20170506_20170515_01_T1_B7.TIF", "B07", 1);

	long int nx = ras->get_nx();
        long int ny = ras->get_ny();

        GeoStar:: Raster *ras2 = img->create_raster("test", GeoStar::REAL32, nx, ny);

	ras->scale(ras2, 0.06, 40);

	delete ras;
	delete ras2;
	delete img;
	delete file;

	}

	\endcode

    \Par Details
	Scale is a useful preprocessing tool to be able to more easily manipulate image values.
	Make sure the types of your rasters are consistient - output raster of type double is best.

	linear scaling: out = scale* (in -offset)
    */
    void scale(Raster *ras_out, const double &offset, const double &mult) const;


/** \brief copy -- copy a slice of an image

    copies a slice of an image to an existing/different channel.  This slice will be initialized to (0,0) in
    the new raster.

    \see set, scale, thresh

    \param[in] inslice
	This array is the area you define to copy data from and write to in new raster.  The dimensions are as follows:
	[0] - x-offset
	[1] - y-offset
	[2] - x-size of slice
	[3] - y-size of slice
	the slice will start at (0,0) in the new raster, i.e. x and y offsets will be zero.


    \param[out] ras_out
      raster that the data is being written to.  The raster must exist and must be the same size or bigger
	as the slice

    \returns
	nothing

    \Par Exceptions
	none

    \Par Example
	copying a slice of a raster:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "existing");

  	GeoStar::Image *img = file->open_image("landsat");

  	GeoStar::Raster *ras;
	ras = img->read_file("LC08_L1TP_027033_20170506_20170515_01_T1_B7.TIF", "B07", 1);

	long int nx = ras->get_nx();
        long int ny = ras->get_ny();

        GeoStar:: Raster *ras2 = img->create_raster("test", GeoStar::REAL32, nx, ny);

	long int slice[4] = {1000, 1500, 4000, 4000};

	ras->copy(slice, ras2);

	delete ras;
	delete ras2;
	delete img;
	delete file;

	}

	\endcode

    \Par Details
	note the inslice is a bare C-array - passing in a vector slice here will not work
    */
    void copy(const long int *inslice, Raster *ras_out) const;

    /** \brief set -- set a slice of a raster to a value

    Sets a slice of a raster to a user defined value.

    \see copy, scale, thresh

    \param[in] slice
	This array is the area you define to copy data from and write to in new raster.  The dimensions are as follows:
	[0] - x-offset
	[1] - y-offset
	[2] - x-size of slice
	[3] - y-size of slice
	The slice should not be bigger than the raster.

    \param[in] value
      value to set the image data to within the slice.  Should be >0.

    \returns
	nothing

    \Par Exceptions
	none

    \Par Example
	set a slice of a raster to 900:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "existing");

  	GeoStar::Image *img = file->open_image("landsat");

  	GeoStar::Raster *ras;
	ras = img->read_file("LC08_L1TP_027033_20170506_20170515_01_T1_B7.TIF", "B07", 1);

	long int slice[4] = {1000, 1500, 4000, 4000};

	ras->set(slice, 900);

	delete ras;
	delete img;
	delete file;

	}

	\endcode

    \Par Details
	note the slice is a bare C-array - passing in a vector slice here will not work
    */
    void set(const long int *slice, const int &value);

/** \brief drawFilledCircle -- allows the user to draw a filled circle, or point,  in an image

    draws a point at a location in an image with a desired color and radius.
	Essentially just the set function, but implemented differently and meant to work with drawing.

    \see drawLine, drawRectangle

    \param[in] x0
	the center x-coordinate of the circle.  Should be able to accomodate the circle radius from this
	center point, so x0 - radius or x0 + radius should not be larger that the image x-size or less than zero.

    \param[in] y0
	the center y-coordinate of the circle.  Should be able to accomodate the circle radius from this
	center point, so y0 - radius or y0 + radius should not be larger that the image y-size or less than zero.

    \param[in] radius
	Represents the radius of the circle.  This radius will extend from x0, y0, and be used to make a circle.

    \param[in] color
	the value of the point or all pixels within the point.

    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException
	RadiusSizeErrorException

    \par Example
	drawing a point in a raster of radius 5 and color 100:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);

	ras->drawFilledCircle(20, 20, 5, 100);

	delete ras;
	delete img;
	delete file;

	}

	\endcode

	\par Details
	The radius will extend in all directions from the center x0 y0 pair, and a slice will be set using the size of that
	radius.  Pixel distance from circle is calculated using dx^2 + dy^2, where dx and dy are the difference from that
	pixel to the center of the circle.  If these distances are less than the radius, the pixel is lit up.

	radiussizeerror exception will be thrown if the radius is less than zero.
	rastersizeerror exception will be thrown if the circle is larger than the raster.

    */
 void drawFilledCircle(long int x0, long int y0, const double radius, const double color);

/** \brief drawLine -- allows the user to draw a line in an image

    draws a line at a point in an image to another point with a desired color and thickness

    \see drawPoint

    \param[in] slice
	vector with 4 dimensions.  dimensions are as follows:
	(1) x-offset from top left of image.  Point where you wish to start your line
	(2) y-offset from top left of image.  Point where you wish to start your line
	(3) dx, or x-offset from starting point.
	(4) dy, or y-offset from starting point.

    \param[in] radius
	the thickness of the line, i.e. how many pixels across the line should be in the
	opposite direction of the line itself.
	should not be larger than the image x or y size.

    \param[in] color
	the value of the line.

    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException
	SliceSizeError
	RadiusSizeErrorException

    \par Example
	drawing a line in a raster of radius 2 and color 100:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);

	std::vector<long int>slice(4);
	slice[0] = 20;
	slice[1] = 20;
	slice[2] = 20;
	slice[3] = 20;

	ras->drawLine(slice, 2, 100);

	delete ras;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{beforeImg.png,height:150px;width:150px;}
        \image html beforeImg.png "Before Line"
        </td>
        <td>
        \imageSize{afterLine.png,height:150px;width:150px;}
        \image html afterLine.png "After Line"
        </td>
        </tr>
        </table>

	\par Details
	The line connects two points, and will draw them for you.  No need to call drawPoint().
	Furthermore, the points will be of the same color and radius as the line.
	RasterSizeError will be thrown if slice exceeds raster dimensions.
	SliceSizeError will be thrown if slice.size() < 4.
	radiussizeerror exception will be thrown if the radius is less than zero.

	This function first calculates the slope of the desired line, then iterates through the slice and draws the bare
	line in y=mx format.  Then the slice is looped through again and the minimum perpendicular distance to the line is found 
	for each point in the slice using the formula (Ax + By + C) / sqrt(A^2 + B^2).  If distance is less than radius, the pixel
	is lit up.  Note that this distance calculation will not work for a vertical line.

    */

   void drawLine(std::vector<long int> slice, const double radius, const double color);


/** \brief drawRectangle -- allows the user to draw a Rectangle in an image

    draws a Rectangle from the beginning slice xy pair to the final slice xy pair, diagonal to the initial.
	Draws with a desired edge color and edge thickness.

    \see drawPoint, drawLine, drawFilledRectangle

    \param[in] slice
	vector with 4 dimensions.  dimensions are as follows:
	(1) x-offset from top left of image.  Point where you wish to start your rectangle
	(2) y-offset from top left of image.  Point where you wish to start your rectangle
	(3) dx, or x-offset from starting point.  Should end up mapping point coordinates
		diagonal to the original point.
	(4) dy, or y-offset from starting point.  Should end up mapping point coordinates
		diagonal to the original point.

    \param[in] radius
	the thickness of the rectangle edges, i.e. how far inward the rectangle edge should extend. radius should not be greater
	than distance between two edges, so for example slice[3] - slice[1] > radius or slice[2] - slice[0] > radius.

    \param[in] color
	the value of the lines connecting each point (edge color).  For a filled rectangle, see drawFilledRectangle.

    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException
	SliceSizeError
	RadiusSizeErrorException

    \par Example
	drawing a rectangle in a raster of radius 8 and color 100:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);

	vector<long int>slice(4) = {20, 20, 180, 180};

	ras->drawRectangle(slice, 8, 100);

	delete ras;
	delete img;
	delete file;

	}

	\endcode

	\par Details
	The two points connected are diagonal from each other - the function fills in the corners itself.
	This function generates an empty rectangle - to draw a filled rectangle, use drawFilledRectangle.
	slizesizeerror exception will be thrown if slize.size() != 4
	rastersizeerror exception will be thrown if your slice is bigger than the raster or goes off the raster.
	radiussizeerror exception will be thrown if the radius is less than zero.

    */
  void drawRectangle(std::vector<long int>slice, const int radius, const int color);

/** \brief drawFilledRectangle -- allows the user to draw a filled Rectangle in an image

    draws a Rectangle from the beginning slice xy pair to the final slice xy pair, diagonal to the initial.
	Draws with a desired edge color and edge thickness, filling after with desired color.

    \see drawPoint, drawLine, drawRectangle

    \param[in] slice
	vector with 4 dimensions.  dimensions are as follows:
	(1) x-offset from top left of image.  Point where you wish to start your rectangle
	(2) y-offset from top left of image.  Point where you wish to start your rectangle
	(3) dx, or x-offset from starting point.  Should end up mapping point coordinates
		diagonal to the original point.
	(4) dy, or y-offset from starting point.  Should end up mapping point coordinates
		diagonal to the original point.

    \param[in] radius
	the thickness of the rectangle edges, i.e. how many pixels across the line should be in the
	opposite direction of the line itself.
	should not be larger than the image x or y size.

    \param[in] lineColor
	the value of the line connecting each point (edge color).

    \param[in] fillColor
	the value of all points inside the boundaries of the rectangle (within the lines), excluding
	the thickness and color of the lines themselves.

    \returns
	nothing

    \par Exceptions
	SliceSizeException
	RasterSizeErrorException
	RadiusSizeException

    \par Example
	drawing a rectangle in a raster of radius 8, lineColor 100, and fillColor 35:
	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);

	vector<long int>slice(4) = {20, 20, 180, 180};

	ras->drawFilledRectangle(slice, 8, 100, 35);

	delete ras;
	delete img;
	delete file;

	}

	\endcode

	\par Details
	The two points connected are diagonal from each other - the function fills in the corners itself.
	This function generates a filled rectangle - to draw an empty rectangle, use drawRectangle.
	slizesizeerror exception will be thrown if slize.size() != 4
	rastersizeerror exception will be thrown if your slice is bigger than the raster or goes off the raster.
	radiussizeerror exception will be thrown if the radius is less than zero.

    */
  void drawFilledRectangle(std::vector<long int>slice, const int radius, const int lineColor, const int fillColor);

/** \brief bitShift -- shifts the bits in an image right or left

    Writes to a new raster that has been bitshifted i bits to the right or left, in order to decrease or increase image contrast.

    \see read, write

    \param[out] rasterOut
	This is the raster object to which the bitshifted data will be written to.  The original image will remain unchanged.

    \param[in] bits
	The number of bits you wish to shift the image.

    \param[in] direction
	This is a string that denotes which direction you want to bitshift the image.  specify true to bitshift right,
	or false to bitshift left.

    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException
	BitException

    \par Example
	bitshifting the test raster 4 bits to the right:

	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);

	GeoStar::Raster *ras2 = new GeoStar::Raster(img, "testOut", GeoStar::REAL32, 200, 200);

	ras->bitshift(ras2, 4, 1);

	delete ras;
	delete ras2;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{beforeImg.png,height:150px;width:150px;}
        \image html beforeImg.png "Before Line"
        </td>
        <td>
        \imageSize{afterBitshiftRight.png,height:150px;width:150px;}
        \image html afterBitshiftRight.png "After bitshift right"
        </td>
	<td>
        \imageSize{afterBitshiftLeft.png,height:150px;width:150px;}
        \image html afterBitshiftLeft.png "After bitshift left"
        </td>
        </tr>
        </table>

	\par Details
	bitShifting to the right will create a raster with decreased contrast, shifting to the left increases contrast.

	rastersizeerror exception will be thrown if your rasterOut is not the same size as the original raster
	bitvalueerror exception will be thrown if you try to bitshift a negative number of bits.

    */
  void bitShift(Raster *rasterOut, int bits, bool direction);

/** \brief addSaltPepper -- adds salt and pepper noise to a raster

    writes to an output raster adding salt and pepper noise to a raster, corrupting it with a probability denoted by low.
	see parameters for more information.

    \see read, write

    \param[out] rasterOut
	This is the raster object to which the bitshifted data will be written to.  The original image will remain unchanged.

    \param[in] low
	this is the low probability threshhold for the raster, and it must be a value between 0 and 0.5.  Whatever the low
	threshhold is, the sum of the low and high threshholds must equal 1.  Values will be randomly selected between 0 and 1 for
	each pixel in the image, and if the random value is less than or equal to the low threshhold the pixel is set to 0.
	The high probability threshhold will be calculated from this value, and if the random value is greater than or equal to the high 	threshhold the pixel is set to 15000 by default.


    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException
	ProbabilityException

    \par Example
	adding salt and pepper noise to the test raster with a probability of 0.05:

	\code
	#include "Geostar.hpp"
	#include <string>
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);

	GeoStar::Raster *ras2 = new GeoStar::Raster(img, "testOut", GeoStar::REAL32, 200, 200);

	ras->addSaltPepper(ras2, 0.05);

	delete ras;
	delete ras2;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{beforeImg.png,height:150px;width:150px;}
        \image html beforeImg.png "Before adding Salt and Pepper"
        </td>
        <td>
        \imageSize{afterSaltPepper.png,height:150px;width:150px;}
        \image html afterSaltPepper.png "After adding salt and pepper"
        </td>
        </tr>
        </table>

	\par Details

	rastersizeerror exception will be thrown if your rasterOut is not the same size as the original raster.
	probability exception will be thrown if low is <0 or >0.5.
	High end value will be defaulted to 15000, if a higher value is needed some image preprocessing may be required.
	high end value will be between 0.5 and 1, and high + low must equal 1.
    */
  void addSaltPepper(Raster *rasterOut, const double low);

/** \brief autoLocalThresh -- threshholds a raster in chunks with an automatic threshhold

    writes to an output raster threshholding the input raster in sectors defined as squares of a user-set diameter.
	Automatically sets a threshhold by finding the average between local min and max in said sector, and progresses
	through image until all areas possible are threshholded.

    \see read, write

    \param[out] rasterOut
	This is the raster object to which the threshholded data will be written to.  The original image will remain unchanged.

    \param[in] partitions
	The image will be divided into partitions before threshholding, and this defines the number of partitions for each row and col, then 	     each distinct thresh.  So for example a 5 partition function call will have 25 total sectors, and in each of those a threshholding 		operation will occur.
	Partitions should not be less than or equal to zero or greater than 150 - as partitions grow the more computationally expensive this 		function becomes. As such, a hard cap of 150 is imposed.

    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException
	PartitionException

    \par Example
	threshholding into ten partitions across ras:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 200, 200);

	GeoStar::Raster *ras2 = new GeoStar::Raster(img, "testOut", GeoStar::REAL32, 200, 200);

	ras->autoLocalThresh(ras2, 10);

	delete ras;
	delete ras2;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{beforeImg.png,height:150px;width:150px;}
        \image html beforeImg.png "Before local threshholding"
        </td>
        <td>
        \imageSize{afterlocalthresh5.png,height:150px;width:150px;}
        \image html afterlocalthresh5.png "After - 5 partitions"
        </td>
        <td>
        \imageSize{afterlocalthresh25.png,height:150px;width:150px;}
        \image html afterlocalthresh25.png "After - 25 partitions"
        </td>
        <td>
        \imageSize{afterlocalthresh50.png,height:150px;width:150px;}
        \image html afterlocalthresh50.png "After - 50 partitions"
        </td>
        <td>
        \imageSize{afterlocalthresh100.png,height:150px;width:150px;}
        \image html afterlocalthresh100.png "After - 100 partitions"
        </td>
        </tr>
        </table>

	\par Details

	rastersizeerror exception will be thrown if your rasterOut is not the same size as the original raster.
	partitionerror exception will be thrown if the partitions are less than or equal to zero or greater than 150.

	The threshholding formula is very flexible - at the moment it just computes the average between the max and min
	of a particular sector, but in reality the threshhold could be any characteristic of the sector data.
	The threshholding tends to get less strict as the number of partitions goes up as well - more partitions means the
	threshhold of each sector tends to be lower and more accurate for that small area.  A good way to view this function is that
	partitions is just a threshhold parameter, and the higher the partitions the lower the threshhold.
    */
  void autoLocalThresh(Raster *rasterOut, const int partitions);



/** \brief FFT_2D -- Performs a two-dimensional Fast Fourier Transform

    writes to two output rasters, one for the real part of the FFT output and one for the imaginary part.  Takes in real data
	from the raster this is called on, transforms across rows to two buffer rasters, and then transforms across cols to
	the final output rasters.  The output rasters will be one dimensional, starting at (0,0) in the output rasters and
	stretching horizontally across in the x-direction. 

    \see read, write, FFT_2D_Inv

    \param[in] img
	The image object you wish to use to create your buffer rasters in.  Typically should be the same image that your input/output
	rasters are stored in.

    \param[out] rasOutReal
	This is the raster object to which the real part of the FFT data will be written to.  The original image will remain unchanged.

    \param[out] rasOutImg
	This is the raster object to which the img part of the FFT data will be written to.  The original image will remain unchanged.

    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException

    \par Example
	Performing a FFT on ras, outputting to two rasters:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasReal = new GeoStar::Raster(img, "Real", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasImg = new GeoStar::Raster(img, "Imaginary", GeoStar::REAL32, 1024, 1024);

	ras->FFT_2D(img, rasReal, rasImg);

	delete ras;
	delete rasReal;
	delete rasImg;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{OutputRealFFT.png,height:150px;width:150px;}
        \image html OutputRealFFT.png "After FFT with a sinusoidal signal - real part"
        </td>
        <td>
        \imageSize{OutputImaginaryFFT.png,height:150px;width:150px;}
        \image html OutputImaginaryFFT.png "After FFT with a sinusoidal signal - imaginary part"
        </td>
        </tr>
        </table>

	\par Details

	rastersizeerror exception will be thrown if your rasOutReal and rasOutImg are not the same size as your input raster.

	The FFT will perform a complex to complex transfer - the real part of the complex numbers will be data from the raster this 		function is called on, but the imaginary part will be set to all zeros.  Data is read line by line and 1D transforms are performed
	row by row to real and imaginary buffer rasters, then column by column from the buffers to the output rasters.
    */
  void FFT_2D(GeoStar::Image *img, Raster *rasOutReal, Raster *rasOutImg);

/** \brief FFT_2D_Inv -- Performs a two-dimensional Inverse Fast Fourier Transform

    writes to one output raster for the real output.  Takes in real data from the raster this is called on, and imaginary data 
	from the rasInImg parameter, transforms across cols to two buffer rasters, and then transforms across rows to the final output raster.

    \see read, write, FFT_2D

    \param[in] img
	The image object you wish to use to create your buffer rasters in.  Typically should be the same image that your input/output
	rasters are stored in.

    \param[out] rasOut
	This is the raster object to which the real part of the InvFFT data will be written to.  The original image will remain unchanged.

    \param[in] rasInImg
	This is the raster object from which the img part of the FFT data will be read.  Real data comes from the raster this function is
	called on.

    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException

    \par Example
	Performing an InvFFT on ras, outputting to a raster:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *rasOut = new GeoStar::Raster(img, "out", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasReal = new GeoStar::Raster(img, "Real", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasImg = new GeoStar::Raster(img, "Imaginary", GeoStar::REAL32, 1024, 1024);

	rasReal->FFT_2D_Inv(img, rasOut, rasImg);

	delete rasOut;
	delete rasReal;
	delete rasImg;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{OutputRealFFT.png,height:150px;width:150px;}
        \image html OutputRealFFT.png "Before InvFFT with a sinusoidal signal - real part"
        </td>
        <td>
        \imageSize{OutputImaginaryFFT.png,height:150px;width:150px;}
        \image html OutputImaginaryFFT.png "Before InvFFT with a sinusoidal signal - imaginary part"
        </td>
        <td>
        \imageSize{afterFFTInv.png,height:150px;width:150px;}
        \image html afterFFTInv.png "After InvFFT with a sinusoidal signal - real data"
        </td>
        </tr>
        </table>


	\par Details

	rastersizeerror exception will be thrown if your rasOut and rasImg are not the same size as your input raster.

	The FFT will perform a complex to complex transfer - the real part of the complex numbers will be data from the raster this 		function is called on, and imaginary from the rasImg parameter.  Data is read line by line and 1D transforms are performed
	col by col to real and imaginary buffer rasters, then row by row from the buffers to the output raster.  The data is divided
	by a factor of 10 after the final transformation to normalize it - otherwise the values are far larger than they should be.
    */
  void FFT_2D_Inv(GeoStar::Image *img, Raster *rasOut, Raster *rasInImg);

/** \brief LowPassFilter -- Performs a low-pass filter on an image

    writes to an output raster, performing a set of Fourier transforms by row and col on the input raster, setting a square of all zeros
	in the FFT output rasters, and transforming back to real data.  

    \see read, write, FFT_2D, set

    \param[in] img
	The image object you wish to use to create your buffer rasters in.  Typically should be the same image that your input/output
	rasters are stored in.

    \param[in] rasInReal
	This is the raster object to which the real part of the FFT data will be written to.  The original image will remain unchanged.

    \param[in] rasInImg
	This is the raster object to which the img part of the FFT data will be written to.  The original image will remain unchanged.

    \param[out] rasOutReal
	This is the raster object the final inverse FFT transform data will be written to.  The original image will remain unchanged.

    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException

    \par Example
	using a low-pass filter on ras, outputting to a single real raster:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasReal = new GeoStar::Raster(img, "Real", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasImg = new GeoStar::Raster(img, "Imaginary", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasOut = new GeoStar::Raster(img, "Output", GeoStar::REAL32, 1024, 1024);

	ras->lowPassFilter(img, rasReal, rasImg, rasOut);

	delete ras;
	delete rasReal;
	delete rasImg;
	delete rasOut;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{beforeBeach.png,height:150px;width:150px;}
        \image html beforeBeach.png "Before low-pass filter"
        </td>
        <td>
        \imageSize{afterFFTFWDINVbeach.png,height:150px;width:150px;}
        \image html afterFFTFWDINVbeach.png "After low-pass filter"
        </td>
        <td>
        \imageSize{beforeBeachTable100.png,height:150px;width:150px;}
        \image html beforeBeachTable100.png "before - table view"
        </td>
        <td>
        \imageSize{afterlowpassbeachtable.png,height:150px;width:150px;}
        \image html afterlowpassbeachtable.png "after low-pass filter - table view"
        </td>
        </tr>
        </table>

	\par Details

	rastersizeerror exception will be thrown if your rasinReal, rasinImg, and rasOut are not the same size as your input raster.

	The FFT will perform a complex to complex transfer - the real part of the complex numbers will be data from the raster this 		function is called on, but the imaginary part will be set to all zeros.  Data is read line by line and 1D transforms are performed
	row by row to real and imaginary buffer rasters, then column by column from the buffers to the output rasters.
	A square with width hard set to 1/3 the input raster's size and data of all zero's is created in the center of the real and imaginary 		rasters using set(), and then this data is inversely transformed using the FFT once more.

	The smaller the square width, the more accurate the final product is to the original image.  1/3 the input raster's size is about as 
	big as the square can go before slice size exceptions are thrown.
    */
  void lowPassFilter(GeoStar::Image *img, Raster *rasInReal, Raster *rasInImg, Raster *rasOut);

/** \brief downsample -- downsample an image

    writes to an output raster,  convoluting an image with a Gaussian kernel to downsample to 1/2 its original size.

    \see read, write, upsample, gaussianPyramid, laplacianPyramid

    \param[out] rasOut
	The output raster the data is written to.  This raster must be 1/2 the original length and 1/2 the original width.

    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException

    \par Example
	downsampling ras to 1/2 size:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasOut = new GeoStar::Raster(img, "Output", GeoStar::REAL32, 1024 / 2, 1024 / 2);

	ras->downsample(rasOut);

	delete ras;
	delete rasOut;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{beforeBeach.png,height:200px;width:350px;}
        \image html beforeBeach.png "Before downsample - actual size 500x748"
        </td>
        <td>
        \imageSize{afterdownsample.png,height:100px;width:200px;}
        \image html afterdownsample.png "After downsampling - actual size 250x374"
        </td>
        </tr>
        </table>

	\par Details

	rastersizeerror exception will be thrown if your rasOut is not half the size of your input raster.

	This function will define a gaussian kernel used to convolve the input raster.  Then all even-numbered rows and cols are
	removed and data is written to the output raster.  This produces a raster 1/4 the area of the original, with a gaussian blur applied.  		This process can be repeated in order to form a Gaussian pyramid.
    */
  void downsample(Raster *rasOut);

  /** \brief upsample -- upsample an image

    writes to an output raster,  convoluting an image with a Gaussian kernel to upsample to twice its original size.

    \see read, write, downsample, gaussianPyramid, laplacianPyramid

    \param[out] rasOut
	The output raster the data is written to.  This raster must be twice the original length and twice the original width.

    \returns
	nothing

    \par Exceptions
	RasterSizeErrorException

    \par Example
	upsampling ras to twice size:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 512, 512);

	GeoStar::Raster *rasOut = new GeoStar::Raster(img, "Output", GeoStar::REAL32, 1024, 1024);

	ras->upsample(rasOut);

	delete ras;
	delete rasOut;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{dogBefore.jpg,height:200px;width:400px;}
        \image html dogBefore.jpg "Before downsample - actual size 424x756"
        </td>
        <td>
        \imageSize{aftergpyramid1_212_378.png,height:100px;width:200px;}
        \image html aftergpyramid1_212_378.png "After downsampling - actual size 212x378"
        </td>
        <td>
        \imageSize{afterupsample.png,height:200px;width:400px;}
        \image html afterupsample.png "After upsampling - actual size 423x755"
        </td>
        </tr>
        </table>

	\par Details

	rastersizeerror exception will be thrown if your rasOut is not twice the size of your input raster.

	This function will define a gaussian kernel used to convolve the input raster.  All even-numbered rows and cols are
	added back in and filled with zeros, the image is convolved, and data is written to the output raster.  This produces a raster twice
	the size of the original, with a gaussian blur removed. This process can be repeated in order to form a Laplacian pyramid.

	Note that the rasOut should be an empty raster of doubles, i.e. all data points should be zero.

	Moreover the image will not be exactly as it began before upsampling - some scaling must be done on both downsampling and 		upsampling to normalize the values after convolution. Information is lost when an image is downsampled, and though this function
	attempts to restore it by estimating neighboring pixel values, multiple upsamples can cause an image to appear blocky.
    */
  void upsample(Raster *rasOut);

/** \brief gaussianPyramid - produce a gaussian pyramid of an image

    Given an input raster, computes the gaussian pyramid of this raster n times. Returns a vector of raster * of size n.

    \see read, write, upsample, downsample, laplacianPyramid

    \param[in] img
	The image within which you want to create your additional rasters.

    \param[in] n
	The height of the pyramid, i.e. how many iterations and rasters you want to end up with.  Must be greater than zero.

    \returns
	A vector of Raster *, which are the original rasters successively downsampled.

    \par Exceptions
	IntegerParameterException

    \par Example
	Producing a Gaussian Pyramid of height 4:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 1024, 1024);

	vector<GeoStar::Raster *> gaussianOutput = ras->gaussianPyramid(img, 4);

	delete ras;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{dogBefore.jpg,height:200px;width:400px;}
        \image html dogBefore.jpg "Before pyramid - actual size 424x756"
        </td>
        <td>
        \imageSize{aftergpyramid1_212_378.png,height:150px;width:300px;}
        \image html aftergpyramid1_212_378.png "level 1 - actual size 212x378"
        </td>
        <td>
        \imageSize{aftergpyramid2_106_189.png,height:100px;width:200px;}
        \image html aftergpyramid2_106_189.png "level 2 - actual size 106_189"
        </td>
        <td>
        \imageSize{aftergpyramid3_53_94.png,height:75px;width:150px;}
        \image html aftergpyramid3_53_94.png "level 3 - actual size 53x94"
        </td>
        <td>
        \imageSize{aftergpyramid4_26_47.png,height:50px;width:100px;}
        \image html aftergpyramid4_26_47.png "level 4 - actual size 26x47"
        </td>
        </tr>
        </table>

	\par Details

	IntegerParameterException will be thrown if n is less than 1

	This function iterates n times, each time creating a new raster 1/2 the size of the previous, calling downsample, and 
	pushing this new raster back into the output vector.  This function will produce a pyramid of height n, where the bottom layer is the
	original raster.  So if gaussianPyramid is called with n = 1, it will return the original raster and the raster downsampled once.
    */
  std::vector<Raster *> gaussianPyramid(Image *img, int n);

/** \brief laplacianPyramid - produce a laplacian pyramid of an image

    Given an input raster, computes the laplacian pyramid of this raster n times. Returns a vector of raster * of size n.
	Typically used to restore a downsampled raster or undo a gaussian pyramid.

    \see read, write, downsample, upsample, gaussianPyramid

    \param[in] img
	The image within which you want to create your additional rasters.

    \param[in] n
	The height of the pyramid, i.e. how many iterations and rasters you want to end up with.  Must be greater than zero.

    \returns
	A vector of Raster *, which are the original rasters successively upsampled.

    \par Exceptions
	IntegerParameterException

    \par Example
	Restoring from a Gaussian Pyramid of height 4:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 1024, 1024);

	vector<GeoStar::Raster *> gaussianOutput = ras->gaussianPyramid(img, 4);

	vector<GeoStar::Raster *> laplacianOutput;
	laplacianOutput = gaussianOutput[4]->laplacianPyramid(img, 4);

	delete ras;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{dogBefore.jpg,height:200px;width:400px;}
        \image html dogBefore.jpg "Before pyramid - actual size 424x756"
        </td>
	<td>
        \imageSize{aftergpyramid4_26_47.png,height:50px;width:100px;}
        \image html aftergpyramid4_26_47.png "4th level gaussian pyramid: to be restored"
        </td>
        <td>
        \imageSize{afterlpyramid1.png,height:75px;width:150px;}
        \image html afterlpyramid1.png "1st level laplacian pyramid - called on 4th level gaussian"
        </td>
        <td>
        \imageSize{afterlpyramid2.png,height:100px;width:200px;}
        \image html afterlpyramid2.png "level 2"
        </td>
        <td>
        \imageSize{afterlpyramid3.png,height:150px;width:300px;}
        \image html afterlpyramid3.png "level 3"
        </td>
        <td>
        \imageSize{afterlpyramid4.png,height:200px;width:400px;}
        \image html afterlpyramid4.png "level 4"
        </td>
        </tr>
        </table>

	\par Details

	IntegerParameterException will be thrown if n is less than 1

	This function iterates n times, each time creating a new raster twice the size of the previous, calling upsample, and 
	pushing this new raster back into the output vector.  This function will produce a pyramid of height n, where the bottom layer is the
	original raster.  So if laplacianPyramid is called with n = 1, it will return the original raster and the raster upsampled once.

	Be careful of calling high values of n (> 5) in this function.  Upsampling tries to restore lost data by estimating with
	neighboring pixel values, but the data is still lost when changing size.  Upsampling many times will create very blocky images,
	especially when upsampling many times from an image downsampled many times (as in the code example).

    */
  std::vector<Raster *> laplacianPyramid(Image *img, int n);

/** \brief harmonicMean - Applies a harmonic mean filter to an image

    Writing to an output raster, computes the harmonic mean for an N * N square within the raster, applies the filter to 
	each datapoint in that square, and repeats until the whole raster has been covered.

    \see read, write, midpointFilter, rangeFilter

    \param[in] n
	The dimensions of each local harmonic mean / square.  Should be a positive odd integer < 12 and > 2.

    \param[out] rasOut
	The output raster to be written to.  Should be same size as raster this is called on.

    \par Exceptions
	IntegerParameterException
	RasterSizeErrorException

    \par Example
	Applying a harmonic mean filter in 5x5 regions:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasOut = new GeoStar::Raster(img, "output", GeoStar::REAL32, 1024, 1024);

	ras->harmonicMean(rasOut, 5);

	delete ras2;
	delete ras;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{beforeBear.jpg,height:300px;width:350px;}
        \image html beforeBear.jpg "Before harmonic mean"
        </td>
	<td>
        \imageSize{afterHarmonicMean5.png,height:300px;width:350px;}
        \image html afterHarmonicMean5.png "after harmonic mean - 5x5 regions"
        </td>
        <td>
        \imageSize{afterHarmonicMean11.png,height:300px;width:350px;}
        \image html afterHarmonicMean11.png "after harmonic mean - 11x11 regions"
        </td>
        </tr>
        </table>

	\par Details

	IntegerParameterException will be thrown if n is less than 3, greater than 11, or not an odd integer.
	RasterSizeErrorException will be thrown if rasOut is not the same size as the raster this is called on.

	This function iterates until the entire raster has been covered, each time partitioning an N*N square of data and
	calculating the harmonic mean for this area.  The value is then applied to all points in the square and moves on.
    */
  void harmonicMean(Raster * rasOut, int n);


/** \brief gradientMask - Applies the chosen gradient mask to an image

    Writing to an output raster, applies the chosen gradient mask to an image by convolving it with a
	3x3 kernel.

    \see read, write, downsample, upsample

    \param[out] rasOut
	The output raster to be written to.  Should be same size as raster this is called on.

    \param[in] mask
	The preferred gradient mask to convolve the image with.  Options are, by number:
	1. North
	2. East
	3. South
	4. West
	5. Northeast
	6. Northwest
	7. Southeast
	8. Southwest

    \par Exceptions
	IntegerParameterException
	RasterSizeErrorException

    \par Example
	Applying a Northeast gradient mask:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasOut = new GeoStar::Raster(img, "output", GeoStar::REAL32, 1024, 1024);

	ras->gradientMask(rasOut, 5);

	delete ras2;
	delete ras;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{beforeBear.jpg,height:350px;width:400px;}
        \image html beforeBear.jpg "Before blurring"
        </td>
	<td>
        \imageSize{afterBlur.png,height:350px;width:400px;}
        \image html afterBlur.png "after blurring"
        </td>
        </tr>
        </table>

	\par Details

	IntegerParameterException will be thrown if n is less than 1 or greater than 8.
	RasterSizeErrorException will be thrown if rasOut is not the same size as the raster this is called on.

	This function will define a kernel based on the chosen mask and convolves the raster with it, producing
	a filtered output.

	The HDF5 read/write does not allow negative numbers in rasters.  This prevents us from using the majority of these
	gradient filters.  For now, the code will just default to a simple blurring filter.  The other gradient filters
	will be kept however in hopes that they can be used one day.


    */
  void gradientMask(Raster * rasOut, int mask);


/** \brief midpointFilter - applies a midpoint filter for local regions across the raster

    Writing to an output raster, computes the midpoint for an N * N square within the raster, applies the filter to 
	each datapoint in that square, and repeats until the whole raster has been covered.

    \see read, write, rangeFilter, harmonicMean

    \param[in] n
	The dimensions of each local midpoint / square.  Should be a positive odd integer < 12 and > 2.

    \param[out] rasOut
	The output raster to be written to.  Should be same size as raster this is called on.

    \par Exceptions
	IntegerParameterException
	RasterSizeErrorException

    \par Example
	Applying a midpoint filter in 9x9 regions:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasOut = new GeoStar::Raster(img, "output", GeoStar::REAL32, 1024, 1024);

	ras->midpointFilter(rasOut, 9);

	delete ras2;
	delete ras;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{beforeBear.jpg,height:300px;width:350px;}
        \image html beforeBear.jpg "Before harmonic mean"
        </td>
	<td>
        \imageSize{aftermidpoint3.png,height:300px;width:350px;}
        \image html aftermidpoint3.png "after midpoint filter - 3x3 regions"
        </td>
	<td>
        \imageSize{aftermidpoint7.png,height:300px;width:350px;}
        \image html aftermidpoint7.png "after midpoint filter - 7x7 regions"
        </td>
        <td>
        \imageSize{aftermidpoint11.png,height:300px;width:350px;}
        \image html aftermidpoint11.png "after midpoint filter - 11x11 regions"
        </td>
        </tr>
        </table>

	\par Details

	IntegerParameterException will be thrown if n is less than 3, greater than 11, or not an odd integer.
	RasterSizeErrorException will be thrown if rasOut is not the same size as the raster this is called on.

	This function iterates until the entire raster has been covered, each time partitioning an N*N square of data and
	calculating the midpoint for this area.  The value is then applied to all points in the square and moves on.
    */
  void midpointFilter(Raster * rasOut, int n);


/** \brief rangeFilter - applies a range filter for local regions across the raster

    Writing to an output raster, computes the range for an N * N square within the raster, applies the filter to 
	each datapoint in that square, and repeats until the whole raster has been covered.

    \see read, write, midpointFilter, harmonicMean

    \param[in] n
	The dimensions of each local range calculation / square.  Should be a positive odd integer < 12 and > 2.

    \param[out] rasOut
	The output raster to be written to.  Should be same size as raster this is called on.

    \par Exceptions
	IntegerParameterException
	RasterSizeErrorException

    \par Example
	Applying a range filter in 3x3 regions:

	\code
	#include "Geostar.hpp"
	using namespace std;

	int main() {
	GeoStar::File *file = new GeoStar::File("a1.h5", "new");

  	GeoStar::Image *img = file->create_image("landsat");

  	GeoStar::Raster *ras = new GeoStar::Raster(img, "test", GeoStar::REAL32, 1024, 1024);

	GeoStar::Raster *rasOut = new GeoStar::Raster(img, "output", GeoStar::REAL32, 1024, 1024);

	ras->rangeFilter(rasOut, 3);

	delete ras2;
	delete ras;
	delete img;
	delete file;

	}

	\endcode

	<table>
        <tr>
        <td>
        \imageSize{beforeBear.jpg,height:300px;width:350px;}
        \image html beforeBear.jpg "Before harmonic mean"
        </td>
        <td>
        \imageSize{afterrange3.png,height:300px;width:350px;}
        \image html afterrange3.png "after range filter - 3x3 regions"
        </td>
	<td>
        \imageSize{afterrange7.png,height:300px;width:350px;}
        \image html afterrange7.png "after range filter - 7x7 regions"
        </td>
        <td>
        \imageSize{afterrange11.png,height:300px;width:350px;}
        \image html afterrange11.png "after range filter - 11x11 regions"
        </td>
        </tr>
        </table>

	\par Details

	IntegerParameterException will be thrown if n is less than 3, greater than 11, or not an odd integer.
	RasterSizeErrorException will be thrown if rasOut is not the same size as the raster this is called on.

	This function iterates until the entire raster has been covered, each time partitioning an N*N square of data and
	calculating the range for this area.  The value is then applied to all points in the square and moves on.
    */
  void rangeFilter(Raster * rasOut, int n);


/** \brief add -- add two rasters

  Adds two rasters together.

  \see subtract, multiply, divide

  \param[in] r2
    The raster to add to this raster. Must have the same dimensions as this.

  \param[in] ras_out
    This raster will contain the sum of the two rasters.

  \returns
    None

  \par Exceptions
    RasterSizeErrorException -- raised when the two rasters have different sizes

  \par Example
    Stores the sum of ras1 and ras2 in the raster "sum"
  \code
    #include "geostar.hpp"
    #include <vector>
    int main() {
    GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
    GeoStar::Image *img = file->create_image("img");
    GeoStar::Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);
    GeoStar::Raster *ras2 = img->create_raster("ras2", GeoStar::INT8U, 100, 100);
    // fill ras1 and ras2 with data

    GeoStar::Raster * sum = img->create_raster("sum", GeoStar::INT8U, 100, 100);
    ras1->add(ras2, sum);
  }
  \endcode

  \par Details
  In order for add to work, this, r2, and ras_out must have the same dimensions. The function goes pixel by pixel and puts the value of this[i][j]+r2[i][j] in ras_out[i][j].
  */
  void add(const GeoStar::Raster * r2, GeoStar::Raster * ras_out);

  /** \brief subtract -- subtract a raster from this raster

    Subtracts a raster from this one.

    \see add, multiply, divide

    \param[in] r2
      The raster to subtract from this raster. Must have the same dimensions as this.

    \param[in] ras_out
      This raster will contain the difference between the two rasters.

    \returns
      None

    \Par Exceptions
      RasterSizeErrorException -- raised when the two rasters have different sizes

      \par Example
        Stores the difference of ras1 and ras2 in the raster "diff"
      \code
        #include "geostar.hpp"
        #include <vector>
        int main() {
        GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
        GeoStar::Image *img = file->create_image("img");
        GeoStar::Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);
        GeoStar::Raster *ras2 = img->create_raster("ras2", GeoStar::INT8U, 100, 100);
        // fill ras1 and ras2 with data

        GeoStar::Raster * diff = img->create_raster("diff", GeoStar::INT8U, 100, 100);
        ras1->subtract(ras2, diff);
      }
      \endcode

    \par Details
    In order for subtract to work, this, r2, and ras_out must have the same dimensions. The function goes pixel by pixel and puts the value of this[i][j]-r2[i][j] in ras_out[i][j].
    */
    void subtract(const GeoStar::Raster * r2, GeoStar::Raster * ras_out);

    /** \brief multiply -- multiply two rasters together pixel-by-pixel

      Multiplies two rasters together pixel-by-pixel.

      \see add, subtract, divide

      \param[in] r2
        The raster to multiply by. Must have the same dimensions as this.

      \param[in] ras_out
        This raster will contain the product of the two rasters.

      \returns
        None

      \Par Exceptions
        RasterSizeErrorException -- raised when the two rasters have different sizes

        \par Example
          Stores the product of ras1 and ras2 in the raster "prod"
        \code
          #include "geostar.hpp"
          #include <vector>
          int main() {
          GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
          GeoStar::Image *img = file->create_image("img");
          GeoStar::Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);
          GeoStar::Raster *ras2 = img->create_raster("ras2", GeoStar::INT8U, 100, 100);
          // fill ras1 and ras2 with data

          GeoStar::Raster * prod = img->create_raster("prod", GeoStar::INT8U, 100, 100);
          ras1->multiply(ras2, prod);
        }
        \endcode

      \par Details
      In order for multiply to work, this, r2, and ras_out must have the same dimensions. The function goes pixel by pixel and puts the value of this[i][j]*r2[i][j] in ras_out[i][j].
      */
      void multiply(const GeoStar::Raster * r2, GeoStar::Raster * ras_out);

      /** \brief divide -- divide this raster by another pixel-by-pixel

        Divide this rasters by another pixel-by-pixel.

        \see add, subtract, multiply

        \param[in] r2
          The raster to divide this by. Must have the same dimensions as this.

        \param[in] ras_out
          This raster will contain the quotient of the two rasters.

        \returns
          None

        \Par Exceptions
          RasterSizeErrorException -- raised when the two rasters have different sizes

          \par Example
            Stores the quotient of ras1 and ras2 in the raster "quot"
          \code
            #include "geostar.hpp"
            #include <vector>
            int main() {
            GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
            GeoStar::Image *img = file->create_image("img");
            GeoStar::Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);
            GeoStar::Raster *ras2 = img->create_raster("ras2", GeoStar::INT8U, 100, 100);
            // fill ras1 and ras2 with data

            GeoStar::Raster * quot = img->create_raster("quot", GeoStar::INT8U, 100, 100);
            ras1->divide(ras2, quot);
          }
          \endcode

        \par Details
        In order for divide to work, this, r2, and ras_out must have the same dimensions. The function goes pixel by pixel and puts the value of this[i][j]/r2[i][j] in ras_out[i][j].
        */
        void divide(const GeoStar::Raster & r2, GeoStar::Raster * ras_out);

        /** \brief resize -- resize this raster to desired dimensions

        This function resizes the given raster to the specified size

        \param[in] image
          The current GeoStar::Image must be passed to this function

        \param[in] resize_width
          The desired width of your raster

        \param[in] resize_height
          The desired height of your raster

        \returns
          The resized raster

        \Par Exceptions
          RasterSizeErrorException -- raised when the specified width or height are negative

        \Par Example

        \code
        #include <string>
#include <iostream>
#include <cstdint>
#include <vector>

#include "geostar.hpp"

#include "boost/filesystem.hpp"


int main() {

  std::vector<uint8_t> data(500*500);

  for(int j=0;j<500;++j) for(int i=0;i<500;++i) data[j*100+i]=(j*100+i)%255;

  // delete output file if already exists
  boost::filesystem::path p("resize.h5");
  boost::filesystem::remove(p);

  GeoStar::File *file = new GeoStar::File("resize.h5","new");

  GeoStar::Image *img = file->create_image("resize_1");

  img = file->open_image("resize_1");

  GeoStar::Raster *ras = img->create_raster("chan1", GeoStar::INT8U, 512, 1024);


  std::vector<long int> slice(4);
  slice[0]=10; slice [1]=15;
  slice[2]=500; slice[3]=500;
  ras->write(slice, data);

  ras->resize(img, 100, 200);

  delete ras;

  delete img;

  delete file;
        \endcode

        <table>
        <tr>
        <td>
        \imageSize{original_resized.png,height:300px;width:400px;}
        \image html original_resized.png "Before Resize"
        </td>
        <td>
        \imageSize{resized.png,height:60px;width:200px;}
        \image html resized.png "Resized Raster"
        </td>
        </tr>
        </table>

        */
        GeoStar::Raster* resize(GeoStar::Image *img, int resize_width, int resize_height);

        void divide(const GeoStar::Raster * r2, GeoStar::Raster * ras_out);

        /** \brief getParent -- returns a pointer to this raster's parent Image

          This function returns a pointer to a this raster's parent Image.

          \see operator+, operator-, operator*, operator/

          \returns
            GeoStar::Image -- parent of this raster

          \par Exceptions
            None

          \par Example
          This function will make a new raster in the same image as the provided raster.
          \code
          #include "geostar.hpp"
          GeoStar::Raster * newChannel(GeoStar::Raster * ras1, std::string name) {
            GeoStar::Image * img = ras1->getParent();
            GeoStar::Raster * ras2 = new GeoStar::Raster(img,name,ras1->raster_datatype,ras1->get_nx(), ras1->get_ny());
            return ras2;
          }
          \endcode

          \par Details
          This function uses getFileName() to retrieve the raster's file, and gets the full file path in string form fo access the image name using substring operations. With both names, we can access an existing file and then access an existing image in that file.
          */
        GeoStar::Image * getParent();

        /** \brief operator+ -- add two rasters

          Returns a raster pointer containing the sum of two rasters.

          \see add

          \param[in] r2
            The raster to add to this raster. Must have the same dimensions as this.

          \returns
            A GeoStar::Raster pointer which contains the sum of the two rasters.

          \par Exceptions
            RasterSizeErrorException -- raised when the two rasters have different sizes

          \par Example
            Creates a 100x100 raster full of 1s, then makes ras2 point to the sum of that raster with itself.
          \code
          #include "geostar.hpp"
          #include <vector>
          int main() {
          GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
          GeoStar::Image *img = file->create_image("img");
          GeoStar:: Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);

          std::vector<uint8_t> data(100*100);
          for(int j=0;j<100;++j) for(int i=0;i<100;++i) data[j*100+i]=1;
          std::vector<long int> slice(4);
          slice[0]=0; slice [1]=0;
          slice[2]=100; slice[3]=100;
          ras1->write(slice, data);

          GeoStar::Raster * ras2 = *ras1 + *ras1;
          }
          \endcode

          \par Details
            This function uses getParent() to be able to make a new raster in "this" raster's Image. It then uses the add() function on that raster to store the summed rasters.
          */
        GeoStar::Raster * operator+(const GeoStar::Raster & r2);

        /** \brief operator- -- subtract a raster from another

          Returns a raster pointer containing the difference between this raster and r2.

          \see subtract

          \param[in] r2
            The raster to subtract from this raster. Must have the same dimensions as this.

          \returns
            A GeoStar::Raster pointer which contains the difference between the two rasters.

          \Par Exceptions
            RasterSizeErrorException -- raised when the two rasters have different sizes

            \Par Example
              Creates a 100x100 raster full of 1s, then makes ras2 point to a raster of zeroes.
            \code
            #include "geostar.hpp"
            #include <vector>
            int main() {
            GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
            GeoStar::Image *img = file->create_image("img");
            GeoStar:: Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);

            std::vector<uint8_t> data(100*100);
            for(int j=0;j<100;++j) for(int i=0;i<100;++i) data[j*100+i]=1;
            std::vector<long int> slice(4);
            slice[0]=0; slice [1]=0;
            slice[2]=100; slice[3]=100;
            ras1->write(slice, data);

            GeoStar::Raster * ras2 = *ras1 - *ras1;
            }
            \endcode

            \par Details
              This function uses getParent() to be able to make a new raster in "this" raster's Image. It then uses the subtract() function on that raster to store the difference between the rasters.
            */
        GeoStar::Raster * operator-(const GeoStar::Raster & r2);

        /** \brief operator* -- multiply a raster by another

          Returns a raster pointer containing the product of this raster and r2.

          \see multiply

          \param[in] r2
            The raster to multiply this raster with. Must have the same dimensions as this.

          \returns
            A GeoStar::Raster pointer which contains the product of the two rasters.

          \Par Exceptions
            RasterSizeErrorException -- raised when the two rasters have different sizes

            \Par Example
              Creates a 100x100 raster full of 2s, then makes ras2 point to a raster of 4s.
            \code
            #include "geostar.hpp"
            #include <vector>
            int main() {
            GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
            GeoStar::Image *img = file->create_image("img");
            GeoStar:: Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);

            std::vector<uint8_t> data(100*100);
            for(int j=0;j<100;++j) for(int i=0;i<100;++i) data[j*100+i]=2;
            std::vector<long int> slice(4);
            slice[0]=0; slice [1]=0;
            slice[2]=100; slice[3]=100;
            ras1->write(slice, data);

            GeoStar::Raster * ras2 = *ras1 * *ras1;
            }
            \endcode

            \par Details
              This function uses getParent() to be able to make a new raster in "this" raster's Image. It then uses the multiply() function on that raster to store the product of the rasters.
            */
        GeoStar::Raster * operator*(const GeoStar::Raster & r2);

        /** \brief operator/ -- divide a raster by another

          Returns a raster pointer containing the quotient of this raster and r2.

          \see divide

          \param[in] r2
            The raster to divide this raster by. Must have the same dimensions as this.

          \returns
            A GeoStar::Raster pointer which contains the quotient of the two rasters.

          \Par Exceptions
            RasterSizeErrorException -- raised when the two rasters have different sizes

            \Par Example
              Creates a 100x100 raster full of 2s, then makes ras2 point to a raster of 1s, a raster divided by itself.
            \code
            #include "geostar.hpp"
            #include <vector>
            int main() {
            GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
            GeoStar::Image *img = file->create_image("img");
            GeoStar:: Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);

            std::vector<uint8_t> data(100*100);
            for(int j=0;j<100;++j) for(int i=0;i<100;++i) data[j*100+i]=2;
            std::vector<long int> slice(4);
            slice[0]=0; slice [1]=0;
            slice[2]=100; slice[3]=100;
            ras1->write(slice, data);

            GeoStar::Raster * ras2 = *ras1 / *ras1;
            }
            \endcode

            \par Details
              This function uses getParent() to be able to make a new raster in "this" raster's Image. It then uses the divide() function on that raster to store the quotient of the rasters.
            */
        GeoStar::Raster * operator/(const GeoStar::Raster & r2);

        /** \brief operator+ -- add a constant to a raster

          Returns a raster pointer containing the raster with a constant value added to each pixel

          \see operator+

          \param[in] val
            The float value to add to each pixel in the raster.

          \returns
            A GeoStar::Raster pointer which contains the raster with a constant value added to each pixel

          \Par Exceptions
            None

            \Par Example
              This block of code intializes a raster where each pixel has a value of 2, then generates a new raster where each pixel of ras1 has 4 added to it.
            \code
            #include "geostar.hpp"
            #include <vector>
            int main() {
            GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
            GeoStar::Image *img = file->create_image("img");
            GeoStar:: Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);

            std::vector<uint8_t> data(100*100);
            for(int j=0;j<100;++j) for(int i=0;i<100;++i) data[j*100+i]=2;
            std::vector<long int> slice(4);
            slice[0]=0; slice [1]=0;
            slice[2]=100; slice[3]=100;
            ras1->write(slice, data);

            GeoStar::Raster * ras2 = *ras1 + 4;
            }
            \endcode

            \par Details
              This function uses getParent() to be able to make a new raster in "this" raster's Image. It then iterates through every pixel and adds the value to each.
            */
        GeoStar::Raster * operator+(const float & val);

        /** \brief operator- -- subtract a constant from a raster

          Returns a raster pointer containing the raster with a constant value subtracted from each pixel

          \see operator-

          \param[in] val
            The float value to subtract from each pixel in the raster.

          \returns
            A GeoStar::Raster pointer which contains the raster with a constant value subtracted from each pixel.

          \Par Exceptions
            None

            \Par Example
              This block of code intializes a raster where each pixel has a value of 2, then generates a new raster where each pixel of ras1 has 1 subtracted from it.
            \code
            #include "geostar.hpp"
            #include <vector>
            int main() {
            GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
            GeoStar::Image *img = file->create_image("img");
            GeoStar:: Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);

            std::vector<uint8_t> data(100*100);
            for(int j=0;j<100;++j) for(int i=0;i<100;++i) data[j*100+i]=2;
            std::vector<long int> slice(4);
            slice[0]=0; slice [1]=0;
            slice[2]=100; slice[3]=100;
            ras1->write(slice, data);

            GeoStar::Raster * ras2 = *ras1 - 1;
            }
            \endcode

            \par Details
              This function uses getParent() to be able to make a new raster in "this" raster's Image. It then iterates through every pixel and subtracts the value from each.
            */
        GeoStar::Raster * operator-(const float & val);

        /** \brief operator* -- multiply a raster by a constant

          Returns a raster pointer containing the raster with a constant value multiplied by each pixel.

          \see operator*

          \param[in] val
            The float value to multiply each pixel by in the raster.

          \returns
            A GeoStar::Raster pointer which contains the raster with a constant value multiplied by each pixel.

          \Par Exceptions
            None

            \Par Example
              This block of code intializes a raster where each pixel has a value of 2, then generates a new raster where each pixel of ras1 has 2 multiplied by it.
            \code
            #include "geostar.hpp"
            #include <vector>
            int main() {
            GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
            GeoStar::Image *img = file->create_image("img");
            GeoStar:: Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);

            std::vector<uint8_t> data(100*100);
            for(int j=0;j<100;++j) for(int i=0;i<100;++i) data[j*100+i]=2;
            std::vector<long int> slice(4);
            slice[0]=0; slice [1]=0;
            slice[2]=100; slice[3]=100;
            ras1->write(slice, data);

            GeoStar::Raster * ras2 = *ras1*2;
            }
            \endcode

            \par Details
              This function uses getParent() to be able to make a new raster in "this" raster's Image. It then iterates through every pixel and multiplies the value by each.
            */
        GeoStar::Raster * operator*(const float & val);

        /** \brief operator/ -- divide a raster by a constant

          Returns a raster pointer containing the raster with each pixel divided by a constant value

          \see operator/

          \param[in] val
            The float value to divide each pixel by in the raster.

          \returns
            A GeoStar::Raster pointer which contains the raster with each pixel divided by a constant value.

          \Par Exceptions
            GeoStar::DivideByZeroException -- occurs when you divide by zero.

            \Par Example
              This block of code intializes a raster where each pixel has a value of 2, then generates a new raster where each pixel of ras1 is divided by 2.
            \code
            #include "geostar.hpp"
            #include <vector>
            int main() {
            GeoStar::File *file = new GeoStar::File("arithmetic.h5","new");
            GeoStar::Image *img = file->create_image("img");
            GeoStar:: Raster *ras1 = img->create_raster("ras1", GeoStar::INT8U, 100, 100);

            std::vector<uint8_t> data(100*100);
            for(int j=0;j<100;++j) for(int i=0;i<100;++i) data[j*100+i]=2;
            std::vector<long int> slice(4);
            slice[0]=0; slice [1]=0;
            slice[2]=100; slice[3]=100;
            ras1->write(slice, data);

            GeoStar::Raster * ras2 = *ras1/2;
            }
            \endcode

            \par Details
              This function uses getParent() to be able to make a new raster in "this" raster's Image. It then iterates through every pixel and divides the value into each.
            */
        GeoStar::Raster * operator/(const float & val);
  }; // end class: Raster

}// end namespace GeoStar


#endif //RASTER_HPP_
