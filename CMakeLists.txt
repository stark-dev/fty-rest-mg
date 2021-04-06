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

# bios-csv
add_subdirectory(bios-csv)

# scripts
add_subdirectory(scripts)

# Install tntnet mapping files in etc/tntnet/bios.d/
# Files, in order
set(MAPPING_FILES
  00_start.xml
  10_common_statics.xml
  20_common_basics.xml
  50_main_api_start.xml
  51_asset_api_fty-rest.xml
  60_main_api_until_eula.xml
  89_main_api_end.xml
  99_end.xml
)
foreach(file ${MAPPING_FILES})
  set(f ${PROJECT_SOURCE_DIR}/resources/${file})
  install(FILES ${f} DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/tntnet/bios.d/)
endforeach()

# Install tntnet mapping files examples in usr/share/bios/examples
# tntnet.xml.example: concat of all mapping files
set(TNTNET_XML_EXAMPLE ${PROJECT_BINARY_DIR}/tntnet.xml.example)
foreach(file ${MAPPING_FILES})
  set(f ${PROJECT_SOURCE_DIR}/resources/${file})
  install(FILES ${f} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bios/examples/ RENAME ${file}.example)
  # cat f in TNTNET_XML_EXAMPLE
  file(READ ${f} aux)
  file(APPEND ${TNTNET_XML_EXAMPLE} "${aux}")
endforeach()
install(FILES ${TNTNET_XML_EXAMPLE} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bios/examples/)
