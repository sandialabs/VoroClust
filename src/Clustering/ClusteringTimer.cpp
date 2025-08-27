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
//  ClusteringTimer.cpp                                           Last modified (08/07/2024) //
///////////////////////////////////////////////////////////////////////////////////////////////
#endif // DOXYGEN_SHOULD_SKIP_THIS


#include "ClusteringTimer.h"

#if defined USE_OPEN_MP
	#include <omp.h>
#else
    #include <ctime>
#endif

ClusteringTimer::ClusteringTimer()
{
	start();
}

ClusteringTimer::~ClusteringTimer()
{

}

int ClusteringTimer::start()
{
#ifdef USE_OPEN_MP
	start_time = omp_get_wtime();
#else
	start_time = double(clock()) / CLOCKS_PER_SEC;
#endif // USE_OPEN_MP
	return 0;
}

int ClusteringTimer::reset_timer()
{
	start();
	return 0;
}

double ClusteringTimer::report_timing()
{
	double computed_time(0.0);

#ifdef USE_OPEN_MP
	computed_time = omp_get_wtime() - start_time;
#else
	computed_time = double(clock()) / CLOCKS_PER_SEC - start_time;
#endif // USE_OPEN_MP
	return computed_time;
}
