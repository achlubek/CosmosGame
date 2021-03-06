
#include "stdafx.h"
vector<string> splitByChar(string src, unsigned char splitter)
{
    vector<string> output = {};
    int i = 0, d = 0;
    while (i < src.size()) {
        if (src[i] == splitter) {
            output.push_back(src.substr(d, i - d));
            d = i;
            while (src[i++] == splitter)  d++;
        }
        else {
            i++;
        }
    }
    if (i == src.size() && d < i) {
        output.push_back(src.substr(d, i));
    }
    return output;
}