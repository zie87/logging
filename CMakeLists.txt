CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)
PROJECT(logging)

# The version number
SET(LOGGING_MAJOR_VERSION 0)
SET(LOGGING_MINOR_VERSION 1)
SET(LOGGING_PATCH_VERSION 0)
SET(LOGGING_VERSION ${LOGGING_MAJOR_VERSION}.${LOGGING_MINOR_VERSION}.${LOGGING_PATCH_VERSION})
# SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


# ENABLE_LANGUAGE(CXX)
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -DBOOST_LOG_DYN_LINK")
ENABLE_LANGUAGE(CXX)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Weffc++ -DBOOST_LOG_DYN_LINK")
ADD_COMPILE_OPTIONS(-std=c++1y) #CMake 2.8.12 or newer

IF ( NOT CMAKE_BUILD_TYPE )
  SET( CMAKE_BUILD_TYPE Debug CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE )
ENDIF()

OPTION(CHECK_VERBOSE "print out output from test programs" OFF)
IF(CHECK_VERBOSE)
    SET(CMAKE_CTEST_COMMAND ctest -V)
ELSE()
    SET(CMAKE_CTEST_COMMAND ctest)
ENDIF()

OPTION(CHECK_WHILE_BUILD "run checks when building all" ON)
IF(CHECK_WHILE_BUILD)
    ADD_CUSTOM_TARGET(unittest_modules ALL COMMAND ${CMAKE_CTEST_COMMAND})
ELSE()
    ADD_CUSTOM_TARGET(unittest_modules COMMAND ${CMAKE_CTEST_COMMAND})
ENDIF()

################################################################################
# FIND DOXYGEN
################################################################################

# FIND_PACKAGE(Doxygen)
# IF (DOXYGEN_FOUND)
#   ADD_SUBDIRECTORY(doc)
# ENDIF (DOXYGEN_FOUND)

################################################################################
# FIND BOOST
################################################################################
FIND_PACKAGE (Threads)
# FIND_PACKAGE( Boost 1.54 COMPONENTS system filesystem program_options thread serialization REQUIRED )


################################################################################
# BUILDING THE LIB
################################################################################

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/inc)

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

ADD_SUBDIRECTORY (src)

################################################################################
# BUILDING TESTS
################################################################################

ENABLE_TESTING()
ADD_SUBDIRECTORY (tests)

