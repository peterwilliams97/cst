/***************************************************************************
 *   Copyright (C) 2006 by Niko Välimäki                                   *
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

#include "CHgtArray.h"

// Construct LCP-array (Space-efficient version of Kasai's algorithm)
CHgtArray::CHgtArray(CSA *csa, const uchar *text, ulong n) {

    _csa = csa;

    ulong i;
    _n = n;
    _HgtBits = new ulong[2 * n / W + 1];
    for (i = 0; i < 2 * n / W + 1; i++) {
        _HgtBits[i] = 0lu;
    }

    // Init Hgt array
    ulong lcp = 0, prev = 0, k = 0;
    for (i = 0; i < n; i++) {
        if (lcp > 0) {
            lcp--;
        }

        ulong j = csa->inverse(i);
        if (j == n - 1) {
            lcp = 0; // Hgt[n] = 0
        } else {
            j = csa->lookup(j + 1);
            while (text[i + lcp] == text[j + lcp] && text[i + lcp] != '\0')
                lcp++;
        }

        k += lcp - prev + 1;
        Tools::SetField(_HgtBits, 1, k, 1);
        k++;
        prev = lcp;
    }

    _Hgt = new BitRank(_HgtBits, 2 * n, true);
}

// Construct from a file
CHgtArray::CHgtArray(CSA *csa, const char *filename) {
    _csa = csa;
    std::ifstream file (filename, ios::in|ios::binary);
    if (file.is_open())
    {
        std::cout << "Loading HgtArray from file: " << filename << std::endl;
        file.read((char *)&_n, sizeof(ulong));
        _HgtBits = new ulong[2 * _n / W + 1];
        for (ulong offset = 0; offset < (2 * _n / W + 1); offset++)
            file.read((char *)(_HgtBits + offset), sizeof(ulong));
        file.close();
    }
    else
    {
        std::cout << "Unable to open file " << filename << std::endl;
        exit(1);
    }

    _Hgt = new BitRank(_HgtBits, 2 * _n, true);
}


void CHgtArray::SaveToFile(const char *filename) {

    std::ofstream file (filename, ios::out|ios::binary|ios::trunc);
    if (file.is_open())
    {
        std::cout << "Writing HgtArray to file: " << filename << std::endl;
        file.write((char *)&_n, sizeof(ulong));
        std::cout << "Writing HgtArray of " << (2 * _n / W + 1) << " words." << std::endl;
        for (ulong offset = 0; offset < (2 * _n / W + 1); offset ++)
            file.write((char *)(_HgtBits + offset), sizeof(ulong));
        file.close();
    }
    else
    {
        std::cout << "Unable to open file " << filename << std::endl;
        exit(1);
    }
}

CHgtArray::~CHgtArray() {
    delete _Hgt; // Deletes HgtBits array!
    _HgtBits = 0;
}

void CHgtArray::SetSA(CSA *csa) {
    _csa = csa;
}

ulong CHgtArray::GetPos(ulong i) const {
    if (i >= _n) {
        return 0;
    }

    ulong k = _csa->lookup(i) + 1;
    return _Hgt->select(k) - 2 * k + 1;
}
