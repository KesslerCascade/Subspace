if(CMAKE_CROSSCOMPILING)
find_package(cxobjgen)
function(add_sidl)
    set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
    foreach(idir ${EXTRA_SIDL_INCLUDE_DIRS})
        set(EXTRA_CXOBJGEN_ARGS ${EXTRA_CXOBJGEN_ARGS} -I${idir})
    endforeach()
    foreach(arg ${ARGN})
        string(REGEX REPLACE "\\.[^.]*$" "" argbase ${arg})
        add_custom_command(
            COMMAND cxobjgen -I${CX_TOP_SOURCE_DIR} -I${CX_TOP_SOURCE_DIR}/cx/include ${EXTRA_CXOBJGEN_ARGS} -f ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            IMPLICIT_DEPENDS "C" ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${argbase}.h
            )
    endforeach()
endfunction()
else()
function(add_sidl)
    set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
    foreach(idir ${EXTRA_SIDL_INCLUDE_DIRS})
        set(EXTRA_CXOBJGEN_ARGS ${EXTRA_CXOBJGEN_ARGS} -I${idir})
    endforeach()
    foreach(arg ${ARGN})
        string(REGEX REPLACE "\\.[^.]*$" "" argbase ${arg})
        add_custom_command(
            COMMAND cxobjgen -I${CX_TOP_SOURCE_DIR} -I${CX_TOP_SOURCE_DIR}/cx/include ${EXTRA_CXOBJGEN_ARGS} -f ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            DEPENDS cxobjgen
            IMPLICIT_DEPENDS "C" ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${argbase}.h
            )
    endforeach()
endfunction()
endif()

function(add_sidl_nodep)
    set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
    foreach(idir ${EXTRA_SIDL_INCLUDE_DIRS})
        set(EXTRA_CXOBJGEN_ARGS ${EXTRA_CXOBJGEN_ARGS} -I${idir})
    endforeach()
    foreach(arg ${ARGN})
        string(REGEX REPLACE "\\.[^.]*$" "" argbase ${arg})
        add_custom_command(
            COMMAND cxobjgen -I${CX_TOP_SOURCE_DIR} -I${CX_TOP_SOURCE_DIR}/cx/include ${EXTRA_CXOBJGEN_ARGS} -f ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${argbase}.h
            )
    endforeach()
endfunction()
