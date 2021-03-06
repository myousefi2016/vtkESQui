# Setup vtkInstantiator registration for this library's classes.
#VTK_MAKE_INSTANTIATOR3(${PROJECT_NAME}${KIT}Instantiator KitInstantiator_SRCS
#                       "${Kit_SRCS}"
#                       ${UEXPORT_PREFIX}${UKIT}_EXPORT
#                       ${PROJECT_BINARY_DIR} "")

#ADD_LIBRARY(${PROJECT_NAME}${KIT} ${Kit_SRCS} ${Kit_EXTRA_SRCS} ${KitInstantiator_SRCS})
ADD_LIBRARY(${PROJECT_NAME}${KIT} ${Kit_SRCS} ${Kit_EXTRA_SRCS})
SET(KIT_LIBRARY_TARGETS ${KIT_LIBRARY_TARGETS} ${PROJECT_NAME}${KIT})

# Allow the user to customize their build with some local options
#
SET(LOCALUSERMACRODEFINED 0)
INCLUDE (${PROJECT_BINARY_DIR}/${KIT}/LocalUserOptions.cmake OPTIONAL)
INCLUDE (${PROJECT_SOURCE_DIR}/${KIT}/LocalUserOptions.cmake OPTIONAL)

# if we are wrapping into Tcl then add the library and extra
# source files
# 
IF (VTKESQUI_WRAP_TCL)
  INCLUDE(${VTKESQUI_CMAKE_DIR}/KitCommonTclWrapBlock.cmake)
ENDIF (VTKESQUI_WRAP_TCL)

# if we are wrapping into Python then add the library and extra
# source files
# 
IF (VTKESQUI_WRAP_PYTHON)
  INCLUDE(${VTKESQUI_CMAKE_DIR}/KitCommonPythonWrapBlock.cmake)
ENDIF (VTKESQUI_WRAP_PYTHON)

# if we are wrapping into Java then add the library and extra
# source files
#
#IF (VTK_WRAP_JAVA)
#  INCLUDE(KitCommonJavaWrapBlock)
#ENDIF (VTK_WRAP_JAVA)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}${KIT} ${KIT_LIBS})

#IF(NOT VTK_INSTALL_NO_LIBRARIES)
#  INSTALL(TARGETS ${PROJECT_NAME}${KIT} 
#    RUNTIME DESTINATION ${VTK_INSTALL_BIN_DIR_CM24} COMPONENT RuntimeLibraries
#    LIBRARY DESTINATION ${VTK_INSTALL_LIB_DIR_CM24} COMPONENT RuntimeLibraries
#    ARCHIVE DESTINATION ${VTK_INSTALL_LIB_DIR_CM24} COMPONENT Development)
#ENDIF(NOT VTK_INSTALL_NO_LIBRARIES)
#IF(NOT VTK_INSTALL_NO_DEVELOPMENT)
#  SET(__files.h)
#  FOREACH(__file ${Kit_SRCS})
#    GET_FILENAME_COMPONENT(__fname ${__file} ABSOLUTE)
#    GET_FILENAME_COMPONENT(__fname.path ${__fname} PATH)
#    GET_FILENAME_COMPONENT(__fname.name ${__fname} NAME_WE)
#    IF ( EXISTS "${__fname.path}/${__fname.name}.h" )
#      SET(__files.h ${__files.h} "${__fname.path}/${__fname.name}.h")
#    ENDIF ( EXISTS "${__fname.path}/${__fname.name}.h" )
#  ENDFOREACH(__file)
#  INSTALL(FILES ${__files.h}
#    DESTINATION ${VTK_INSTALL_INCLUDE_DIR_CM24}
#    COMPONENT Development)
#ENDIF(NOT VTK_INSTALL_NO_DEVELOPMENT)

#VTK_EXPORT_KIT("${KIT}" "${UKIT}" "${Kit_SRCS}")

# If the user defined a custom macro, execute it now and pass in all the srcs
# 
IF(LOCALUSERMACRODEFINED)
  LocalUserOptionsMacro( "${Kit_SRCS}"       "${Kit_EXTRA_SRCS}"
                         "${KitTCL_SRCS}"    "${Kit_TCL_EXTRA_SRCS}"
                         "${KitJava_SRCS}"   "${Kit_JAVA_EXTRA_SRCS}"
                         "${KitPython_SRCS}" "${Kit_PYTHON_EXTRA_SRCS}")
ENDIF(LOCALUSERMACRODEFINED)


# Apply user-defined properties to the library targets.
IF(VTK_LIBRARY_PROPERTIES)
  SET_TARGET_PROPERTIES(${KIT_LIBRARY_TARGETS} PROPERTIES
    ${VTK_LIBRARY_PROPERTIES}
    )
ENDIF(VTK_LIBRARY_PROPERTIES)
