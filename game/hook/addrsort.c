#include "module.h"

static void qsswap(addr_t *a, addr_t *b)
{
    addr_t t = *a;
    *a       = *b;
    *b       = t;
}

static int partition(addr_t *addrs, int low, int high) {

    // Initialize pivot to be the first element
    int p = addrs[low];
    int i = low;
    int j = high;

    while (i < j) {

        // Find the first element greater than
        // the pivot (from starting)
        while (addrs[i] <= p && i <= high - 1) {
            i++;
        }

        // Find the first element smaller than
        // the pivot (from last)
        while (addrs[j] > p && j >= low + 1) {
            j--;
        }
        if (i < j) {
            qsswap(&addrs[i], &addrs[j]);
        }
    }
    qsswap(&addrs[low], &addrs[j]);
    return j;
}

void quickSort(addr_t *addrs, int low, int high) {
    if (low < high) {

        // call partition function to find Partition Index
        int pi = partition(addrs, low, high);

        // Recursively call quickSort() for left and right
        // half based on Partition Index
        quickSort(addrs, low, pi - 1);
        quickSort(addrs, pi + 1, high);
    }
}

void addrListSort(AddrList* l)
{
    quickSort(l->addrs, 0, (int)(l->num - 1));
}