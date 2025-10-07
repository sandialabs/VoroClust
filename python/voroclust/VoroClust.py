import os
import sys
import csv
import numpy as np

## REFERENCE:  https://stackoverflow.com/questions/1051254/check-if-python-package-is-installed
import importlib.util
def check_package_installed(name):
    if name in sys.modules:
        installed = True
    elif (spec := importlib.util.find_spec(name)) is not None:
        #module = importlib.util.module_from_spec(spec)
        #sys.modules[name] = module
        #spec.loader.exec_module(module)
        installed = True
    else:
        installed = False
    return installed

"""
# Check packages
SKLEARN_INSTALLED = check_package_installed("sklearn")

### Original hdbscan package seems to be crashing
#HDBSCAN_INSTALLED = check_package_installed("hdbscan")
HDBSCAN_INSTALLED = False

SKFDA_INSTALLED = check_package_installed("skfda")

# Other methods
if SKLEARN_INSTALLED: 
    import sklearn.cluster

if HDBSCAN_INSTALLED:
    import hdbscan


# Fuzzy c-means
# https://fda.readthedocs.io/en/latest/modules/ml/autosummary/skfda.ml.clustering.FuzzyCMeans.html
# python3 -m pip install scikit-fda
if SKFDA_INSTALLED:
    from skfda.ml.clustering import FuzzyCMeans
    from skfda import FDataGrid
"""

from PIL import Image

import pandas as pd
import shlex

import matplotlib as mpl
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
#from scipy.interpolate import interp2d

from matplotlib import colors

from time import perf_counter

from scipy.io import loadmat

from argparse import ArgumentParser
#print(sys.version)

from . import voroclust


class VoroClust(voroclust.voroclust):

    def __init__(self, data=None, data_filename=None, data_size=None, data_dimensions=None, radius=None, detail_ceiling=None, descent_limit=None, num_threads=1, data_tree_filename=None, num_rows=None, num_cols=None):
        #VCC.__init__(self)

        self.num_rows = num_rows
        self.num_cols = num_cols

        if data is not None:
            self.data = data
        elif data_filename is not None:
            self.data = self.load_data(data_filename)
        else:
            raise NotImplementedError("\n[*] ERROR: must provide data or data_filename as input to Voronoi_Clustering...\n")


        self.input_data = self.data
        
        self.DEBUG = False
        self.resolution_x = None
        self.resolution_y = None

        # Automatically determine dimensions if shaped array is provided
        if (data_size is None) and (data_dimensions is None):

            array_dims = len(self.data.shape)

            self.debug_print(self.data.shape)
            self.debug_print(array_dims)

            # Assume array is either (num_examples x num_dims) or ( (res_x x res_y) x num_dims)
            assert( (array_dims == 2) or (array_dims == 3) )
            if array_dims == 2:
                data_size = self.data.shape[0]
                data_dimensions = self.data.shape[1]
            else:
                self.resolution_x = self.data.shape[0]
                self.resolution_y = self.data.shape[1]
                data_size = self.data.shape[0] * self.data.shape[1]
                data_dimensions = self.data.shape[2]
                self.data = np.reshape(self.data, [data_size, data_dimensions])

            self.data = self.data.flatten()

        # Call initializer from base class
        self.debug_print("\n[*] Calling C++ __init__ method\n")
        if data_tree_filename is not None:
            voroclust.voroclust.__init__(self, data=self.data, data_size=data_size, data_dimensions=data_dimensions, 
                         radius=radius, detail_ceiling=detail_ceiling, descent_limit=descent_limit, num_threads=num_threads, data_tree_filename=data_tree_filename)
        else:
            voroclust.voroclust.__init__(self, data=self.data, data_size=data_size, data_dimensions=data_dimensions, 
                         radius=radius, detail_ceiling=detail_ceiling, descent_limit=descent_limit, num_threads=num_threads)
            
        self.debug_print("[*] Completed C++ __init__ call")
        
        # Assign attributes
        self.data = data
        self.data_size = data_size
        self.data_dimensions = data_dimensions
        self.radius = radius
        self.detail_ceiling = detail_ceiling
        self.descent_limit = descent_limit
        self.num_threads = num_threads


    def load_data(self, data_filename):
        """Load data into object from file (if passed a string) or from a NumPy array.  
        The original data shape is stored for reshaping predicted labels, and the
        input data is flattened to shape (n,d) where n is the number of example data
        points and d is the feature dimension for the dataset. 

        Parameters
        ----------
        data : NumPy array or filename string
               Input data for clustering algorithm.
        """
        
        # Check if a filename was passed and load data based on extension
        self.image_data = False
        if isinstance(data_filename, str):
            extension = os.path.splitext(data_filename)[-1]
            if extension.lower() in set([".npy"]):
                self.data = np.load(data_filename)

            elif extension.lower() in set([".csv"]):
                #assert self.num_rows is not None , "\n\n[*] ERROR: 'num_rows' must be specified for CSV input; aborting..."
                self.data = np.loadtxt(data_filename, delimiter=",")
                #self.mpi_print("CSV DATA:\n{:}\n".format(data.shape))
                if (self.num_rows is not None) and (self.num_cols is not None):
                    self.data = np.reshape(self.data, [self.num_rows, self.num_cols, -1])
                    self.image_data = True
                elif (self.num_rows is not None):
                    self.data = np.reshape(self.data, [self.num_rows, -1])
                else:
                    print("\n[*] WARNING: no input shape specified; assuming shape '{:}'\n".format(self.data.shape))
                    
            elif extension.lower() in set([".mat"]):
                #self.data = load_matlab_matrix(data_filename)
                #self.data = scipy.io.loadmat(data_filename)
                self.data = loadmat(data_filename)
            elif extension.lower() in set([".png", ".jpg", ".jpeg"]):
                #self.data = load_image(data_filename)
                # Load the image using PIL
                img = Image.open(data_filename).convert("RGB")  # Ensure it's in RGB mode
                self.data = np.array(img) / 255.0
            else:
                msg = "\n[*] Invalid extension '{:}'; currently only support '.npy', '.csv', '.mat', '.png', and '.jpeg'\n"
                raise NotImplementedError(msg.format(extension))
                

        # Store original shape information for reformatting prediction array
        print("\n[*] Input Data:")
        print(self.data.shape)
        #if self.jaccard_distance:
        #    self.mpi_print(data)

        self.original_data_shape = self.data.shape

        if len(self.data.shape) == 1:
            self.feature_dim = 1
            self.prediction_data_shape = self.data.shape[0]
        else:
            self.feature_dim = self.data.shape[-1]
            self.prediction_data_shape = list(self.data.shape[:-1]) #+ [1]
            if len(self.prediction_data_shape) == 2:
                self.image_data = True

        #print("\n[*] Reshaped Input Data:")
        #print(reshaped_data.shape)

        return self.data

    def plot_predictions(self):
        if self.image_data:
            self.plot_predictions_imshow()
        else:
            self.plot_predictions_scatter()

    def plot_predictions_imshow(self):
        
        pred_vals = self.reshape_predictions()
        fig, axes = plt.subplots(nrows=1, ncols=2, figsize=(12,8))

        CMAP = 'tab20c'
        axes[0].imshow(self.input_data)
        axes[0].set_title("Input Data", fontsize=18)
        axes[1].imshow(pred_vals, cmap=CMAP)
        axes[1].set_title("Cluster Results", fontsize=18)
        plt.show()
        
    def plot_predictions_scatter(self):
        if (self.feature_dim != 2):
            print("\n[*] WARNING: plotting is only supported for 2D data and images; skipping plots...\n")
        else:
            # Plot results
            plt.scatter(self.input_data[:,0], self.input_data[:,1], c=self.predictions)
            plt.show()
        
    ###  Omit print statements unless in debug mode
    def debug_print(self, string):
        if self.DEBUG: print(string)


    """
    def fit(self, max_clusters=99999, fixed_seed=-1):
        self.execute(max_clusters=max_clusters, fixed_seed=fixed_seed)
        self.predictions = self.getLabels()
    """
    def fit(self, fixed_seed=-1, noise_style=None, max_clusters=None, noise_threshold=None):

        # Base clustering operations
        self.execute(fixed_seed=fixed_seed)

        # Post-processing
        noise_indices = None
        if noise_style is not None:
            if noise_style == "Prune Clusters":
                start_time = perf_counter()
                self.labelByMaxClusters(max_clusters)                
                end_time = perf_counter()
                print("\n[*] Clusters pruned in {:.2f} seconds".format(end_time - start_time))                
            elif noise_style == "Assign Noise by Quantile":
                start_time = perf_counter()                
                self.labelNoise(noise_threshold)
                end_time = perf_counter()
                print("\n[*] Noise assigned in {:.2f} seconds".format(end_time - start_time))                
                

        post_start_time = perf_counter()                                
        self.predictions = self.getLabels()
        labels = np.unique(self.predictions)

        if noise_style == "Assign Noise by Quantile":
            noise_indices = (self.predictions == -1)

        post_end_time = perf_counter()                                            
        print("[*] VCC.fit() post-processing completed in {:.2f} seconds".format(post_end_time - post_start_time))


        print(f"predictions: {self.predictions.shape}")
        print(f"labels: {labels.shape}")
        
        return self.predictions, labels, noise_indices
    


    ###  Assign bottom quantile of clusters to separate 'noise' class
    def assign_noise(self, noise_threshold=0.0, noise_id=-1):

        vcpred = self.predictions

        if noise_threshold == 0.0:
            new_labels = np.unique(vcpred)
            return vcpred, new_labels, None
        
        self.debug_print(self.predictions.shape)
        self.debug_print(vcpred.shape)
        self.debug_print(np.max(np.abs(self.predictions - vcpred)))

        vcpred_new = vcpred.copy()
        labels = np.unique(vcpred)
    
        
        self.debug_print(vcpred_new.shape)
        counts = np.zeros([len(labels)])
    
        for k, label in enumerate(labels):
            ck = np.sum(vcpred == label)
            counts[k] = ck
    
        sort_inds = np.argsort(counts)
        self.debug_print(counts)
        self.debug_print(sort_inds)
    
        full_count = np.sum(counts)
    
        cutoff = noise_threshold * full_count
        self.debug_print("noise_cutoff = {:.2f}".format(cutoff))
    
        noise_count = 0
        noise_labels = []
        for k, s_ind in enumerate(sort_inds):
            self.debug_print("noise_count_{:} = {:}".format(k,noise_count))
            noise_count += counts[sort_inds[k]]
            if noise_count <= cutoff:
                #noise_labels.append(label)  ## ORIGINAL
                noise_labels.append(labels[s_ind])
            else:
                break
            
        print("\n[*] Discarding {:} labels as noise\n".format(len(noise_labels)))
        for nlabel in noise_labels:
            vcpred_new[vcpred_new == nlabel] = noise_id
            
    
        noise_indices = (vcpred_new == noise_id)
        print("Noise points: {:} / {:}".format(np.sum(noise_indices), int(full_count)))
    
        vcpred = vcpred_new
        new_labels = np.unique(vcpred)
        num_clusters = len(new_labels)
        print("\n\nNUM_CLUSTERS = {:}   (after noise assignment)\n\n".format(num_clusters))
        print(new_labels)

        self.predictions = vcpred
    
        return vcpred, new_labels, noise_indices


    def reshape_predictions(self):
        if (self.resolution_x is not None) and (self.resolution_y is not None):
            return self.predictions.reshape([self.resolution_x, self.resolution_y])
        else:
            return self.predictions


    def reshape_data(self):
        if (self.resolution_x is not None) and (self.resolution_y is not None):
            return self.data.reshape([self.resolution_x, self.resolution_y, self.data_dimensions])
        else:
            #return self.data
            return self.data.reshape([self.data_size, self.data_dimensions])            
        
        

    def get_sphere_meta(self):
        spheresVisited = self.getGraphMetadata(2)
        spheresEnabled = self.getGraphMetadata(3)
        spheresClusterId = self.getGraphMetadata(4)


        print("\nSPHERE META:\n{:}\n{:}\n{:}\n".format(spheresVisited, spheresEnabled, spheresClusterId))
        return spheresVisited, spheresEnabled, spheresClusterId

    def get_sphere_info(self):
        spheres = self.getSpheres()
        #print(spheres)

        sphere_labels = self.predictions[spheres]
        #print(sphere_labels)
        return spheres, sphere_labels

    def get_seeds(self):
        spheres, sphere_labels = self.get_sphere_info()
        spheresVisited, spheresEnabled, spheresClusterId = self.get_sphere_meta()

        enabled_seeds = []
        enabled_seed_inds = []        
        enabled_seed_labels = []
        disabled_seeds = []
        disabled_seed_labels = []
        disabled_seed_inds = []

        # Reshape data into array
        reshaped_data = self.reshape_data()
        
        for k, enabled in enumerate(spheresEnabled):
            ind = spheres[k]
            seed = reshaped_data[ind]
            label = sphere_labels[k]
            if enabled:
                enabled_seeds.append(seed)
                enabled_seed_inds.append(ind)
                enabled_seed_labels.append(label)                
            else:
                disabled_seeds.append(seed)
                disabled_seed_inds.append(ind)
                disabled_seed_labels.append(label)                

        enabled_seeds = np.array(enabled_seeds)
        disabled_seeds = np.array(disabled_seeds)        

        print(enabled_seeds.shape)
        print(self.data.shape)
        return enabled_seeds, enabled_seed_inds, enabled_seed_labels, disabled_seeds, disabled_seed_inds, disabled_seed_labels


if __name__ == "__main__":
    #total_start_time = perf_counter()
    #FLAGS, parser = getFlags()

    #if FLAGS.settings_file is not None:
    #    FLAGS = update_flags(FLAGS.settings_file, FLAGS, parser)

    #main(FLAGS)
    #total_end_time = perf_counter()
    #print("\n ( Python call completed in {:.2f} seconds )\n".format(total_end_time - total_start_time))

    # Specify algorithm hyperparameters
    R = 0.25
    detail_ceiling = 0.8
    descent_limit = 0.1
    NUM_THREADS = 8


    # Specify optional post-processing options
    noise_style = "Assign Noise by Quantile"
    max_clusters = None
    noise_threshold = 0.05

    # Initialize clustering model
    model = VoroClust(data_filename="./noisy_moons.csv",
                      radius=R,
                      detail_ceiling=detail_ceiling,
                      descent_limit=descent_limit,
                      num_threads=NUM_THREADS)

    # Fit clustering model to data
    cluster_vals, labels, noise_indices = model.fit(noise_style=noise_style,
                                                    max_clusters=max_clusters,
                                                    noise_threshold=noise_threshold)

    # Plot results
    data = model.input_data
    plt.scatter(data[:,0], data[:,1], c=cluster_vals)
    plt.show()

    
