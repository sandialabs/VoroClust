#ifndef DOXYGEN_SHOULD_SKIP_THIS
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
//  ClusteringSmartTree.h                                         Last modified (08/07/2024) //
///////////////////////////////////////////////////////////////////////////////////////////////
#endif // DOXYGEN_SHOULD_SKIP_THIS

#ifndef _VOROCLUST_SMART_TREE_H_
#define _VOROCLUST_SMART_TREE_H_

#include "ClusteringCommon.h"

class ClusteringSmartTree
{

public:

	ClusteringSmartTree();

	ClusteringSmartTree(size_t num_dim);

	~ClusteringSmartTree();

	enum point_cloud_type{surface, curve, corners, no_narrow_region};

	int clear_memory();

	int reset_tree(size_t num_dim);

	int save_tree_csv(std::string file_name);

	size_t get_num_tree_points() { return _num_points; };

	size_t get_num_features() { return _num_features; };

	size_t get_num_dimensions() { return _num_dim; };

	int get_tree_point(size_t point_index, double* point);

	int set_points(size_t num_points, size_t num_dim, double* points);

	int add_point(double* pnt, double balance_factor = 1.5);

	int add_points(size_t num_points, double* pnts, double balance_factor = 1.5);

	int build_balanced_kd_tree();

	size_t get_tree_height();

	int get_closest_tree_point(double* x, size_t& closest_tree_point, double& closest_distance);

	int get_tree_points_in_sphere(double* x, double r, size_t& num_points_in_sphere, size_t*& points_in_sphere);

	void write_tree_to_binary(std::string filename);
	bool init_from_binary(std::string filename);
private:

	int init_memory();

	int kd_tree_balance_quicksort(size_t target_pos, size_t left, size_t right, size_t active_dim, size_t* tree_nodes_sorted);

	int kd_tree_quicksort_adjust_target_position(size_t target_pos, size_t left, size_t right, size_t active_dim, size_t* tree_nodes_sorted);

	int kd_tree_add_point(size_t seed_index);

	int re_enumerate_points_for_better_memory_access();
	
	int restore_original_order();

	int kd_tree_get_nodes_order(size_t node_index, size_t& num_traversed, size_t* ordered_indices);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int kd_tree_get_closest_seed(double* x, size_t d_index, size_t node_index,
		                         size_t& closest_seed, double& closest_distance,
		                         size_t& num_nodes_visited);

	int kd_tree_get_seeds_in_sphere(double* x, double r, size_t d_index, size_t node_index,
		                            size_t& num_points_in_sphere, size_t*& points_in_sphere, size_t& capacity);

	double distance_squared(double* point1, double* point2);
private:
	size_t _num_points;
	size_t _points_cap;
	size_t _num_dim;
	size_t _num_features;
	
	double* _points; 
	
	size_t  _tree_origin;
	size_t  _tree_height;
	size_t* _tree_right; 
	size_t* _tree_left;

	size_t* _point_old_index;
	size_t* _point_new_index;

	
};

#endif

