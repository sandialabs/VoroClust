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
from enum import unique
import time
import warnings

import numpy as np
import matplotlib
import matplotlib.pyplot as plt

from sklearn import cluster, datasets, mixture
from sklearn.neighbors import kneighbors_graph
from sklearn.preprocessing import StandardScaler,MinMaxScaler
from itertools import cycle, islice
from sklearn.preprocessing import minmax_scale
import vc_clustering
import hdbscan
from CONFIGS import get_config_dict

print(vc_clustering.__file__)
help(vc_clustering)

seed = 12345
np.random.seed(seed)
nthreads = 12
config_dict = get_config_dict()

data_folder = config_dict['data_folder']
hdbscan_test = np.load(data_folder + 'hdbscan_density.npy', allow_pickle=True)
zero_separation_test = np.load(data_folder + 'zero_separation.npy', allow_pickle=True)

# ============
# Generate datasets. We choose the size big enough to see the scalability
# of the algorithms, but not too big to avoid too long running times
# ============
n_samples = 3000
noisy_circles = datasets.make_circles(n_samples=n_samples, factor=0.5, noise=0.05)
noisy_moons = datasets.make_moons(n_samples=n_samples, noise=0.05)

# ============
# Set up cluster parameters
# ============
plt.figure(figsize=(9 * 2 + 3, 13))
plt.subplots_adjust(
    left=0.02, right=0.98, bottom=0.001, top=0.95, wspace=0.01, hspace=0.01
)

plot_num = 1

datasets = [
    (
        noisy_circles[0],
        {
            'pointsize':10,
            'VSD2_args':{'radius': 0.3, 'detail_ceiling': 0.7, 'descent_limit': 0.5, 'max_clusters': 2},
            'KMeans_args':{'n_clusters':2},
            'BIRCH_args':{'threshold': 0.305, 'branching_factor': 100, 'n_clusters':2},
            'DBSCAN_args':{'eps' : .3, 'min_samples':5},
            'HDBSCAN_args':{'min_cluster_size': 10, 'min_samples':1}
        },
    ),
    (
        noisy_moons[0],
        {
            'pointsize':10,
            'VSD2_args':{'radius': 0.3, 'detail_ceiling': 0.5, 'descent_limit': 0.1, 'max_clusters': 2},
            'KMeans_args':{'n_clusters':2},
            'BIRCH_args':{'threshold': 0.305, 'branching_factor': 100, 'n_clusters':2},
            'DBSCAN_args':{'eps' : .3, 'min_samples':5},
            'HDBSCAN_args':{'min_cluster_size': 10, 'min_samples':1}
        },
    ),
    (
        zero_separation_test,
        {
            'pointsize':3,
            'VSD2_args':{'radius': 0.07, 'detail_ceiling': 0.27, 'descent_limit': 0.27, 'max_clusters': 2},
            'KMeans_args':{'n_clusters':2},
            'BIRCH_args':{'threshold': 0.305, 'branching_factor': 100, 'n_clusters':2},
            #with eeps too large, everything is a single cluster. Too small and everything is broken up. 
            #very difficult to find a single eps that works for very different density clusters
            'DBSCAN_args':{'eps' : .057, 'min_samples':1},
            'HDBSCAN_args':{'min_cluster_size': 130, 'cluster_selection_epsilon':.05, 'min_samples':1}
        },
    ),
    (
        hdbscan_test,
        {
            'pointsize':10,
            'VSD2_args':{'radius': 0.11, 'detail_ceiling': 0.25, 'descent_limit': 0.2, 'noise_threshold': .2},
            'KMeans_args':{'n_clusters':6},
            'BIRCH_args':{'threshold': 0.305, 'branching_factor': 100, 'n_clusters':6},
            'DBSCAN_args':{'eps' : .14, 'min_samples':13},
            'HDBSCAN_args':{'min_cluster_size': 130, 'min_samples':1}
        },
    )
]

for i_dataset, (dataset, algo_params) in enumerate(datasets):
    dataset = StandardScaler().fit_transform(dataset)

    data_size = dataset.shape[0]
    data_dimensions = dataset.shape[1]

    kmeans = cluster.KMeans(**algo_params['KMeans_args'])
    dbscan = cluster.DBSCAN(n_jobs=-1,**algo_params['DBSCAN_args'])
    birch = cluster.Birch(**algo_params['BIRCH_args'])
    hdbscan_clust = hdbscan.HDBSCAN(core_dist_n_jobs=nthreads, **algo_params['HDBSCAN_args'])

    cfg = algo_params['VSD2_args']

    t0 = time.time()
    vsd2 = vc_clustering.VCC(dataset.flatten(), data_size, data_dimensions, radius=cfg["radius"], detail_ceiling=cfg["detail_ceiling"], descent_limit=cfg["descent_limit"])
    t1 = time.time()
    vsd2_init_time = t1-t0

    clustering_algorithms = (
        ("K-Means", kmeans),
        ("BIRCH", birch),
        ("DBSCAN", dbscan),
        ("HDBSCAN", hdbscan_clust),
        ("VSD2", vsd2),
    )

    for name, algorithm in clustering_algorithms:
        t0 = time.time()

        # catch warnings related to neighbors_graph
        with warnings.catch_warnings():
            warnings.filterwarnings(
                "ignore",
                message="the number of connected components of the "
                + "connectivity matrix is [0-9]{1,2}"
                + " > 1. Completing it to avoid stopping the tree early.",
                category=UserWarning,
            )
            warnings.filterwarnings(
                "ignore",
                message="Graph is not fully connected, spectral embedding"
                + " may not work as expected.",
                category=UserWarning,
            )
            if name == "VSD2":
                algorithm.execute(seed)
                if 'noise_threshold' in cfg:
                    algorithm.labelNoise(cfg["noise_threshold"])
                elif 'max_clusters' in cfg:
                    algorithm.labelByMaxClusters(cfg["max_clusters"])
            else:
                algorithm.fit(dataset)

        if hasattr(algorithm, "labels_"):
            y_pred = algorithm.labels_.astype(int)
        elif name == "VSD2":
            y_pred = algorithm.getLabels()

            print(len(np.unique(y_pred)))
        else:
            y_pred = algorithm.predict(dataset)
        t1 = time.time()

        if name == "VSD2":
            total_time = (t1-t0) + vsd2_init_time
        else:
            total_time = (t1-t0)

        plt.subplot(len(datasets), len(clustering_algorithms), plot_num)
        if i_dataset == 0:
            plt.title(name, size=35)

        colors = np.array(
            list(
                islice(
                    cycle(
                        [
                            "#377eb8",
                            "#ff7f00",
                            "#4daf4a",
                            "#f781bf",
                            "#a65628",
                            "#984ea3",
                            "#999999",
                            "#e41a1c",
                            "#dede00",
                        ]
                    ),
                    int(max(y_pred) + 1),
                )
            )
        )
        # add black color for outliers (if any)
        colors = np.append(colors, ["#000000"])

        plt.scatter(dataset[:, 0], dataset[:, 1], s=algo_params['pointsize'], color=colors[y_pred])

        plt.xlim(-2.5, 2.5)
        plt.ylim(-2.5, 2.5)
        plt.xticks(())
        plt.yticks(())
        plt.text(
            0.99,
            0.01,
            ("%.3fs" % total_time).lstrip("0"),
            transform=plt.gca().transAxes,
            size=35,
            horizontalalignment="right",
        )
        plot_num += 1

plt.savefig('Results/toy_examples.pdf', bbox_inches='tight')
plt.show()
