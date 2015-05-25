#ifndef VERTEX_H
#define VERTEX_H

#include <vector>
#include <utility>

#include "residual_edge.h"

// Vertex data structure used by RTF
struct Vertex
{
    int Height = 0;
    int ExcessFlow = 0;

    int RelabelCount = 0;
    int PushCount = 0;

    std::vector<ResidualEdge>::const_iterator NCurrent;
};

#endif // VERTEX_H
