// Copyright 2015 Peter
#include <iostream>
#include <algorithm>
#include <vector>

#include "inc/io.h"
#include "inc/graph_generation.h"
#include "inc/ford-fulkerson.h"
#include "inc/relabel_to_front.h"
#include "stopwatch/stopwatch.h"
#include "inc/residual_network.h"

using namespace std;

int GetFlow(ResidualNetwork &B)
{
    int s = 0;
    for (int i = 0; i < B.getCount(); ++i) {
        s += B.getWeight(B.getSink(), i);
    }

    return s;
}

int main()
{
    Stopwatch S;
    S.set_mode(REAL_TIME);

    //
    // Initialization
    //
    //

    // int N = 500;
    // vector<vector<int>> raw = GraphGeneration::GenerateRandomGraph(N, 80, 1, 100000);
    // ResidualNetwork E(raw, 0, N - 1);
    //
    // IO::WriteGraph(E);

    ResidualNetwork E = IO::ReadGraph();

    //
    // RELABEL TO FRONT
    //
    S.start("RTF");
    RelabelToFront RTF(E);
    RTF.Run();
    S.stop("RTF");

    //
    // FORD FULKERSON
    //
    S.start("FF");
    FordFulkerson FF(E);
    FF.Run();
    S.stop("FF");

    //
    // RESULTS
    //
    cout << "\nFF Flow is  : " << GetFlow(FF.E);
    cout << "\nRTF Flow is : " << GetFlow(RTF.E);

    cout << "\n";
    cout << "\nFF Time is  : " << S.get_total_time("FF");
    cout << "\nRTF Time is : " << S.get_total_time("RTF");

    // for (auto i : raw) {
    //     cout << endl;
    //     for(auto j: i) {
    //         cout << j << " ";
    //     }
    // }

    // IO::printResidualNetwork(E, 3, "E");
    // IO::printResidualNetwork(FF.E, "FF", 3);
    // IO::printResidualNetwork(RTF.E, "RTF", 3);

    cout << endl;
    return 0;
}
