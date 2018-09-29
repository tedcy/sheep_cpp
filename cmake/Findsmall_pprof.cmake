set(PACKAGE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/src/extends/small_pprof)
FIND_PATH(small_pprof_INCLUDE_DIRS small_pprof.h ${PACKAGE_PATH}/)
FIND_LIBRARY(GPERFHTTPD_LIBRARIES libgperftools-httpd.a PATH ${PACKAGE_PATH}/build)
FIND_LIBRARY(PROFILE_LIBRARIES libprofiler.a PATH ${PACKAGE_PATH}/build)
FIND_LIBRARY(TCMALLOC_LIBRARIES libtcmalloc.a PATH ${PACKAGE_PATH}/build)

SET (small_pprof_LIBRARIES ${GPERFHTTPD_LIBRARIES} ${PROFILE_LIBRARIES} ${TCMALLOC_LIBRARIES})
IF (small_pprof_INCLUDE_DIRS AND GPERFHTTPD_LIBRARIES AND PROFILE_LIBRARIES AND TCMALLOC_LIBRARIES)
	SET(small_pprof_FOUND TRUE)
ENDIF (small_pprof_INCLUDE_DIRS AND small_pprof_LIBRARIES)
IF (small_pprof_FOUND)
    IF (NOT small_pprof_FIND_QUIETLY)
        MESSAGE(STATUS "Found small_pprof: ${small_pprof_LIBRARIES}")
    ENDIF (NOT small_pprof_FIND_QUIETLY)
ELSE (small_pprof_FOUND)
    IF (small_pprof_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find small_pprof library")
    ENDIF (small_pprof_FIND_REQUIRED)
ENDIF (small_pprof_FOUND)
