IF(VTK_TCL_INCLUDE_DIR)
  INCLUDE_DIRECTORIES("${VTK_TCL_INCLUDE_DIR}" "${VTK_TK_INCLUDE_DIR}")
ENDIF(VTK_TCL_INCLUDE_DIR)

VTK_WRAP_TCL3(${PROJECT_NAME}${KIT}TCL KitTCL_SRCS
  "${Kit_SRCS}"
  "${Kit_TCL_EXTRA_CMDS}")
ADD_LIBRARY(${PROJECT_NAME}${KIT}TCL ${KitTCL_SRCS} ${Kit_TCL_EXTRA_SRCS})
SET(KIT_LIBRARY_TARGETS ${KIT_LIBRARY_TARGETS} ${PROJECT_NAME}${KIT}TCL)
TARGET_LINK_LIBRARIES (${PROJECT_NAME}${KIT}TCL ${PROJECT_NAME}${KIT} ${KIT_TCL_LIBS})
#IF(NOT VTK_INSTALL_NO_LIBRARIES)
#  INSTALL(TARGETS ${PROJECT_NAME}${KIT}TCL
#    RUNTIME DESTINATION ${VTK_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
#    LIBRARY DESTINATION ${VTK_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
#    ARCHIVE DESTINATION ${VTK_INSTALL_LIB_DIR_CM24} COMPONENT Development)
#ENDIF(NOT VTK_INSTALL_NO_LIBRARIES)
IF(KIT_TCL_DEPS)
  ADD_DEPENDENCIES(${PROJECT_NAME}${KIT}TCL ${KIT_TCL_DEPS})
ENDIF(KIT_TCL_DEPS)

