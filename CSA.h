/*
 * Compressed suffix array by Veli M�kinen,
 * additional changes by Niko V�lim�ki
 */

#ifndef _CSA_H_
#define _CSA_H_
#include <iostream>
#include <queue>
#include <iomanip>
#include "BitRank.h"
#include "wtreebwt.h"

class CSA {
private:
    class TCodeEntry {
    public:
        uint count;
        uint bits;
        uint code;
        TCodeEntry() {count = 0; bits = 0; code = 0u;};
    };

    class THuffAlphabetRank {
    // using fixed 0...255 alphabet
    private:
        BitRank *bitrank;
        THuffAlphabetRank *left;
        THuffAlphabetRank *right;
        TCodeEntry *codetable;
        uchar ch;
        bool leaf;
    public:
        THuffAlphabetRank(uchar *, ulong, TCodeEntry *, uint);
        ~THuffAlphabetRank();
        bool Test(uchar *, ulong);

        inline ulong rank(int c, ulong i) { // returns the number of characters c before and including position i
            THuffAlphabetRank *temp=this;
            if (codetable[c].count == 0) return 0;
            uint level = 0;
            uint code = codetable[c].code;
            while (!temp->leaf) {
                if ((code & (1u<<level)) == 0) {
                i = i-temp->bitrank->rank(i);
                    temp = temp->left;
                }
                else {
                    i = temp->bitrank->rank(i)-1;
                    temp = temp->right;
                }
               ++level;
            }
            return i+1;
        }

        inline bool IsCharAtPos(int c, ulong i) {
            THuffAlphabetRank *temp=this;
            if (codetable[c].count == 0) return false;
            uint level = 0;
            uint code = codetable[c].code;
            while (!temp->leaf) {
                if ((code & (1u<<level))==0) {
                    if (temp->bitrank->IsBitSet(i)) return false;
                    i = i-temp->bitrank->rank(i);
                    temp = temp->left;
                }
                else {
                    if (!temp->bitrank->IsBitSet(i)) return false;
                    i = temp->bitrank->rank(i)-1;
                    temp = temp->right;
                }
               ++level;
            }
            return true;
        }
        inline int charAtPos(ulong i) {
            THuffAlphabetRank *temp=this;
            while (!temp->leaf) {
                if (temp->bitrank->IsBitSet(i)) {
                i = temp->bitrank->rank(i)-1;
                temp = temp->right;
            }
            else {
                i = i-temp->bitrank->rank(i);
                    temp = temp->left;
            }
            }
            return (int)temp->ch;
        }
    };

    class node {
    private:
        uint weight;
        uchar value;
        node *child0;
        node *child1;

        void maketable( uint code, uint bits, TCodeEntry *codetable ) const;
        static void count_chars(uchar *, ulong , TCodeEntry *);
        static uint SetBit(uint , uint , uint );
    
    public:
        node( uchar c = 0, uint i = 0 ) {
            value = c;
            weight = i;
            child0 = 0;
            child1 = 0;
        }

        node( node* c0, node *c1 ) {
            value = 0;
            weight = c0->weight + c1->weight;
            child0 = c0;
            child1 = c1;
        }


        bool operator>( const node &a ) const {
            return weight > a.weight;
        }

        static TCodeEntry *makecodetable(uchar *, ulong);
    };

    static const uchar print = 1;
    static const uchar report = 1;
    ulong _n;
    uint samplerate;
    ulong C[256];
    ulong bwtEndPos;
    THuffAlphabetRank *alphabetrank;
    BitRank *sampled;
    ulong *suffixes;
    ulong *positions;
    TCodeEntry *codetable;

    // Private methods
    uchar *BWT(uchar *);
    uchar *LoadFromFile(const char *);
    void SaveToFile(const char *, uchar *);
    void maketables();

public:
    CSA(uchar *, ulong, uint, const char * = 0, const char * = 0);
    ~CSA();
    ulong Search(uchar *, ulong, ulong *, ulong *);
    ulong lookup(ulong);
    ulong inverse(ulong);
    ulong Psi(ulong);
    uchar * substring(ulong, ulong);
};

#endif
