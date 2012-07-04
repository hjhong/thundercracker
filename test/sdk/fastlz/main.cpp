/*
 * Check the FastLZ1 implementation against a reference.
 * The compressor uses too much memory to run inside SVM,
 * so we use some pre-canned compressed data to test.
 */

#include <sifteo.h>
using namespace Sifteo;

#include "fastlz.h"
#include "testdata.h"   


void main()
{
    static uint8_t buffer[32000];

    // First, use the reference decoder to check our data.
    unsigned size = fastlz_decompress(testdata_compressed, sizeof testdata_compressed, buffer, sizeof buffer);
    ASSERT(size == sizeof testdata_plaintext - 1);
    ASSERT(!memcmp8(testdata_plaintext, buffer, size));
    bzero(buffer);
    ASSERT(memcmp8(testdata_plaintext, buffer, size));

    // Now check the firmware's decoder
    unsigned size2 = _SYS_decompress_fastlz1(buffer, sizeof buffer, testdata_compressed, sizeof testdata_compressed);
    ASSERT(size2 == size);
    ASSERT(!memcmp8(testdata_plaintext, buffer, size));

    LOG("Success.\n");
}