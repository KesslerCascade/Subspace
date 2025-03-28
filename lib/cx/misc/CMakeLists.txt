#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
#    htbench.c)

#add_executable(htbench htbench.c)
#target_link_libraries(htbench cx)

include (${CX_TOP_SOURCE_DIR}/cmake/werror.cmake)

set(PRESETS_MAKER_SRCS
    presetsmaker.c
    )

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${PRESETS_MAKER_SRCS})

add_executable(presetsmaker ${PRESETS_MAKER_SRCS})
target_link_libraries(presetsmaker cx)
set_property(TARGET presetsmaker PROPERTY FOLDER "utils")

if(CX_LUA)
    add_executable(luacmd luacmd.c)
    target_link_libraries(luacmd cxlua)
    set_property(TARGET luacmd PROPERTY FOLDER "utils")
endif()

if(CX_STANDALONE)
    add_custom_target(cx_cmake_presets_regenerate
        COMMAND presetsmaker ${CX_TOP_SOURCE_DIR}/CMakePresets.in.json ${CX_TOP_SOURCE_DIR}/CMakePresets.json
        DEPENDS presetsmaker)
    set_property(TARGET cx_cmake_presets_regenerate PROPERTY FOLDER "generate")
endif()
