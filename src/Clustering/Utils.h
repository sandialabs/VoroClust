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
#ifndef _VOROCLUST_UTILS_H_
#define _VOROCLUST_UTILS_H_

#include "Sphere.h"
#include<string>
#include<iostream>
#include<fstream>
#include<sstream> 
#include<vector>

namespace utils {

    template <class T>
    size_t resize_array(T*& input_array, size_t data_dimensions, size_t capacity, size_t new_capacity);


	bool load_csv(std::string filename, size_t& data_size, size_t& data_dimensions, double*& data);
	bool load_binary(std::string filename, size_t* data_size, size_t* data_dimensions, double*& data);
	void write_binary_from_csv(std::string input_filename, std::string output_filename);
	void write_data_to_binary(std::string output_file, size_t data_size, size_t data_dimensions, double* data);
	void write_data_to_csv(std::string output_file, size_t data_size, size_t data_dimensions, double* data);

	void write_spheres_to_bin(Sphere* spheres, size_t num_spheres, std::string output_file);
	void load_spheres(std::string input_file, Sphere*& spheres, size_t& num_spheres);

}


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

#endif
