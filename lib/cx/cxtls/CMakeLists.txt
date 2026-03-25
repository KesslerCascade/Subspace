add_library(cxtls STATIC
    cxtls.c)

target_link_libraries(cxtls cx mbedtls mbedx509 mbedcrypto)
target_include_directories(cxtls PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
