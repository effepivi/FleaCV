cmake_minimum_required(VERSION 2.8)
project( Flea2CV )
find_package( OpenCV REQUIRED )


ADD_EXECUTABLE (Flea2CV Flea2CV.cxx)
TARGET_INCLUDE_DIRECTORIES (Flea2CV PUBLIC ${OpenCV_INCLUDE_DIRS} /usr/include/flycapture)
TARGET_link_directories(Flea2CV PUBLIC /usr/lib)
TARGET_LINK_LIBRARIES (Flea2CV   ${OpenCV_LIBS} flycapture)
