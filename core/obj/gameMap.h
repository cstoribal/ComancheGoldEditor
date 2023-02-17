#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <vector>

class gameMap
{
public:
    gameMap();


    bool readFromCharVec(const std::vector<char> & dataIn);
};

#endif // GAMEMAP_H
