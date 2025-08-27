"""
# BSD 2-Clause License
#
# Copyright (c) 2025, Sandia National Laboratories
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Example Usage:

from CONFIGS import get_config_dict

config_dict = get_config_dict()

config = config_dict['BR2_Hires']

if config is not None:
    radius = config["radius"]
    descent_limit = config["descent_limit"]
    detail_ceiling = config["detail_ceiling"]
    max_clusters = config["max_clusters"]                            

vcc = vc.VCC(data = ... ,\
            data_size = ... \
            data_dimensions = ... ,\
            radius = radius, detail_ceiling = detail_ceiling, descent_limit = descent_limit, num_threads = 16)
    
seed = np.random.choice([n for n in range(999999)])
vcc.execute(max_clusters=max_clusters, fixed_seed=seed)

"""


def get_config_dict():
    
    # SALINAS
    salinas_config = {"name": "salinasA", "pixels" : [83,86], "has_gt" : True,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':True,'HDBSCAN':True,
        'VSD2_args':{'radius': 0.08, 'detail_ceiling': 0.8, 'descent_limit': 0.25, 'max_clusters': 6},
        'KMeans_args':{'n_clusters':6},
        'BIRCH_args': {'threshold': 0.3, 'branching_factor': 150, 'n_clusters': 6},
        'DBSCAN_args': {'eps' : .05, 'min_samples':100},
        'HDBSCAN_args': {'min_cluster_size': 150, 'cluster_selection_epsilon':0.0, 'min_samples':1}        
    }
    
    # PAVIA
    pavia_config = {"name": "pavia", "pixels" : [610,340], "has_gt" : True,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':False,'HDBSCAN':False,
        'VSD2_args':{'radius': 0.19, 'detail_ceiling': 0.7, 'descent_limit': 0.1, 'max_clusters': 6},
        #dataset supposed to have 9 clusters, but after testing n=6 clearly gave the best scores  
        'KMeans_args':{'n_clusters':6},
        'BIRCH_args': {'threshold': 0.25, 'branching_factor': 10, 'n_clusters':7},
        'DBSCAN_args': {'eps' : .13, 'min_samples':1250},
        'HDBSCAN_args': {'min_cluster_size': 2000, 'min_samples':1}
    }
    
    # NEVADA
    nevada_config = {"name": "nevada", "pixels" : [628,557], "has_gt" : False,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':False,'HDBSCAN':False,
        'VSD2_args':{'radius': 0.35, 'detail_ceiling': 0.8, 'descent_limit': 0.6, 'max_clusters': 4},
        'KMeans_args':{'n_clusters':4},
        'BIRCH_args': {'threshold': 1.5, 'branching_factor': 10,'n_clusters':4}, 
        'DBSCAN_args': {'eps' : .13, 'min_samples':1000},
        'HDBSCAN_args': {'min_cluster_size': 2000, 'min_samples':1}
    } 
    
    # BR1
    br1_config = {"name": "br1", "pixels" : [3593,4703], "has_gt" : False,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':False,'HDBSCAN':False,
        'VSD2_args':{'radius': 0.15, 'detail_ceiling': 0.7, 'descent_limit': 0.4, 'max_clusters': 3},
        'KMeans_args':{'n_clusters':3},
        'BIRCH_args': {'threshold': 0.25, 'branching_factor': 150}     
    }
    br1_reduced_config = {"name": "br1_reduced", "pixels" : [472,564], "has_gt" : False,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':True,'HDBSCAN':True,
        'VSD2_args':{'radius': 0.115, 'detail_ceiling': 0.9, 'descent_limit': 0.25, 'max_clusters': 3},
        'KMeans_args':{'n_clusters':3},
        'BIRCH_args':{'threshold': 0.275, 'branching_factor': 150},
        'DBSCAN_args':{'eps' : .037, 'min_samples':100},
        'HDBSCAN_args':{'min_cluster_size': 120, 'cluster_selection_epsilon': .05, 'min_samples':100}
    } 

    # BR2
    br2_config = {"name": "br2", "pixels" : [3591,4759], "has_gt" : False,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':False,'HDBSCAN':False,
        'VSD2_args':{'radius': 0.1, 'detail_ceiling': 0.9, 'descent_limit': 0.1, 'max_clusters': 4},
        'KMeans_args':{'n_clusters':4},
        'BIRCH_args':{'threshold': 0.226, 'branching_factor': 250, 'n_clusters':4}
    }

    br2_reduced_config = {"name": "br2_reduced", "pixels" : [354,428], "has_gt" : False,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':True,'HDBSCAN':True,
        'VSD2_args':{'radius': 0.05, 'detail_ceiling': 0.75, 'descent_limit': 0.4, 'max_clusters': 4},
        'KMeans_args':{'n_clusters':4},
        'BIRCH_args':{'threshold': 0.175, 'branching_factor': 75, 'n_clusters': 4},
        'DBSCAN_args':{'eps' : .04, 'min_samples':200},
        'HDBSCAN_args':{'min_cluster_size': 75, 'cluster_selection_epsilon': .5}
    }

    # OPEN FIELD
    of_config = {"name": "of", "pixels" : [3593, 4713], "has_gt" : False,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':False,'HDBSCAN':False,
        'VSD2_args':{'radius': 0.11, 'detail_ceiling': 0.95, 'descent_limit': 0.35, 'max_clusters': 3},
        'KMeans_args':{'n_clusters':3},
        'BIRCH_args':{'threshold': 0.28, 'branching_factor': 500, 'n_clusters':3},
    }
    of_reduced_config = {"name": "of_reduced", "pixels" : [449,589], "has_gt" : False,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':True,'HDBSCAN':True,
        'VSD2_args':{'radius': 0.1, 'detail_ceiling': 0.9, 'descent_limit': 0.2, 'max_clusters': 3},
        'KMeans_args':{'n_clusters':3},
        'BIRCH_args':{'threshold': 0.1, 'branching_factor': 60, 'n_clusters':3},
        'DBSCAN_args':{'eps' : .05, 'min_samples':75},
        'HDBSCAN_args':{'min_cluster_size': 175, 'cluster_selection_epsilon': .15}
    }

    # GOLF COURSE
    gc_config = {"name": "gc", "pixels" : [3593,4703], "has_gt" : False,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':False,'HDBSCAN':False,
        'VSD2_args':{'radius': 0.1, 'detail_ceiling': 0.95, 'descent_limit': 0.4, 'max_clusters': 4},
        'KMeans_args':{'n_clusters':4},
        'BIRCH_args':{'threshold': 0.28, 'branching_factor': 500, 'n_clusters':4}
    }
    gc_reduced_config = {"name": "gc_reduced", "pixels" : [449,588], "has_gt" : False,
        'VSD2':True,'KMeans':True,'BIRCH':True,'DBSCAN':False,'HDBSCAN':False,
        'VSD2_args':{'radius': 0.1, 'detail_ceiling': 0.8, 'descent_limit': 0.3, 'max_clusters': 3},
        'KMeans_args':{'n_clusters':3},
        'BIRCH_args':{'threshold': 0.2, 'branching_factor': 10, 'n_clusters':3},
        #DBSCAN choking on this dataset, so could not find good parameters
        'DBSCAN_args':{'eps' : .05, 'min_samples':75},
        'HDBSCAN_args':{'min_cluster_size': 75, 'cluster_selection_epsilon': .25}        
    }
    
    ###  COMPREHENSIVE DICTIONARY
    config_dict = dict()
    config_dict["data_folder"] = "C:/Users/ljmoyni/vorocrust/data/Clustering_Datasets/"

    config_dict["SalinasA"] = salinas_config
    config_dict["Pavia"] = pavia_config
    config_dict["Nevada"] = nevada_config
    
    config_dict["BR1"] = br1_config
    config_dict["BR1_reduced"] = br1_reduced_config

    config_dict["BR2"] = br2_config
    config_dict["BR2_reduced"] = br2_reduced_config

    config_dict["OF"] = of_config
    config_dict["OF_reduced"] = of_reduced_config

    config_dict["GC"] = gc_config
    config_dict["GC_reduced"] = gc_reduced_config
    
    return config_dict
