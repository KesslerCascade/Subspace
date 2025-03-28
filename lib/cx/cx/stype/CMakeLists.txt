set(STYPE_SRCS
    stype/stconvert.c
    stype/stvar.c
    stype/stype.c
    stype/stype_stvar.c)
set(STYPE_INCLUDES
    stype/alltypes.inc
    stype/stconvert.h
    stype/stvar.h
    stype/stype.h
    stype/stype_stvar.h)

set(CX_SRCS
    ${CX_SRCS}
    ${STYPE_SRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${STYPE_INCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)
