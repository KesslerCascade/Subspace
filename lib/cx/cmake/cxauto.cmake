if(CMAKE_CROSSCOMPILING)
find_package(cxautogen)
function(add_cxautogen)
    set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
    foreach(idir ${EXTRA_CXAUTOGEN_INCLUDE_DIRS})
        set(EXTRA_CXAUTOGEN_ARGS ${EXTRA_CXAUTOGEN_ARGS} -I${idir})
    endforeach()
    foreach(arg ${ARGN})
        string(REGEX REPLACE "\\.[^.]*$" "" argbase ${arg})
        add_custom_command(
            COMMAND cxautogen -I${CX_TOP_SOURCE_DIR} -I${CX_TOP_SOURCE_DIR}/cx/include -S${CMAKE_CURRENT_SOURCE_DIR} -B${CMAKE_CURRENT_BINARY_DIR} ${EXTRA_CXAUTOGEN_ARGS} -f ${arg}
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            IMPLICIT_DEPENDS "C" ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${argbase}.h
            )
    endforeach()
endfunction()
else()
function(add_cxautogen)
    set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
    foreach(idir ${EXTRA_CXAUTOGEN_INCLUDE_DIRS})
        set(EXTRA_CXAUTOGEN_ARGS ${EXTRA_CXAUTOGEN_ARGS} -I${idir})
    endforeach()
    foreach(arg ${ARGN})
        string(REGEX REPLACE "\\.[^.]*$" "" argbase ${arg})
        add_custom_command(
            COMMAND cxautogen -I${CX_TOP_SOURCE_DIR} -I${CX_TOP_SOURCE_DIR}/cx/include -S${CMAKE_CURRENT_SOURCE_DIR} -B${CMAKE_CURRENT_BINARY_DIR} ${EXTRA_CXAUTOGEN_ARGS} -f ${arg}
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            DEPENDS cxautogen
            IMPLICIT_DEPENDS "C" ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${argbase}.h
            )
    endforeach()
endfunction()
endif()

function(add_cxautogen_nodep)
    set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
    foreach(idir ${EXTRA_CXAUTOGEN_INCLUDE_DIRS})
        set(EXTRA_CXAUTOGEN_ARGS ${EXTRA_CXAUTOGEN_ARGS} -I${idir})
    endforeach()
    foreach(arg ${ARGN})
        string(REGEX REPLACE "\\.[^.]*$" "" argbase ${arg})
        add_custom_command(
            COMMAND cxautogen -I${CX_TOP_SOURCE_DIR} -I${CX_TOP_SOURCE_DIR}/cx/include -S${CMAKE_CURRENT_SOURCE_DIR} -B${CMAKE_CURRENT_BINARY_DIR} ${EXTRA_CXAUTOGEN_ARGS} -f ${arg}
            MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${arg}
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${argbase}.h
            )
    endforeach()
endfunction()
