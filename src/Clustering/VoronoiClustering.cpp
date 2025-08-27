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
#include "VoronoiClustering.h"

VoronoiClustering::VoronoiClustering(std::string input_filename, double radius, double detail_ceiling, double descent_limit, int num_threads, std::string tree_input_filename)
	:
	_cfg{
	/*radius          = */radius,
	/*radius2         = */radius * radius,
	/*detail_ceiling  = */detail_ceiling,
	/*descent_limit   = */descent_limit,
	/*max_clusters    = */0,
	/*noise_threshold = */0,
	/*use_data_tree   = */true,
	/*num_threads     = */num_threads
	},
	_input_filename(input_filename),
	_data(),
	_data_size(0),
	_data_dimensions(0),
	_data_tree(),
	_data_labels(),
	_spheres(),
	_num_spheres(0),
	_spheres_capacity(0),
	_sphere_graph(),
	_sphere_coordinates(),
	_external_allocation(false)
{
	ClusteringTimer timer;

#if defined USE_OPEN_MP
	if (_cfg.num_threads < 1)
	{
		_cfg.num_threads = omp_get_num_procs();
	}
#else
	if (_cfg.num_threads < 1)
	{
		_cfg.num_threads = 1;
	}
#endif

	bool is_csv = input_filename.find(".csv", input_filename.size() - 4) != std::string::npos;
	bool is_bin = input_filename.find(".bin", input_filename.size() - 4) != std::string::npos;

	if(is_csv)
	{
		//passing size/dimensions by reference because they might be changed if given data file does not match user inputs
		bool data_loaded = utils::load_csv(_input_filename, _data_size, _data_dimensions, _data);
		if (!data_loaded)
			return;
	}
	else if (is_bin)
	{
		bool data_loaded = utils::load_binary(_input_filename, &_data_size, &_data_dimensions, _data);
		if (!data_loaded)
			return;
	}
	else
	{
		std::cout << "ERROR: failed to initialize VoronoiClustering. Input file " << input_filename << " must be .csv or .bin." << std::endl;
		return;
	}

	std::cout << "data loaded from file in " << timer.report_timing() << " seconds" << std::endl;
	timer.reset_timer();

	if (_data_dimensions > 100)
	{
		_cfg.use_data_tree = false;
	}

	if (_cfg.use_data_tree)
	{
		if (!tree_input_filename.empty())
		{
			_data_tree.init_from_binary(tree_input_filename);
			std::cout << "k-d tree loaded from file in " << timer.report_timing() << " seconds" << std::endl << std::endl;
		}
		else
		{
			_data_tree.set_points(_data_size, _data_dimensions, _data);
			std::cout << "k-d tree constructed in " << timer.report_timing() << " seconds" << std::endl << std::endl;
		}
	}
	
	_data_labels = new int[_data_size]();
}

VoronoiClustering::VoronoiClustering(double* data, size_t data_size, size_t data_dimensions, double radius, double detail_ceiling, double descent_limit, int* data_labels, int num_threads, std::string tree_input_filename)
	:
	_cfg{ 
	/*radius          = */radius,
	/*radius2         = */radius * radius,
	/*detail_ceiling  = */detail_ceiling,
	/*descent_limit   = */descent_limit,
	/*max_clusters    = */0,
	/*noise_threshold = */0,
	/*use_data_tree   = */true,
	/*num_threads     = */num_threads
	},
	_input_filename(""),
	_data(data),
	_data_size(data_size),
	_data_dimensions(data_dimensions),
	_data_tree(),
	_data_labels(data_labels),
	_spheres(),
	_num_spheres(0),
	_spheres_capacity(0),
	_sphere_graph(),
	_sphere_coordinates(),
	_external_allocation(true)
{
	ClusteringTimer timer;

#if defined USE_OPEN_MP
	if (_cfg.num_threads < 1)
	{
		_cfg.num_threads = omp_get_num_procs();
	}
#else
	if (_cfg.num_threads < 1)
	{
		_cfg.num_threads = 1;
	}
#endif

	if (_data_dimensions > 100)
	{
		_cfg.use_data_tree = false;
	}

	if (_cfg.use_data_tree)
	{
		if (!tree_input_filename.empty())
		{
			bool success = _data_tree.init_from_binary(tree_input_filename);
			if (success)
			{
				std::cout << "k-d tree loaded from file in " << timer.report_timing() << " seconds" << std::endl << std::endl;
			}
			else
			{
				_data_tree.set_points(_data_size, _data_dimensions, _data);
				std::cout << "k-d tree constructed in " << timer.report_timing() << " seconds" << std::endl << std::endl;
			}

		}
		else
		{
			_data_tree.set_points(_data_size, _data_dimensions, _data);
			std::cout << "k-d tree constructed in " << timer.report_timing() << " seconds" << std::endl << std::endl;
		}
	}
}

VoronoiClustering::~VoronoiClustering()
{
	//no need to delete if we never allocated anything
	if (_data_size == 0)
	{
		return;
	}

	if (!_external_allocation)
	{
		delete[] _data;
		delete[] _data_labels;
	}

	reset_spheres();
}

void VoronoiClustering::reset_spheres()
{
	//if spheres were never created, no need to delete
	if (_num_spheres == 0)
	{
		return;
	}

	for (int i = 0; i < _num_spheres; i++)
	{
		delete[] _spheres[i].indices;
	}
	delete[] _spheres;
	delete[] _sphere_coordinates;
	_num_spheres = 0;
}

void VoronoiClustering::execute(int fixed_seed)
{

	if (_data_size == 0)
	{
		std::cout << "ERROR: VoronoiClustering::execute failed, data size is 0." << std::endl;
		return;
	}

	ClusteringTimer total_time;
	ClusteringTimer timer;

	if (_num_spheres == 0)
	{
		//Starts off holding the indices of the data 
		int* active_pool = new int[_data_size];
#pragma omp parallel for num_threads(_cfg.num_threads)
		for (int i = 0; i < _data_size; i++)
		{
			active_pool[i] = i;
		}
		size_t active_size = _data_size;
		unsigned long seed = (unsigned long)time(0);
		if (fixed_seed > 0)
		{
			seed = fixed_seed;
			std::cout << "(fixed) random seed " << seed << std::endl;
		}
		else
		{
			std::cout << "random seed " << seed << std::endl;
		}

		ClusteringRandomSampler rsampler((int)seed);

		// shuffle data points
		for (size_t i = 0; i < _data_size; i++)
		{
			size_t j = size_t(rsampler.generate_uniform_random_number() * _data_size);
			if (j == _data_size) j--;
			size_t tmp = active_pool[i];
			active_pool[i] = active_pool[j];
			active_pool[j] = tmp;
		}
		//std::shuffle(&active_pool[0], &active_pool[_data_size - 1], std::default_random_engine(seed));

		std::cout << "initialize active pool " << timer.report_timing() << " seconds" << std::endl;
		timer.reset_timer();

		reset_spheres();
		_spheres_capacity = 100;
		_spheres = new Sphere[_spheres_capacity];
		if (!_cfg.use_data_tree)
		{
			_sphere_coordinates = new double[_spheres_capacity * _data_dimensions];
		}
		//Populate a number of spheres with serial looping (probably faster than parallel with low numbers of spheres)
		size_t initial_run_size = 100 < _data_size ? 100 : _data_size;

		//do everythin serial if we don't have multiple threads
		if (_cfg.num_threads < 2)
		{
			initial_run_size = _data_size;
		}
		generate_sphere_cover(active_pool, initial_run_size);

		if (initial_run_size < _data_size)
		{
			//starting from where the serial version finished, find the rest of the spheres in parallel
			generate_sphere_cover_parallel(active_pool, initial_run_size);
		}
		delete[] active_pool;

		std::cout << _num_spheres << " spheres selected in " << timer.report_timing() << " seconds " << std::endl;
		timer.reset_timer();

		if (_cfg.use_data_tree)
		{
			//for each point, find all the spheres within radius, and count the interior points for each of those spheres
#pragma omp parallel for num_threads(_cfg.num_threads)
			for (int i = 0; i < _num_spheres; i++)
			{
				//will handle appropriate allocation for _interior_point_indices[i]\A0
				_data_tree.get_tree_points_in_sphere(&_data[_spheres[i].data_index * _data_dimensions], _cfg.radius, _spheres[i].count, _spheres[i].indices);
			}

			std::cout << "interior points counted with data tree in " << timer.report_timing() << " seconds" << std::endl;
			timer.reset_timer();
		}
		else
		{
			size_t* interior_indices_capacity = new size_t[_num_spheres];
			for (int i = 0; i < _num_spheres; i++)
			{
				_spheres[i].count = 0;
				interior_indices_capacity[i] = 100;
				_spheres[i].indices = new size_t[100];
			}

#pragma omp parallel for num_threads(_cfg.num_threads)
			for (int i = 0; i < _data_size; i++)
			{
				for (int j = 0; j < _num_spheres; j++)
				{
					double dist2 = distance_squared(&_data[i * _data_dimensions], &_sphere_coordinates[j * _data_dimensions]);

					if (dist2 < _cfg.radius2)
					{
						#pragma omp critical
						{
							if (_spheres[j].count == interior_indices_capacity[j])
							{
								interior_indices_capacity[j]=utils::resize_array<size_t>(_spheres[j].indices, 1, interior_indices_capacity[j], 2 * interior_indices_capacity[j]);
							}
							_spheres[j].indices[_spheres[j].count] = i;
							_spheres[j].count++;
						}
					}
				}
			}
			delete[] interior_indices_capacity;

			std::cout << "interior points counted in " << timer.report_timing() << " seconds" << std::endl;
			timer.reset_timer();
		}

		//sort interior points based on count
		std::sort(_spheres, _spheres + _num_spheres, [](const Sphere sphere1, const Sphere sphere2)
			{
				return sphere1.count > sphere2.count;
			});

		std::cout << "interior points sorted in " << timer.report_timing() << " seconds" << std::endl;
		timer.reset_timer();
	}
	else
	{
		std::cout << _num_spheres << " spheres loaded from file, so skipping selection..." << std::endl;
	}

	_sphere_graph.initialize(_num_spheres);

	for (int i = 0; i < _num_spheres; i++)
	{
		//node key in the graph is the index of that point within the full dataset 
		_sphere_graph.add_node(i, 10);
	}

	//OMP NOTE: connect needs to do memory allocation
	for (int i = 0; i < _num_spheres; i++)
	{
		//any neighbors earlier than i in the the list will have already created an edge between them
		//so only need to consider spheres after i
		for (int j = i+1; j < _num_spheres; j++)
		{
			double dist2 = distance_squared(&_data[_spheres[i].data_index * _data_dimensions], &_data[_spheres[j].data_index * _data_dimensions]);
			if (dist2 < 4 * _cfg.radius2)
			{
				_sphere_graph.connect_graph_nodes(i, j);
			}
		}
	}

	std::cout << "graph generated in " << timer.report_timing() << " seconds" << std::endl;
	timer.reset_timer();

	_sphere_graph.cluster_propagation(_spheres, _cfg.detail_ceiling, _cfg.descent_limit);

	std::cout << "clustering in " << timer.report_timing() << " seconds" << std::endl;
	timer.reset_timer();

	//all clusters are active, and border spheres are not included in assignment
	label_by_max_clusters(0);

	std::cout << "data labeled in " << timer.report_timing() << " seconds" << std::endl;

	std::cout << "total time to execute: " << total_time.report_timing() << " seconds" << std::endl;
}

void VoronoiClustering::generate_sphere_cover(int * active_pool, size_t active_pool_size)
{
	for (int i = 0; i < active_pool_size; i++)
	{
		int data_index = active_pool[i];
		bool inside_sphere = false;
		for (int j = 0; j < _num_spheres; j++)
		{
			double dist2 = distance_squared(&_data[_spheres[j].data_index * _data_dimensions], &_data[data_index * _data_dimensions]);
			if (dist2 < _cfg.radius2)
			{
				inside_sphere = true;
				break;
			}
		}

		if (!inside_sphere)
		{
			if (_num_spheres == _spheres_capacity)
			{
				size_t initial_capacity = _spheres_capacity;
				_spheres_capacity = utils::resize_array<Sphere>(_spheres, 1, _spheres_capacity, 2 * _spheres_capacity);
				if(!_cfg.use_data_tree)
					initial_capacity = utils::resize_array<double>(_sphere_coordinates, _data_dimensions, initial_capacity, 2 * initial_capacity);
			}

			if(!_cfg.use_data_tree)
				std::copy(_data + data_index * _data_dimensions, _data + data_index * _data_dimensions + _data_dimensions, _sphere_coordinates + _num_spheres * _data_dimensions);

			_spheres[_num_spheres].data_index = data_index;
			_spheres[_num_spheres].sphere_index = _num_spheres;
			_num_spheres++;
		}
	}
}

void VoronoiClustering::generate_sphere_cover_parallel(int* active_pool, size_t start_index)
{
	int num_worker_threads = _cfg.num_threads - 1;
	int points_per_thread = 100;

	bool* batch_validity = new bool[num_worker_threads * points_per_thread];
	size_t* batch_indices = new size_t[num_worker_threads * points_per_thread];
	size_t* prev_batch_indices = new size_t[num_worker_threads * points_per_thread];
	size_t* temp_batch_indices = new size_t[num_worker_threads * points_per_thread];
	size_t batch_size = 0;
	size_t prev_batch_size = 0;

	batch_size = make_batch(active_pool, start_index, batch_indices, num_worker_threads * points_per_thread);

	ThreadPool pool(num_worker_threads);
	pool.start();

	//final index in the current batch
	while (batch_size > 0)
	{
		//can't pass batch_indices directly, because it creates a race condition between main thread updating the indices for next batch, and child threads using indices
		std::copy(batch_indices, batch_indices + num_worker_threads * points_per_thread, temp_batch_indices);
		for (int i = 0; i < batch_size; i += points_per_thread)
		{
			int npoints = points_per_thread;
			if (i + npoints > batch_size)
			{
				npoints = batch_size - i;
			}
			std::function<void()> job = [this, i, temp_batch_indices, npoints, batch_validity]() {
				return VoronoiClustering::is_valid_sphere(_data, &temp_batch_indices[i], npoints, _spheres, _num_spheres, _cfg.radius2, _data_dimensions, &batch_validity[i]);
			};
			pool.queue_job(job);		
		}

		//while the above child threads are running, use the main thread to prepare next batch of points
		std::copy(batch_indices, batch_indices + num_worker_threads * points_per_thread, prev_batch_indices);
		prev_batch_size = batch_size;
		//begin with the next data point after the previous batch
		start_index = start_index + prev_batch_size;
		batch_size = make_batch(active_pool, start_index, batch_indices, num_worker_threads * points_per_thread);

		//wait until all threads have finished
		while(pool.busy()){}

		//unfortuntely, can't do this computation in main thread until async threads return. Order matters
		//As an example, say we eliminate batch point 5 because it is inside batch point 3. 
		//However, it might turn out that point 3 was eliminated by the async check, and therefore point 5 was actually a valid sphere
		//Need to wait for the async thread results to avoid this edge case.
		add_batch_to_spheres(prev_batch_indices, batch_validity, prev_batch_size);
	}
	delete[] batch_indices;
	delete[] prev_batch_indices;
	delete[] temp_batch_indices;
	delete[] batch_validity;
}

size_t VoronoiClustering::make_batch(int* active_pool, size_t start_index, size_t* batch_indices, size_t max_batch_size)
{
	size_t batch_size = 0;
	for (int i = start_index; i < _data_size; i++)
	{
		batch_indices[batch_size] = active_pool[i];
		batch_size++;

		//found enough points to fill the batch
		if (batch_size == max_batch_size)
		{
			break;
		}
	}

	return batch_size;
}

bool VoronoiClustering::is_valid_sphere(double* data, size_t* batch_indices, size_t num_points, Sphere* spheres, size_t num_spheres, double radius2, size_t data_dimensions, bool* results)
{
	double distance2;
	double dx;
	for (int i = 0; i < num_points; i++)
	{
		results[i] = true;
		for (int j = 0; j < num_spheres; j++)
		{
			distance2 = 0;
			for (int k = 0; k < data_dimensions; k++)
			{
				double dx = data[batch_indices[i] * data_dimensions + k] - data[spheres[j].data_index * data_dimensions + k];
				distance2 += dx * dx;
			}
			if (distance2 < radius2)
			{
				results[i] = false;
				break;
			}
		}
	}

	return true;
}

void VoronoiClustering::add_batch_to_spheres(size_t* batch_indices, bool* batch_validity, size_t batch_size)
{
	for (int i = 0; i < batch_size; i++)
	{
		if (!batch_validity[i])
		{
			continue;
		}

		for (int j = 0; j < i; j++)
		{
			if (!batch_validity[j])
			{
				continue;
			}

			double distance2 = 0;
			for (int k = 0; k < _data_dimensions; k++)
			{
				double dx = _data[batch_indices[i] * _data_dimensions + k] - _data[batch_indices[j] * _data_dimensions + k];
				distance2 += dx * dx;
			}

			if (distance2 < _cfg.radius2)
			{
				batch_validity[i] = false;
				break;
			}
		}

		if (batch_validity[i])
		{
			if (_num_spheres == _spheres_capacity)
			{
				size_t initial_capacity = _spheres_capacity;
				_spheres_capacity = utils::resize_array<Sphere>(_spheres, 1, _spheres_capacity, 2 * _spheres_capacity);

				if(!_cfg.use_data_tree)
					_spheres_capacity = utils::resize_array<double>(_sphere_coordinates, _data_dimensions, initial_capacity, 2 * initial_capacity);
			}

			if(!_cfg.use_data_tree)
				std::copy(_data + batch_indices[i] * _data_dimensions, _data + batch_indices[i] * _data_dimensions + _data_dimensions, _sphere_coordinates + _num_spheres * _data_dimensions);

			_spheres[_num_spheres].data_index = batch_indices[i];
			_spheres[_num_spheres].sphere_index = _num_spheres;

			_num_spheres++;
		}
	}
}

double VoronoiClustering::distance_squared(double* point1, double* point2)
{
	double distance2 = 0;

//bool use_omp = false;
//#if defined USE_OPEN_MP
//	use_omp = _data_dimensions > 3 * omp_get_num_procs();
//#endif

#pragma omp simd reduction(+:distance2)
	for (int j = 0; j < _data_dimensions; j++)
	{
		double dx = point1[j] - point2[j];
		distance2 += dx * dx;
	}
	
	//omp parallel for seems to be slow, even with 200+ dimensions, so disabling it for now. 
	//reduction is necessary for consistent results. 
//#pragma omp parallel for num_threads(_cfg.num_threads) reduction(+:distance2)
//	for (int j = 0; j < _data_dimensions; j++)
//	{
//		double dx = point1[j] - point2[j];
//		distance2 += dx * dx;
//	}
	
	return distance2;
}

void VoronoiClustering::label_by_max_clusters(size_t max_clusters)
{
	_sphere_graph.set_active_clusters(max_clusters);

	std::fill(_data_labels, _data_labels + _data_size, -2);

#pragma omp parallel for num_threads(_cfg.num_threads)
	for (int i = 0; i < _num_spheres; i++)
	{
		//only considering enabled (non-border) spheres inside active clusters.
		if (!_sphere_graph.graph[i][SphereGraph::ENABLED] ||
			!_sphere_graph.is_cluster_active(_sphere_graph.graph[i][SphereGraph::CLUSTER_ID]))
		{
			continue;
		}
		//loop through the interior points for each sphere, and label them based on the sphere's cluster id
		for (int j = 0; j < _spheres[i].count; j++)
		{
			_data_labels[_spheres[i].indices[j]] = _sphere_graph.graph[i][SphereGraph::CLUSTER_ID];
		}
	}

	//all remaining unlabeled points (inactive cluster and border spheres) will be assigned the same as the nearest active sphere
	label_remaining(true);
}

void VoronoiClustering::label_noise(double noise_threshold)
{
	_sphere_graph.set_active_clusters(noise_threshold);

	std::fill(_data_labels, _data_labels + _data_size, -2);

#pragma omp parallel for num_threads(_cfg.num_threads)
	for (int i = 0; i < _num_spheres; i++)
	{
		//only considering enabled (non-border) spheres
		if (!_sphere_graph.graph[i][SphereGraph::ENABLED])
		{
			continue;
		}

		//for active clusters label interior points based on cluster id
		if (_sphere_graph.is_cluster_active(_sphere_graph.graph[i][SphereGraph::CLUSTER_ID]))
		{
			for (int j = 0; j < _spheres[i].count; j++)
			{
				_data_labels[_spheres[i].indices[j]] = _sphere_graph.graph[i][SphereGraph::CLUSTER_ID];
			}
		}
		//for inactive clusters, label interior points as noise (-1)
		else
		{
			for (int j = 0; j < _spheres[i].count; j++)
			{
				_data_labels[_spheres[i].indices[j]] = -1;
			}
		}
	}

	//only remaining unlabeled points are on the borders. These will be assigned the same as the nearest non-border sphere 
	label_remaining(false);
}

void VoronoiClustering::label_remaining(bool active_clusters_only)
{
	ClusteringSmartTree node_tree(_data_dimensions);
	size_t* tree_id_map = new size_t[_num_spheres];
	size_t tree_count = 0;

	//collect all non-border spheres that are in an active cluster
	for (int i = 0; i < _num_spheres; i++)
	{
		//want to either get all non-border spheres in active clusters,
		//or all non-border spheres, period.
		if (_sphere_graph.graph[i][SphereGraph::ENABLED]
			&& (!active_clusters_only || _sphere_graph.is_cluster_active(_sphere_graph.graph[i][SphereGraph::CLUSTER_ID])))
		{
			node_tree.add_point(&_data[_spheres[i].data_index * _data_dimensions], -1);
			//tree only contains enabled nodes, so need to map each of it's ids to the nodes in the FULL graph
			tree_id_map[tree_count] = i;
			tree_count++;
		}
		else
		{
			tree_id_map[tree_count] = SIZE_MAX;
		}
	}
	node_tree.build_balanced_kd_tree();

	//assign all unlabeled points to the nearest sphere in the above tree
#pragma omp parallel for num_threads(_cfg.num_threads)
	for (int i = 0; i < _data_size; i++)
	{
		if (_data_labels[i] != -2)
		{
			continue;
		}

		size_t closest_tree_point;
		double closest_distance;

		node_tree.get_closest_tree_point(&_data[i * _data_dimensions], closest_tree_point, closest_distance);

		size_t nearest_sphere_center = _spheres[tree_id_map[closest_tree_point]].data_index;
		_data_labels[i] = _data_labels[nearest_sphere_center];
	}

	delete[] tree_id_map;
}

void VoronoiClustering::write_spheres_to_bin(std::string output_file) 
{
	if (_num_spheres == 0)
	{
		std::cout << "Warning: Attempting to write sphere data to file before initializing." << std::endl;
		return;
	}

	utils::write_spheres_to_bin(_spheres, _num_spheres, output_file);
}

void VoronoiClustering::load_spheres(std::string input_file)
{
	if (_num_spheres != 0)
	{
		std::cout << "ERROR: Attempting to read sphere data from file after initializing." << std::endl;
		return;
	}

	utils::load_spheres(input_file, _spheres, _num_spheres);
	_spheres_capacity = _num_spheres;
}

void VoronoiClustering::write_data_tree_to_bin(std::string filename)
{
	if (!_cfg.use_data_tree || _data_size == 0) {
		std::cout << "ERROR: could not write data tree to bin" << filename << ". Tree not initialized." << std::endl;
		return;
	}

	_data_tree.write_tree_to_binary(filename);
}

void VoronoiClustering::write_labels(std::string output_folder, bool include_data)
{
	std::string output_filename = output_folder + "data_labels_" + std::to_string(_cfg.radius) + "_" + std::to_string(_cfg.detail_ceiling) + "_" + std::to_string(_cfg.descent_limit) + ".csv";
	std::ofstream output_stream(output_filename, std::ios::trunc);
	if (!output_stream.is_open()) {
		std::cout << "ERROR: could not open output file " << output_filename << std::endl;
		return;
	}

	for (int i = 0; i < _data_size; i++)
	{
		output_stream << _data_labels[i] << std::endl;
	}
}
