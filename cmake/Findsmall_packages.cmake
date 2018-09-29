set(PACKAGE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/src/extends/small_packages)
FIND_PATH(small_packages_INCLUDE_DIRS small_packages.h ${PACKAGE_PATH}/)
FIND_LIBRARY(small_packages_LIBRARIES libsmall_packages.a PATH ${PACKAGE_PATH}/build)

find_package(jsoncpp CONFIG REQUIRED)
SET(small_packages_INCLUDE_DIRS ${small_packages_INCLUDE_DIRS} ${jsoncpp_INCLUDE_DIRS})
SET(small_packages_LIBRARIES ${small_packages_LIBRARIES} jsoncpp_lib_static)

find_package(log REQUIRED)
SET(small_packages_INCLUDE_DIRS ${small_packages_INCLUDE_DIRS} ${log_INCLUDE_DIRS})
SET(small_packages_LIBRARIES ${small_packages_LIBRARIES} ${log_LIBRARIES})

IF (small_packages_INCLUDE_DIRS AND small_packages_LIBRARIES)
	SET(small_packages_FOUND TRUE)
ENDIF (small_packages_INCLUDE_DIRS AND small_packages_LIBRARIES)
IF (small_packages_FOUND)
    IF (NOT small_packages_FIND_QUIETLY)
        MESSAGE(STATUS "Found small_packages: ${small_packages_LIBRARIES}")
    ENDIF (NOT small_packages_FIND_QUIETLY)
ELSE (small_packages_FOUND)
    IF (small_packages_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find small_packages library")
    ENDIF (small_packages_FIND_REQUIRED)
ENDIF (small_packages_FOUND)

find_package(Boost REQUIRED COMPONENTS system)
if (Boost_FOUND)
    SET(small_packages_INCLUDE_DIRS ${small_packages_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
    SET(small_packages_LIBRARIES ${small_packages_LIBRARIES} ${Boost_LIBRARIES})
endif()


