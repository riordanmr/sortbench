// sortbench.cpp - Program to benchmark sorting algorithms.
// Includes implementations of those algorithms.
// The initial algorithm to be benchmarked is Shellsort.
//
// Created by Mark Riordan on 2023-05-02.
//
// On Marks-iMac-Pro the release build location is:
// /Users/mrr/Library/Developer/Xcode/DerivedData/sortbench-ctcrgrohjviqkchcipkjtnucyeou/Build/Products/Release/sortbench

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <string>
#include <cmath>

using namespace std;

struct DataRecord {
    char data[72];
};

typedef DataRecord *ArrayElementType;

typedef uint64_t sb_timer_t;

bool elementGreaterThan(ArrayElementType &first, ArrayElementType &second)
{
    return (strncmp(first->data, second->data, 6) > 0);
}

FILE *fileLog = NULL;
void openLogFile()
{
    fileLog = fopen("sortbench.csv", "a");
}

void closeLogFile()
{
    fclose(fileLog);
}

void writeLogRec(const char *sortName, int64_t nRecs, int64_t seed, int64_t elapsedNs, bool bSortedOK)
{
    double elapsedSecs = 0.000000001 * elapsedNs;
    double recsPerSec = nRecs / elapsedSecs;
    fprintf(fileLog,
            "%s,%lld,%lld,%lld,%f,%s\n",sortName, nRecs, seed, elapsedNs, recsPerSec,
            bSortedOK ? "true":"false");
}

enum TypGap {GAP_CIURA_22, CAP_CIURA_225, CAP_CIURA_225_ODD, GAP_CIURA_SQRT5, GAP_KNUTH73, GAP_MAX};
const int NUM_GAPS = 32;
int64_t allGaps[GAP_MAX][NUM_GAPS] = {
    {1, 4, 10, 23, 57, 132, 301, 701, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},
    {1, 4, 10, 23, 57, 132, 301, 701, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},
    {1, 4, 10, 23, 57, 132, 301, 701, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},
    {1, 4, 10, 23, 57, 132, 301, 701, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1},
    {0}
};

const char *nameOfGapType(TypGap gapType)
{
    const char *name = "Unknown";
    struct TypGapToName {
        TypGap gtn_type;
        const char *gtn_name;
    } aryTypeToName[] = {
        {GAP_CIURA_22, "Ciura22"},
        {CAP_CIURA_225, "Ciura225"},
        {CAP_CIURA_225_ODD, "Ciura225Odd"},
        {GAP_CIURA_SQRT5, "CiuraSqrt5"},
        {GAP_KNUTH73, "Knuth73"},
        {GAP_MAX, NULL}
    };
    for(int j=0; aryTypeToName[j].gtn_name!=NULL; j++) {
        if(gapType == aryTypeToName[j].gtn_type) {
            name = aryTypeToName[j].gtn_name;
            break;
        }
    }
    return name;
}

void buildGaps()
{
    int j, gap;
    int64_t *gaps;
    
    // Build Ciura gaps, starting with the hard-coded results from Ciura, and
    // extending them by multiplying the previous gap by 2.2 (11/5).
    gaps = allGaps[GAP_CIURA_22];
    for(j=0; gaps[j]>=0; j++) {
        if(gaps[j]==0) {
            gaps[j] = 11*gaps[j-1] / 5;
        }
    }
    
    gaps = allGaps[CAP_CIURA_225];
    for(j=0; gaps[j]>=0; j++) {
        if(gaps[j]==0) {
            gaps[j] = (uint64_t)(2.25*gaps[j-1]);
        }
    }
    
    gaps = allGaps[CAP_CIURA_225_ODD];
    for(j=0; gaps[j]>=0; j++) {
        if(gaps[j]==0) {
            gaps[j] = 1 | (uint64_t)(2.25*gaps[j-1]);
        }
    }
    
    gaps = allGaps[GAP_CIURA_SQRT5];
    double sqrt5 = sqrt(5.0);
    bool bDoFive = true;
    for(j=0; gaps[j]>=0; j++) {
        if(gaps[j]==0) {
            if(bDoFive) {
                gaps[j] = 5*gaps[j-2];
            } else {
                gaps[j] = gaps[j-1] * sqrt5;
            }
        }
    }
    
    gaps = allGaps[GAP_KNUTH73];
    for(j=0; j<NUM_GAPS-1; j++) {
        gap = pow(3, j+1);
        gap--;
        gap /= 2;
        gaps[j] = gap;
    }
    gaps[NUM_GAPS-1] = -1;
}

void printOneGap(TypGap gapType)
{
    int64_t *gaps = allGaps[gapType];gaps = allGaps[gapType];
    printf("%s: ", nameOfGapType(gapType));
    for(int j=0; gaps[j]>=0; j++) {
        printf(" %lld", gaps[j]);
    }
    printf("\n");
}

void printGaps()
{
    int iGapType;
    for(TypGap gapType=GAP_CIURA_22; gapType<GAP_MAX;
        (iGapType = (int) gapType, iGapType++, gapType = (TypGap) iGapType)) {
        printOneGap(gapType);
    }
}

sb_timer_t getCurrentNanoseconds()
{
    // http://www.manpagez.com/man/3/clock_gettime_nsec_np/
    sb_timer_t nano = clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW);
    return nano;
}

static uint64_t randoms[4];

char getRandomChar()
{
    static const char *possibleChars = "abcdefghijklmnopqrstuvwxyz012345";
    uint64_t random = ((randoms[0] + randoms[2]) ^ 0x2d135) + (randoms[1]>>7);
    randoms[0] = randoms[1];
    randoms[1] = randoms[2];
    randoms[2] = randoms[3];
    randoms[3] = random;
    char result = possibleChars[(random>>5) & 0x1f];
    return result;
}

void setRandomSeed(uint64_t seed)
{
    randoms[0] = seed;
    randoms[1] = (seed << 3) ^ 0x136;
    randoms[2] = seed + ((seed << 6) ^ 0x400);
    randoms[3] = (seed << 9) ^ 0x59031;
}

ArrayElementType * createArray(int64_t nElements, DataRecord *&arrayData)
{
    arrayData = new DataRecord[nElements];
    ArrayElementType *arrayPointers = new ArrayElementType[nElements];
    for(int64_t j=0; j<nElements; j++) {
        DataRecord *pRec = &arrayData[j];
        arrayPointers[j] = pRec;
        int ichar;
        for(ichar=0; ichar<sizeof(pRec->data)-1; ichar++) {
            pRec->data[ichar] = getRandomChar();
        }
        pRec->data[ichar] = '\0';
    }
    
    return arrayPointers;
}

// Delete the array of data that was created for sorting, plus the
// array of pointers to data records.
// Entry:   pArray  is the array of pointers to records.
// Exit:    The data array, plus the array of pointers to records,
//          have been deleted.
//void deleteArray(ArrayElementType *pArray)
//{
//    delete pArray[0];
//    delete pArray;
//}

// Sort an array using ShellSort.
// Entry:   a   is an array of pointers to data records.
//          n   is the number of elements in the array to sort.
//          gaps is an array of ShellSort gaps, starting with 1.
//              Note that the gaps are increasing, not decreasing.
// Exit:    a   has been sorted in increasing order.
void shellSort(ArrayElementType a[], int64_t n, int64_t gaps[])
{
    ArrayElementType temp;
    int64_t gap, igap, i, j;
    
    if(n <= 1) return;
    
    // Find the gap to start with.
    for(igap=0; gaps[igap]<n && gaps[igap]>0; igap++);
    igap--;
    
    for(; igap>=0; igap--) {
        gap = gaps[igap];
        //printf("gap=%d\n", gap);
        for(i=gap; i<n; i++) {
            //printf("Loop 2: i=%d gap=%d n=%d\n", i, gap, n);
            temp = a[i];
            // shift earlier gap-sorted elements up until the correct location for a[i] is found
            for(j=i; (j>=gap) && elementGreaterThan(a[j-gap],temp); j -= gap) {
                //printf("Setting a[%d]=a[%d]\n", j, j-gap);
                a[j] = a[j-gap];
            }
            a[j] = temp;
        }
    }
}

// Returns true if the array elements are in proper order.
bool checkArrayOrder(ArrayElementType * pArray, int64_t n)
{
    bool bOK=true;
    if(n>1) {
        for(int j=0; j<(n-1); j++) {
            if(elementGreaterThan(pArray[j], pArray[j+1])) {
                bOK = false;
                break;
            }
        }
    }
    return bOK;
}

bool doOneSort(int64_t n, int64_t gaps[], sb_timer_t &elapsedNs)
{
    bool bOK=true;
    DataRecord *arrayData;
    ArrayElementType * pArray = createArray(n, arrayData);
    sb_timer_t start = getCurrentNanoseconds();
    shellSort(pArray, n, gaps);
    elapsedNs = getCurrentNanoseconds() - start;
    bOK = checkArrayOrder(pArray, n);
    delete []arrayData;
    delete []pArray;
    return bOK;
}

void doSorts()
{
    sb_timer_t elapsedNs;
    TypGap gapType;
    int iGapType;
    for(gapType=GAP_CIURA_22; gapType<GAP_MAX; (iGapType = (int) gapType, iGapType++, gapType = (TypGap) iGapType)) {
        printf("Using ShellSort with gap sequence %s\n", nameOfGapType(gapType));
        string sortName = "ShellSort";
        sortName += nameOfGapType(gapType);
        int64_t *gaps = allGaps[gapType];
        for(int64_t nOrig=10; nOrig<=1000000; nOrig*=10) {
            for(int64_t add=0; add<2; add++) {
                int64_t n = nOrig + add;
                for(int loop=0; loop<10; loop++) {
                    uint64_t seed = 301 + loop;
                    setRandomSeed(seed);
                    bool bOK = doOneSort(n, gaps, elapsedNs);
                    writeLogRec(sortName.c_str(), n, seed, elapsedNs, bOK);
                    double elapsedSecs = 0.000000001 * elapsedNs;
                    double recsPerSec = n / elapsedSecs;
                    printf("%s size %lld seed %lld took %f sec for %.1f recs/sec; ret %s\n",
                           nameOfGapType(gapType), n, seed, elapsedSecs, recsPerSec, bOK ? "true":"false");
                }
            }
        }
    }
}

void printArray(ArrayElementType * pArray, int64_t n)
{
    for(int irec=0; irec<n; irec++) {
        printf("%3d: ", irec);
        for(int ichar=0; ichar<sizeof(pArray[0]->data); ichar++) {
            putchar(pArray[irec]->data[ichar]);
        }
        putchar('\n');
    }
}

void testTimer()
{
    sb_timer_t start = getCurrentNanoseconds();
    usleep(10000);
    sb_timer_t stop = getCurrentNanoseconds();
    sb_timer_t elapsed = stop - start;
    printf("usleep(10000) gave elapsed %lld ns\n", elapsed);
    start = getCurrentNanoseconds();
    stop = getCurrentNanoseconds();
    elapsed = stop - start;
    printf("Two consec calls gave elapsed %lld ns\n", elapsed);
}

void testRNG()
{
    setRandomSeed(762);
    for(int j=0; j<888; j++) {
        putchar(getRandomChar());
    }
    putchar('\n');
}

void testOrder()
{
    DataRecord *arrayData;
    int nElements = 3;
    ArrayElementType * pArray = createArray(nElements, arrayData);
    pArray[0]->data[0] = 'a';
    pArray[1]->data[0] = 'b';
    pArray[2]->data[0] = 'c';
    if(checkArrayOrder(pArray, nElements)) {
        printf("checkArrayOrder OK for correct order\n");
    } else {
        printf("!! checkArrayOrder failed for correct order\n");
    }
    
    pArray[1]->data[0] = 'f';
    pArray[2]->data[0] = 'e';
    if(checkArrayOrder(pArray, nElements)) {
        printf("!! checkArrayOrder failed for wrong order\n");
    } else {
        printf("checkArrayOrder OK for wrong order\n");
    }
    delete []arrayData;
    delete []pArray;
}

void testGenArray()
{
    int n = 8;
    for(int loop=0; loop<5; loop++) {
        uint64_t seed = 301 + loop;
        setRandomSeed(seed);
        DataRecord *arrayData;
        ArrayElementType * pArray = createArray(n, arrayData);
        printf("Generated array for seed %lld:\n", seed);
        printArray(pArray, n);
        delete []arrayData;
        delete []pArray;
    }
}

void testGenAndShellSort()
{
    int64_t n = 12;
    DataRecord *arrayData;
    ArrayElementType * pArray = createArray(n, arrayData);
    printf("Generated array:\n");
    printArray(pArray, n);
    int64_t gaps[] = {1, 4, 10, 23, 57, 132, 301, 701, -1};
    shellSort(pArray, n, gaps);
    printf("Sorted array:\n");
    printArray(pArray, n);
    if(checkArrayOrder(pArray, n)) {
        printf("Sorting is OK\n");
    } else {
        printf("!! Sorting is bad\n");
    }
    delete []arrayData;
    delete []pArray;
}

void testGaps()
{
    printGaps();
}

int main(int argc, const char * argv[]) {
    openLogFile();
    buildGaps();

    //testTimer();
    //testRNG();
    //testOrder();
    //testGenArray();
    //testGenAndShellSort();
    testGaps();
    
    doSorts();
    closeLogFile();
    return 0;
}
