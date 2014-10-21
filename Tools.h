/*
 * Collection of basic tools and defines
 */

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <iostream>
#include <fstream>
#include <ctime>
#include <climits>
#include <cstdlib>
#include <stdint.h>

#define KBYTE 1024
#define MBYTE (KBYTE * KBYTE)
#define GBYTE (KBYTE * MBYTE)

// DEBUG FLAGS !@#$
#define NDEBUG 1
//#define INDEXREPORT 1 
//#define DEBUG_CRMQ 1
//#define DEBUG_SUBBLOCK_RMQ 1
#define SSTREE_TIMER 1
//#define SSTREE_HEAPPROFILE 1

#ifdef WIN32
//  _WIN64 is for the windows build, __amd64__ is for gcc (linux/mac)
 #if defined(_WIN64) || defined(__amd64__)
  #define __WORDSIZE 64
 #else
  #define __WORDSIZE 32
  //#error "Want 64 bit"
 #endif
#endif

// Generates an error if __WORDSIZE is not defined
#ifndef __WORDSIZE
#error Missing definition of __WORDSIZE; Please define __WORDSIZE in Tools.h!
#endif

// Check word length on GNU C/C++:
// __WORDSIZE should be defined in <bits/wordsize.h>, which is #included from <limits.h>
#if __WORDSIZE == 64
 #define W 64
#else
 #define W 32
#endif

#define WW (W * 2)
#define Wminusone (W - 1)

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint
#define uint unsigned int
#endif
//#ifndef ulong

 #if __WORDSIZE == 64
//#define ulong uint64_t
  //#define ulong unsigned long long
typedef uint64_t ulong;
 #else
   //#error "Want 64 bit (2)"
  //#define ulong unsigned long
   typedef unsigned long ulong;
 #endif
//#endif

#ifdef _WIN32
double log2(double x);
#endif

class Tools
{
private:
    static double startTime;
public:
    static void StartTimer();
    static double GetTime();
    static uchar *GetRandomString(uint, uint, uint &);
    static void PrintBitSequence(ulong *, ulong);
    static uchar *GetFileContents(char *, ulong =0);
    static ulong ustrlen(uchar *);
    static char *uctoc(uchar *, bool = false);
    static uchar *ctouc(char *, bool = false);
    static void RemoveControlCharacters(uchar *);
    static ulong *bp2bitstream(uchar *);
    static uint FloorLog2(ulong);
    static uint CeilLog2(ulong);
    static uint bits (ulong);
    static uint *MakeTable();
    static uint FastFloorLog2(uint);

    static inline void SetField(ulong *A, uint len, ulong index, ulong x) {
        ulong i = index * len / W;
        ulong j = index * len - i * W;
        ulong mask = (j + len < W ? ~0lu << (j + len) : 0)
                     | (W - j < W ? ~0lu >> (W - j) : 0);
        A[i] = (A[i] & mask) | x << j;
        if (j + len > W) {
            mask = ((~0lu) << (len + j - W));
            A[i+1] = (A[i+1] & mask) | x >> (W - j);
        }
    }

    static inline ulong GetField(ulong *A, uint len, ulong index) {
        ulong i = index * len / W;
        ulong j = index * len - W * i;
        ulong result;

        if (j + len <= W) {
            result = (A[i] << (W - j - len)) >> (W - len);
        } else {
            result = A[i] >> j;
            result = result | (A[i+1] << (WW - j - len)) >> (W - len);
        }
        return result;
    }
    
    static inline ulong GetVariableField(ulong *A, uint len, ulong index)
    {
        ulong i = index / W;
        ulong j = index - W * i;
        ulong result;
       
        if (j + len <= W) {
            result = (A[i] << (W-j-len)) >> (W-len);
        } else {
             result = A[i] >> j;
             result = result | (A[i+1] << (WW-j-len)) >> (W-len);
        }
        return result;
    }

    static inline void SetVariableField(ulong *A, uint len, ulong index, ulong x) {
        ulong i = index / W;
        ulong j = index - i * W;
        ulong mask = (j + len < W ? ~0lu << (j + len) : 0)
                     | (W - j < W ? ~0lu >> (W - j) : 0);
        A[i] = (A[i] & mask) | x << j;
        if (j + len > W) {
            mask = ((~0lu) << (len + j - W));
            A[i + 1] = (A[i + 1] & mask)| x >> (W - j);
        }
    }
};

#endif
