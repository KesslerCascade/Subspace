#include <cx/container/hashtable.h>
#include <cx/container/sarray.h>
#include <cx/string.h>
#include <cx/string/strtest.h>

#define TEST_FILE httest
#define TEST_FUNCS httest_funcs
#include "common.h"

static int test_init()
{
    hashtable ht = 0;
    
    // Test basic initialization
    htInit(&ht, string, int32, 8);
    
    if (!ht)
        return 1;
    
    if (htSize(ht) != 0)
        return 1;
    
    if (htKeyType(ht) != stType(string))
        return 1;
    
    if (htValType(ht) != stType(int32))
        return 1;
    
    htDestroy(&ht);
    
    if (ht != 0)
        return 1;
    
    // Test with different types
    htInit(&ht, int32, string, 16);
    
    if (htKeyType(ht) != stType(int32))
        return 1;
    
    if (htValType(ht) != stType(string))
        return 1;
    
    htDestroy(&ht);
    
    return 0;
}

static int test_insert()
{
    hashtable ht = 0;
    htInit(&ht, string, int32, 8);
    
    // Test basic insert
    htelem elem = htInsert(&ht, string, _S"one", int32, 1);
    if (!elem)
        return 1;
    
    if (htSize(ht) != 1)
        return 1;
    
    // Insert multiple values
    htInsert(&ht, string, _S"two", int32, 2);
    htInsert(&ht, string, _S"three", int32, 3);
    htInsert(&ht, string, _S"four", int32, 4);
    htInsert(&ht, string, _S"five", int32, 5);
    
    if (htSize(ht) != 5)
        return 1;
    
    // Test overwrite behavior (default is to overwrite)
    htInsert(&ht, string, _S"three", int32, 33);
    
    if (htSize(ht) != 5)  // Size should remain the same
        return 1;
    
    int32 val;
    if (!htFind(ht, string, _S"three", int32, &val))
        return 1;
    
    if (val != 33)
        return 1;
    
    // Test HT_Ignore flag - should not overwrite
    htInsert(&ht, string, _S"three", int32, 333, HT_Ignore);
    
    if (!htFind(ht, string, _S"three", int32, &val))
        return 1;
    
    if (val != 33)  // Should still be 33, not 333
        return 1;
    
    htDestroy(&ht);
    
    return 0;
}

static int test_find()
{
    hashtable ht = 0;
    htInit(&ht, string, int32, 8);
    
    // Insert test data
    htInsert(&ht, string, _S"alpha", int32, 100);
    htInsert(&ht, string, _S"beta", int32, 200);
    htInsert(&ht, string, _S"gamma", int32, 300);
    htInsert(&ht, string, _S"delta", int32, 400);
    
    // Test finding existing keys
    int32 val;
    if (!htFind(ht, string, _S"alpha", int32, &val))
        return 1;
    if (val != 100)
        return 1;
    
    if (!htFind(ht, string, _S"gamma", int32, &val))
        return 1;
    if (val != 300)
        return 1;
    
    // Test finding non-existent key
    if (htFind(ht, string, _S"epsilon", int32, &val))
        return 1;
    
    // Test finding without extracting value (just checking existence)
    htelem elem = htFind(ht, string, _S"beta", none, NULL);
    if (!elem)
        return 1;
    
    // Test htHasKey
    if (!htHasKey(ht, string, _S"delta"))
        return 1;
    
    if (htHasKey(ht, string, _S"nothere"))
        return 1;
    
    htDestroy(&ht);
    
    return 0;
}

static int test_htelem()
{
    hashtable ht = 0;
    htInit(&ht, string, int32, 8);
    
    // Insert and get htelem
    htelem elem1 = htInsert(&ht, string, _S"first", int32, 111);
    htelem elem2 = htInsert(&ht, string, _S"second", int32, 222);
    htelem elem3 = htInsert(&ht, string, _S"third", int32, 333);
    
    if (!elem1 || !elem2 || !elem3)
        return 1;
    
    // Test hteVal macro to get values from elements
    int32 val1 = hteVal(ht, int32, elem1);
    int32 val2 = hteVal(ht, int32, elem2);
    int32 val3 = hteVal(ht, int32, elem3);
    
    if (val1 != 111 || val2 != 222 || val3 != 333)
        return 1;
    
    // Test hteValPtr to get pointer to value
    int32 *ptr1 = hteValPtr(ht, int32, elem1);
    if (!ptr1 || *ptr1 != 111)
        return 1;
    
    // Modify through pointer
    *ptr1 = 1111;
    
    // Verify modification
    htFind(ht, string, _S"first", int32, &val1);
    if (val1 != 1111)
        return 1;
    
    // Test hteKey macro to get keys from elements
    string key2 = hteKey(ht, string, elem2);
    if (!strEq(key2, _S"second"))
        return 1;
    
    // Find an element and use it
    htelem elem4 = htFind(ht, string, _S"third", none, NULL);
    if (!elem4)
        return 1;
    
    int32 val4 = hteVal(ht, int32, elem4);
    if (val4 != 333)
        return 1;
    
    htDestroy(&ht);
    
    return 0;
}

static int test_remove()
{
    hashtable ht = 0;
    htInit(&ht, string, int32, 8);
    
    // Insert test data
    htInsert(&ht, string, _S"red", int32, 10);
    htInsert(&ht, string, _S"green", int32, 20);
    htInsert(&ht, string, _S"blue", int32, 30);
    htInsert(&ht, string, _S"yellow", int32, 40);
    htInsert(&ht, string, _S"cyan", int32, 50);
    
    if (htSize(ht) != 5)
        return 1;
    
    // Test basic removal
    if (!htRemove(&ht, string, _S"green"))
        return 1;
    
    if (htSize(ht) != 4)
        return 1;
    
    // Verify it's actually gone
    if (htHasKey(ht, string, _S"green"))
        return 1;
    
    // Test removing non-existent key
    if (htRemove(&ht, string, _S"purple"))
        return 1;
    
    if (htSize(ht) != 4)
        return 1;
    
    // Test htExtract to get value while removing
    int32 extracted;
    if (!htExtract(&ht, string, _S"blue", int32, &extracted))
        return 1;
    
    if (extracted != 30)
        return 1;
    
    if (htSize(ht) != 3)
        return 1;
    
    if (htHasKey(ht, string, _S"blue"))
        return 1;
    
    // Remove remaining items
    htRemove(&ht, string, _S"red");
    htRemove(&ht, string, _S"yellow");
    htRemove(&ht, string, _S"cyan");
    
    if (htSize(ht) != 0)
        return 1;
    
    htDestroy(&ht);
    
    return 0;
}

static int test_iterator()
{
    hashtable ht = 0;
    htInit(&ht, string, int32, 8);
    
    // Insert test data
    htInsert(&ht, string, _S"apple", int32, 1);
    htInsert(&ht, string, _S"banana", int32, 2);
    htInsert(&ht, string, _S"cherry", int32, 3);
    htInsert(&ht, string, _S"date", int32, 4);
    htInsert(&ht, string, _S"elderberry", int32, 5);
    
    // Test iterator
    htiter iter;
    int count = 0;
    int sum = 0;
    
    if (!htiInit(&iter, ht))
        return 1;
    
    do {
        string key = htiKey(string, iter);
        int32 val = htiVal(int32, iter);
        
        // Verify key is not empty
        if (strEmpty(key))
            return 1;
        
        // Verify value is in expected range
        if (val < 1 || val > 5)
            return 1;
        
        count++;
        sum += val;
    } while (htiNext(&iter));
    
    htiFinish(&iter);
    
    // Should have iterated over all 5 items
    if (count != 5)
        return 1;
    
    // Sum should be 1+2+3+4+5 = 15
    if (sum != 15)
        return 1;
    
    // Test iterator on empty hashtable
    htClear(&ht);
    
    if (!htiInit(&iter, ht)) {
        // This is expected - empty hashtable returns false
        // But we should still be able to check validity
        if (htiValid(&iter))
            return 1;
    } else {
        return 1;  // Should have returned false for empty table
    }
    
    htDestroy(&ht);
    
    return 0;
}

static int test_order()
{
    hashtable ht = 0;
    htInit(&ht, int32, string, 16);
    
    // Insert items in a specific order
    htInsert(&ht, int32, 10, string, _S"ten");
    htInsert(&ht, int32, 20, string, _S"twenty");
    htInsert(&ht, int32, 30, string, _S"thirty");
    htInsert(&ht, int32, 40, string, _S"forty");
    htInsert(&ht, int32, 50, string, _S"fifty");
    
    // Collect insertion order using iterator
    sa_int32 keys;
    saInit(&keys, int32, 5);
    
    htiter iter;
    if (htiInit(&iter, ht)) {
        do {
            int32 key = htiKey(int32, iter);
            saPush(&keys, int32, key);
        } while (htiNext(&iter));
        htiFinish(&iter);
    }
    
    // Verify we got all keys
    if (saSize(keys) != 5)
        return 1;
    
    // Verify the insertion order is preserved (should be 10, 20, 30, 40, 50)
    // This hashtable implementation guarantees insertion order is retained
    if (keys.a[0] != 10 || keys.a[1] != 20 || keys.a[2] != 30 ||
        keys.a[3] != 40 || keys.a[4] != 50)
        return 1;
    
    // Test that removing and re-adding maintains consistency
    htRemove(&ht, int32, 30);
    htInsert(&ht, int32, 30, string, _S"thirty-new");
    
    string val;
    if (!htFind(ht, int32, 30, string, &val))
        return 1;
    
    if (!strEq(val, _S"thirty-new"))
        return 1;
    
    strDestroy(&val);
    saDestroy(&keys);
    htDestroy(&ht);
    
    return 0;
}

static int test_complex()
{
    hashtable ht = 0;
    htInit(&ht, string, string, 8);
    
    // Test with string keys and values
    htInsert(&ht, string, _S"greeting", string, _S"Hello");
    htInsert(&ht, string, _S"farewell", string, _S"Goodbye");
    htInsert(&ht, string, _S"thanks", string, _S"Thank you");
    
    string val = 0;
    if (!htFind(ht, string, _S"greeting", string, &val))
        return 1;
    
    if (!strEq(val, _S"Hello"))
        return 1;
    
    strDestroy(&val);
    
    // Test htClone
    hashtable ht2 = 0;
    htClone(&ht2, ht);
    
    if (htSize(ht2) != 3)
        return 1;
    
    if (!htFind(ht2, string, _S"farewell", string, &val))
        return 1;
    
    if (!strEq(val, _S"Goodbye"))
        return 1;
    
    strDestroy(&val);
    
    // Modify original, clone should be unaffected
    htInsert(&ht, string, _S"greeting", string, _S"Hi");
    
    if (!htFind(ht, string, _S"greeting", string, &val))
        return 1;
    if (!strEq(val, _S"Hi"))
        return 1;
    strDestroy(&val);
    
    if (!htFind(ht2, string, _S"greeting", string, &val))
        return 1;
    if (!strEq(val, _S"Hello"))  // Should still be original
        return 1;
    strDestroy(&val);
    
    htDestroy(&ht);
    htDestroy(&ht2);
    
    return 0;
}

testfunc httest_funcs[] = {
    { "init", test_init },
    { "insert", test_insert },
    { "find", test_find },
    { "htelem", test_htelem },
    { "remove", test_remove },
    { "iterator", test_iterator },
    { "order", test_order },
    { "complex", test_complex },
    { 0, 0 }
};
