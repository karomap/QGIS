SET(OWS_SRCS
  qgsowsprovider.cpp
  qgsowsdataitems.cpp
)
SET(OWS_MOC_HDRS
  qgsowsprovider.h
  qgsowsdataitems.h
)

INCLUDE_DIRECTORIES (
  ${CMAKE_SOURCE_DIR}/src/core
  ${CMAKE_SOURCE_DIR}/src/core/expression
  ${CMAKE_SOURCE_DIR}/src/core/geometry
  ${CMAKE_SOURCE_DIR}/src/core/auth
  ${CMAKE_SOURCE_DIR}/src/core/metadata
  ${CMAKE_SOURCE_DIR}/src/gui
  ${CMAKE_SOURCE_DIR}/src/gui/auth
  ${CMAKE_SOURCE_DIR}/external

  ${CMAKE_BINARY_DIR}/src/core
  ${CMAKE_BINARY_DIR}/src/gui
  ${CMAKE_BINARY_DIR}/src/ui
)
INCLUDE_DIRECTORIES(SYSTEM
)

QT5_WRAP_CPP(OWS_MOC_SRCS ${OWS_MOC_HDRS})
ADD_LIBRARY (owsprovider MODULE ${OWS_SRCS} ${OWS_MOC_SRCS})

TARGET_LINK_LIBRARIES (owsprovider
  qgis_core
)

IF (WITH_GUI)
  TARGET_LINK_LIBRARIES (owsprovider
    qgis_gui
  )
ENDIF ()

INSTALL(TARGETS owsprovider
  RUNTIME DESTINATION ${QGIS_PLUGIN_DIR}
  LIBRARY DESTINATION ${QGIS_PLUGIN_DIR})
