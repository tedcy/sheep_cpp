set(PACKAGE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/src/net)
FIND_PATH(net_INCLUDE_DIRS net.h ${PACKAGE_PATH}/)
FIND_LIBRARY(net_LIBRARIES libnet.a PATH ${PACKAGE_PATH}/build)

IF (net_INCLUDE_DIRS AND net_LIBRARIES)
    SET(net_INCLUDE_DIRS ${net_INCLUDE_DIRS} ${SHEEP_PROJECT_SOURCE_DIR}/src/net/common)
    SET(net_INCLUDE_DIRS ${net_INCLUDE_DIRS} ${SHEEP_PROJECT_SOURCE_DIR}/src/net/common/tcp)
    SET(net_INCLUDE_DIRS ${net_INCLUDE_DIRS} ${SHEEP_PROJECT_SOURCE_DIR}/src/net/server)
    SET(net_INCLUDE_DIRS ${net_INCLUDE_DIRS} ${SHEEP_PROJECT_SOURCE_DIR}/src/net/client)
	SET(net_FOUND TRUE)
ENDIF (net_INCLUDE_DIRS AND net_LIBRARIES)
IF (net_FOUND)
    IF (NOT net_FIND_QUIETLY)
        MESSAGE(STATUS "Found net ${net_LIBRARIES}")
    ENDIF (NOT net_FIND_QUIETLY)
ELSE (net_FOUND)
    IF (net_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find net library")
    ENDIF (net_FIND_REQUIRED)
ENDIF (net_FOUND)

find_package(small_packages REQUIRED)
SET(net_INCLUDE_DIRS ${net_INCLUDE_DIRS} ${small_packages_INCLUDE_DIRS})
SET(net_LIBRARIES ${net_LIBRARIES} ${small_packages_LIBRARIES})

find_package(log REQUIRED)
SET(net_INCLUDE_DIRS ${net_INCLUDE_DIRS} ${log_INCLUDE_DIRS})
SET(net_LIBRARIES ${net_LIBRARIES} ${log_LIBRARIES})
