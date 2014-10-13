/***************************************************************************
 *   Copyright (C) 2006 by Veli Mäkinen                                    *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/


#include <cstring>
#include <iostream>
#include "SSTree.h"
#include "Tools.h"
using namespace std;

static const char *
alphabet = "abcdefghijklmnopqrstuvxxyz ";
static const char *
quick_brown = "the quick brown fox jumped over the lazy dog ";

// Longest Common SubString (A, B)
void lcss(int n_chars, int n_overlap, int show_strings, double min_time) {

#if 1
    // A=aaabbbccc, B=ccbbbaa ==> lcss(A,B)=bbb
    //const char *A = "aaabbbccc", *B = "ccbbbaa";
    //uchar *text = (uchar*)"aaabbbccc$ccbbbaa";

    //const char *A = "12345", *B = "3456";
    //uchar *text = (uchar *)"12345$3456";
    char *A = (char *)calloc(n_chars + 1, 1);
    char *B = (char *)calloc(n_chars + 1, 1);
    int n_alphabet = strlen(alphabet);
    int n_quick_brown = strlen(quick_brown);
    for (int i = 0; i < n_chars; i++) {
        A[i] = alphabet[i % n_alphabet];
        B[i] = alphabet[i % n_alphabet];
        if (i % 10  == 0 && i > 1) {
            B[i] = B[i - 1];
        }
    }
    for (int i = n_chars / 2; i < n_overlap; i++) {
        A[i] = quick_brown[i % n_quick_brown];
        B[i] = quick_brown[i % n_quick_brown];
    }
    A[n_overlap] = 'X';
    B[n_overlap] = 'Y';

    string s_A = string(A);
    string s_B = string(B);

#else
    const char *A = "1234567890 madamimadam_abdefghi";
    string s_A = string(A);
    string s_B = string(s_A.rbegin(), s_A.rend());
    const char *B = s_B.c_str();
#endif

    string s_text = s_A + string("$") + s_B;

    uchar *text = (uchar *)s_text.c_str();

    // One could read from file as follows:
    // uchar *text = Tools::GetFileContents("filename", maxlen);

    ulong n = s_text.size();
    n++; // this takes the end symbol 0u as part of the text

    ulong splitpos = s_A.size(); // A is to the left, B is to the right

    if (show_strings >= 1) {
        cout << "\nCreating Sadakane's suffix tree n = " << n << "  ...\n";
    }

    Tools::StartTimer();
    double total_time = 0.0;
    int num_loops = 0;

    do {
#if 1    
        SSTree *sst = new SSTree(text, n);
        // text could be deleted now, and recovered as follows:
 #if 0
        cout << "... for text A$B=";
        uchar *sstring = sst->substring(0, n - 1);
        cout << (char *)sstring << '\n';
        free(sstring);
 #endif
#else
        // NOTE: to save some memory during construction, one can also
        // instruct the constructor to delete the text as soon as it is
        // no longer needed by calling it with an additional parameter:
        SSTree *sst = new SSTree(text, n, true);
#endif

        /****************************************************************
        * Suffix tree is now represented using several compressed       *
        * data structures. Most important to understand the usage       *
        * is the Balanced Parantheses (BP) representation of the tree   *
        * hierarchy: the tree is traversed prefix order printing "("    *
        * when a node is visited first time and printing ")" when       *
        * a node is visited last time. E.g. "((()())())" is a tree with *
        * root having two children, its left child having two leaves,   *
        * and its right child being a leaf.                             *
        *                                                               *
        * A NODE IN THE TREE IS REPRESENTED BY THE INDEX OF THE         *
        * CORRESPONDING "(" IN THE BP REPRESENTATION                    *
        *                                                               *
        * All the traversal operations operate on the indexes on BP.    *
        ****************************************************************/
        if (show_strings >= 1) {
            cout << "\nComputing longest common substring of A and B...\n";
        }

        // position of last leaf of root in BP
        ulong lastleaf = sst->rightmost(0);
    
        if (show_strings >= 1) {
            printf("lastleaf = %lu\n", lastleaf);
            printf("lastleaf/n = %.1f\n", (double)lastleaf / (double)n);
        }

        bool *left = new bool[lastleaf + 1];
        bool *right = new bool[lastleaf + 1];

        /*******************************************************************
        * BP representation makes it easy to allocate additional fields    *
        * associated with suffix tree nodes. In this example, we use       *
        * tables left and right that will be filled such that left[i]=true *
        * (right[i]=true) iff a suffix of A (of B) is located in the       *
        * subtree rooted at a node whose "(" is at BP[i].                  *
        *                                                                  *
        * NOTE: To minimize space-usage one should use tailor-made boolean *
        * storage; sizeof(bool)=byte, not 1 bit. File Tools.cpp provides   *
        * such. The following illustrates:                                 *
        *                                                                  *
        * ulong *left = new ulong[(lastleaf+1)/W +1]; // W=comp. word size *
        * SetField(left,1,i,0); // sets i-th 1-bit length field to 0       *
        * GetField(left,1,i); // should return 0 now                       *
        *******************************************************************/

        /*******************************************************************
        * The following makes a reverse prefix order traversal on the tree.*
        * This is accomplished just by scanning BP backwards.              *
        * This guarantees that once a node is reached all its children     *
        * have been visited, and its counters left and right updated       *
        * correctly.                                                       *
        *******************************************************************/

        for (ulong i = 0; i <= lastleaf; i++) {
           left[i] = false;
           right[i] = false;
        }

        for (ulong i = lastleaf; i > 0; i--) {
            if (sst->isleaf(i)) {
               if (sst->textpos(i) < splitpos) {
                   left[i] = true;
                   right[i] = false;
               } else {
                   left[i] = false;
                   right[i] = true;
               }
            }
            if (sst->isOpen(i)) {
               left[sst->parent(i)] |= left[i];
               right[sst->parent(i)] |= right[i];
            }
        }

        /*******************************************************************
        * Now node i has left[i] && right[i] iff label(root,i) is          *
        * substring of both A and B.                                       *
        * Let's find the longest such substring, i.e., deepest node        *
        * in the tree satisfying the condition.                            *
        *******************************************************************/

        ulong maxdepth=0, maxindex=0;
        for (ulong i = 0; i <= lastleaf; i++) {
            if (sst->isOpen(i) && left[i] && right[i] && sst->depth(i) > maxdepth) {
                maxdepth = sst->depth(i);
                maxindex = i;
            }
        }

        // Let's find a text position containing the substring
        // ulong textpos = sst->textpos(sst->firstChild(maxindex));
        if (show_strings >= 2) {
            cout << "lcss(\nA='" << A << "',\nB='" << B << "')\n='";
        }
        
        uchar *pathlabel = sst->pathlabel(maxindex);
        if (show_strings >= 1) {
            cout << (char *)pathlabel << "'\n\n";
        }
        free(pathlabel);

        delete sst;
        delete[] left;
        delete[] right;

        num_loops += 1;
    } while ((total_time = Tools::GetTime()) < min_time);

    double ave_time = total_time / (double)num_loops;
    printf("n=%d,n_chars=%d,n_overlap=%d,time=%.2f,chars/sec=%.1f\n", n, n_chars, n_overlap, 
            ave_time, (double)n / ave_time);
    fflush(stdout);
}

int main() {
    //_CrtSetBreakAlloc(950);
    int show_strings = 0;
    double min_time = 5.0;

    for (int num_passes = 0; num_passes < 10; num_passes++) {
        for (int i = 1; i < 10; i++) {
            lcss(i * 100 * 1000,    50, show_strings, min_time);
            lcss(i * 100 * 1000,  5000, show_strings, min_time);
            lcss(i * 100 * 1000, 50000, show_strings, min_time);
        }
        for (int i = 1; i < 10; i++) {
            lcss(i * 1000 * 1000, 50, show_strings, min_time);
        }
        for (int i = 1; i < 10; i += 2) {
            lcss(i * 10 * 1000 * 1000, 50, show_strings, min_time);
        }
        for (int i = 1; i < 10; i += 2) {
            lcss(i * 100 * 1000 * 1000, 50, show_strings, min_time);
        }
        for (int i = 1; i < 10; i += 2) {
            lcss(i * 1000 * 1000 * 1000, 50, show_strings, min_time);
        }

        lcss(1 * 1000 * 1000, 50000, show_strings, min_time);
        lcss(2 * 1000 * 1000, 50000, show_strings, min_time);
        lcss(5 * 1000 * 1000, 50000, show_strings, min_time);
        lcss(10 * 1000 * 1000, 1000 * 1000, show_strings, min_time);
        lcss(30 * 1000 * 1000, 1000 * 1000, show_strings, min_time);
        lcss(99 * 1000 * 1000, 1000 * 1000, show_strings, min_time);
    }
#ifdef WIN32
    /* Dump memory leaks if win32 debug build */
    _CrtDumpMemoryLeaks();
#endif
    return 0;
}
