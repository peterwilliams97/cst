
// Closed hash table

#include "Hash.h"

// creates a table to store up to n values with guaranteed load factor.
// vbits = # of bits per entry, ENTRIES CANNOT HAVE ZERO VALUE
Hash::Hash(ulong n, uint vbits, double factor) {
//   hash H = malloc (sizeof(struct shash));
//      int i,N;
//      if (n == 0) return NULL;
    ulong N = (ulong)(n * factor);
    if (N <= n) {
        N = n + 1;
    }
    _size = (1lu << Tools::bits(N-1)) - 1;
    _bits = vbits;
     //H->table = malloc ((((H->size+1)*vbits+W-1)/W)*sizeof(uint));
    _table = new ulong[((_size + 1) * vbits + W - 1) / W];
#ifdef INDEXREPORT
     printf ("     Also created hash table of %i bits\n",
            (((_size + 1) * vbits + W - 1) / W ) *W );
#endif
    for (ulong i = 0; i < (((_size + 1) * vbits + W - 1) / W); i++) {
      _table[i] = 0;
    }
}

// frees the structure
Hash::~Hash () {
    delete[] _table;
}

// inserts an entry, not prepared for overflow
void Hash::insertHash(ulong key, ulong value) {
    ulong pos = (key * PRIME1) & _size;
//     if (bitget(table, pos*bits, bits) != 0)
    if (Tools::GetVariableField(_table, _bits, pos * _bits) != 0) {
        do {
            pos = (pos + PRIME2) & _size;
        } while (Tools::GetVariableField(_table, _bits, pos * _bits));
    }
//      bitput(table, pos*bits, bits, value);
    Tools::SetVariableField(_table, _bits, pos * _bits, value);
}

// looks for a key, returns first value (zero => no values)
// writes in pos a handle to get next values
uint Hash::searchHash(ulong key, ulong *h) {
    *h = (key*PRIME1) & _size;
//      return bitget(H->table,*h*H->bits,H->bits);
    return Tools::GetVariableField(_table, _bits, *h * _bits);
}

// gets following values using handle *pos, which is rewritten
// returns next value (zero => no more values)
uint Hash::nextHash(ulong *h) {
    *h = (*h + PRIME2) & _size;
//      return bitget(table, *h*bits, bits);
    return Tools::GetVariableField(_table, _bits, *h * _bits);
}
