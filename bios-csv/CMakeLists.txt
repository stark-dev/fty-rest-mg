cmake_minimum_required(VERSION 3.13)
##############################################################################################################

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake)
##############################################################################################################

set(EXE_NAME "bios-csv")

# Build exe
etn_target(exe ${EXE_NAME}
  SOURCES
    bios-csv.cc
    ../src/*.cc
  INCLUDE_DIRS
    ../include
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

# Install usr/libexec/fty-rest/bios-csv
install(FILES ${PROJECT_BINARY_DIR}/bios-csv/${EXE_NAME} DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR}/${PROJECT_NAME_HYPHEN})
