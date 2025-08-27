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

#include "Utils.h"

template size_t utils::resize_array<size_t>(size_t*& input_array, size_t data_dimensions, size_t capacity, size_t new_capacity);
template size_t utils::resize_array<Sphere>(Sphere*& input_array, size_t data_dimensions, size_t capacity, size_t new_capacity);

template <class T>
size_t utils::resize_array(T*& input_array, size_t data_dimensions, size_t capacity, size_t new_capacity)
{
	T* tmp = new T[new_capacity * data_dimensions];

	std::copy(input_array, input_array + capacity * data_dimensions, tmp);
	delete[] input_array;

	input_array = tmp;
	return new_capacity;
}

bool utils::load_csv(std::string filename, size_t& data_size, size_t& data_dimensions, double*& data)
{
	size_t measured_dimensions = 0;

	//ifstream to read from
	std::ifstream file_stream(filename, std::ios::in);

	if (!file_stream.is_open()) {
		std::cerr << "ERROR: could not open input file " << filename << std::endl;
		return false;
	}

	std::string line;
	std::string datum;

	//count the number of lines in the data file, and the number of elements on each line
	data_size = 0;
	size_t data_capacity = 100;
	while (std::getline(file_stream, line))
	{

		if (data_size == data_capacity)
		{
			data_capacity = resize_array<double>(data, data_dimensions, data_capacity, 2 * data_capacity);
		}

		//use the first line to confirm number of dimensions and allocate data based on that
		if (data_size == 0)
		{
			std::vector<double> first_data_point;
			std::stringstream line_stream(line);
			while (std::getline(line_stream, datum, ','))
			{
				first_data_point.push_back(std::stod(datum));
			}

			data_dimensions = first_data_point.size();
			data = new double[data_capacity * (data_dimensions)];
		}

		std::stringstream line_stream(line);
		size_t datum_count = 0;
		while (std::getline(line_stream, datum, ','))
		{
			if (datum_count < data_dimensions)
			{
				data[data_size * (data_dimensions)+datum_count] = std::stod(datum);
			}
			datum_count++;
		}

		data_size++;
	}

	return true;
}

bool utils::load_binary(std::string filename, size_t* data_size, size_t* data_dimensions, double*& data)
{
	std::ifstream input_stream(filename, std::ios::binary);

	if (!input_stream.is_open()) {
		std::cerr << "ERROR: could not open binary input file " << filename << std::endl;
		return false;
	}

	//note these variables are pointers rather than reference so this read works
	input_stream.read(reinterpret_cast<char*>(data_size), sizeof(size_t));
	input_stream.read(reinterpret_cast<char*>(data_dimensions), sizeof(size_t));

	data = new double[(*data_size) * (*data_dimensions)];
	input_stream.read(reinterpret_cast<char*>(data), (*data_size) * (*data_dimensions) * sizeof(double));

	return true;
}

void utils::write_data_to_binary(std::string output_file, size_t data_size, size_t data_dimensions, double* data)
{
	std::ofstream output_stream(output_file, std::ios::out | std::ios::binary | std::ios::trunc);

	if (!output_stream.is_open()) {
		std::cerr << "ERROR: could not open binary output file " << output_file << std::endl;
		return;
	}

	output_stream.write(reinterpret_cast<const char*>(&data_size), sizeof(size_t));
	output_stream.write(reinterpret_cast<const char*>(&data_dimensions), sizeof(size_t));
	output_stream.write(reinterpret_cast<const char*>(data), data_size * data_dimensions * sizeof(double));
}

void utils::write_data_to_csv(std::string output_file, size_t data_size, size_t data_dimensions, double* data)
{
	std::ofstream output_stream(output_file, std::ios::out | std::ios::trunc);

	if (!output_stream.is_open()) {
		std::cerr << "ERROR: could not open csv output file " << output_file << std::endl;
		return;
	}

	for (int i = 0; i < data_size; i++)
	{
		for (int j = 0; j < data_dimensions; j++)
		{
			output_stream << data[i * data_dimensions + j];
			if (j < data_dimensions - 1)
				output_stream << ",";
		}
		output_stream << std::endl;
	}

}

void utils::write_binary_from_csv(std::string input_filename, std::string output_filename)
{
	size_t data_size = 0;
	size_t data_dimensions = 0;
	double* data;
	bool data_loaded = load_csv(input_filename, data_size, data_dimensions, data);
	if (!data_loaded)
	{
		std::cout << "ERROR: failed to load file " << input_filename << " in order to write to binary." << std::endl;
		return;
	}

	write_data_to_binary(output_filename, data_size, data_dimensions, data);
}

void utils::write_spheres_to_bin(Sphere* spheres, size_t num_spheres, std::string output_file)
{
	std::ofstream output_stream(output_file, std::ios::out | std::ios::binary | std::ios::trunc);

	if (!output_stream.is_open()) {
		std::cerr << "ERROR: could not open output file " << output_file << std::endl;
	}

	output_stream.write(reinterpret_cast<const char*>(&num_spheres), sizeof(size_t));
	for (int i = 0; i < num_spheres; i++)
	{
		output_stream.write(reinterpret_cast<const char*>(&(spheres[i].sphere_index)), sizeof(size_t));
		output_stream.write(reinterpret_cast<const char*>(&(spheres[i].data_index)), sizeof(size_t));
		output_stream.write(reinterpret_cast<const char*>(&(spheres[i].count)), sizeof(size_t));

		output_stream.write(reinterpret_cast<const char*>(spheres[i].indices), spheres[i].count * sizeof(size_t));
	}

	output_stream.close();
}

void utils::load_spheres(std::string input_file, Sphere*& spheres, size_t& num_spheres)
{
	std::ifstream input_stream(input_file, std::ios::in | std::ios::binary);

	if (!input_stream.is_open()) {
		std::cerr << "ERROR: could not open spheres file " << input_file << std::endl;
		return;
	}

	input_stream.read(reinterpret_cast<char*>(&num_spheres), sizeof(size_t));

	spheres = new Sphere[num_spheres];

	for (int i = 0; i < num_spheres; i++)
	{
		input_stream.read(reinterpret_cast<char*>(&(spheres[i].sphere_index)), sizeof(size_t));
		input_stream.read(reinterpret_cast<char*>(&(spheres[i].data_index)), sizeof(size_t));
		input_stream.read(reinterpret_cast<char*>(&(spheres[i].count)), sizeof(size_t));

		spheres[i].indices = new size_t[spheres[i].count];
		input_stream.read(reinterpret_cast<char*>(spheres[i].indices), spheres[i].count * sizeof(size_t));
	}

	input_stream.close();
}
