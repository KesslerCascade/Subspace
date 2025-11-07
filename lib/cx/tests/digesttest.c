#include <cx/digest.h>
#include <cx/string.h>
#include <stdio.h>
#include <string.h>

#define TEST_FILE digesttest
#define TEST_FUNCS digesttest_funcs
#include "common.h"

// Helper function to convert binary digest to hex string
static void digest_to_hex(uint8* digest, uint32 size, char* out)
{
    const char* hex = "0123456789abcdef";
    for (uint32 i = 0; i < size; i++) {
        out[i * 2] = hex[digest[i] >> 4];
        out[i * 2 + 1] = hex[digest[i] & 0xf];
    }
    out[size * 2] = '\0';
}

// Test MD5 algorithm
static int test_digest_md5()
{
    Digest digest;
    uint8 result[16];
    char hexresult[33];

    // Test 1: Empty string
    digestInit(&digest, DIGEST_MD5);
    digestFinish(&digest, result);
    digest_to_hex(result, 16, hexresult);
    if (strcmp(hexresult, "d41d8cd98f00b204e9800998ecf8427e") != 0)
        return 1;

    // Test 2: "abc"
    digestInit(&digest, DIGEST_MD5);
    digestUpdate(&digest, (uint8*)"abc", 3);
    digestFinish(&digest, result);
    digest_to_hex(result, 16, hexresult);
    if (strcmp(hexresult, "900150983cd24fb0d6963f7d28e17f72") != 0)
        return 1;

    // Test 3: "message digest"
    digestInit(&digest, DIGEST_MD5);
    digestUpdate(&digest, (uint8*)"message digest", 14);
    digestFinish(&digest, result);
    digest_to_hex(result, 16, hexresult);
    if (strcmp(hexresult, "f96b697d7cb7938d525a2f31aaf161d0") != 0)
        return 1;

    // Test 4: "abcdefghijklmnopqrstuvwxyz"
    digestInit(&digest, DIGEST_MD5);
    digestUpdate(&digest, (uint8*)"abcdefghijklmnopqrstuvwxyz", 26);
    digestFinish(&digest, result);
    digest_to_hex(result, 16, hexresult);
    if (strcmp(hexresult, "c3fcd3d76192e4007dfb496cca67e13b") != 0)
        return 1;

    // Test 5: 55 bytes (just before padding boundary)
    // MD5 block size is 64 bytes, padding adds 1 byte (0x80) + 8 bytes for length
    // So 55 bytes is the maximum that fits in one block
    digestInit(&digest, DIGEST_MD5);
    uint8 data55[55];
    memset(data55, 'A', 55);
    digestUpdate(&digest, data55, 55);
    digestFinish(&digest, result);
    digest_to_hex(result, 16, hexresult);
    if (strcmp(hexresult, "e38a93ffe074a99b3fed47dfbe37db21") != 0)
        return 1;

    // Test 6: 56 bytes (exactly at padding boundary - needs second block)
    digestInit(&digest, DIGEST_MD5);
    uint8 data56[56];
    memset(data56, 'B', 56);
    digestUpdate(&digest, data56, 56);
    digestFinish(&digest, result);
    digest_to_hex(result, 16, hexresult);
    if (strcmp(hexresult, "ee0a6d91f573b47a98f7ce0b4d671c5f") != 0)
        return 1;

    // Test 7: 64 bytes (exactly one full block)
    digestInit(&digest, DIGEST_MD5);
    uint8 data64[64];
    memset(data64, 'C', 64);
    digestUpdate(&digest, data64, 64);
    digestFinish(&digest, result);
    digest_to_hex(result, 16, hexresult);
    if (strcmp(hexresult, "96d2704b7115c040215a81e658b74d8c") != 0)
        return 1;

    // Test 8: 119 bytes (second block boundary - 1)
    digestInit(&digest, DIGEST_MD5);
    uint8 data119[119];
    memset(data119, 'D', 119);
    digestUpdate(&digest, data119, 119);
    digestFinish(&digest, result);
    digest_to_hex(result, 16, hexresult);
    if (strcmp(hexresult, "029ba7bf0a9569048c049ffbaa851902") != 0)
        return 1;

    // Test 9: 120 bytes (exactly at second block boundary)
    digestInit(&digest, DIGEST_MD5);
    uint8 data120[120];
    memset(data120, 'E', 120);
    digestUpdate(&digest, data120, 120);
    digestFinish(&digest, result);
    digest_to_hex(result, 16, hexresult);
    if (strcmp(hexresult, "4195f7932c650e5ad0905857a32d35a1") != 0)
        return 1;

    // Test 10: Multiple updates
    digestInit(&digest, DIGEST_MD5);
    digestUpdate(&digest, (uint8*)"The quick brown ", 16);
    digestUpdate(&digest, (uint8*)"fox jumps over ", 15);
    digestUpdate(&digest, (uint8*)"the lazy dog", 12);
    digestFinish(&digest, result);
    digest_to_hex(result, 16, hexresult);
    if (strcmp(hexresult, "9e107d9d372bb6826bd81d3542a419d6") != 0)
        return 1;

    return 0;
}

// Test SHA-1 algorithm
static int test_digest_sha1()
{
    Digest digest;
    uint8 result[20];
    char hexresult[41];

    // Test 1: Empty string
    digestInit(&digest, DIGEST_SHA1);
    digestFinish(&digest, result);
    digest_to_hex(result, 20, hexresult);
    if (strcmp(hexresult, "da39a3ee5e6b4b0d3255bfef95601890afd80709") != 0)
        return 1;

    // Test 2: "abc"
    digestInit(&digest, DIGEST_SHA1);
    digestUpdate(&digest, (uint8*)"abc", 3);
    digestFinish(&digest, result);
    digest_to_hex(result, 20, hexresult);
    if (strcmp(hexresult, "a9993e364706816aba3e25717850c26c9cd0d89d") != 0)
        return 1;

    // Test 3: "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    digestInit(&digest, DIGEST_SHA1);
    digestUpdate(&digest, (uint8*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
    digestFinish(&digest, result);
    digest_to_hex(result, 20, hexresult);
    if (strcmp(hexresult, "84983e441c3bd26ebaae4aa1f95129e5e54670f1") != 0)
        return 1;

    // Test 4: 55 bytes (just before padding boundary)
    digestInit(&digest, DIGEST_SHA1);
    uint8 data55[55];
    memset(data55, 'A', 55);
    digestUpdate(&digest, data55, 55);
    digestFinish(&digest, result);
    digest_to_hex(result, 20, hexresult);
    if (strcmp(hexresult, "5021b3d42aa093bffc34eedd7a1455f3624bc552") != 0)
        return 1;

    // Test 5: 56 bytes (exactly at padding boundary - needs second block)
    digestInit(&digest, DIGEST_SHA1);
    uint8 data56[56];
    memset(data56, 'B', 56);
    digestUpdate(&digest, data56, 56);
    digestFinish(&digest, result);
    digest_to_hex(result, 20, hexresult);
    if (strcmp(hexresult, "021f99328a6a79566f055914466ae1654d16ab01") != 0)
        return 1;

    // Test 6: 64 bytes (exactly one full block)
    digestInit(&digest, DIGEST_SHA1);
    uint8 data64[64];
    memset(data64, 'C', 64);
    digestUpdate(&digest, data64, 64);
    digestFinish(&digest, result);
    digest_to_hex(result, 20, hexresult);
    if (strcmp(hexresult, "15e762b2667aa87c563ca15f253f7288f9d4e235") != 0)
        return 1;

    // Test 7: 119 bytes (second block boundary - 1)
    digestInit(&digest, DIGEST_SHA1);
    uint8 data119[119];
    memset(data119, 'D', 119);
    digestUpdate(&digest, data119, 119);
    digestFinish(&digest, result);
    digest_to_hex(result, 20, hexresult);
    if (strcmp(hexresult, "ce5e713dee33b63d1414768b73ca6a69fff7f5f8") != 0)
        return 1;

    // Test 8: 120 bytes (exactly at second block boundary)
    digestInit(&digest, DIGEST_SHA1);
    uint8 data120[120];
    memset(data120, 'E', 120);
    digestUpdate(&digest, data120, 120);
    digestFinish(&digest, result);
    digest_to_hex(result, 20, hexresult);
    if (strcmp(hexresult, "fc911ed90dd1028a406ecbe94fa296b29d192711") != 0)
        return 1;

    // Test 9: Multiple updates
    digestInit(&digest, DIGEST_SHA1);
    digestUpdate(&digest, (uint8*)"The quick brown ", 16);
    digestUpdate(&digest, (uint8*)"fox jumps over ", 15);
    digestUpdate(&digest, (uint8*)"the lazy dog", 12);
    digestFinish(&digest, result);
    digest_to_hex(result, 20, hexresult);
    if (strcmp(hexresult, "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12") != 0)
        return 1;

    return 0;
}

// Test SHA-256 algorithm
static int test_digest_sha256()
{
    Digest digest;
    uint8 result[32];
    char hexresult[65];

    // Test 1: Empty string
    digestInit(&digest, DIGEST_SHA256);
    digestFinish(&digest, result);
    digest_to_hex(result, 32, hexresult);
    if (strcmp(hexresult, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") != 0)
        return 1;

    // Test 2: "abc"
    digestInit(&digest, DIGEST_SHA256);
    digestUpdate(&digest, (uint8*)"abc", 3);
    digestFinish(&digest, result);
    digest_to_hex(result, 32, hexresult);
    if (strcmp(hexresult, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") != 0)
        return 1;

    // Test 3: "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    digestInit(&digest, DIGEST_SHA256);
    digestUpdate(&digest, (uint8*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
    digestFinish(&digest, result);
    digest_to_hex(result, 32, hexresult);
    if (strcmp(hexresult, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1") != 0)
        return 1;

    // Test 4: 55 bytes (just before padding boundary)
    digestInit(&digest, DIGEST_SHA256);
    uint8 data55[55];
    memset(data55, 'A', 55);
    digestUpdate(&digest, data55, 55);
    digestFinish(&digest, result);
    digest_to_hex(result, 32, hexresult);
    if (strcmp(hexresult, "8963cc0afd622cc7574ac2011f93a3059b3d65548a77542a1559e3d202e6ab00") != 0)
        return 1;

    // Test 5: 56 bytes (exactly at padding boundary - needs second block)
    digestInit(&digest, DIGEST_SHA256);
    uint8 data56[56];
    memset(data56, 'B', 56);
    digestUpdate(&digest, data56, 56);
    digestFinish(&digest, result);
    digest_to_hex(result, 32, hexresult);
    if (strcmp(hexresult, "821c30ffb748ac6d776ad4972a6cbc7ca32e6aaf63b68808e7fe92321dfbb6b8") != 0)
        return 1;

    // Test 6: 64 bytes (exactly one full block)
    digestInit(&digest, DIGEST_SHA256);
    uint8 data64[64];
    memset(data64, 'C', 64);
    digestUpdate(&digest, data64, 64);
    digestFinish(&digest, result);
    digest_to_hex(result, 32, hexresult);
    if (strcmp(hexresult, "27d926a6708d6769d97d065e7a5a860ab496eab272c03d1cebc55ec71b954fa2") != 0)
        return 1;

    // Test 7: 119 bytes (second block boundary - 1)
    digestInit(&digest, DIGEST_SHA256);
    uint8 data119[119];
    memset(data119, 'D', 119);
    digestUpdate(&digest, data119, 119);
    digestFinish(&digest, result);
    digest_to_hex(result, 32, hexresult);
    if (strcmp(hexresult, "500c8db66d87d52bb804ebad5fc338518421e010a9dea478b3f955b14dfceddb") != 0)
        return 1;

    // Test 8: 120 bytes (exactly at second block boundary)
    digestInit(&digest, DIGEST_SHA256);
    uint8 data120[120];
    memset(data120, 'E', 120);
    digestUpdate(&digest, data120, 120);
    digestFinish(&digest, result);
    digest_to_hex(result, 32, hexresult);
    if (strcmp(hexresult, "c9c21eae11dd344d43c4a6af575c83bd00e0d6ab4d64dc49e0ca5d42ab015de9") != 0)
        return 1;

    // Test 9: Multiple updates
    digestInit(&digest, DIGEST_SHA256);
    digestUpdate(&digest, (uint8*)"The quick brown ", 16);
    digestUpdate(&digest, (uint8*)"fox jumps over ", 15);
    digestUpdate(&digest, (uint8*)"the lazy dog", 12);
    digestFinish(&digest, result);
    digest_to_hex(result, 32, hexresult);
    if (strcmp(hexresult, "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592") != 0)
        return 1;

    return 0;
}

testfunc digesttest_funcs[] = {
    { "md5", test_digest_md5 },
    { "sha1", test_digest_sha1 },
    { "sha256", test_digest_sha256 },
    { 0, 0 }
};
