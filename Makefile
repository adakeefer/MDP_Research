
BOOST_INCLUDES=-Iboost_1_63_0
BOOST_LIBRARIES=boost_1_63_0/stage/lib/libboost_filesystem.a \
		boost_1_63_0/stage/lib/libboost_system.a 

HDF5_INCLUDES=-Ihdf5-1.10.0-patch1/include
HDF5_LIBRARIES=hdf5-1.10.0-patch1/lib/libhdf5.a -lhdf5_cpp -lz -ldl -lm -Lgdal-2.1.3/lib/

GDAL_INCLUDES=-Igdal-2.1.3/include -Igdal-2.1.3/frmts/gtiff/libtiff
GDAL_LIBRARIES=gdal-2.1.3/lib/libgdal.a -lfreexl -lhdf5_cpp -L/home/adamk/Desktop/basecode/hdf5-1.10.0-patch1/lib -lhdf5 -lhdf5_cpp -logdi -lgif -ljpeg -lpng -lcfitsio -L/usr/lib -lpq -lz -lpthread -lm -lrt -ldl -lcurl -lxml2 -L/usr/lib/i386-linux-gnu -lkmlbase -lkmlengine -ljson-c -ljsoncpp -lkmldom -lcrypto -lcryptopp -lcrypto++ -ltiff -lgeotiff -ltiffxx -ljpeg -lsqlite3 -lgeos_c -lodbc -lodbcinst -lexpat -lxerces-c -lpthread -ljasper -lnetcdf -lpcre -lqhull -lopenjpeg -lopenjpeg_JPWL -lopenjp2

FFTW_INCLUDES=-Ifftw-3.3.7/include
FFTW_LIBRARIES=fftw-3.3.7/lib/libfftw3.a

CARIO_INCLUDES=-I/usr/local/include/cairo -I/usr/local/include/sigc++-3.0/sigc++ -I/usr/local/include/cairomm-1.16/cairomm -I/usr/local/include/pixman-1
CAIRO_LIBRARIES=-L/usr/local/lib/libcairo.a -lcairo -lsigc-3.0 -lpixman-1 -lcairomm-1.16

INCL=${BOOST_INCLUDES} ${HDF5_INCLUDES} ${GDAL_INCLUDES} ${FFTW_INCLUDES} ${CAIRO_INCLUDES}
LIBS=${BOOST_LIBRARIES} ${HDF5_LIBRARIES} ${GDAL_LIBRARIES} ${FFTW_LIBRARIES} ${CAIRO_LIBRARIES}

WARN=-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused

STD=-std=c++0x

File.o: File.cpp File.hpp Exceptions.hpp attributes.hpp
	g++ -c -o File.o File.cpp ${INCL}

Image.o: Image.cpp Image.hpp File.hpp Exceptions.hpp attributes.hpp
	g++ -c -o Image.o Image.cpp ${INCL}

Raster.o: Raster.cpp Raster.hpp RasterType.hpp Image.hpp Exceptions.hpp attributes.hpp
	g++ -c -o Raster.o Raster.cpp ${INCL}

attributes.o: attributes.cpp attributes.hpp
	g++ -c -o attributes.o attributes.cpp ${INCL}

test1: test1.cpp File.o File.hpp Image.o Image.hpp Raster.o Raster.hpp Exceptions.hpp attributes.o attributes.hpp
	g++ ${STD} -o test1 test1.cpp File.o Image.o Raster.o attributes.o ${INCL} ${LIBS}

test2: test2.cpp File.o File.hpp Image.o Image.hpp Raster.o Raster.hpp Exceptions.hpp attributes.o attributes.hpp
	g++ ${STD} -o test2 test2.cpp File.o Image.o Raster.o attributes.o ${INCL} ${LIBS}

test3: test3.cpp File.o File.hpp Image.o Image.hpp Raster.o Raster.hpp Exceptions.hpp attributes.o attributes.hpp
	g++ ${STD} -o test3 test3.cpp File.o Image.o Raster.o attributes.o ${INCL} ${LIBS}

test4: test4.cpp File.o File.hpp Image.o Image.hpp Raster.o Raster.hpp Exceptions.hpp attributes.o attributes.hpp
	g++ ${STD} -o test4 test4.cpp File.o Image.o Raster.o attributes.o ${INCL} ${LIBS}

linkerTests: linkerTests.cpp File.o File.hpp Image.o Image.hpp attributes.o attributes.hpp
	g++ ${STD} -o linkerTests linkerTests.cpp File.o Image.o attributes.o ${INCL} ${LIBS}

cairoTests: cairoTests.cpp File.o File.hpp Image.o Image.hpp Raster.o Raster.hpp attributes.o attributes.hpp
	g++ ${STD} -o cairoTests cairoTests.cpp File.o Image.o Raster.o attributes.o ${INCL} ${LIBS}
## had to do:
## ./configure --prefix=`pwd` --with-df5=/home/lep/MDP2/codes/demo9/hdf5-1.10.0-patch1 --without-hdf4
## export LD_LIBRARY_PATH=/home/lep/MDP2/codes/demo9/hdf5-1.10.0-patch1/lib
GDAL_LIBS=-lfreexl -L/usr/local/lib -lgeos_c  -lsqlite3 -lodbc -lodbcinst -lexpat -lxerces-c -lpthread -ljasper -lnetcdf -L/home/adamk/Desktop/basecode/hdf5-1.10.0-patch1/lib -lhdf5  -logdi -lgif -ljpeg -lpng -lcfitsio -L/usr/lib -lpq -lz -lpthread -lm -lrt -ldl -lcurl -lxml2
testgd: testgd.cpp
	g++ -o testgd testgd.cpp -Igdal-2.1.3/include gdal-2.1.3/lib/libgdal.a ${GDAL_LIBS}
