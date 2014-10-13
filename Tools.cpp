/*
 * Collection of basic tools and defines
 */

#include <stdio.h>
#include <string.h>
#include "Tools.h"

#ifdef _WIN32
double log2(double x) {
    return log(x) / log(2.0);
}
#endif

time_t Tools::startTime;

void Tools::StartTimer() {
    startTime = time(NULL);
}

double Tools::GetTime() {
    time_t stopTime = time(NULL);
    return difftime( stopTime, startTime );
}

uchar *Tools::GetRandomString(uint min, uint max, uint &alphabetSize) {
    uint len = std::rand() % (max - min) + min;
    alphabetSize = std::rand() % 26 + 1;
    uchar* temp = new uchar[len + 2];
    for (uint i = 0; i < len; i++) {
        temp[i] = 97 + std::rand() % alphabetSize;
    }
    temp[len] = 0u;
    temp[len+1] = '\0';
    return temp;
}

ulong *Tools::bp2bitstream(uchar *bp)
{
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

void Tools::PrintBitSequence(ulong *A, ulong len)
{
    for(ulong i = 0; i < len; i++) {
        if (GetField(A, 1, i))
            std::cout << "1";
        else
            std::cout << "0";
    }
    std::cout << "\n";
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
uint *Tools::MakeTable()
{
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

uint Tools::FastFloorLog2(uint i)
{
    uint *table = MakeTable(); uint u;
    if (i >> 24)    u = 22 + table[ i >> 24] ;
    if (i >> 16)    u = 14 + table[ i >> 16] ;
    if (i >> 8)     u = 6 + table[ i >> 8] ;
    u =  table[i] - 1;
    delete [] table;
    return u;
}

uint Tools::CeilLog2(ulong i)
{
    uint j = FloorLog2(i);
    if ((ulong)(1lu << j) != i)
        return j + 1;

    return j;
}

uchar *Tools::GetFileContents(char *filename, ulong maxSize)
{
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
uint Tools::bits (ulong n) {
    uint b = 0;
    while (n) {
        b++;
        n >>= 1;
    }
    return b;
}
