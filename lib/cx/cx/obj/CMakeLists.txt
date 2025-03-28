set(OBJSRCS
    obj/objclass.c
    obj/objiface.c
    obj/objimpl.c
    obj/objstdif.c
    obj/objweak.c
    obj/stype_obj.c)
set(OBJINCLUDES
    obj/obj_private.h
    obj/objclass.h
    obj/objiface.h
    obj/objimpl.h
    obj/objstdif.h
    obj/stype_obj.h)

set(CX_SRCS
    ${CX_SRCS}
    ${OBJSRCS}
    PARENT_SCOPE)
set(CX_INCLUDES
    ${CX_INCLUDES}
    ${OBJINCLUDES}
    PARENT_SCOPE)

set(CX_CMAKE_FILES
    ${CX_CMAKE_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
    PARENT_SCOPE)
