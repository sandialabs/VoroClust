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

#include <gtest/gtest.h>
#include <iostream>
#include <cstdio>

#include<VoronoiClustering.h>
#include<Utils.h>

void check_loaded_data(double* data, size_t size, size_t dimensions, double* test_data, const size_t test_size, const size_t test_dimensions)
{
	ASSERT_EQ(size, test_size);
	ASSERT_EQ(dimensions, test_dimensions);

	for (int i = 0; i < test_size; i++)
	{
		for (int j = 0; j < test_dimensions; j++)
		{
			EXPECT_EQ(data[i * dimensions + j], test_data[i * test_dimensions + j]);
		}
	}
}

TEST(BasicIO, BasicIO) {

	constexpr const size_t test_size = 10;
	constexpr const size_t test_dimensions = 3;

	double* test_data = new double[test_size * test_dimensions];
	for (int i = 0; i < test_size; i++)
	{
		for (int j = 0; j < test_dimensions; j++)
		{
			test_data[i * test_dimensions + j] = (double)i;
		}
	}

	//int* test_data_labels = new int[test_size];
	//VoronoiClustering voroclust(test_data, test_size, test_dimensions, .1, .9, .05, test_data_labels, 1);
	std::string bin_filename = "test.bin";
	std::string csv_filename = "test.csv";

	utils::write_data_to_csv(csv_filename, test_size, test_dimensions, test_data);

	double* csv_data;
	size_t data_size = 0;
	size_t data_dimensions = 0;
	utils::load_csv(csv_filename, data_size, data_dimensions, csv_data);
	check_loaded_data(csv_data, data_size, data_dimensions, test_data, test_size, test_dimensions);
	
	utils::write_binary_from_csv(csv_filename, bin_filename);

	double* bin_data;
	data_size = 0;
	data_dimensions = 0;
	utils::load_binary(bin_filename, &data_size, &data_dimensions, bin_data);
	check_loaded_data(bin_data, data_size, data_dimensions, test_data, test_size, test_dimensions);

	delete[] test_data;
	delete[] csv_data;
	delete[] bin_data;

	std::remove(bin_filename.c_str());
	std::remove(csv_filename.c_str());
}
