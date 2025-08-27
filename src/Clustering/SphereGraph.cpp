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

#include "SphereGraph.h"

SphereGraph::SphereGraph() 
	: is_initialized(false),
	graph(nullptr),
	num_nodes(0),
	_graph_capacity(0),
	_num_clusters(0),
	_cluster_points(),
	_active_clusters()
{
}

SphereGraph::SphereGraph(size_t initial_size)
	: _graph_capacity(initial_size),
	num_nodes(0),
	_num_clusters(0),
	_cluster_points(),
	_active_clusters(),
	is_initialized(true)
{
	graph = new size_t * [_graph_capacity];
	for (int i = 0; i < _graph_capacity; i++)
	{
		graph[i] = nullptr;
	}
}

SphereGraph::~SphereGraph()
{
	//if not initialized, nothing was allocated and don't need to deallocate
	if (!is_initialized)
	{
		return;
	}

	for (int i = 0; i < _graph_capacity; i++)
	{
		if (graph[i] != nullptr)
		{
			delete[] graph[i];
		}
	}
	delete[] graph;
	delete[] _cluster_points;
	delete[] _active_clusters;
}

void SphereGraph::initialize(size_t initial_size)
{
	if (is_initialized)
	{
		return;
	}

	_graph_capacity = initial_size;
	num_nodes = 0;
	graph = new size_t * [_graph_capacity];
	for (int i = 0; i < _graph_capacity; i++)
	{
		graph[i] = nullptr;
	}

	is_initialized = true;
}

void SphereGraph::initialize( SphereGraph* input) 
{
	initialize(input->get_capacity());
	for (int i = 0; i < input->num_nodes; i++)
	{
		add_node(i, input->graph[i][CAPACITY]);
		copy_node(input->graph[i], graph[i]);
	}
}

void SphereGraph::copy_node(const size_t* input, size_t* output)
{
	output[CAPACITY] = input[CAPACITY];
	output[NUM_NEIGHBORS] = input[NUM_NEIGHBORS];
	output[VISITED] = input[VISITED];
	output[ENABLED] = input[ENABLED];
	output[CLUSTER_ID] = input[CLUSTER_ID];

	for (size_t i = 0; i < input[NUM_NEIGHBORS]; i++)
	{
		output[metadata_size + i] = input[metadata_size + i];
	}
}

void SphereGraph::add_node(size_t new_node, size_t node_capacity)
{
	//no-op if the node already exists
	if (graph[new_node] != nullptr)
	{
		std::cout << "Warning: add_node node " << new_node << " already exists." << std::endl;
		return;
	}

	//if there's no more capacity in the graph, expand it
	if (new_node >= _graph_capacity)
	{
		size_t cap = 2 * _graph_capacity;
		size_t** tmp = new size_t * [cap];
		for (int i = 0; i < _graph_capacity; i++)
		{
			size_t* tmp_node = new size_t[metadata_size + graph[i][CAPACITY]];
			copy_node(graph[i], tmp_node);
			tmp[i] = tmp_node;

			//delete allocation that has now been copied into tmp
			delete[] graph[i];
		}
		delete[] graph;

		//set newly created capacity to nullptr
		for (size_t i = _graph_capacity; i < cap; i++)
		{
			tmp[i] = nullptr;
		}

		//point graph to new memory
		graph = tmp;

		_graph_capacity = cap;
	}

	graph[new_node] = new size_t[metadata_size + node_capacity];

	//set initial capacity
	graph[new_node][CAPACITY] = node_capacity;

	//keep track of number of neighbors
	graph[new_node][NUM_NEIGHBORS] = 0;

	//whether or not a node has been visited in traversal
	graph[new_node][VISITED] = 0;

	graph[new_node][ENABLED] = 1;
	graph[new_node][CLUSTER_ID] = SIZE_MAX;

	for (int i = 0; i < node_capacity; i++)
	{
		graph[new_node][metadata_size + i] = SIZE_MAX;
	}

	num_nodes++;
}

void SphereGraph::connect_graph_nodes(size_t inode, size_t jnode)
{
	connect_graph_nodes_directional(inode, jnode);
	connect_graph_nodes_directional(jnode, inode);
}

void SphereGraph::connect_graph_nodes_directional(size_t inode, size_t jnode)
{
	//no-op if inode doesn't exist
	if (inode >= _graph_capacity || graph[inode] == nullptr)
	{
		std::cout << "Warning: connect_graph_nodes_directional node " << inode << " does not exist." << std::endl;
		return;
	}

	bool connected(false);
	size_t num_neighbors(graph[inode][NUM_NEIGHBORS]);
	//check if inode already has an edge to jnode 
	for (size_t i = 0; i < num_neighbors; i++)
	{
		if (graph[inode][metadata_size + i] == jnode)
		{
			return;
		}
	}

	//add edge to jnode
	graph[inode][metadata_size + num_neighbors] = jnode;
	num_neighbors++; 
	graph[inode][NUM_NEIGHBORS]++;

	//if inode is at capacity, double it 
	if (num_neighbors == graph[inode][CAPACITY])
	{
		//allocate more space and then copy node data
		size_t cap = 2 * graph[inode][CAPACITY];
		size_t* tmp = new size_t[metadata_size + cap];
		copy_node(graph[inode], tmp);
		tmp[CAPACITY] = cap;
		for (size_t i = graph[inode][CAPACITY]; i < cap; i++)
		{
			tmp[metadata_size + i] = -1;
		}

		delete[] graph[inode];
		graph[inode] = tmp;
	}
}

bool SphereGraph::is_connected(size_t inode, size_t jnode)
{
	//no-op if inode doesn't exist
	if (inode >= _graph_capacity || graph[inode] == nullptr)
	{
		std::cout << "Warning: is_connected node " << inode << " does not exist." << std::endl;
		return false;
	}

	bool connected(false);
	for (size_t i = 0; i < graph[inode][NUM_NEIGHBORS]; i++)
	{
		if (graph[inode][metadata_size + i] == jnode)
		{
			return true;
		}
	}
	return false;
}

//TODO: assumes nodes are in order of greatest interior points.
//Nodes are added in VoronoiUnsupervised in the correct order, but SphereGraph has nothing enforcing that.
//Should update class to guarantee the assumption
void SphereGraph::cluster_propagation(Sphere* spheres, double detail_ceiling, double descent_limit)
{
	//reset all nodes to unvisited
	reset_visited();
	_num_clusters = 0;
	size_t  cluster_capacity = 30;
	_cluster_points = new size_t[cluster_capacity];

	size_t front_capacity = 100;
	size_t* node_front = new size_t[front_capacity];

	for (size_t i = 0; i < num_nodes; i++)
	{
		size_t start_node = i;
		if (graph[start_node][VISITED])
		{
			continue;
		}

		node_front[0] = start_node;
		size_t max_count = spheres[start_node].count;
		graph[start_node][VISITED] = 1;

		size_t front_size(1);
		
		size_t num_cluster_points(0);
		size_t num_cluster_nodes(0);
		while (front_size > 0)
		{

			//find the node in the front with the most interior points
			size_t index_max(SIZE_MAX), num_max(0);
			for (size_t j = 0; j < front_size; j++)
			{
				size_t node = node_front[j];
				if (spheres[node].count > num_max)
				{
					num_max = spheres[node].count;
					index_max = j;
				}
			}

			//swap max node with last node in the front
			size_t tmp = node_front[index_max];
			node_front[index_max] = node_front[front_size - 1];
			node_front[front_size - 1] = tmp;

			//get max node, and remove it from the front
			size_t current_node = node_front[front_size - 1]; 
			front_size--;

			graph[current_node][CLUSTER_ID] = _num_clusters;

			//check neighbors. If any have greater density or we are below limit, disable current node
			bool disable_node = false;
			if ((double)spheres[current_node].count < descent_limit * (double)max_count)
			{
				disable_node = true;
			}
			else
			{
				for (int i = 0; i < graph[current_node][NUM_NEIGHBORS]; i++)
				{
					size_t neighbor = graph[current_node][metadata_size + i];

					if (graph[neighbor][VISITED] && graph[neighbor][CLUSTER_ID] != SIZE_MAX && graph[neighbor][CLUSTER_ID] != _num_clusters && graph[neighbor][ENABLED])
					{
						std::cout << "Error: Two clusters connected directly, without border sphere." << std::endl;
					}

					//if neighbor has been visited (current or different cluster), do not consider it. 
					if (graph[neighbor][VISITED])
					{
						continue;
					}

					if ((double)spheres[current_node].count < detail_ceiling * (double)max_count &&
					    spheres[neighbor].count > (1.0 + 0.01) * spheres[current_node].count)
					{
						disable_node = true;
						break;
					}
				}
			}

			if (disable_node)
			{
				graph[current_node][ENABLED] = 0;
			}
			else
			{
				graph[current_node][ENABLED] = 1;

				num_cluster_nodes++;
				num_cluster_points += spheres[current_node].count;
				
				//add neighbors to the front.
				for (size_t i = 0; i < graph[current_node][NUM_NEIGHBORS]; i++)
				{
					size_t neighbor = graph[current_node][metadata_size + i];
					if (graph[neighbor][VISITED])
					{
						continue;
					}

					if (front_size == front_capacity)
					{
						front_capacity = utils::resize_array(node_front,1, front_capacity, 2 * front_capacity);
					}

					graph[neighbor][VISITED] = 1;
					node_front[front_size] = neighbor;
					front_size++;
				}
			}
		}

		if (num_cluster_nodes > 0)
		{
			if (_num_clusters == cluster_capacity)
			{
				cluster_capacity = utils::resize_array(_cluster_points, 1, cluster_capacity, 2 * cluster_capacity);
			}
			_cluster_points[_num_clusters] = num_cluster_points;
			_num_clusters++;
		}
	}

	//all clusters are active to start with
	_active_clusters = new bool[_num_clusters];
	std::fill(_active_clusters, _active_clusters + _num_clusters, true);

	delete[] node_front;
}

void SphereGraph::set_active_clusters(size_t max_clusters)
{
	//activate all clusters
	std::fill(_active_clusters, _active_clusters + _num_clusters, true);

	//if '0' clusters, ALL clusters are active
	if (max_clusters == 0)
		return;

	size_t* sorted_clusters = new size_t[_num_clusters];
	for (int i = 0; i < _num_clusters; i++)
	{
		sorted_clusters[i] = i;
	}
	std::sort(sorted_clusters, sorted_clusters + _num_clusters, [this](const size_t index1, const size_t index2)
		{
			return _cluster_points[index1] > _cluster_points[index2];
		});

	//disable smallest clusters exceeding the maximum allowed number
	for (int i = max_clusters; i < _num_clusters; i++)
	{
		_active_clusters[sorted_clusters[i]] = false;
	}
}

void SphereGraph::set_active_clusters(double noise_threshold)
{
	if (noise_threshold < 0)
	{
		std::cout << "Warning: negative noise threshold is invalid. Setting to 0." << std::endl;
	}

	//activate all clusters
	std::fill(_active_clusters, _active_clusters + _num_clusters, true);

	std::pair<size_t, size_t>* sorted_clusters = new std::pair<size_t, size_t>[_num_clusters];
	size_t total_points = 0;
	for (int i = 0; i < _num_clusters; i++)
	{
		sorted_clusters[i].first = i;
		sorted_clusters[i].second = _cluster_points[i];
		total_points += _cluster_points[i];
	}
	std::sort(sorted_clusters, sorted_clusters + _num_clusters, [this](const std::pair<size_t, size_t> cluster1 , const std::pair<size_t, size_t> cluster2)
		{
			return cluster1.second < cluster2.second;
		});

	size_t cutoff = (size_t)(noise_threshold * total_points);
	size_t sum = 0;
	for (int i = 0; i < _num_clusters; i++)
	{
		if ((sum + sorted_clusters[i].second) < cutoff)
		{
			_active_clusters[sorted_clusters[i].first] = false;
			sum += sorted_clusters[i].second;
		}
		else
		{
			break;
		}
	}
}

bool SphereGraph::is_cluster_active(size_t cluster_id)
{
	return _active_clusters[cluster_id];
}

void SphereGraph::reset_visited()
{
	//reset all nodes to unvisited
	for (size_t i = 0; i < num_nodes; i++)
	{
		graph[i][VISITED] = 0;
	}
}

size_t* SphereGraph::get_nodes_metadata(size_t metadata_index)
{
	size_t* nodes_metadata;

	if (metadata_index >= metadata_size)
	{
		std::cout << "Warning: invalid metadata index " << metadata_index << std::endl;
		return nullptr;
	}

	nodes_metadata = new size_t[num_nodes];
	for (int i = 0; i < num_nodes; i++)
	{
		nodes_metadata[i] = graph[i][metadata_index];
	}

	return nodes_metadata;
}
