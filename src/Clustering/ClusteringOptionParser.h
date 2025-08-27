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
//  ClusteringOptionParser.h                                       Last modified (08/07/2024)//
///////////////////////////////////////////////////////////////////////////////////////////////
#endif // DOXYGEN_SHOULD_SKIP_THIS

#ifndef _VOROCLUST_OPTIONPARSER_H_
#define _VOROCLUST_OPTIONPARSER_H_

#include "ClusteringCommon.h"
class ClusteringOptionParser
{
    public:
		ClusteringOptionParser(const int argc, char* argv[])
		: appname(argv[0]),
		argvec(argv+1, argv+argc),
		config_filename(),
		data_file(),
		output_folder(),
		radius(1.0),
		max_clusters(SIZE_MAX),
		noise_threshold(0.0),
		use_noise_threshold(false),
		use_max_clusters(false),
		detail_ceiling(1.0),
		descent_limit(0.0),
		fixed_seed(-1),
		num_threads(1),
		read_data_tree_file(),
		write_data_tree_file(),
		read_sphere_file(),
		write_sphere_file(),
		write_data_binary_file()
		{
			parse_arguments();
		}

		~ClusteringOptionParser() = default;

		bool option_exists(const std::string& option)
		{
			return std::find(argvec.begin(), argvec.end(), option) != argvec.end();
		}

		void PrettyPrint()
		{
			for(auto arg: this->argvec)
			{
				std::cout << "- " << arg << std::endl;
			}
		}

		void usage() const {
		
			std::cout << "\t\tVOROCLUST 1.0" << std::endl
                                << "Copyright 2024 National Technology & Engineering Solutions of Sandia, LLC (NTESS)." << std::endl
                                << "Under the terms of Contract DE-NA0003525 with NTESS, there is a non-exclusive license" << std::endl
                                << "for use of this work by or on behalf of the U.S. Government. Export of this data may" << std::endl
                                << "require a license from the United States Government." << std::endl
                                << "\n" << std::endl
                                << "VoroClust accepts one argument: a configuration file." << std::endl
				<< "\texample: voroclust.exe path/to/config.in" << std::endl
				<< "\tA minimal config file example is: " << std::endl
				<< "\t\tDATA_FILE=path/to/input_file.csv" << std::endl
				<< "\t\tOUTPUT_FOLDER = path/to/output/folder/" << std::endl
				<< "\t\tRADIUS = .1" << std::endl
				<< "\t\tNOISE_THRESHOLD = .01" << std::endl
				<< "\t\tDETAIL_CEILING = .85" << std::endl
				<< "\t\tDESCENT_LIMIT = .15" << std::endl

				<< std::endl << std::endl
				<< "Valid config parameters are..." << std::endl
				<< "\tDATA_FILE= The path to a .csv data file, where each row is a single n-dimensional point. Assumes no header row. For reduced storage and faster load times, it is also possible to use a .bin file. See WRITE_DATA_BIN_FILE." << std::endl
				<< "\tOUTPUT_FOLDER= Location where all output files will be written." << std::endl
				<< "\tRADIUS= The radius of spheres used to cover the domain." << std::endl
				<< "\tNOISE_THRESHOLD= Determines the fraction of data points which will be labeled as noise in a postprocessing step." << std::endl
				<< "\tMAX_CLUSTERS= Smallest clusters will be disabled in postprocessing if the total number exceeds this value" << std::endl
				<< "\t\t--->NOISE_THRESHOLD and MAX_CLUSTERS are mutually exclusive. The last one defined in the config file will be used, and the other ignored. If you don't want any postprocessing, both can be left out." << std::endl << std::endl
				<< "\tDETAIL_CEILING= Value between 0 and 1. Controls clustering propagation." << std::endl
				<< "\tDESCENT_LIMIT= Value between 0 and 1. Controls clustering propagation." << std::endl
				<< "\t\t--->DETAIL_CEILING should be greater than DESCENT_LIMIT" << std::endl << std::endl
				<< "\tFIXED_SEED=Set a fixed seed. Defaults to -1 (random operation)" << std::endl
				<< "\tNUM_THREADS= Number of OpenMP threads to use. Defaults to 1. If less than 1 and OpenMP is available, will be set to the number of cores available (omp_get_num_procs)" << std::endl
				<< "\tREAD_DATA_TREE_FILE= To save time, we can load the data's Kd-Tree from a .bin file, rather than recomputing it." << std::endl
				<< "\tWRITE_DATA_TREE_FILE= Write the Kd-Tree to a .bin file for future use." << std::endl
				<< "\tREAD_SPHERE_FILE= To save time, we can load the sphere cover from a .bin file, rather than recomputing it." << std::endl
				<< "\tWRITE_SPHERE_FILE= Write the sphere cover to a .bin file for future use." << std::endl
				<< "\t\t--->The above is useful because computing the sphere cover is both expensive and also the only step that depends on the radius. Therefore, after saving the spheres it is possible to rapidly search for optimal DETAIL_CEILING/DESCENT_LIMIT parameters for the chosen radius." << std::endl << std::endl
				<< "\tWRITE_DATA_BIN_FILE= Given a .csv DATA_FILE, writes the data to a .bin for reduced storage and faster loading. If this parameter is present, will skip clustering and ONLY write the file." << std::endl;
		}

		void parse_arguments(void)
		{
			if( argvec.size() == 0 || option_exists("-h") )
			{
				usage();
				exit(0);
			}

			config_filename = argvec[0];
			parse_config_file();
			validate();
		}

		void parse_config_file()
		{
			std::ifstream config_file(config_filename.c_str());

			if (!config_file)
			{
				std::cout << "Could not open configuration file: " << config_filename << std::endl;
				usage();
				exit(0);
			}

			std::cout << "VoroClust input parameters:" << std::endl;

			std::string line("");
			while (std::getline(config_file, line))
			{
				std::vector<std::string> tokens;
				std::string token;
				std::string trimmed_token;
				std::stringstream line_stream(line);

				while (std::getline(line_stream, token, '='))
				{
					//trim all whitespace from the line
					//(fragile solution broken by paths with whitespace)
					std::stringstream token_stream(token);
					while (std::getline(token_stream, trimmed_token,' '))
					{
						if(!trimmed_token.empty())
						tokens.push_back(trimmed_token);
					}
				}

				if (tokens.size() != 2)
				{
					std::cout << "Invalid config line: " << line << std::endl;
					continue;
				}

				if (tokens[0] == "DATA_FILE")
					data_file = tokens[1];
				else if (tokens[0] == "OUTPUT_FOLDER")
					output_folder = tokens[1];
				else if (tokens[0] == "RADIUS")
					radius = std::stod(tokens[1]);
				else if (tokens[0] == "MAX_CLUSTERS")
				{
					max_clusters = std::stoi(tokens[1]);
					use_noise_threshold = false;
					use_max_clusters = true;
				}
				else if (tokens[0] == "NOISE_THRESHOLD")
				{
					noise_threshold = std::stod(tokens[1]);
					use_noise_threshold = true;
					use_max_clusters = false;
				}
				else if (tokens[0] == "DETAIL_CEILING")
					detail_ceiling = std::stod(tokens[1]);
				else if (tokens[0] == "DESCENT_LIMIT")
					descent_limit = std::stod(tokens[1]);
				else if (tokens[0] == "FIXED_SEED")
					fixed_seed = std::stoi(tokens[1]);
				else if (tokens[0] == "NUM_THREADS")
					num_threads = std::stoi(tokens[1]);
				else if (tokens[0] == "READ_DATA_TREE_FILE")
					read_data_tree_file = tokens[1];
				else if (tokens[0] == "WRITE_DATA_TREE_FILE")
					write_data_tree_file = tokens[1];
				else if (tokens[0] == "READ_SPHERE_FILE")
					read_sphere_file = tokens[1];
				else if (tokens[0] == "WRITE_SPHERE_FILE")
					write_sphere_file = tokens[1];
				else if (tokens[0] == "WRITE_DATA_BIN_FILE")
					write_data_binary_file = tokens[1];
				else
					std::cout << "Invalid config parameter: " << tokens[0] << std::endl;
			}
		
			std::cout << "\t* DATA_FILE           = " << data_file << std::endl;
			std::cout << "\t* OUTPUT_FOLDER       = " << output_folder << std::endl;
			std::cout << "\t* RADIUS              = " << radius << std::endl;
			if(use_max_clusters)
				std::cout << "\t* MAX_CLUSTERS        = " << max_clusters << std::endl;
			if(use_noise_threshold)
				std::cout << "\t* NOISE_THRESHOLD     = " << noise_threshold << std::endl;

			std::cout << "\t* DETAIL_CEILING      = " << detail_ceiling << std::endl;
			std::cout << "\t* DESCENT_LIMIT       = " << descent_limit << std::endl;
			std::cout << "\t* FIXED_SEED          = " << fixed_seed << std::endl;
			std::cout << "\t* READ_DATA_TREE_FILE = " << read_data_tree_file << std::endl;
			std::cout << "\t* WRITE_DATA_TREE_FILE= " << write_data_tree_file << std::endl;
			std::cout << "\t* READ_SPHERE_FILE    = " << read_sphere_file << std::endl;
			std::cout << "\t* WRITE_SPHERE_FILE   = " << write_sphere_file << std::endl;
			std::cout << "\t* WRITE_DATA_BIN_FILE = " << write_data_binary_file << std::endl;

			std::cout << "\t* NUM_THREADS         = " << num_threads << std::endl;
			#if defined USE_OPEN_MP
						std::cout << "\t\t---> omp_get_num_procs() = " << omp_get_num_procs() << std::endl;
			#endif
		}

		void validate()
		{
			bool valid_args = true;
			if (radius < 0)
			{
				std::cout << "ERROR: Invalid RADIUS " << radius << std::endl;
				valid_args = false;
			}

			if (noise_threshold < 0)
			{
				std::cout << "ERROR: Invalid NOISE_THRESHOLD " << noise_threshold << std::endl;
				valid_args = false;
			}

			if (detail_ceiling < 0)
			{
				std::cout << "ERROR: Invalid DETAIL_CEILING " << detail_ceiling << std::endl;
				valid_args = false;
			}

			if (descent_limit < 0)
			{
				std::cout << "ERROR: Invalid DESCENT_LIMIT " << descent_limit << std::endl;
				valid_args = false;
			}

			if (!read_data_tree_file.empty())
			{
				bool is_bin = read_data_tree_file.find(".bin", read_data_tree_file.size() - 4) != std::string::npos;
				if (!is_bin)
				{
					std::cout << "ERROR: Invalid data tree file. Must be .bin." << std::endl;
					valid_args = false;
				}
			}

			if (!write_data_tree_file.empty())
			{
				bool is_bin = write_data_tree_file.find(".bin", write_data_tree_file.size() - 4) != std::string::npos;
				if (!is_bin)
				{
					std::cout << "ERROR: Invalid data tree file. Must be .bin." << std::endl;
					valid_args = false;
				}
			}

			if (!read_sphere_file.empty())
			{
				bool is_bin = read_sphere_file.find(".bin", read_sphere_file.size() - 4) != std::string::npos;
				if (!is_bin)
				{
					std::cout << "ERROR: Invalid sphere file for reading. Must be .bin." << std::endl;
					valid_args = false;
				}
			}

			if (!write_sphere_file.empty())
			{
				bool is_bin = write_sphere_file.find(".bin", write_sphere_file.size() - 4) != std::string::npos;
				if (!is_bin)
				{
					std::cout << "ERROR: Invalid sphere file for writing. Must be .bin." << std::endl;
					valid_args = false;
				}
			}

			if (!write_data_binary_file.empty())
			{
				bool is_bin = write_data_binary_file.find(".bin", write_data_binary_file.size() - 4) != std::string::npos;
				if (!is_bin)
				{
					std::cout << "ERROR: Invalid data bin file. Must be .bin." << std::endl;
					valid_args = false;
				}
				bool is_csv = data_file.find(".csv", write_data_binary_file.size() - 4) != std::string::npos;
				if (!is_csv)
				{
					std::cout << "ERROR: When writing data to .bin, expects the provided data file to be .csv. Either remove WRITE_DATA_BIN_FILE parameter, or provide .csv data file." << std::endl;
					valid_args = false;
				}
			}

			if (!valid_args)
			{
				usage();
				exit(1);
			}
		}


		std::string appname;
		std::vector<std::string> argvec;

		std::string config_filename;

		std::string data_file;
		std::string output_folder;

		double radius;
		size_t max_clusters;
		double noise_threshold;
		bool use_noise_threshold;
		bool use_max_clusters;

		double detail_ceiling;
		double descent_limit;

		int fixed_seed;
		int num_threads;

		//read data that was previously formatted as a kd-tree
		std::string read_data_tree_file;
		std::string write_data_tree_file;
		std::string read_sphere_file;
		std::string write_sphere_file;
		std::string write_data_binary_file;
};


#endif
