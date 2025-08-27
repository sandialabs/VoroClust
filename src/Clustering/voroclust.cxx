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

#include <ClusteringOptionParser.h>
#include <VoronoiClustering.h>

int main(int argc, char* argv[])
{
	ClusteringOptionParser options(argc, argv);

	if (!options.write_data_binary_file.empty())
	{
		utils::write_binary_from_csv(options.data_file, options.write_data_binary_file);
		std::cout << "Finished writing " << options.data_file << " to " << options.write_data_binary_file << std::endl;
		return 0;
	}

	VoronoiClustering voroclust(options.data_file, options.radius, options.detail_ceiling, options.descent_limit, options.num_threads, options.read_data_tree_file);			
	if (!options.read_sphere_file.empty())
	{
		voroclust.load_spheres(options.read_sphere_file);
	}

	if (!options.write_data_tree_file.empty())
	{
		voroclust.write_data_tree_to_bin(options.write_data_tree_file);
	}

	voroclust.execute(options.fixed_seed);

	if (options.use_noise_threshold)
		voroclust.label_noise(options.noise_threshold);
	else if(options.use_max_clusters)
		voroclust.label_by_max_clusters(options.max_clusters);

	voroclust.write_labels(options.output_folder, false);
	if (!options.write_sphere_file.empty())
	{
		voroclust.write_spheres_to_bin(options.write_sphere_file);
	}

	return 0;
}

