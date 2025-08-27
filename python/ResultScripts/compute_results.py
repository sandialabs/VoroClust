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
import csv
from logging import exception
import numpy
import matplotlib.pyplot as plt
from matplotlib import interactive

import pandas as pd
from PIL import Image

import hdbscan
import sklearn.cluster
from sklearn.metrics import davies_bouldin_score, adjusted_rand_score, fowlkes_mallows_score

import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import vc_clustering
import time
from CONFIGS import get_config_dict

class ClusterTests:

    def __init__(self, config, data_folder, num_threads):
        self.num_threads = num_threads
        self.config = config
        self.data_folder = data_folder

        self.output = {'Algorithm': [],
                'Time': [],
                'Num_Clusters':[],
                'Clusters': [],
                'Davies_Bouldin': [],
                'Adjusted_Rand': [],
                'Fowlkes_Mallows':[],
                'Parameters':[]}

        self.label_filenames = { 
            "VSD2" : 'Results/' + self.config['name'] + '_VSD2_labels.csv',
            "KMeans" : 'Results/' + self.config['name'] + '_KMeans_labels.csv',
            "BIRCH" : 'Results/' + self.config['name'] + '_BIRCH_labels.csv',
            "DBSCAN" : 'Results/' + self.config['name'] + '_DBSCAN_labels.csv',
            "HDBSCAN" : 'Results/' + self.config['name'] + '_HDBSCAN_labels.csv'
        }

        self.data_labels = {
            "VSD2" : None,    
            "KMeans" : None,
            "BIRCH" : None,
            "DBSCAN" : None,
            "HDBSCAN" : None
        }
        self.write_labels = True

        self.stats_filename = 'Results/' + self.config['name'] + '_stats.txt'

        self.data = pd.read_csv(self.data_folder + self.config["name"] + ".csv", delimiter=',', header = None).values

        self.ground_truth = None
        if self.config["has_gt"]:
            self.ground_truth = pd.read_csv(self.data_folder + self.config["name"] + "_gt.csv", delimiter=',', header = None).values
            self.ground_truth = numpy.squeeze(self.ground_truth)

            #many of the pixels are not actually labeled  in the ground truth. Need to mask these to only consider labeled points in scoring 
            self.ground_truth_mask = [False if val == 0 else True for val in self.ground_truth]
            self.masked_ground_truth = [val for i,val in enumerate(self.ground_truth) if self.ground_truth_mask[i]]


    def update_output(self,alg_name, elapsed_time):

        num_clusters = len(numpy.unique(self.data_labels[alg_name]))

        self.output['Algorithm'].append(alg_name)
        self.output['Time'].append(elapsed_time)
        self.output['Num_Clusters'].append(num_clusters)
        self.output['Clusters'].append(numpy.unique(self.data_labels[alg_name]))
        if num_clusters > 1:
            self.output['Davies_Bouldin'].append(davies_bouldin_score(self.data, self.data_labels[alg_name]))

            if self.config["has_gt"]:
                masked_labels = [val for i,val in enumerate(self.data_labels[alg_name]) if self.ground_truth_mask[i]]

                self.output['Adjusted_Rand'].append(adjusted_rand_score(self.masked_ground_truth,masked_labels))
                self.output['Fowlkes_Mallows'].append(fowlkes_mallows_score(self.masked_ground_truth,masked_labels))
            else:
                self.output['Adjusted_Rand'].append("")
                self.output['Fowlkes_Mallows'].append("")

        else:
            self.output['Davies_Bouldin'].append("")
            self.output['Adjusted_Rand'].append("")
            self.output['Fowlkes_Mallows'].append("")

        self.output['Parameters'].append(str(self.config[alg_name + "_args"]))

        if self.write_labels:
            df = pd.DataFrame(self.data_labels[alg_name])
            df.to_csv(self.label_filenames[alg_name], index=False, header=False)

    def compute_VSD2(self):
        seed = -1
        data_size = self.data.shape[0]
        data_dimensions = self.data.shape[1]
        flat_data = self.data.flatten()
        cfg = self.config['VSD2_args']

        tic = time.perf_counter()
        vcc = vc_clustering.VCC(flat_data, data_size, data_dimensions, cfg["radius"], cfg["detail_ceiling"], cfg["descent_limit"], self.num_threads)
        vcc.execute(seed)
        vcc.labelByMaxClusters(cfg["max_clusters"])
        self.data_labels["VSD2"] = vcc.getLabels()
        toc = time.perf_counter()

        elapsed = toc - tic
        self.update_output("VSD2", elapsed)

    def compute_KMeans(self):
        tic = time.perf_counter()
        self.data_labels["KMeans"] = sklearn.cluster.KMeans(**self.config['KMeans_args'], init='k-means++', n_init='auto').fit_predict(self.data)
        toc = time.perf_counter()
        elapsed = toc - tic

        self.update_output("KMeans", elapsed)

    def compute_BIRCH(self):
        tic = time.perf_counter()
        self.data_labels["BIRCH"] = sklearn.cluster.Birch(**self.config['BIRCH_args']).fit_predict(self.data)
        toc = time.perf_counter()

        elapsed = toc - tic
        self.update_output("BIRCH", elapsed)

    def compute_DBSCAN(self):
        tic = time.perf_counter()
        self.data_labels["DBSCAN"] = sklearn.cluster.DBSCAN(n_jobs=-1, **self.config['DBSCAN_args']).fit_predict(self.data)
        toc = time.perf_counter()

        elapsed = toc - tic
        self.update_output("DBSCAN", elapsed)

    def compute_HDBSCAN(self):
        tic = time.perf_counter()
        hdb = hdbscan.HDBSCAN(core_dist_n_jobs=self.num_threads, **self.config['HDBSCAN_args'])
        hdb.fit(self.data)
        self.data_labels["HDBSCAN"] = hdb.labels_ 
        toc = time.perf_counter()

        elapsed = toc - tic
        self.update_output("HDBSCAN", elapsed)

    def compute(self, write_labels=True):
        self.write_labels = write_labels

        if self.config['KMeans']:
            self.compute_KMeans()

        if self.config['BIRCH']:
            self.compute_BIRCH()

        if self.config['DBSCAN']:
            self.compute_DBSCAN()

        if self.config['HDBSCAN']:
            self.compute_HDBSCAN()

        if self.config['VSD2']:
            self.compute_VSD2()

        if self.write_labels:
            self.print_output()

    def print_output(self):
        with pd.option_context('display.width', 3000, 'display.max_rows', None, 'display.max_columns', None, 'display.max_colwidth',None):
            print(pd.DataFrame(self.output))
            print(pd.DataFrame(self.output), file=open(self.stats_filename,'a'))

    def plot(self, interactive=True):
        pixels = self.config["pixels"]

        fig, ax = plt.subplots(3,2)
        fig.suptitle(self.config["name"])

        plot_row = 0
        plot_col = 0
        if self.config["has_gt"]:
            gt_2d = self.ground_truth.reshape((pixels[0], pixels[1]))
            ax[plot_row,plot_col].imshow(gt_2d)
            ax[plot_row,plot_col].set_title("Ground Truth", size=8)
            ax[plot_row,plot_col].set_xticks(())
            ax[plot_row,plot_col].set_yticks(())
        else:
            fig.delaxes(ax[plot_row,plot_col])

        

        plot_col += 1
        for alg in self.data_labels:
            if self.data_labels[alg] is not None:
                labels_2d = self.data_labels[alg].reshape((pixels[0], pixels[1]))
                plt.imsave(fname='Results/' + self.config["name"] + '_' + alg +'.pdf', cmap='viridis', arr=labels_2d, format='pdf')
            else:
                try:
                    self.data_labels[alg] = pd.read_csv(self.label_filenames[alg], delimiter=',', header = None).values.flatten()
                    labels_2d = self.data_labels[alg].reshape((pixels[0], pixels[1]))
                except Exception as except_type:
                    print("Missing labels for {0}, {1}".format(self.config["name"], alg))
                    fig.delaxes(ax[plot_row,plot_col])
                    labels_2d = None
                
            if labels_2d is not None:
                plt.imsave(fname='Results/' + self.config["name"] + '_' + alg +'.pdf', cmap='viridis', arr=labels_2d, format='pdf')

                ax[plot_row,plot_col].imshow(labels_2d)
                ax[plot_row,plot_col].set_title(alg, size=8)
                ax[plot_row,plot_col].set_xticks(())
                ax[plot_row,plot_col].set_yticks(())

            plot_col += 1

            if plot_col > 1:
                plot_row += 1
                plot_col = 0

        if interactive:
            plt.show()

def VSD2_parameter_search(dataset_key, radius_list, detail_ceiling_list = [.9,.8,.7], descent_limit_list = [.1,.2,.3]):

    config_dict = get_config_dict()
    data_folder = config_dict["data_folder"]

    config = config_dict[dataset_key]
    print(config)

    config['VSD2'] = True
    config['KMeans'] = False
    config['BIRCH'] = False
    config['DBSCAN'] = False
    config['HDBSCAN'] = False

    tester = ClusterTests(config, data_folder, 12)

    for radius in radius_list:
        for detail_ceiling in detail_ceiling_list:
            for descent_limit in descent_limit_list:
                config['VSD2_args']['radius'] = radius
                config['VSD2_args']['detail_ceiling'] = detail_ceiling
                config['VSD2_args']['descent_limit'] = descent_limit
                tester.compute(False)

                print((radius,detail_ceiling,descent_limit, tester.output['Davies_Bouldin'][-1], tester.output['Adjusted_Rand'][-1], tester.output['Fowlkes_Mallows'][-1] ))
                tester.plot(True)

    tester.print_output()

def BIRCH_parameter_search(dataset_key, n_list, threshold_list, branching_list):

    config_dict = get_config_dict()
    data_folder = config_dict["data_folder"]

    config = config_dict[dataset_key]
    print(config)

    config['VSD2'] = False
    config['KMeans'] = False
    config['BIRCH'] = True
    config['DBSCAN'] = False
    config['HDBSCAN'] = False

    tester = ClusterTests(config, data_folder, 12)

    for n in n_list:
        for threshold in threshold_list:
            for branching in branching_list:
                config['BIRCH_args']['n_clusters'] = n
                config['BIRCH_args']['threshold'] = threshold
                config['BIRCH_args']['branching_factor'] = branching
                tester.compute(False)

                print((n,threshold,branching, tester.output['Davies_Bouldin'][-1], tester.output['Adjusted_Rand'][-1], tester.output['Fowlkes_Mallows'][-1] ))
                tester.plot(False)

    tester.print_output()

def KMeans_parameter_search(dataset_key, n_list):

    config_dict = get_config_dict()
    data_folder = config_dict["data_folder"]

    config = config_dict[dataset_key]
    print(config)

    config['VSD2'] = False
    config['KMeans'] = True
    config['BIRCH'] = False
    config['DBSCAN'] = False
    config['HDBSCAN'] = False

    tester = ClusterTests(config, data_folder, 12)

    for n in n_list:
        config['KMeans_args']['n_clusters'] = n
        tester.compute(False)

        print((n, tester.output['Davies_Bouldin'][-1], tester.output['Adjusted_Rand'][-1], tester.output['Fowlkes_Mallows'][-1] ))
        tester.plot(False)

    tester.print_output()

def DBSCAN_parameter_search(dataset_key, eps_list, samples_list):

    config_dict = get_config_dict()
    data_folder = config_dict["data_folder"]

    config = config_dict[dataset_key]
    print(config)

    config['VSD2'] = False
    config['KMeans'] = False
    config['BIRCH'] = False
    config['DBSCAN'] = True
    config['HDBSCAN'] = False

    tester = ClusterTests(config, data_folder, 12)

    for eps in eps_list:
        for min_samples in samples_list:
            config['DBSCAN_args']['eps'] = eps
            config['DBSCAN_args']['min_samples'] = min_samples
            tester.compute(False)

            print((eps,min_samples, tester.output['Davies_Bouldin'][-1], tester.output['Adjusted_Rand'][-1], tester.output['Fowlkes_Mallows'][-1] ))
            tester.plot(False)

    tester.print_output()

def HDBSCAN_parameter_search(dataset_key, samples_list, cluster_size_list, selection_epsilon_list):

    config_dict = get_config_dict()
    data_folder = config_dict["data_folder"]

    config = config_dict[dataset_key]
    print(config)

    config['VSD2'] = False
    config['KMeans'] = False
    config['BIRCH'] = False
    config['DBSCAN'] = False
    config['HDBSCAN'] = True

    tester = ClusterTests(config, data_folder, 12)

    for min_samples in samples_list:
        for min_cluster_size in cluster_size_list:
            for cluster_selection_epsilon in selection_epsilon_list:
                config['HDBSCAN_args']['min_samples'] = min_samples
                config['HDBSCAN_args']['min_cluster_size'] = min_cluster_size
                config['HDBSCAN_args']['cluster_selection_epsilon'] = cluster_selection_epsilon
                tester.compute(False)

                print((min_samples,min_cluster_size,cluster_selection_epsilon, tester.output['Davies_Bouldin'][-1], tester.output['Adjusted_Rand'][-1], tester.output['Fowlkes_Mallows'][-1] ))
                tester.plot(True)

    tester.print_output()

if __name__ == '__main__':
    config_dict = get_config_dict()
    data_folder = config_dict["data_folder"]

    config = config_dict['BR1']

    config['VSD2'] = True
    config['KMeans'] = False
    config['BIRCH'] = False
    config['DBSCAN'] = False
    config['HDBSCAN'] = False

    print(config)

    tester = ClusterTests(config, data_folder, 12)
    tester.compute()
    tester.plot()
