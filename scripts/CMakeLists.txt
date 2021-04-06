cmake_minimum_required(VERSION 3.13)
##############################################################################################################

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake)
##############################################################################################################

# pre create
execute_process(
  COMMAND ${PROJECT_SOURCE_DIR}/scripts/src/git_details.sh
  OUTPUT_FILE ${PROJECT_BINARY_DIR}/scripts/.git_details-${PROJECT_NAME}
)

configure_file(src/bios-passwd.in ${PROJECT_BINARY_DIR}/scripts/bios-passwd @ONLY)
configure_file(src/testpass.sh ${PROJECT_BINARY_DIR}/scripts/testpass.sh @ONLY)

## libexec
install(FILES ${PROJECT_BINARY_DIR}/scripts/bios-passwd DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/${PROJECT_NAME})
install(FILES ${PROJECT_BINARY_DIR}/scripts/testpass.sh DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/${PROJECT_NAME})
## share
install(FILES ${PROJECT_BINARY_DIR}/scripts/.git_details-${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bios)
