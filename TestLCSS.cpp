/***************************************************************************
 *   Copyright (C) 2006 by Veli M�kinen                                    *
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

// Longest Common SubString (A,B)
void lcss(int n_chars, int m_overlap) {
#if 0
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
    for (int i = 0; i < m_overlap; i++) {
        A[i] = quick_brown[i % n_quick_brown];
        B[i] = quick_brown[i % n_quick_brown];
    }
    A[m_overlap] = 'X';
    B[m_overlap] = 'Y';

    string s_text = string(A) + string("$") + string(B);
#else
    const char *A = "1234567890 madamimadam_abdefghi";
    string s_A = string(A);
    string s_B = string(s_A.rbegin(), s_A.rend());
    string s_text = s_A + string("$") + s_B;
    const char *B = s_B.c_str();
#endif

    uchar *text = (uchar *)s_text.c_str();

    // One could read from file as follows:
    // uchar *text = Tools::GetFileContents("filename", maxlen);

    ulong n = strlen((char *)text);
    n++; // this takes the end symbol 0u as part of the text

    ulong splitpos = strlen(A); // A is to the left, B is to the right

    cout << "\nCreating Sadakane's suffix tree...\n";
    SSTree *sst = new SSTree(text, n);
    // text could be deleted now, and recovered as follows:

    cout << "... for text A$B=";
    cout << (char *)sst->substring(0, n - 1) << '\n';

    // NOTE: to save some memory during construction, one can also
    // instruct the constructor to delete the text as soon as it is
    // no longer needed by calling it with an additional parameter:
    // SSTree *sst = new SSTree(text, n, true);

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

    cout << "\nComputing longest common substring of A and B...\n";

    // position of last leaf of root in BP
    ulong lastleaf = sst->rightmost(0);
    ulong i;
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

    for (i = 0; i <= lastleaf; i++) {
       left[i] = false;
       right[i] = false;
    }

    for (i = lastleaf; i > 0; i--) {
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
    for (i = 0; i <= lastleaf; i++) {
        if (sst->isOpen(i) && left[i] && right[i] && (sst->depth(i) > maxdepth)) {
            maxdepth = sst->depth(i);
            maxindex = i;
        }
    }

   // Let's find a text position containing the substring
   // ulong textpos = sst->textpos(sst->firstChild(maxindex));

   cout << "lcss(\nA='" << A << "',\nB='" << B << "')\n='";
   cout << (char *)sst->pathlabel(maxindex) << "'\n\n";

   delete sst;
   delete[] left;
   delete[] right;
}

int main() {
    lcss(1000, 50);
    return 0;
}
