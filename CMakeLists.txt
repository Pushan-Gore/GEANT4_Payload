# $Id: CMakeLists.txt 86065 2014-11-07 08:51:15Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Payload_dir)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/externals/g4sipm)
include_directories(externals/g4sipm/g4sipm/include)
include_directories(externals/g4sipm/externals/jansson/src)
include_directories(externals/g4sipm/externals/gtest/include)
include_directories(externals/g4sipm/sample/include)
set(LIBS ${LIBS} boost_program_options g4sipm g4sipm_sample boost_date_time jansson)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(Payload Payload.cc ${sources} ${headers})
target_link_libraries(Payload g4sipm g4sipm_sample boost_program_options boost_date_time jansson ${Geant4_LIBRARIES})
#Add GODDESS code here

#------ Boost ------#
# Find Boost package
#find_package(Boost REQUIRED regex)

# Add include directories to the list of directories searched by the compiler for include files:
#include_directories(${Boost_INCLUDE_DIRS})

# Set Boost library list
#set(LIBS_BOOST boost_regex)

#------ zlib ------#
# Find zlib package
#find_package(ZLIB REQUIRED)

# Add include directories to the list of directories searched by the compiler for include files:

#include_directories(${ZLIB_INCLUDE_DIRS})

# Set zlib library list
#set(LIBS_ZLIB ${ZLIB_LIBRARIES})

#------ GODDeSS ------#
# Locate GODDeSS
#set(GODDESS_LIB_DIR $ENV{GODDESS}/build/G4BasicObjects/*/)
#find_library(G4PropertyFileReadInTools NAMES G4PropertyFileReadInTools HINTS "${GODDESS_LIB_DIR}")
#find_library(G4Scintillator NAMES G4Scintillator HINTS "${GODDESS_LIB_DIR}")
#find_library(G4Fibre NAMES G4Fibre HINTS "${GODDESS_LIB_DIR}")
#find_library(G4GeneralParticleSource NAMES G4GeneralParticleSource HINTS "${GODDESS_LIB_DIR}")
#find_library(G4PhotonDetector NAMES G4PhotonDetector HINTS "${GODDESS_LIB_DIR}")
#find_library(DataProcessing NAMES DataProcessing HINTS "${GODDESS_LIB_DIR}")
#set(LIBS ${G4PropertyFileReadInTools} ${G4Scintillator} ${G4Fibre} ${G4GeneralParticleSource} ${G4PhotonDetector} ${DataProcessing})

# Add include directories to the list of directories searched by the compiler for include files:
#set(GODDESS_INCLUDE_DIR $ENV{GODDESS}/source/G4BasicObjects/)
#include_directories(${GODDESS_INCLUDE_DIR}/G4PropertyFileReadInTools/include)
#include_directories(${GODDESS_INCLUDE_DIR}/G4Scintillator/include)
#include_directories(${GODDESS_INCLUDE_DIR}/G4Fibre/include)
#include_directories(${GODDESS_INCLUDE_DIR}/G4GeneralParticleSource/include)
#include_directories(${GODDESS_INCLUDE_DIR}/G4PhotonDetector/include)
#include_directories(${GODDESS_INCLUDE_DIR}/DataProcessing/include)

#target_link_libraries(Payload ${LIBS_BOOST} ${LIBS_ZLIB} ${LIBS_GODDESS})
#add_dependencies(Payload ${LIBS_BOOST} ${LIBS_ZLIB} ${LIBS_GODDESS})

#Goddess code ends here

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(PAYLOAD_SCRIPTS
  Payload.in
  Payload.out
  init_vis.mac
  run1.mac
  run2.mac
  vis.mac
  pdf.mac
  pdf_400_to_450.mac
  )

foreach(_script ${PAYLOAD_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(Payload_dir DEPENDS Payload)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS Payload DESTINATION bin)


