#include <mpi.h>
#include <iostream>
#include <vector>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <algorithm>

using namespace std;

// Calculating the number of vertices and edges in the graph
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

// Calculating the number of colors used in the graph
int calculateColorCount(vector<int> colors)
{
    int count = 0;

    for (int c : colors)
    {
        count = (c >= count) ? c : count;
    }

    return count;
}

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
            

    // Creating a temporary graph to store the adjacency sets
    vector<unordered_set<int>> tempGraph(tempsize);

    while (file >> col1 >> col2 >> col3) {
        
        // For debugging purposes
        // cout << "Col1: " << col1 << ", Col2: " << col2 << ", Col3: " << col3 << std::endl;

        // Adding edge to adjacency set (to avoid duplicates)
        tempGraph[col1-1].insert(col2-1);
        tempGraph[col2-1].insert(col1-1); // Since the graph is undirected
    }

    file.close();

    // Converting the sets to adjacency lists used in the algorithm
    for (int i = 0; i < tempsize; ++i) {
        graph.push_back(vector<int>(tempGraph[i].begin(), tempGraph[i].end()));
        sort(graph[i].begin(), graph[i].end()); // Optional: To keep neighbors sorted

    }

}

// Assigning colors to vertices from the process rank's assigned range
void assignColors(int start, int end, const vector<vector<int>> &graph, vector<int> &colors, int &rank) {

    double startTime = MPI_Wtime();

    int V = graph.size(); // Number of vertices
    vector<bool> available(V, true); // Tracking available colors

    for (int v = start; v < end; v++) {
        // Reseting available colors for this vertex at each coloring step
        fill(available.begin(), available.end(), true);

        // Mark colors of adjacent vertices as unavailable
        for (int neighbor : graph[v]) {
            if (colors[neighbor] != -1) {
                available[colors[neighbor]] = false;
            }
        }
        // Assigning the smallest available color
        for (int c = 0; c < V; c++) {
            if (available[c]) {
                colors[v] = c;
                break;
            }
        }
    }
    double stopTime = MPI_Wtime();
    cout << "Rank " << rank << " colored vertices in " << stopTime - startTime << " seconds" << endl;
}

void ConflictCheck(const vector<vector<int>> &graph, vector<int> &finalColors, vector<int> &conflictVertices, bool &status) 
{
    int V = graph.size();
    for (int v = 0; v < V; v++) 
    {
            for (int neighbor : graph[v]) {
                if (finalColors[v] == finalColors[neighbor] && v > neighbor) {
                    status = true;
                    //For debugging purposes
                    // cout << "Conflict detected between vertices " << v << " and " << neighbor << endl;
                    conflictVertices.push_back(v);
                    break;
            }
        }
    }

    if (status == false) {
        cout << "No conflicts found!" << endl;
    }

}

void ConflictResolve(const vector<vector<int>> &graph, vector<int> &finalColors, vector<int> &conflictVertices, bool &status) 
{
    int V = graph.size();
    for (int v : conflictVertices) 
    {
        vector<bool> available(V, true);
        for (int neighbor : graph[v]) {
            if (finalColors[neighbor] != -1) {
                available[finalColors[neighbor]] = false;
            }
        }
        for (int c = 0; c < V; c++) {
            if (available[c]) {
                finalColors[v] = c;
                break;
            }
        }
    }
    
    // cout << "Final Colors after resolving conflicts:" << endl;
    // for (int v = 0; v < V; v++) {
    //     cout << "Vertex " << v << " -> Color " << finalColors[v] << endl;
    // }
}

int main(int argc, char **argv) {

    // Initializing MPI
    MPI_Init(&argc, &argv);

    // Getting the rank and size of the process
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Some examples of graphs to test the algorithm

    //Example 1
    // vector<vector<int>> graph;
    // convertMatrixToGraph("494_bus.txt", graph);
    
    //Example 2
    vector<vector<int>> graph;
    convertMatrixToGraph("1138_bus.txt", graph);

    //Example 3
    // vector<vector<int>> graph = {
    //     {1, 2, 3},      // Neighbors of Vertex 0
    //     {0, 4},         // Neighbors of Vertex 1
    //     {0, 4, 5},      // Neighbors of Vertex 2
    //     {0, 4},         // Neighbors of Vertex 3
    //     {1, 2, 3, 5},   // Neighbors of Vertex 4
    //     {2, 4}          // Neighbors of Vertex 5
    // };

    const int V = graph.size(); // Number of vertices

    vector<int> colors(V, -1); // Initialize all vertices as uncolored (-1 means uncolored)
    vector<int> finalColors(V, -1); // Store the final colors
    vector<int> conflictVertices; // Store the conflicting vertices

    // Partitioning the vertices among the processes
    int blockSize = V / size;
    int remainder = V % size;
    int start, end;

    if (rank < remainder) {
        start = rank * (blockSize + 1);
        end = start + blockSize + 1;
    } else {
        start = rank * blockSize + remainder;
        end = start + blockSize;
    }

    // Each process colors its assigned part
    assignColors(start, end, graph, colors, rank);

    // Each process prints its assigned part
    // cout << "Rank " << rank << " colored vertices: " << endl;
    // for (int v = start; v < end; v++) {
    //     cout << "Vertex " << v << " -> Color " << colors[v] << endl;
    // }

    // Rank 0 collects the results
    if (rank == 0) {
        copy(colors.begin(), colors.end(), finalColors.begin());

        for (int i = 1; i < size; i++) {
            int recvStart, recvEnd;

            if (i < remainder) {
                recvStart = i * (blockSize + 1);
                recvEnd = recvStart + blockSize + 1;
            } else {
                recvStart = i * blockSize + remainder;
                recvEnd = recvStart + blockSize;
            }

            MPI_Recv(finalColors.data() + recvStart, recvEnd - recvStart, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // cout << "\nFinal Colors (from rank 0):" << endl;
        // for (int v = 0; v < V; v++) {
        //     cout << "Vertex " << v << " -> Color " << finalColors[v] << endl;
        // }

    } else {
        // Send the local colors to rank 0
        MPI_Send(colors.data() + start, end - start, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Rank 0 solve the conflicts
    if (rank == 0) 
    {

        int V, E;
        calculateVertexEdgeCount(graph, V, E);

        std::cout << "G(V, E) " << std::endl;
        std::cout << "Number of vertices: " << V << std::endl;
        std::cout << "Number of edges: " << E << std::endl;
        std::cout << std::endl;

        bool status = false;

        double startTime = MPI_Wtime();

        ConflictCheck(graph, finalColors, conflictVertices, status);
        ConflictResolve(graph, finalColors, conflictVertices, status);

        double stopTime = MPI_Wtime();
        
        cout << "Conflict check and resolution took " << stopTime - startTime << " seconds" << endl;

        int colorCount = calculateColorCount(finalColors) + 1;

        cout << "The count of used colors is: " << colorCount << endl;

        // cout << "Vertex Colors:" << endl;
        // for (int v = 0; v < V; v++) {
        //     cout << "Vertex " << v << " --> " << finalColors[v] << endl;
        // }
    }
    
    // Finalizing MPI
    MPI_Finalize();
    return 0;
}
