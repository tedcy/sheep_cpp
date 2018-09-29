set(PACKAGE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/src/extends/small_watcher)
FIND_PATH(small_watcher_INCLUDE_DIRS small_watcher.h ${PACKAGE_PATH}/)
FIND_LIBRARY(small_watcher_LIBRARIES libsmall_watcher.a PATH ${PACKAGE_PATH}/build)
IF (small_watcher_INCLUDE_DIRS AND small_watcher_LIBRARIES)
	SET(small_watcher_FOUND TRUE)
ENDIF (small_watcher_INCLUDE_DIRS AND small_watcher_LIBRARIES)
IF (small_watcher_FOUND)
    IF (NOT small_watcher_FIND_QUIETLY)
        MESSAGE(STATUS "Found small_watcher: ${small_watcher_LIBRARIES}")
    ENDIF (NOT small_watcher_FIND_QUIETLY)
ELSE (small_watcher_FOUND)
    IF (small_watcher_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find small_watcher library")
    ENDIF (small_watcher_FIND_REQUIRED)
ENDIF (small_watcher_FOUND)

find_package(small_http_client REQUIRED)
SET(small_watcher_INCLUDE_DIRS ${small_watcher_INCLUDE_DIRS} ${small_http_client_INCLUDE_DIRS})
SET(small_watcher_LIBRARIES ${small_watcher_LIBRARIES} ${small_http_client_LIBRARIES})
