find_package (Threads REQUIRED)

include (${CX_TOP_SOURCE_DIR}/cmake/werror.cmake)

if(NOT CMAKE_CROSSCOMPILING)
if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD" OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CX_PLATFORM_SRCS
	../cx/platform/unix/unix_error.c
	../cx/platform/unix/unix_fs_file.c
	../cx/platform/unix/unix_fs_fs.c
	../cx/platform/unix/unix_os.c
	../cx/platform/unix/unix_time_clock.c)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(CX_PLATFORM_SRCS
            ${CX_PLATFORM_SRCS}
            ../cx/platform/unix/linux_futex.c)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
        set(CX_PLATFORM_SRCS
            ${CX_PLATFORM_SRCS}
            ../cx/platform/unix/freebsd_futex.c)
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CX_PLATFORM_SRCS
	../cx/platform/win/win_error.c
	../cx/platform/win/win_fs_file.c
	../cx/platform/win/win_fs_fs.c
	../cx/platform/win/win_futex.c
	../cx/platform/win/win_os.c
	../cx/platform/win/win_time_clock.c)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

set(CXOBJGEN_SRCS
    classes.c
    header.c
    impl.c
    interfaces.c
    objtypes.c
    parser.c
    cxobjgen.c
    stub.c)
set(CX_SRCS
    cx/obj/objclass.c
    cx/obj/objiface.c
    cx/obj/objimpl.c
    cx/obj/objstdif.c
    cx/obj/objweak.c
    cx/obj/stype_obj.c
    ../cx/closure/cchain.c
    ../cx/closure/closure.c
    ../cx/closure/stype_closure.c
    ../cx/container/hashtable.c
    ../cx/container/sarray.c
    ../cx/container/stype_hashtable.c
    ../cx/container/stype_sarray.c
    ../cx/debug/dbgtypes.c
    ../cx/debug/error.c
    ../cx/serialize/lineparse.c
    ../cx/serialize/sbfsfile.c
    ../cx/serialize/sbstring.c
    ../cx/serialize/streambuf.c
    ../cx/stype/stconvert.c
    ../cx/stype/stvar.c
    ../cx/stype/stype.c
    ../cx/stype/stype_stvar.c
    ${CX_PLATFORM_SRCS}
    ../cx/string/strbase.c
    ../cx/string/strbase64.c
    ../cx/string/strcase.c
    ../cx/string/strcompare.c
    ../cx/string/strconcat.c
    ../cx/string/strencoding.c
    ../cx/string/strfind.c
    ../cx/string/striter.c
    ../cx/string/strjoin.c
    ../cx/string/strnum_fp.c
    ../cx/string/strnum_int.c
    ../cx/string/strrope.c
    ../cx/string/strsplit.c
    ../cx/string/strsubstr.c
    ../cx/string/stype_string.c
    ../cx/fs/path.c
    ../cx/fs/pathmatch.c
    ../cx/thread/mutex.c
    ../cx/thread/rwlock.c
    ../cx/thread/sema.c
    ../cx/utils/lazyinit.c
    ../cx/utils/murmur.c
    ../cx/utils/scratch.c
    ../cx/xalloc/cstrutil.c
    ../cx/xalloc/oom.c
    ../cx/xalloc/xalloc.c
    )
source_group("cx" FILES
    ${CX_SRCS})

set(CXOBJGEN_INCLUDES
    objtypes.h
    cxobjgen.h
    objtypes.sidl
    )

source_group("" FILES
    ${CXOBJGEN_SRCS}
    ${CXOBJGEN_INCLUDES})

add_executable(cxobjgen
    ${CXOBJGEN_SRCS}
    ${CXOBJGEN_INCLUDES}
    ${CX_SRCS})

target_compile_definitions(cxobjgen PRIVATE CX_BUILDING_CXOBJGEN)

target_link_libraries(cxobjgen ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} mimalloc pcre2-8-object)
target_include_directories(cxobjgen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} SYSTEM PUBLIC ${CX_TOP_SOURCE_DIR} ${CX_TOP_SOURCE_DIR}/cx/include ${CX_TOP_BINARY_DIR})
set_property(TARGET cxobjgen PROPERTY FOLDER "utils")

export(TARGETS cxobjgen FILE "${CMAKE_BINARY_DIR}/cxobjgen-config.cmake")

if(CX_STANDALONE)
    add_custom_target(cxobjgen_regenerate
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/objclass.c ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/objclass.c
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/objclass.h ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/objclass.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/objiface.c ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/objiface.c
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/objiface.h ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/objiface.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/objimpl.c ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/objimpl.c
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/objimpl.h ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/objimpl.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/objstdif.sidl ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/objstdif.sidl
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/objstdif.c ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/objstdif.c
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/objweak.c ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/objweak.c
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/obj_private.h ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/obj_private.h
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/stype_obj.c ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/stype_obj.c
        COMMAND ${CMAKE_COMMAND} -E copy ${CX_TOP_SOURCE_DIR}/cx/obj/stype_obj.h ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/stype_obj.h
        COMMAND cxobjgen -f ${CX_TOP_SOURCE_DIR}/cxobjgen/cx/obj/objstdif.sidl
        COMMAND cxobjgen -f -I${CX_TOP_SOURCE_DIR} ${CX_TOP_SOURCE_DIR}/cxobjgen/objtypes.sidl
        DEPENDS cxobjgen)
    set_property(TARGET cxobjgen_regenerate PROPERTY FOLDER "generate")
endif()

endif()     # NOT CMAKE_CROSSCOMPILING
