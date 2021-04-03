cmake_minimum_required(VERSION 3.13)
##############################################################################################################

project(fty_rest
  VERSION "1.0.0"
  DESCRIPTION "Common core REST API for 42ity project"
  LANGUAGES CXX)

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake)
##############################################################################################################

# Define the servlet
etn_target(shared ${PROJECT_NAME}
  SOURCES
    src/*.cc
  INCLUDE_DIRS
    include
  USES
    fty-cmake-rest
    cidr
    magic
    sasl2
    cxxtools
    tntnet
    tntdb
    czmq
    mlm
    fty_proto
    fty_shm
    fty_asset_activator
    fty_common
    fty_common_logging
    fty_common_rest
    fty_common_db
    fty_common_mlm
    fty_common_messagebus
    fty_common_dto
)

# Get the ecpp
file(GLOB ETN_IPM2_REST_ECPP_FILES
  ecpp/*.ecpp
)

# Transform ecpp files
fty_ecppbuilder(
  TARGET ${PROJECT_NAME}
  WORKDIR src
  ECPP
    ${ETN_IPM2_REST_ECPP_FILES}
)

# Tests
#add_subdirectory(tests)

# Install tntnet mapping files
#install(FILES resources/55_etn-mass-management-rest.xml DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/tntnet/bios.d/)
