/*
 * Collection of basic tools and defines
 */
#ifdef WIN32
#include <Windows.h>
#endif
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "Tools.h"

using namespace std;

#ifdef _WIN32
double log2(double x) {
    return log(x) / log(2.0);
}
#endif

typedef int64_t os_time_millis_t;

/*
 * Returns the number of milliseconds since the epoch.
 * This code is inspired by the Java JVM implementation of System.currentTimeMillis()
 */
double
my_time() {
#ifdef WIN32
    #define FT2INT64(ft) ((os_time_millis_t)(ft).dwHighDateTime << 32 | (os_time_millis_t)(ft).dwLowDateTime)
    static os_time_millis_t fileTime_1_1_70 = 0;
    SYSTEMTIME st0;
    FILETIME   ft0;

    if (fileTime_1_1_70 == 0) {
        /* Initialize fileTime_1_1_70 -- the Win32 file time of midnight
         * 1/1/70.
         */
        memset(&st0, 0, sizeof(st0));
        st0.wYear  = 1970;
        st0.wMonth = 1;
        st0.wDay   = 1;
        SystemTimeToFileTime(&st0, &ft0);
        fileTime_1_1_70 = FT2INT64(ft0);
    }

    GetSystemTime(&st0);
    SystemTimeToFileTime(&st0, &ft0);

    return (FT2INT64(ft0) - fileTime_1_1_70) / 10000.0 / 1000.0;

#else
    struct timeval t;
    gettimeofday(&t, 0);
    return ((os_time_millis_t)t.tv_sec) * 1000 + (os_time_millis_t)(t.tv_usec/1000);
#endif
}

double Tools::startTime;

void Tools::StartTimer() {
    startTime = my_time();
}

double Tools::GetTime() {
    time_t stopTime = time(NULL);
    return my_time() - startTime;
}

uchar *Tools::GetRandomString(uint min, uint max, uint &alphabetSize) {
    uint len = std::rand() % (max - min) + min;
    alphabetSize = std::rand() % 26 + 1;
    uchar *temp = new uchar[len + 2];
    for (uint i = 0; i < len; i++) {
        temp[i] = 97 + std::rand() % alphabetSize;
    }
    temp[len] = 0u;
    temp[len+1] = '\0';
    return temp;
}

ulong *Tools::bp2bitstream(uchar *bp) {
    ulong len = strlen((char *)bp);
    ulong *A = new ulong[len / W + 1];
    for (ulong i = 0; i < len; i++) {
        if (bp[i] == '(')
            SetField(A, 1, i, 1);
        else
            SetField(A, 1, i, 0);
    }
    return A;
}

void Tools::PrintBitSequence(ulong *A, ulong len) {
    for (ulong i = 0; i < len; i++) {
        if (GetField(A, 1, i))
            cout << "1";
        else
            cout << "0";
    }
    cout << "\n";
}

uint Tools::FloorLog2(ulong i)
{
    uint b = 0;
    if (i == 0) {
        return 0;
    }
    while (i) {
        b++;
        i >>= 1;
    }
    return b - 1;
}

//Creating table to find logn in small time
uint *Tools::MakeTable() {
    uint *table = new uint[512];
    for (uint i = 0; i < 9; i++)
    {
        if (i == 0)
           table[i] = 0;
        if (i >= 1 && i < (1 << 1 ))
           table[i] = 1;
        if (i >= (1 << 1 ) && i < (1 << 2 ))
           table[i] = 2;
        if (i >= (1 << 2 ) && i < (1 << 3 ))
           table[i] = 3;
        if (i >= (1 << 3 ) && i < (1 << 4 ))
           table[i] = 4;
        if (i >= (1 << 4 ) && i < (1 << 5 ))
           table[i] = 5;
        if (i >= (1 << 5 ) && i < (1 << 6 ))
           table[i] = 6;
        if (i >= (1 << 6 ) && i < (1 << 7 ))
           table[i] = 6;
        if (i >= (1 << 7 ) && i < (1 << 8 ))
           table[i] = 7;
        if (i >= (1 << 8 ) && i < (1 << 9 ))
           table[i] = 8;
    }
    return table;
}

uint Tools::FastFloorLog2(uint i) {

    uint *table = MakeTable(); uint u;
    if (i >> 24)    u = 22 + table[ i >> 24] ;
    if (i >> 16)    u = 14 + table[ i >> 16] ;
    if (i >> 8)     u = 6 + table[ i >> 8] ;
    u =  table[i] - 1;
    delete [] table;
    return u;
}

uint Tools::CeilLog2(ulong i) {

    uint j = FloorLog2(i);
    if ((ulong)(1lu << j) != i)
        return j + 1;

    return j;
}

uchar *Tools::GetFileContents(char *filename, ulong maxSize) {
    std::ifstream::pos_type posSize;
    std::ifstream file ((char *)filename, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        posSize = file.tellg();
        ulong size = (ulong)posSize;
        if (maxSize != 0 && size > maxSize)
            size = maxSize;
        char *memblock = new char [size + 1];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        memblock[size] = '\0';
        file.close();
    return (uchar *)memblock;
    }
    else
        return 0;
}

/*
ulong Tools::ustrlen(uchar *text)
{
   ulong i=0;

   while (text[i]>0u) i++;
   return i;
}

char *Tools::uctoc(uchar *text, bool remove)
{
   ulong i, len = ustrlen(text);

   //printf("%d\n",len);

   char *result = new char[len+1];

   result[len] = '\0';

   for(i=0;i<len;i++)
      result[i] = (int)text[i]-128;

   if (remove) delete text;

   return result;
}

uchar *Tools::ctouc(char *text, bool remove)
{
   ulong i, len = strlen(text);

   uchar *result = new uchar[len+1];

   result[len] = 0u;

   for(i=0;i<len;i++)
      result[i] = (int)text[i]+128;

   if (remove) delete text;

   return result;
}

void Tools::RemoveControlCharacters(uchar *data)
{
    // Remove characters that are lexicographically smaller than default end character '#'
    // (and all characters '#')
    ulong i = 0;
    while (data[i] != '\0')
    {
        if (data[i] <= '#')
            data[i] = '_';
        i++;
    }

}
*/
uint Tools::bits(ulong n) {
    uint b = 0;
    while (n) {
        b++;
        n >>= 1;
    }
    return b;
}
