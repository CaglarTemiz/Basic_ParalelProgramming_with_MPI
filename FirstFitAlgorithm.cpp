#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <algorithm>
using namespace std;


//File reader function to convert the matrix representation into adjacency graph
void convertMatrixToGraph(const string& filename, vector<vector<int>>& graph) {
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
    }

    int col1, col2, tempsize;
    double col3;

    if(filename == "494_bus.txt" ){
        tempsize = 494;
    }
    else if (filename == "1138_bus.txt")
    {
        tempsize = 1138;
    }

    vector<unordered_set<int>> tempGraph(tempsize);

    while (file >> col1 >> col2 >> col3) {
        
        //For debugging purposes
        //std::cout << "Col1: " << col1 << ", Col2: " << col2 << ", Col3: " << col3 << std::endl;

        // Add edge to adjacency set (to avoid duplicates)
        tempGraph[col1-1].insert(col2-1);
        tempGraph[col2-1].insert(col1-1); // Since the graph is undirected
    }

    file.close();

    // Converting the adjacency sets to adjacency lists
    for (int i = 0; i < tempsize; ++i) {
        graph.push_back(vector<int>(tempGraph[i].begin(), tempGraph[i].end()));
        sort(graph[i].begin(), graph[i].end()); // Optional: To keep neighbors sorted
    }

}

//Function to calculate the number of vertices and edges for information purposes
void calculateVertexEdgeCount(const vector<vector<int>>& graph, int& V, int& E){

    V = graph.size();
    E = 0;

    for(int v = 0; v < V; v++)
    {
        for (int e : graph[v])
        {
            E++;
        }
    }

    E /= 2;
}

//Function to calculate the number of colors used for information purposes
int calculateColorCount(vector<int> colors)
{
    int count = 0;

    for (int c : colors)
    {
        count = (c >= count) ? c : count;
    }

    return count;
}


//First Fit Algorithm for Graph Coloring
void FirstFitColoring(const vector<vector<int>>& graph, int V) {
    vector<int> colors(V, -1); // Storing colors for each vertex (-1 means uncolored)
    vector<bool> available(V, true); // Tracking availability of colors for neighbors

    auto start = std::chrono::high_resolution_clock::now();

    for (int v = 0; v < V; v++) {
        
        // Step 1: Mark colors used by neighbors as unavailable
        for (int adj : graph[v]) {
            if (colors[adj] != -1) { // If the neighbor is colored
                available[colors[adj]] = false;
            }
        }

        // Step 2: Assign the smallest available color
        for (int color = 0; color < V; color++) {
            if (available[color]) {
                colors[v] = color; // Assign the color
                break;
            }
        }

        // Step 3: Reset the availability array for the next vertex
        for (int adj : graph[v]) {
            if (colors[adj] != -1) {
                available[colors[adj]] = true; // Reset availability
            }
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();

    // Printing the duration of the algorithm and the number of colors used
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "The duration of vertex coloring is: " << duration.count() << " microseconds" << endl;
    std::cout << "The count of used colors is: " << calculateColorCount(colors)+1 << endl;
    std::cout << std::endl;


    // Printing the assigned colors to check the correctness of the algorithm

    // cout << "Vertex Colors:" << endl;
    // std::cout << std::endl;

    // for (int v = 0; v < V; v++) {
    //     cout << "Vertex " << v << " --> " << colors[v] << endl;
    // }
}

int main() {
    // Example graph: Adjacency list representation

    int V, E;

    // Some example graphs

    // Example 1
    // vector<vector<int>> graph = {
    //    {1, 2, 3},      // Vertex 0
    //    {0, 4},         // Vertex 1
    //    {0, 4, 5},      // Vertex 2
    //    {0, 4},         // Vertex 3
    //    {1, 2, 3, 5},   // Vertex 4
    //    {2, 4}          // Vertex 5
    // };
    
    // Example 2
    // vector<vector<int>> graph = {
    //     {1, 3},      // Vertex 0
    //     {0, 2, 3, 4}, // Vertex 1
    //     {1, 4},       // Vertex 2
    //     {0, 1, 5},    // Vertex 3
    //     {1, 2, 5},    // Vertex 4
    //     {3, 4}        // Vertex 5
    // };

    // Example 3
    //vector<vector<int>> graph = {
    //        {1, 2},      // Vertex 0
    //        {0, 3},      // Vertex 1
    //        {0, 3},      // Vertex 2
    //        {1, 2},   // Vertex 3
    //    };

    // Example 4
    // vector<vector<int>> graph;
    // convertMatrixToGraph("494_bus.txt", graph);

    vector<vector<int>> graph;
    convertMatrixToGraph("1138_bus.txt", graph);

    calculateVertexEdgeCount(graph, V, E);

    std::cout << "G(V, E) " << std::endl;
    std::cout << "Number of vertices: " << V << std::endl;
    std::cout << "Number of edges: " << E << std::endl;
    std::cout << std::endl;
    
    FirstFitColoring(graph, V);
    
    return 0;
}
