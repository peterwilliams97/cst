#ifndef _REPLACE_PATTERN_H_
#define _REPLACE_PATTERN_H_
#include <iostream>
#include "Tools.h"

class ReplacePattern {
private:
    ulong *answer;
    uint sampleRate;
    void createtable(uint);

public:
    ReplacePattern(uint, uint);
    ~ReplacePattern();
    ulong returnWord(ulong *, ulong, ulong);
    ulong *returnRP(ulong *, ulong, ulong, ulong);
};

#endif
