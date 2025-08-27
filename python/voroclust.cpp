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
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl_bind.h>

#include <vector>
#include <string>
#include "VoronoiClustering.h"

class VoroClust {
public:
	VoroClust() { /*TODO make it so default constructor is actually usable. */ }
	VoroClust(pybind11::array_t<double> data, size_t data_size, size_t data_dimensions, double radius, double detail_ceiling = .85, double descent_limit = .25, int num_threads = 1, std::string data_tree_filename = "");
	~VoroClust();
	
	static VoroClust* initialize(pybind11::array_t<double> data, size_t data_size, size_t data_dimensions, double radius, double detail_ceiling = .85, double descent_limit = .25, int num_threads = 1, std::string data_tree_filename = "");

	void execute(int fixed_seed = -1);

	void load_spheres(std::string filename);
	void write_spheres(std::string filename);
	void write_data_tree(std::string filename);

	void label_by_max_clusters(size_t max_clusters);
	void label_noise(double noise_threshold);

	pybind11::array_t<int> get_labels() { return _labels_py; }
	pybind11::array_t<size_t> get_spheres();
	pybind11::array_t<size_t> get_interior_points();
	pybind11::array_t<size_t> get_graph_metadata(size_t metadata_index);
private:
	int* _labels;
	pybind11::array_t<int> _labels_py;
	pybind11::array_t<size_t> _spheres;
	//this is an array of std::pair in the c++, but convert to flat array in python because it is get-only
	//and can't figure out how to make pybind11 work with pairs.
	pybind11::array_t<size_t> _interior_points;

	size_t _data_size;
	size_t _data_dimensions;
	pybind11::buffer_info _data_info;
	double* _data_ptr;
	VoronoiClustering* _mainObj;
};

VoroClust* VoroClust::initialize(pybind11::array_t<double> data, size_t data_size, size_t data_dimensions, double radius, double detail_ceiling, double descent_limit, int num_threads, std::string data_tree_filename)
{
	return new VoroClust(data, data_size, data_dimensions, radius, detail_ceiling, descent_limit, num_threads, data_tree_filename);
}

VoroClust::VoroClust(pybind11::array_t<double> data, size_t data_size, size_t data_dimensions, double radius, double detail_ceiling, double descent_limit, int num_threads, std::string data_tree_filename)
	: _mainObj(),
	_labels(),
	_data_size(data_size),
	_data_dimensions(data_dimensions)
{
	_data_info = data.request();
	if (_data_info.size != data_size * data_dimensions)
	{
		throw std::runtime_error("Data size does not match the given values for NumPoints and NumDimensions.");
	}

	if (radius <= 0)
	{
		throw std::runtime_error("Radius must be greater than 0.");
	}

	//get raw ptr from numpy input array
	_data_ptr = (double*)_data_info.ptr;
	_labels = new int[data_size];

	_mainObj = new VoronoiClustering(_data_ptr, data_size, data_dimensions, radius, detail_ceiling, descent_limit, _labels, num_threads, data_tree_filename);

}

VoroClust::~VoroClust() {
	delete _mainObj;
	delete[] _labels;

	//do not deallocate here, as this was originally allocated in the Python so python needs to do GC
	//delete[] _data_ptr;
}

void VoroClust::load_spheres(std::string filename)
{
	_mainObj->load_spheres(filename);
}

void VoroClust::write_spheres(std::string filename)
{
	_mainObj->write_spheres_to_bin(filename);
}

void VoroClust::write_data_tree(std::string filename)
{
	_mainObj->write_data_tree_to_bin(filename);
}

void VoroClust::execute(int fixed_seed)
{
	ClusteringTimer timer_total;

	_mainObj->execute(fixed_seed);

	//https://people.duke.edu/~ccc14/cspy/18G_C++_Python_pybind11.html
	_labels_py = pybind11::array(pybind11::buffer_info(
		_labels,
		sizeof(int),
		pybind11::format_descriptor<int>::value,
		1,
		{ _data_size },
		{ sizeof(int) }
	));

	std::cout << "PythonWrapper total time: " << timer_total.report_timing() << " seconds" << std::endl;
}

void VoroClust::label_by_max_clusters(size_t max_clusters)
{
	_mainObj->label_by_max_clusters(max_clusters);

	//https://people.duke.edu/~ccc14/cspy/18G_C++_Python_pybind11.html
	_labels_py = pybind11::array(pybind11::buffer_info(
		_labels,
		sizeof(int),
		pybind11::format_descriptor<int>::value,
		1,
		{ _data_size },
		{ sizeof(int) }
	));
}

void VoroClust::label_noise(double noise_threshold)
{
	_mainObj->label_noise(noise_threshold);

	//https://people.duke.edu/~ccc14/cspy/18G_C++_Python_pybind11.html
	_labels_py = pybind11::array(pybind11::buffer_info(
		_labels,
		sizeof(int),
		pybind11::format_descriptor<int>::value,
		1,
		{ _data_size },
		{ sizeof(int) }
	));
}

pybind11::array_t<size_t> VoroClust::get_spheres()
{
	size_t num_spheres;

	num_spheres = _mainObj->get_num_spheres();
	if (num_spheres == 0)
	{
		std::cout << "Warning: There are no spheres defined. Must run 'execute' first." << std::endl;
		return pybind11::array_t<size_t>();
	}

	Sphere* spheres;
	spheres = _mainObj->get_spheres();

	size_t* data_indices = new size_t[num_spheres];
	for (int i = 0; i < num_spheres; i++)
	{
		data_indices[i] = spheres[i].data_index;
	}

	_spheres = pybind11::array(pybind11::buffer_info(
		data_indices,
		sizeof(size_t),
		pybind11::format_descriptor<size_t>::value,
		1,
		{ num_spheres },
		{ sizeof(size_t) }
	));

	return _spheres;
}

pybind11::array_t<size_t> VoroClust::get_graph_metadata(size_t metadata_index)
{
	size_t num_spheres = _mainObj->get_num_spheres();
	if (num_spheres == 0)
	{
		std::cout << "Warning: There are no spheres defined. Must run 'execute' first." << std::endl;
		return pybind11::array_t<size_t>();
	}

	size_t* metadata = _mainObj->get_graph_metadata(metadata_index);

	pybind11::array_t<size_t> python_metadata = pybind11::array(pybind11::buffer_info(
		metadata,
		sizeof(size_t),
		pybind11::format_descriptor<size_t>::value,
		1,
		{ num_spheres },
		{ sizeof(size_t) }
	));

	return python_metadata;
}

pybind11::array_t<size_t> VoroClust::get_interior_points()
{
	size_t num_spheres;

	num_spheres = _mainObj->get_num_spheres();
	if (num_spheres == 0)
	{
		std::cout << "Warning: There are no spheres defined. Must run 'execute' first." << std::endl;
		return pybind11::array_t<size_t>();
	}

	Sphere* spheres;
	spheres = _mainObj->get_spheres();

	size_t* interior_points = new size_t[num_spheres];
	for (int i = 0; i < num_spheres; i++)
	{
		interior_points[i] = spheres[i].count;
	}

	_interior_points = pybind11::array(pybind11::buffer_info(
		interior_points,
		sizeof(size_t),
		pybind11::format_descriptor<size_t>::value,
		1,
		{ num_spheres },
		{ sizeof(size_t) }
	));

	return _interior_points;
}

PYBIND11_MODULE(voroclust, m) {
	std::string initialize_usage = R"(
========== Usage ==========
	Inputs Required:
		1. Input data = flat array of doubles
		2. Num Points = number of points in the given data array.
		3. Num Dimensions = number of dimensions in each point. So the full size of the array should be NumPoints x NumDimensions
		4. Radius = double greater than 0
	Inputs Optional:
		5. Detail Ceiling = double between 0 and 1. Defaults to .85. 
		6. Descent Limit = double between 0 and 1. Defaults to .25. 
		7. Num Threads = optional int, default to 1. The number of threads to be used by OpenMP
	)";

	std::string execute_usage = R"(
========== Usage ==========
	Inputs Required:
		1. Fixed Seed = optional scalar int. Default -1. If less than 0, seed will be picked using time (not fixed). 
		   Otherwise seed for RNG is set to given value.
	Output:
		1. nx1 numpy array of ints, with each entry representing the selected cluster for one of the n points in input data
	)";

	m.doc() = "";

	pybind11::class_<VoroClust>(m, "voroclust")
		.def(pybind11::init<>()) // constructor
		.def(pybind11::init(&VoroClust::initialize), initialize_usage.c_str(), pybind11::arg("data"), pybind11::arg("data_size"), pybind11::arg("data_dimensions"), pybind11::arg("radius"),
			pybind11::arg("detail_ceiling") = 0.85, pybind11::arg("descent_limit") = 0.25, pybind11::arg("num_threads") = 1, pybind11::arg("data_tree_filename") = "",
			//python takes ownership of returned pointer to VoroClust object. Python will call destructor when it goes out of scope.
			//https://pybind11.readthedocs.io/en/stable/advanced/functions.html
			pybind11::return_value_policy::take_ownership
		)
		.def("execute", &VoroClust::execute, execute_usage.c_str(), pybind11::arg("fixed_seed") = -1)
		.def("loadSpheres", &VoroClust::load_spheres, "", pybind11::arg("filename"))
		.def("writeSpheres", &VoroClust::write_spheres, "", pybind11::arg("filename"))
		.def("writeDataTree", &VoroClust::write_data_tree, "", pybind11::arg("filename"))
		.def("labelByMaxClusters", &VoroClust::label_by_max_clusters, "", pybind11::arg("max_clusters"))
		.def("labelNoise", &VoroClust::label_noise, "", pybind11::arg("noise_threshold"))
		.def("getLabels", &VoroClust::get_labels)
		.def("getSpheres", &VoroClust::get_spheres)
		.def("getGraphMetadata", &VoroClust::get_graph_metadata, pybind11::arg("metadata_index"))
		.def("getInteriorPoints", &VoroClust::get_interior_points);
}
