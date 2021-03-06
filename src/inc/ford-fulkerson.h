// Copyright 2015 Peter

#ifndef FORD_FULKERSON_H
#define FORD_FULKERSON_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

#include "residual_edge.hpp"
#include "residual_network.h"

class FordFulkerson
{

  public:
    // List of ancestors for every vertex after the BFS
    std::vector<int> V;
    std::vector<ResidualEdge *> A;
    int VertexCount;
    int Source, Sink;

    int IterationsCount = 0;

    ResidualNetwork E;

    FordFulkerson(const ResidualNetwork &);
    ~FordFulkerson();

    void Run();

  private:
    /**
     * @brief Use BFS to find a path sink ~> source
     * @details The path is saved in the lsit of ancestors (this->V)
     *
     * @return True if a path exists, false if it does not
     */
    bool GetPath();

    /**
     * @brief Augment flow along a path sink ~> source
     * @details Use the list of ancestors to go along the path found by GetPath and augment it with the minimum flow possible
     */
    void AugmentPath();
};

#endif // FORD_FULKERSON_H
