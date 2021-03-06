set(PACKAGE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/src/small_client)
FIND_PATH(small_client_INCLUDE_DIRS small_client.h ${PACKAGE_PATH}/)
#FIND_LIBRARY(small_client_LIBRARIES libsmall_client.a PATH ${PACKAGE_PATH}/build)

IF (small_client_INCLUDE_DIRS)
#IF (small_client_INCLUDE_DIRS AND small_client_LIBRARIES)
	SET(small_client_FOUND TRUE)
ENDIF (small_client_INCLUDE_DIRS)
#ENDIF (small_client_INCLUDE_DIRS AND small_client_LIBRARIES)
IF (small_client_FOUND)
    IF (NOT small_client_FIND_QUIETLY)
        MESSAGE(STATUS "Found small_client ${small_client_INCLUDE_DIRS}")
    ENDIF (NOT small_client_FIND_QUIETLY)
ELSE (small_client_FOUND)
    IF (small_client_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find small_client library")
    ENDIF (small_client_FIND_REQUIRED)
ENDIF (small_client_FOUND)

find_package(small_http_parser REQUIRED)
SET(small_client_INCLUDE_DIRS ${small_client_INCLUDE_DIRS} ${small_http_parser_INCLUDE_DIRS})
SET(small_client_LIBRARIES ${small_client_LIBRARIES} ${small_http_parser_LIBRARIES})

find_package(net REQUIRED)
SET(small_client_INCLUDE_DIRS ${small_client_INCLUDE_DIRS} ${net_INCLUDE_DIRS})
SET(small_client_LIBRARIES ${small_client_LIBRARIES} ${net_LIBRARIES})

find_package(small_hiredis REQUIRED)
SET(small_client_INCLUDE_DIRS ${small_client_INCLUDE_DIRS} ${small_hiredis_INCLUDE_DIRS})
SET(small_client_LIBRARIES ${small_client_LIBRARIES} ${small_hiredis_LIBRARIES})

find_package(log REQUIRED)
SET(small_client_INCLUDE_DIRS ${small_client_INCLUDE_DIRS} ${log_INCLUDE_DIRS})
SET(small_client_LIBRARIES ${small_client_LIBRARIES} ${log_LIBRARIES})
