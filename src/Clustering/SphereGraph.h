///////////////////////////////////////////////////////////////////////////////////////////////
//                                   BSD 2-Clause License                                    // 
///////////////////////////////////////////////////////////////////////////////////////////////
//                             Copyright (c) 2025, Sandia National Laboratories              // 
//                                                                                           // 
// Redistribution and use in source and binary forms, with or without modification, are      // 
// permitted provided that the following conditions are met:                                 // 
//                                                                                           // 
// 1. Redistributions of source code must retain the above copyright notice, this            // 
//    list of conditions and the following disclaimer.                                       // 
//                                                                                           // 
// 2. Redistributions in binary form must reproduce the above copyright notice,              // 
//    this list of conditions and the following disclaimer in the documentation              // 
//    and/or other materials provided with the distribution.                                 // 
//                                                                                           // 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"               // 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE                 // 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            // 
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE              // 
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL                // 
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR                // 
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER                // 
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,             // 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE             // 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      //
///////////////////////////////////////////////////////////////////////////////////////////////
//                                     Author                                                //
//                                Mohamed S. Ebeida                                          //
//                                msebeid@sandia.gov                                         //
///////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _VOROCLUST_SPHERE_GRAPH_H_
#define _VOROCLUST_SPHERE_GRAPH_H_

#include "Sphere.h"
#include "ClusteringCommon.h"
#include "Utils.h"
#include <stack>
#include <queue>

class SphereGraph
{
public:

    //default constructor is no-op. If used, need to also call initialize.
    SphereGraph();
    SphereGraph(size_t initial_size);
    ~SphereGraph();

    void initialize(size_t initial_size);
    void initialize(SphereGraph* input);
    void add_node(size_t node, size_t node_capacity);
    void connect_graph_nodes(size_t inode, size_t jnode);
    void connect_graph_nodes_directional(size_t inode, size_t jnode);
    bool is_connected(size_t inode, size_t jnode);
    void cluster_propagation(Sphere* interior_points, double detail_ceiling, double descent_limit);
    size_t get_capacity() const { return _graph_capacity;  }

    void set_active_clusters(size_t max_clusters);
    void set_active_clusters(double noise_threshold);
    bool is_cluster_active(size_t cluster_id);

    size_t* get_nodes_metadata(size_t metadata_index);

    //need to store some metadata for each node, in addition to their connections.
    //metadata stored before neighbors, so graph[node][metadata_size + i] gives the ith neighbor 
    static constexpr size_t metadata_size = 5;
    enum metadata { CAPACITY, NUM_NEIGHBORS, VISITED, ENABLED, CLUSTER_ID };

    bool is_initialized;
    size_t** graph;
    size_t num_nodes;

private:
    void copy_node(const size_t* input, size_t* output);
    void reset_visited();

    size_t _graph_capacity;
    size_t _num_clusters;
    size_t* _cluster_points;
    bool* _active_clusters;
};

#endif
