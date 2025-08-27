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

#ifndef _VOROCLUST_H_
#define _VOROCLUST_H_

#include "ClusteringRandomSampler.h"
#include "ClusteringTimer.h"
#include "ClusteringSmartTree.h"
#include "Configuration.h"
#include "SphereGraph.h"
#include "Sphere.h"
#include "ThreadPool.h"
#include "Utils.h"

class VoronoiClustering
{

public:
	VoronoiClustering(std::string input_filename, double radius, double detail_ceiling, double descent_limit, int num_threads, std::string tree_input_filename = "");
	VoronoiClustering(double* data, size_t data_size, size_t data_dimensions, double radius, double detail_ceiling, double descent_limit, int* data_labels, int num_threads, std::string tree_input_filename = "");

	~VoronoiClustering();

	void execute(int fixed_seed = -1);
	void label_by_max_clusters(size_t max_clusters);
	void label_noise(double noise_threshold);

	Sphere* get_spheres() { return _spheres; }
	size_t get_num_spheres() { return _num_spheres; }
	SphereGraph get_sphere_graph() { return _sphere_graph; }
	size_t* get_graph_metadata(size_t metadata_index) { return _sphere_graph.get_nodes_metadata(metadata_index); }

	void write_spheres_to_bin(std::string output_file);
	void load_spheres(std::string input_file);
	void write_data_tree_to_bin(std::string output_file);
	void write_labels(std::string output_folder, bool include_data = false);

private:

	void generate_sphere_cover(int* active_pool, size_t active_pool_size);
	void generate_sphere_cover_parallel(int* active_pool, size_t start_index);

	static bool is_valid_sphere(double* data, size_t* batch_indices, size_t num_points, Sphere* spheres, size_t num_spheres, double radius2, size_t data_dimensions, bool* results);
	size_t make_batch(int* active_pool, size_t start_index, size_t* batch_indices, size_t max_batch_size);
	void add_batch_to_spheres(size_t* batch_indices, bool* batch_validity, size_t batch_size);

	double distance_squared(double* point1, double* point2);
	void reset_spheres();
	void label_remaining(bool active_clusters_only);

	Configuration _cfg;
	std::string _input_filename;

	double* _data;
	size_t _data_size;
	size_t _data_dimensions;
	ClusteringSmartTree _data_tree;
	int* _data_labels;

	Sphere* _spheres;
	size_t _num_spheres;
	size_t _spheres_capacity;

	SphereGraph _sphere_graph;

	//this is redundant data, because the same coordinates can be retrieved using the _spheres data_index along with _data
	//However, keeping the coordinates in a separate array can give significant speedup during interior point counting, due to improved data locality
	double* _sphere_coordinates;

	//used to control deallocation of resources. 
	//If initialized through python interface, python handles both allocation AND deallocation of data arrays
	bool _external_allocation;
};

#endif
