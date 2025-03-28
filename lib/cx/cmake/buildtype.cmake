# Defaults for the standard CMake build types if we aren't explicitly given a configured type
if(NOT DEFINED ${CX_BUILD_TYPE})
    set(CX_BUILD_TYPE $<$<CONFIG:Debug>:Debug>$<$<CONFIG:Dev,DevNoOpt>:Dev>$<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:Release>)
endif()

set (isdebug "$<STREQUAL:${CX_BUILD_TYPE},Debug>")
set (isdev "$<STREQUAL:${CX_BUILD_TYPE},Dev>")
set (isdevordebug "$<OR:${isdev},${isdebug}>")
set (isrelease "$<STREQUAL:${CX_BUILD_TYPE},Release>")