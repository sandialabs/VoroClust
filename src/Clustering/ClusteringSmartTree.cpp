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
//  ClusteringSmartTree.cpp                                       Last modified (08/07/2024) //
///////////////////////////////////////////////////////////////////////////////////////////////
#endif // DOXYGEN_SHOULD_SKIP_THIS

#include "ClusteringSmartTree.h"

ClusteringSmartTree::ClusteringSmartTree()
{
	init_memory();
}

ClusteringSmartTree::ClusteringSmartTree(size_t num_dim)
{
	init_memory();
	reset_tree(num_dim);
}

bool ClusteringSmartTree::init_from_binary(std::string filename)
{
	std::ifstream input_stream(filename, std::ios::in | std::ios::binary);

	if (!input_stream.is_open()) {
		std::cerr << "ERROR: could not open ClusteringSmartTree file " << filename << std::endl;
		return false;
	}

	input_stream.read(reinterpret_cast<char*>(&_num_points), sizeof(size_t));
	input_stream.read(reinterpret_cast<char*>(&_num_dim), sizeof(size_t));
	input_stream.read(reinterpret_cast<char*>(&_num_features), sizeof(size_t));

	_points_cap = _num_points;
	_points = new double[_num_features * _num_points];
	_tree_left = new size_t[_num_points];
	_tree_right = new size_t[_num_points];
	_point_old_index = new size_t[_num_points];
	_point_new_index = new size_t[_num_points];

	input_stream.read(reinterpret_cast<char*>(&_tree_origin), sizeof(size_t));
	input_stream.read(reinterpret_cast<char*>(&_tree_height), sizeof(size_t));

	input_stream.read(reinterpret_cast<char*>(_points), _num_features * _num_points * sizeof(double));
	input_stream.read(reinterpret_cast<char*>(_tree_left), _num_points * sizeof(size_t));
	input_stream.read(reinterpret_cast<char*>(_tree_right), _num_points * sizeof(size_t));

	input_stream.read(reinterpret_cast<char*>(_point_old_index), _num_points * sizeof(size_t));
	input_stream.read(reinterpret_cast<char*>(_point_new_index), _num_points * sizeof(size_t));

	input_stream.close();

	return true;
}

ClusteringSmartTree::~ClusteringSmartTree()
{
	clear_memory();
}

void ClusteringSmartTree::write_tree_to_binary(std::string filename)
{
	std::ofstream output_stream(filename, std::ios::out | std::ios::binary | std::ios::trunc);

	if (!output_stream.is_open()) {
		std::cerr << "ERROR: could not open ClusteringSmartTree output file " << filename << std::endl;
		return;
	}

	output_stream.write(reinterpret_cast<const char*>(&_num_points), sizeof(size_t));
	output_stream.write(reinterpret_cast<const char*>(&_num_dim), sizeof(size_t));
	output_stream.write(reinterpret_cast<const char*>(&_num_features), sizeof(size_t));

	output_stream.write(reinterpret_cast<const char*>(&_tree_origin), sizeof(size_t));
	output_stream.write(reinterpret_cast<const char*>(&_tree_height), sizeof(size_t));

	output_stream.write(reinterpret_cast<const char*>(_points), _num_features * _num_points * sizeof(double));
	output_stream.write(reinterpret_cast<const char*>(_tree_left), _num_points * sizeof(size_t));
	output_stream.write(reinterpret_cast<const char*>(_tree_right), _num_points * sizeof(size_t));

	output_stream.write(reinterpret_cast<const char*>(_point_old_index), _num_points * sizeof(size_t));
	output_stream.write(reinterpret_cast<const char*>(_point_new_index), _num_points * sizeof(size_t));

	output_stream.close();
}

int ClusteringSmartTree::init_memory()
{
	_points_cap = 0; _num_points = 0;
	_num_dim = 0; _num_features = 0;
	_tree_origin = SIZE_MAX; _tree_height = 0;
	_points = 0; _tree_left = 0; _tree_right = 0;
	_point_old_index = 0; _point_new_index = 0;
	return 0;
}

int ClusteringSmartTree::clear_memory()
{
	#pragma region Clear Memory:
	if (_points != 0) delete[] _points;
	if (_tree_left != 0) delete[] _tree_left;
	if (_tree_right != 0) delete[] _tree_right;
	if (_point_old_index != 0) delete[] _point_old_index;
	if (_point_new_index != 0) delete[] _point_new_index;
	init_memory();
	return 0;
	#pragma endregion
}

int ClusteringSmartTree::reset_tree(size_t num_dim)
{
	#pragma region Reset Tree:
	clear_memory();

	_num_dim = num_dim;
	_num_features = num_dim;

	_points_cap = 100;
	_points = new double[_points_cap * _num_features];
	_tree_left = new size_t[_points_cap];
	_tree_right = new size_t[_points_cap];
	_point_new_index = new size_t[_points_cap];
	_point_old_index = new size_t[_points_cap];
	return 0;
	#pragma endregion
}

int ClusteringSmartTree::save_tree_csv(std::string file_name)
{
	#pragma region Save tree to CSV file:
	std::fstream file(file_name.c_str(), std::ios::out);
	size_t num_dim(_num_dim);

	file << "x1 coord";
	for (size_t idim = 1; idim < num_dim; idim++) file << ",x" << idim + 1 << "coord";
	file << std::endl;

	for (size_t i = 0; i < _num_points; i++)
	{
		size_t index = _point_new_index[i];
		file << std::setprecision(16) << _points[index * num_dim];
		for (size_t idim = 1; idim < num_dim; idim++) file << "," << _points[index * num_dim + idim];
		file << std::endl;
	}
	return 0;
	#pragma endregion
}


int ClusteringSmartTree::get_tree_point(size_t point_index, double* point)
{
	size_t point_new_index = _point_new_index[point_index];
	for (size_t ifeature = 0; ifeature < _num_features; ifeature++) point[ifeature] = _points[point_new_index * _num_features + ifeature];
	return 0;
}


int ClusteringSmartTree::set_points(size_t num_points, size_t num_dim, double* points)
{
	#pragma region Set Points:
	if (_points_cap > 0)
	{
		std::cout << "*** ClusteringSmartTree_Warning!! Improper use of set_points on populated tree. Clearing memory and continuing ***" << std::endl;
		clear_memory();
	}

	_points_cap = num_points;
	_num_points = num_points;
	_num_dim = num_dim;
	_num_features = num_dim;
	_points = new double[_points_cap * _num_features];
	_tree_left = new size_t[_points_cap];
	_tree_right = new size_t[_points_cap];
	_point_new_index = new size_t[_points_cap];
	_point_old_index = new size_t[_points_cap];

	for (size_t ipnt = 0; ipnt < num_points; ipnt++)
	{
		for (size_t ifeat = 0; ifeat < _num_features; ifeat++)
		{
			_points[ipnt * _num_features + ifeat] = points[ipnt * _num_features + ifeat];
		}
	}
	for (size_t iseed = 0; iseed < _num_points; iseed++)
	{
		_tree_left[iseed] = iseed; _tree_right[iseed] = iseed;
		_point_old_index[iseed] = iseed; _point_new_index[iseed] = iseed;
	}

	build_balanced_kd_tree();

	return 0;
	#pragma endregion
}


int ClusteringSmartTree::add_point(double* pnt, double balance_factor)
{
	#pragma region Add a Point:

	if (_num_points == _points_cap)
	{
		_points_cap *= 2;
		double* tmp_points = new double[_points_cap * _num_features];
		size_t* tmp_tree_left = new size_t[_points_cap];
		size_t* tmp_tree_right = new size_t[_points_cap];
		size_t* tmp_point_new_index = new size_t[_points_cap];
		size_t* tmp_point_old_index = new size_t[_points_cap];
		for (size_t ipnt = 0; ipnt < _num_points; ipnt++)
		{
			for (size_t ifeat = 0; ifeat < _num_features; ifeat++)
			{
				tmp_points[ipnt * _num_features + ifeat] = _points[ipnt * _num_features + ifeat];
			}
			tmp_tree_left[ipnt] = _tree_left[ipnt];
			tmp_tree_right[ipnt] = _tree_right[ipnt];
			tmp_point_new_index[ipnt] = _point_new_index[ipnt];
			tmp_point_old_index[ipnt] = _point_old_index[ipnt];
		}
		delete[] _points; _points = tmp_points;
		delete[] _tree_left; _tree_left = tmp_tree_left;
		delete[] _tree_right; _tree_right = tmp_tree_right;
		delete[] _point_new_index; _point_new_index = tmp_point_new_index;
		delete[] _point_old_index; _point_old_index = tmp_point_old_index;
	}

	if (_num_features == 0)
	{
		std::cout << "*** ClusteringSmartTree_ERORR!! Number of Smart Tree Features is zero!! ***" << std::endl;
		return 1;
	}

	for (size_t ifeat = 0; ifeat < _num_features; ifeat++)
	{
		_points[_num_points * _num_features + ifeat] = pnt[ifeat];
	}

	_tree_left[_num_points] = _num_points;
	_tree_right[_num_points] = _num_points;

	_point_new_index[_num_points] = _num_points;
	_point_old_index[_num_points] = _num_points;

	_num_points++;

	kd_tree_add_point(_num_points - 1);

	if (balance_factor > 0 && _tree_height > balance_factor * size_t(ceil(log2(_num_points + 1.0)))) 
		build_balanced_kd_tree();

	return 0;
	#pragma endregion
}

int ClusteringSmartTree::add_points(size_t num_points, double* pnts, double balance_factor)
{
	#pragma region Add Points:
	
	if (_num_features == 0)
	{
		std::cout << "*** ClusteringSmartTree_ERORR!! Number of Smart Tree Features is zero!! ***" << std::endl;
		return 1;
	}

	if (_num_points + num_points >= _points_cap)
	{
		while (_num_points + num_points >= _points_cap) _points_cap *= 2;

		double* tmp_points = new double[_points_cap * _num_features];
		size_t* tmp_tree_left = new size_t[_points_cap];
		size_t* tmp_tree_right = new size_t[_points_cap];
		size_t* tmp_point_new_index = new size_t[_points_cap];
		size_t* tmp_point_old_index = new size_t[_points_cap];
		for (size_t ipnt = 0; ipnt < _num_points; ipnt++)
		{
			for (size_t ifeat = 0; ifeat < _num_features; ifeat++)
			{
				tmp_points[ipnt * _num_features + ifeat] = _points[ipnt * _num_features + ifeat];
			}
			tmp_tree_left[ipnt] = _tree_left[ipnt];
			tmp_tree_right[ipnt] = _tree_right[ipnt];
			tmp_point_new_index[ipnt] = _point_new_index[ipnt];
			tmp_point_old_index[ipnt] = _point_old_index[ipnt];
		}

		delete[] _points; _points = tmp_points;
		delete[] _tree_left; _tree_left = tmp_tree_left;
		delete[] _tree_right; _tree_right = tmp_tree_right;
		delete[] _point_new_index; _point_new_index = tmp_point_new_index;
		delete[] _point_old_index; _point_old_index = tmp_point_old_index;
	}

	for (size_t ipnt = 0; ipnt < num_points; ipnt++)
	{
		for (size_t ifeat = 0; ifeat < _num_features; ifeat++)
		{
			_points[(_num_points + ipnt) * _num_features + ifeat] = pnts[ipnt * _num_features + ifeat];
		}

		_tree_left[_num_points + ipnt] = _num_points + ipnt;
		_tree_right[_num_points + ipnt] = _num_points + ipnt;

		_point_new_index[_num_points + ipnt] = _num_points + ipnt;
		_point_old_index[_num_points + ipnt] = _num_points + ipnt;
	}

	_num_points += num_points;
	for (size_t ipnt = 0; ipnt < num_points; ipnt++) kd_tree_add_point(_num_points - num_points + ipnt);

	if (balance_factor > 0 && _tree_height > balance_factor * size_t(ceil(log2(_num_points + 1.0))))
		build_balanced_kd_tree();
	return 0;
	#pragma endregion
}


int ClusteringSmartTree::build_balanced_kd_tree()
{
	#pragma region Build Balanced kd-tree:

	restore_original_order();

	size_t* tree_nodes_sorted = new size_t[_num_points];
	for (size_t i = 0; i < _num_points; i++) tree_nodes_sorted[i] = i;

	for (size_t iseed = 0; iseed < _num_points; iseed++)
	{
		_tree_left[iseed] = iseed; _tree_right[iseed] = iseed;		
	}
	_tree_origin = SIZE_MAX;
	_tree_height = 0;

	size_t target_pos = _num_points / 2;
	kd_tree_balance_quicksort(target_pos, 0, _num_points - 1, 0, tree_nodes_sorted);

	delete[] tree_nodes_sorted;

	re_enumerate_points_for_better_memory_access();

	return 0;
	#pragma endregion
}

size_t ClusteringSmartTree::get_tree_height()
{
	return _tree_height;
}

int ClusteringSmartTree::get_closest_tree_point(double* x, size_t& closest_tree_point, double& closest_distance)
{
	#pragma region Closest Neighbor Search using kd tree:
	closest_tree_point = SIZE_MAX;
	closest_distance = DBL_MAX;
	size_t num_nodes_visited = 0;
	if (_num_points == 0) return 1;
	kd_tree_get_closest_seed(x, 0, _tree_origin, closest_tree_point, closest_distance, num_nodes_visited);
	return 0;
	#pragma endregion
}

int ClusteringSmartTree::get_tree_points_in_sphere(double* x, double r, size_t& num_points_in_sphere, size_t*& points_in_sphere)
{
	#pragma region tree sphere neighbor search:
	num_points_in_sphere = 0;
	size_t capacity = 10;
	points_in_sphere = new size_t[capacity];
	kd_tree_get_seeds_in_sphere(x, r, 0, _tree_origin, num_points_in_sphere, points_in_sphere, capacity);
	if (num_points_in_sphere == 0)
	{
		delete[] points_in_sphere;
		points_in_sphere = 0;
	}
	return 0;
	#pragma endregion
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// private Methods
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ClusteringSmartTree::kd_tree_balance_quicksort(size_t target_pos, size_t left, size_t right, size_t active_dim, size_t* tree_nodes_sorted)
{
	#pragma region kd tree balance:
	kd_tree_quicksort_adjust_target_position(target_pos, left, right, active_dim, tree_nodes_sorted);

	// target position is correct .. add to tree
	kd_tree_add_point(tree_nodes_sorted[target_pos]);

	/* recursion */
	active_dim++;
	if (active_dim == _num_dim) active_dim = 0;

	if (target_pos + 1 < right)
	{
		kd_tree_balance_quicksort((target_pos + 1 + right) / 2, target_pos + 1, right, active_dim, tree_nodes_sorted);
	}
	else if (right > target_pos)
	{
		kd_tree_add_point(tree_nodes_sorted[right]);
	}

	if (target_pos > left + 1)
	{
		kd_tree_balance_quicksort((left + target_pos - 1) / 2, left, target_pos - 1, active_dim, tree_nodes_sorted);
	}
	else if (left < target_pos)
	{
		kd_tree_add_point(tree_nodes_sorted[left]);
	}
	return 0;
	#pragma endregion
}

int ClusteringSmartTree::kd_tree_quicksort_adjust_target_position(size_t target_pos, size_t left, size_t right, size_t active_dim, size_t* tree_nodes_sorted)
{
	#pragma region kd tree Quick sort pivot:
	size_t i = left, j = right;

	size_t pivot_seed = tree_nodes_sorted[(left + right) / 2];
	double pivot = _points[pivot_seed * _num_features + active_dim];

	/* partition */
	while (i <= j)
	{
		while (_points[tree_nodes_sorted[i] * _num_features + active_dim] < pivot)
			i++;
		while (_points[tree_nodes_sorted[j] * _num_features + active_dim] > pivot)
			j--;

		if (i <= j)
		{
			size_t tmp_index = tree_nodes_sorted[i];
			tree_nodes_sorted[i] = tree_nodes_sorted[j];
			tree_nodes_sorted[j] = tmp_index;

			i++;
			if (j > 0) j--;
		}
	};

	/* recursion */
	if (i < right && i <= target_pos && right >= target_pos)
		kd_tree_quicksort_adjust_target_position(target_pos, i, right, active_dim, tree_nodes_sorted);
	if (j > 0 && left < j && left <= target_pos && j >= target_pos)
		kd_tree_quicksort_adjust_target_position(target_pos, left, j, active_dim, tree_nodes_sorted);
	return 0;
	#pragma endregion
}

int ClusteringSmartTree::kd_tree_add_point(size_t seed_index)
{
	#pragma region kd tree add point:
	if (_tree_origin == SIZE_MAX)
	{
		_tree_origin = seed_index; _tree_height = 1;
		return 0;
	}

	// insert sphere into tree
	size_t parent_index(_tree_origin); size_t d_index(0);
	size_t branch_height(1);
	while (true)
	{
		if (_points[seed_index * _num_features + d_index] > _points[parent_index * _num_features + d_index])
		{
			if (_tree_right[parent_index] == parent_index)
			{
				_tree_right[parent_index] = seed_index;
				branch_height++;
				break;
			}
			else
			{
				parent_index = _tree_right[parent_index];
				branch_height++;
			}
		}
		else
		{
			if (_tree_left[parent_index] == parent_index)
			{
				_tree_left[parent_index] = seed_index;
				branch_height++;
				break;
			}
			else
			{
				parent_index = _tree_left[parent_index];
				branch_height++;
			}
		}
		d_index++;
		if (d_index == _num_dim) d_index = 0;
	}
	if (branch_height > _tree_height) _tree_height = branch_height;
	return 0;
	#pragma endregion
}

int ClusteringSmartTree::re_enumerate_points_for_better_memory_access()
{
	#pragma region Re-enumerate Points for better memory access:

	double* points_sorted = new double[_points_cap * _num_features];
	size_t num_traversed(0);
	kd_tree_get_nodes_order(_tree_origin, num_traversed, _point_old_index);

	for (size_t i = 0; i < _num_points; i++)
	{
		size_t current_point = _point_old_index[i];
		_point_new_index[current_point] = i;
	}

	// new tree containers with the current order
	size_t* tree_right_sorted = new size_t[_points_cap];
	size_t* tree_left_sorted = new size_t[_points_cap];
	for (size_t i = 0; i < _num_points; i++)
	{
		size_t current_point = _point_old_index[i];
		for (size_t ifeature = 0; ifeature < _num_features; ifeature++) points_sorted[i * _num_features + ifeature] = _points[current_point * _num_features + ifeature];
		tree_right_sorted[i] = _point_new_index[_tree_right[current_point]];
		tree_left_sorted[i] = _point_new_index[_tree_left[current_point]];
	}
	_tree_origin = _point_new_index[_tree_origin];
	delete[] _tree_right; _tree_right = tree_right_sorted; 
	delete[] _tree_left; _tree_left = tree_left_sorted;
	delete[] _points; _points = points_sorted;
	return 0;
	#pragma endregion
}

int ClusteringSmartTree::restore_original_order()
{
	#pragma region Restore original order
	double* old_points = new double[_points_cap * _num_features];
	for (size_t ipnt = 0; ipnt < _num_points; ipnt++)
	{
		size_t old_index = _point_old_index[ipnt];
		for (size_t ifeature = 0; ifeature < _num_features; ifeature++)
		{
			old_points[old_index * _num_features + ifeature] = _points[ipnt * _num_features + ifeature];
		}
	}
	for (size_t ipnt = 0; ipnt < _num_points; ipnt++)
	{
		_point_old_index[ipnt] = ipnt;
		_point_new_index[ipnt] = ipnt;
	}
	delete[] _points; _points = old_points;
	return 0;
	#pragma endregion
}

int ClusteringSmartTree::kd_tree_get_nodes_order(size_t node_index, size_t& num_traversed, size_t* ordered_indices)
{
	#pragma region Get tree nodes traverse order:
	ordered_indices[num_traversed] = node_index; num_traversed++;
	if (_tree_right[node_index] != node_index) kd_tree_get_nodes_order(_tree_right[node_index], num_traversed, ordered_indices);
	if (_tree_left[node_index] != node_index)  kd_tree_get_nodes_order(_tree_left[node_index], num_traversed, ordered_indices);
	return 0;
	#pragma endregion
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int ClusteringSmartTree::kd_tree_get_closest_seed(double* x, size_t d_index, size_t node_index,
	                                    size_t& closest_seed, double& closest_distance,
	                                    size_t& num_nodes_visited)
{
	#pragma region kd tree closest neighbor search:
	if (d_index == _num_dim) d_index = 0;

	double dst = sqrt(distance_squared(x, &_points[node_index * _num_features]));

	num_nodes_visited++;
	if (dst < closest_distance)
	{
		// add to neighbors
		closest_seed = _point_old_index[node_index];
		closest_distance = dst;
	}

	double neighbor_max = x[d_index] + closest_distance;
	if (_tree_right[node_index] != node_index && neighbor_max > _points[node_index * _num_features + d_index])
	{
		kd_tree_get_closest_seed(x, d_index + 1, _tree_right[node_index], closest_seed, closest_distance, num_nodes_visited);
	}

	double neighbor_min = x[d_index] - closest_distance;
	if (_tree_left[node_index] != node_index && neighbor_min < _points[node_index * _num_features + d_index])
	{
		kd_tree_get_closest_seed(x, d_index + 1, _tree_left[node_index], closest_seed, closest_distance, num_nodes_visited);
	}
	return 0;
	#pragma endregion
}

int ClusteringSmartTree::kd_tree_get_seeds_in_sphere(double* x, double r, size_t d_index, size_t node_index,
	                                      size_t& num_points_in_sphere, size_t*& points_in_sphere, size_t& capacity)
{
	#pragma region kd tree recursive sphere neighbor search:
	if (d_index == _num_dim) d_index = 0;

	double dst_sq = distance_squared(&_points[node_index * _num_features], x);

	if (dst_sq < r * r)
	{
		points_in_sphere[num_points_in_sphere] = _point_old_index[node_index];
		num_points_in_sphere++;

		if (num_points_in_sphere == capacity)
		{
			capacity *= 2;
			size_t* new_points = new size_t[capacity];
			for (size_t ipoint = 0; ipoint < num_points_in_sphere; ipoint++) new_points[ipoint] = points_in_sphere[ipoint];
			delete[] points_in_sphere;
			points_in_sphere = new_points;
		}
	}

	bool check_right(false), check_left(false);
	double neighbor_min(x[d_index] - r), neighbor_max(x[d_index] + r);

	if (_tree_right[node_index] != node_index && neighbor_max > _points[node_index * _num_features + d_index])
	{
		kd_tree_get_seeds_in_sphere(x, r, d_index + 1, _tree_right[node_index], num_points_in_sphere, points_in_sphere, capacity);
	}

	if (_tree_left[node_index] != node_index && neighbor_min < _points[node_index * _num_features + d_index])
	{
		kd_tree_get_seeds_in_sphere(x, r, d_index + 1, _tree_left[node_index], num_points_in_sphere, points_in_sphere, capacity);
	}
	return 0;
	#pragma endregion
}

double ClusteringSmartTree::distance_squared(double* point1, double* point2)
{
	double distance2 = 0;

	for (int j = 0; j < _num_dim; j++)
	{
		double dx = point1[j] - point2[j];
		distance2 += dx * dx;
	}

	return distance2;
}
