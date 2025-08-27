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
//  ClusteringRandomSampler.h                                     Last modified (08/07/2024) //
///////////////////////////////////////////////////////////////////////////////////////////////
#endif // DOXYGEN_SHOULD_SKIP_THIS


#ifndef _VOROCLUST_RANDOM_SAMPLER_H_
#define _VOROCLUST_RANDOM_SAMPLER_H_

#include "ClusteringCommon.h"

class ClusteringRandomSampler
{

public:

    ClusteringRandomSampler();

    ClusteringRandomSampler(size_t thread_id);

    ClusteringRandomSampler(int seed);

    ~ClusteringRandomSampler();

    void initiate_random_number_generator(unsigned long x);

	double generate_uniform_random_number();

	double generate_normal_random_number(double mean, double variance);

	int sample_uniformly_from_unit_box(size_t num_dim, double* dart);

	int sample_uniformly_from_box(size_t num_dim, double* xmin, double* xmax, double* dart);

	int sample_uniformly_from_unit_sphere(double* dart, size_t num_dim);

	int sample_uniformly_from_unit_variance_gaussian(double* dart, size_t num_dim);

	int sample_uniformly_from_unit_ball(double* dart, size_t num_dim);

	int sample_uniformly_from_simplex(double* dart, size_t num_dim, size_t num_corners, double** x);

    int sample_uniformly_from_simplex(double* dart, size_t num_dim, size_t num_corners, double* x);

	size_t sample_uniformly_from_discrete_cdf(size_t num_cells, double* cdf);

private:
	int quicksort(double* x, size_t left, size_t right);

	int test_uniform_sphere_sampling();

private:
    static size_t numOfRandomSamplers;

    // variables for Random number generator
    double Q[1220];
    int indx;
    double cc;
    double c; /* current CSWB */
    double zc;	/* current SWB `borrow` */
    double zx;	/* SWB seed1 */
    double zy;	/* SWB seed2 */
    size_t qlen;/* length of Q array */

};

#endif

