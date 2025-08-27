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
NEW_DATA = False
SAVE_PDFs = True

### clear; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --GC_HiRes
### clear; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --OF_LoRes
### clear; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --BR1_LoRes

###  DBSCAN CHECKS (!)
#rsync -v $WORKSTATION:Research/Xray_Classification/Updated_Clustering/Revised_Experiments/results/nevada_road_dbscan_labels.csv ./; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --NEVADA



# rsync -v $WORKSTATION:Research/Xray_Classification/Updated_Clustering/Revised_Experiments/results/nevada_road_vc_labels.csv ./; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --NEVADA
# rsync -v $WORKSTATION:Research/Xray_Classification/Updated_Clustering/Revised_Experiments/results/pavia_university_vc_labels.csv ./; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --PAVIA
# rsync -v $WORKSTATION:Research/Xray_Classification/Updated_Clustering/Revised_Experiments/results/OF_HiRes_labels_VC.npy ./ ; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --OF_HiRes
# rsync -v $WORKSTATION:Research/Xray_Classification/Updated_Clustering/Revised_Experiments/results/salinas_VC_labels.npy ./ ; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --SALINAS

# rsync -v $WORKSTATION:Research/Xray_Classification/Updated_Clustering/Revised_Experiments/results/GC_HiRes_labels_VC.npy ./ ; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --GC_HiRes

# rsync -v $WORKSTATION:Research/Xray_Classification/Updated_Clustering/Revised_Experiments/results/GC_LoRes_labels_VC.npy ./ ; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --GC_LoRes

# rsync -v $WORKSTATION:Research/Xray_Classification/Updated_Clustering/Revised_Experiments/results/BR1_HiRes_labels_VC.npy ./ ; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --BR1_HiRes

# rsync -v $WORKSTATION:Research/Xray_Classification/Updated_Clustering/Revised_Experiments/results/BR2_HiRes_labels_VC.npy ./ ; python Class_Implementation.py --USE_CUSTOM_CMAP --MAKE_CONSECUTIVE --BR2_HiRes


SHOW_INCONSISTENT = False

import numpy as np
import sys
import os
import argparse
import matplotlib.pyplot as plt

### COLOR MAP FROM:  https://github.com/danfenghong/IEEE_TGRS_SpectralFormer
from matplotlib import colors
from scipy.io import loadmat
import matplotlib as mpl

from matplotlib.backends.backend_pdf import PdfPages
"""
with PdfPages('example.pdf') as pdf:
    plt.imshow(vals, vmin=vmin, vmax=vmax, cmap=CMAP)
    fig = plt.gcf()
    pdf.savefig(fig)
"""

### Additional colors:  https://jiffyclub.github.io/palettable/#finding-palettes
from palettable.colorbrewer.qualitative import Paired_12
from palettable.cartocolors.qualitative import Prism_10
from palettable.tableau import GreenOrange_12

from palettable.mycarta import LinearL_5
from palettable.mycarta import LinearL_7
from palettable.mycarta import LinearL_8
from palettable.mycarta import LinearL_10
from palettable.mycarta import LinearL_12
from palettable.mycarta import Cube1_7
from palettable.mycarta import Cube1_8
from palettable.mycarta import Cube1_10
from palettable.mycarta import Cube1_12

from skimage import measure
from skimage import filters
#import cv2

from functools import reduce

from Python_Library import load_array, create_cmap, add_suplot_colorbar, warning_string

from CONFIGS import get_config_dict


#MAKE_CONSECUTIVE = True

class ClusterLabels:

    def __init__(self, vals, cmap=None, MAKE_CONSECUTIVE=False, target_label_count=None):
        self.vals = vals
        self.cmap = cmap
        self.update_labels()
        self.MAKE_CONSECUTIVE = MAKE_CONSECUTIVE
        self.target_label_count = target_label_count

        self.DEBUG_LABEL_COUNTS = False

    def update_labels(self):
        self.labels = np.unique(self.vals)
        self.label_set = set(self.labels)


    def get_label_map(self, locs, labels):

        label_map = []

        mapped_sources = set()
        mapped_targets = set()
        for i, loc in enumerate(locs):
            source_label = self.vals[loc[0],loc[1]]
            target_label = labels[i]
            if (not source_label in mapped_sources):
                if (not target_label in mapped_targets):
                    label_map.append([source_label,target_label])
                    mapped_sources.add(source_label)
                    mapped_targets.add(target_label)

        return label_map
            
        
        
    def remap_labels(self, label_map):

        overflow_label = np.max(self.labels) + 1
        target_labels = [ l[1] for l in label_map ]
        target_count = len(target_labels)
        if target_count > 0:
            max_target_label = np.max(target_labels)
            overflow_label = np.max([overflow_label, max_target_label+1])

            # Check for many-to-one mappings (!)
            max_occurence = max(target_labels, key=target_labels.count)
            label_map = self.remove_repeated_targets(label_map)
            
        overflow_map = []


        DEBUG = False
        original_label_count = len(np.unique(self.vals))
        
        if DEBUG: print("labels:")
        if DEBUG: print(self.labels)
        
        if DEBUG: print("label_map:")
        if DEBUG: print(label_map)

        overflow_labels = set()
        # Check for labels being mapped over
        for label_pair in label_map:
            source_label, target_label = label_pair
            if target_label in self.label_set:
                overflow_labels.add(target_label)

        # Check to see if these labels are already being reassigned                
        for label_pair in label_map:
            source_label, target_label = label_pair
            if source_label in overflow_labels:
                overflow_labels.remove(source_label)
                
        # Assign overflow labels if needed
        for label_pair in label_map:
            source_label, target_label = label_pair
            if target_label in overflow_labels:
                overflow_map.append([target_label, overflow_label])
                overflow_label += 1

                
        new_vals = np.zeros(self.vals.shape, dtype=int)

        if DEBUG: print("overflow_map:")
        if DEBUG: print(overflow_map)
        
        unchanged_labels = set(self.labels)
        for label_pair in overflow_map:
            source_label, target_label = label_pair
            new_vals[self.vals == source_label] = target_label
            if DEBUG: print("[overflow] {:} -> {:}".format(source_label, target_label))
            if source_label in unchanged_labels:
                unchanged_labels.remove(source_label)

        self.do_not_change_labels = set()
        for label_pair in label_map:
            source_label, target_label = label_pair
            new_vals[self.vals == source_label] = target_label
            if DEBUG: print("[label_map] {:}".format(source_label))
            if source_label in unchanged_labels:
                unchanged_labels.remove(source_label)
            self.do_not_change_labels.add(target_label)

        for unchanged_label in unchanged_labels:
            new_vals[self.vals == unchanged_label] = unchanged_label


        self.vals = new_vals

        self.update_labels()

        final_label_count = len(np.unique(self.vals))
        if self.DEBUG_LABEL_COUNTS: print("[remap] original_label_count: {:}".format(original_label_count))
        if self.DEBUG_LABEL_COUNTS: print("[remap]    final_label_count: {:}".format(final_label_count))
        assert(original_label_count == final_label_count)
        
        if self.MAKE_CONSECUTIVE:
            # Remap values to range {0, ... , len(labels) - 1}
            self.make_labels_consecutive()


    def map_overlaps(self, target_vals):

        DEBUG = False
        original_label_count = len(np.unique(self.vals))        
        target_vals = target_vals.copy().astype(int)

        omit_list = self.do_not_change_labels
        label_map = []
        
        if DEBUG: print(self.labels)
        for k, label in enumerate(self.labels):
            #print("iter {:}".format(k))
            if label in omit_list:
                continue

            target_labels = np.unique(target_vals)
            overlap_mask = (self.vals == label)

            AVOID_STALL = True
            if AVOID_STALL:
                overlap_vals = target_vals[overlap_mask]
                overlap_labels = np.unique(overlap_vals)
                max_occurence = None
                max_count = 0
                for lab in overlap_labels:
                    count = np.sum(overlap_vals == lab)
                    if count > max_count:
                        max_count = count
                        max_occurence = lab
                if DEBUG: print("label {:}  ~  max_occurence {:}".format(label, max_occurence))
            else:
                overlap_labels = target_vals[overlap_mask].tolist()
                max_occurence = max(overlap_labels,key=overlap_labels.count)

            if max_occurence != label:
                if max_occurence not in omit_list:
                    label_map.append([label, max_occurence])
                    #if max_occurence not in self.label_set:
                    #    self.vals[overlap_mask] = max_occurence


        def check_valid_map(lmap):
            info = None
            lmap_length = len(lmap)
            if lmap_length == 0:
                return True, info
            target_list = [ l[1] for l in lmap ]
            max_occurence = max(target_list, key=target_list.count)
            if (lmap_length > 1) and (max_occurence > 1):
                if DEBUG: print(max_occurence)
                if DEBUG: print("[max_occurence > 1]")
                info = "repeated_targets"
                return False, info
            for l in target_list:
                if l in omit_list:
                    if DEBUG: print("[l in omit_list]")
                    return False, info
            return True, info
            



        final_label_count = len(np.unique(self.vals))
        if self.DEBUG_LABEL_COUNTS: print("[mapov] original_label_count: {:}".format(original_label_count))
        if self.DEBUG_LABEL_COUNTS: print("[mapov]    final_label_count: {:}".format(final_label_count))
        
        
        valid, info = check_valid_map(label_map)
        if valid:
            self.remap_labels(label_map)
        else:            
            if info == "repeated_targets":
                label_map = self.remove_repeated_targets(label_map)
                valid, info = check_valid_map(label_map)
                if valid:
                    self.remap_labels(label_map)
                else:
                    print(warning_string("\n\n[*] WARNING: invalid label map encountered in 'map_overlaps' (!)\n\n"))
                    if DEBUG: print(omit_list)
                    if DEBUG: print(label_map)
                        
                    
    def remove_repeated_targets(self, lmap):
        target_list = [ l[1] for l in lmap ]
        unique_target_list = np.unique(target_list)
        t_array = np.array(target_list,dtype=int)
        #lm_array = np.array(lmap,dtype=int)
        new_lmap = []
        for label in unique_target_list:
            first_index = np.where(t_array == label)[0][0]
            new_lmap.append(lmap[first_index])
                
        return new_lmap
        

    def make_labels_consecutive(self):

        DEBUG = False
        original_label_count = len(np.unique(self.vals))
        
        if self.target_label_count is not None:
            #print("[ADJUST MAX CONSECUTIVE LABEL]")
            max_consec_label = np.max([self.target_label_count, original_label_count])
        else:
            max_consec_label = original_label_count
            
        new_vals = self.vals.copy()
        unused_labels = []
        #for k in range(len(self.labels)):
        for k in range(max_consec_label):
            if (not k in self.label_set):
                unused_labels.append(k)

        #minval = np.min(self.labels)
        #if minval == -1:
        #    ZERO_INDEXED = False
        #    print(self.labels)
        #else:
        #    ZERO_INDEXED = True
                
        remapped_labels = 0
        label_map = []
        for i, label in enumerate(self.label_set):
            #if ZERO_INDEXED:
            #    if (label >= len(self.labels)) or (label < 0):
            #        label_map.append([label, unused_labels[remapped_labels]])
            #        remapped_labels += 1
            #if (label >= len(self.labels)) or (label < 0):
            if (label >= max_consec_label) or (label < 0):
                label_map.append([label, unused_labels[remapped_labels]])
                remapped_labels += 1

        #if DEBUG: print(label_map)
        
        for label_pair in label_map:
            source_label, target_label = label_pair
            new_vals[self.vals == source_label] = target_label

        self.vals = new_vals
        
        self.update_labels()

        final_label_count = len(np.unique(self.vals))
        if self.DEBUG_LABEL_COUNTS: print("[makec] original_label_count: {:}".format(original_label_count))
        if self.DEBUG_LABEL_COUNTS: print("[makec]    final_label_count: {:}".format(final_label_count))
        assert(original_label_count == final_label_count)

            
    def show_vals(self):
        print("shape = {:}".format(self.vals.shape))
        print("min = {:}  max = {:}".format(np.min(self.vals),np.max(self.vals)))
        print(np.unique(self.vals))
        img = plt.imshow(self.vals, cmap=self.cmap)
        plt.show()


        
class Viewer:

    def __init__(self, vals, cmap=None):
        self.vals = vals
        self.cmap = cmap


    def view_location(self, loc_i, loc_j, BUFFER=1, PLOT=True):

        alpha = 0.1
        alpha_mask = alpha * np.ones(self.vals.shape)
        
        alpha_mask[loc_i,loc_j] = 1.0
        #BUFFER = None
        #BUFFER = 1
        if BUFFER is not None:
            for k in range(0,BUFFER+1):
                for l in range(0,BUFFER+1):
                    alpha_mask[loc_i+l,loc_j+k] = 1.0
                    alpha_mask[loc_i-l,loc_j+k] = 1.0                
                    alpha_mask[loc_i+l,loc_j-k] = 1.0
                    alpha_mask[loc_i-l,loc_j-k] = 1.0                

        label = self.vals[loc_i,loc_j]
        #print(self.vals[loc_i,loc_j])

        if PLOT:
            img = plt.imshow(self.vals, cmap=self.cmap)
            img.set_alpha(alpha_mask)
            plt.show()
        return label

    def show_vals(self):
        print("shape = {:}".format(self.vals.shape))
        print("min = {:}  max = {:}".format(np.min(self.vals),np.max(self.vals)))
        print(np.unique(self.vals))
        img = plt.imshow(self.vals, cmap=self.cmap)
        plt.show()


    def check_locations(self, loc_list, BUFFER=1, PLOT=True):
        labels = []
        for loc in loc_list:
            label = self.view_location(loc[0], loc[1], BUFFER=BUFFER, PLOT=PLOT)
            labels.append(label)
            print("({:2},{:2})  label = {:}".format(loc[0], loc[1], label))
        return labels


class ConsistentLabeler:

    def __init__(self, target_file, file_list, file_labels, shape=None, cmap=None, dtype=float, MAKE_CONSECUTIVE=False):
        self.target_file = target_file
        self.file_list = file_list
        self.file_labels = file_labels
        self.shape = shape
        self.cmap = cmap
        self.dtype = dtype
        self.MAKE_CONSECUTIVE = MAKE_CONSECUTIVE

        self.DEBUG_LABEL_COUNTS = False
        
        self.load_data(dtype=dtype)

        
        self.viewer = Viewer(self.target_vals, cmap=self.cmap)

        if self.MAKE_CONSECUTIVE:
            self.target_cluster_labels = ClusterLabels(self.target_vals, cmap=self.cmap, MAKE_CONSECUTIVE=self.MAKE_CONSECUTIVE)
            self.target_cluster_labels.make_labels_consecutive()
            self.target_vals = self.target_cluster_labels.vals

    def load_data(self, dtype=float):
        self.target_vals = load_array(self.target_file, shape=self.shape)
        self.original_data_list = []
        for filename in self.file_list:
            vals = load_array(filename, shape=self.shape, dtype=dtype)
            #print(np.unique(vals))
            self.original_data_list.append(vals)

    def get_location_labels(self, locations, PLOT=False):
        labels = self.viewer.check_locations(locations, PLOT=PLOT)
        return labels


            
    def relabel_data(self, locations, labels):

        DEBUG = False
        
        self.relabeled_data_list = []

        target_label_count = len(np.unique(self.target_vals))

        for i, vals in enumerate(self.original_data_list):
            if DEBUG: print("[*] relabel_data (iter {:})".format(i))
            cluster_labels = ClusterLabels(vals, cmap=self.cmap, MAKE_CONSECUTIVE=self.MAKE_CONSECUTIVE, target_label_count=target_label_count)
            #cluster_labels.show_vals()

            original_label_count = len(cluster_labels.labels)
            if self.DEBUG_LABEL_COUNTS: print("\n\n[relab] original_label_count: {:}".format(original_label_count))
            
            original_labels = cluster_labels.labels
            
            label_map = cluster_labels.get_label_map(locations, labels)
            cluster_labels.remap_labels(label_map)
            #cluster_labels.show_vals()

            ### MAKE SURE LABEL COUNTS ARE NOT CHANGING
            remapped_label_count = len(cluster_labels.labels)            
            #print("[relab] remapped_label_count: {:}".format(remapped_label_count))
            assert(original_label_count == remapped_label_count)
            
            MAP_BASED_ON_OVERLAP = True
            if MAP_BASED_ON_OVERLAP:
                cluster_labels.map_overlaps(self.target_vals)
            
            
            self.relabeled_data_list.append(cluster_labels.vals)

            remapped_label_count = len(cluster_labels.labels)
            remapped_labels = cluster_labels.labels
            if DEBUG: print("original labels = {:}  ,  remapped labels = {:}".format(original_label_count, remapped_label_count))
            if DEBUG: print(original_labels)
            if DEBUG: print(remapped_labels)

            ### MAKE SURE LABEL COUNTS ARE NOT CHANGING
            if self.DEBUG_LABEL_COUNTS: print("[relab] remapped_label_count: {:}".format(remapped_label_count))
            assert(original_label_count == remapped_label_count)

        ### MAKE SURE ALL RELABELS ARE VALID
        self.assert_valid_relabels()

        
    def assert_valid_relabels(self):
        valid = True

        def valid_relabel(ovals,rvals):
            o_labels = np.unique(ovals)
            o_count = len(o_labels)
            r_labels = np.unique(rvals)
            r_count = len(r_labels)
            if (o_count != r_count):
                # Make sure label count has not changed
                return False
            for k in range(r_count):
                # Make sure new labels correspond to same pixels/region as original labels
                r_lab = r_labels[k]
                mask = (rvals == r_lab)
                o_masked = ovals[mask]
                o_lab = o_masked[0]
                if not (o_masked == o_lab).all():
                    return False
            return True
        
        for k in range(len(self.original_data_list)):
            ovals = self.original_data_list[k]
            rvals = self.relabeled_data_list[k]
            if (not valid_relabel(ovals,rvals)):
                valid = False
                break
            
        assert(valid)


    def save_pdfs(self, FLAGS):
        data_list = self.relabeled_data_list
        full_min = np.min([np.min(vals) for vals in data_list])
        full_max = np.max([np.max(vals) for vals in data_list])

        #figsize = (5,5)
        figsize = None
        fontsize = 18
        fig, ax = plt.subplots(nrows=1, ncols=1, figsize=figsize)

        # Remove all axis ticks
        ax.set_xticks([])
        ax.set_yticks([])


        for i, vals in enumerate(data_list):
            interpolation = 'nearest'
            img = ax.imshow(vals, self.cmap, vmin=full_min, vmax=full_max, interpolation=interpolation)
            #plt.tight_layout()

            #file_labels = ["Ground Truth", "BIRCH", "HDBSCAN", "VC Clustering", "DBSCAN", "K-Means"]
            method_strings = {"Ground Truth": "gt", "BIRCH": "birch", "K-Means": "kmeans", "MeanShift": "meanshift",
                              "DBSCAN": "dbscan", "HDBSCAN": "hdbscan", "VC Clustering": "vc", "VC": "vc", "KMeans": "kmeans"}
            cap_method_strings = {"Ground Truth": "GT", "BIRCH": "BIRCH", "K-Means": "KMeans", "MeanShift": "MeanShift",
                                  "DBSCAN": "DBSCAN", "HDBSCAN": "HDBSCAN", "VC Clustering": "VC", "VC": "VC", "KMeans": "KMeans"}

            if FLAGS.SALINAS:
                dataset_string = "SalinasA"
                method_strings = cap_method_strings
                method_strings["VC Clustering"] = "vc"
                method_strings["K-Means"] = "kmeans"
            elif FLAGS.PAVIA:
                dataset_string = "pavia_university"
                method_strings["VC Clustering"] = "vcc"
            elif FLAGS.NEVADA:
                dataset_string = "nevada_road"
                method_strings["VC Clustering"] = "vcc"
            elif FLAGS.BR1_HiRes:
                dataset_string = "BR1_full"
                method_strings = cap_method_strings
            elif FLAGS.BR2_HiRes:
                dataset_string = "BR2_full"
                method_strings = cap_method_strings
                method_strings["K-Means"] = "kmeans"
                method_strings["KMeans"] = "kmeans"                
            elif FLAGS.BR1_LoRes:
                dataset_string = "BR1_reduced"
            elif FLAGS.BR2_LoRes:
                dataset_string = "BR2_reduced"
            elif FLAGS.OF_HiRes:
                dataset_string = "OF"
                method_strings = cap_method_strings
                method_strings["K-Means"] = "kmeans"
                method_strings["KMeans"] = "kmeans"                
            elif FLAGS.GC_HiRes:
                dataset_string = "GC"
                method_strings = cap_method_strings
                method_strings["K-Means"] = "kmeans"
                method_strings["KMeans"] = "kmeans"                
            elif FLAGS.OF_LoRes:
                dataset_string = "OF_reduced"
            elif FLAGS.GC_LoRes:
                dataset_string = "GC_reduced"
                

            method_string = method_strings[self.file_labels[i]]
            with PdfPages('{:}_{:}.pdf'.format(dataset_string, method_string)) as pdf:
                fig = plt.gcf()
                plt.gca().set_axis_off()
                plt.subplots_adjust(top = 1, bottom = 0, right = 1, left = 0, 
                                    hspace = 0, wspace = 0)
                plt.margins(0,0)
                pdf.savefig(fig,bbox_inches='tight',pad_inches=0.0)

        
    def plot_results(self, ORIGINAL=False, SHOW_CENTERS=False, SAVE=False, prefix=None):

        
        if ORIGINAL:
            data_list = self.original_data_list
        else:
            data_list = self.relabeled_data_list

        full_min = np.min([np.min(vals) for vals in data_list])
        full_max = np.max([np.max(vals) for vals in data_list])

        if len(data_list) <= 3:
            nrows = 1
            ncols = 3
            figsize = (14,4.5)
        else:
            nrows = 2
            ncols = 3
            #figsize = (16,9)
            figsize = (14,10)
            #fontsize = 14
            
        fontsize = 18
        fig, axes = plt.subplots(nrows=nrows, ncols=ncols, figsize=figsize)

        # Handle the one-row case 
        if len(data_list) <= 3:
            #print(axes)
            axes = np.expand_dims(axes, axis=0)

        
        img_with_cbar_max = None

        # Remove all axis ticks
        for ax_row in range(nrows):
            for ax_col in range(ncols):
                axes[ax_row,ax_col].set_xticks([])
                axes[ax_row,ax_col].set_yticks([])
        
        for i, vals in enumerate(data_list):

            # Handle overflow data
            if i >= nrows*ncols:                
                continue

            ax_col = int(np.mod(i,3))
            ax_row = int((i - ax_col)/3)
            #img = axes[ax_row,ax_col].imshow(vals, self.cmap) #, vmin=full_min, vmax=full_max)

            #interpolation = 'none'
            interpolation = 'nearest'
            img = axes[ax_row,ax_col].imshow(vals, self.cmap, vmin=full_min, vmax=full_max, interpolation=interpolation)
            axes[ax_row,ax_col].set_title(self.file_labels[i], fontsize=fontsize)
            #axes[ax_row,ax_col].set_xticks([])
            #axes[ax_row,ax_col].set_yticks([])

            labels = np.unique(vals)
            print("{:17} {:}".format(self.file_labels[i], labels))

            if np.max(vals) == full_max:
                img_with_cbar_max = img

                
            if SHOW_CENTERS:
                center_info = self.get_center_info(vals)
                for label, center in center_info:
                    string = str(label)
                    #x, y = center
                    y, x = center
                    axes[ax_row,ax_col].text(x, y, string, bbox=dict(fill=True, facecolor='white', edgecolor='black', linewidth=1))

                    
        # Remove borders from unused axes
        if len(data_list) < nrows*ncols:
            for i in range(len(data_list), nrows*ncols):
                ax_col = int(np.mod(i,3))
                ax_row = int((i - ax_col)/3)
                axes[ax_row,ax_col].set_axis_off()                    

        # Remove gaps between plots
        plt.tight_layout()

        # Add shared colorbar
        #add_suplot_colorbar(fig, img_with_cbar_max)
        fig = plt.gcf()
        add_suplot_colorbar(fig, img)

        
        
        #SAVE = True
        DPI = 350
        if SAVE:
            if ORIGINAL:
                save_filename = "original_colors"
            else:
                save_filename = "revised_colors"

            if prefix is not None:
                save_filename = save_filename + "_" + prefix + ".png"
            else:
                save_filename = save_filename + ".png"

            print("\n[*] Saving plot to '{:}'\n".format(save_filename))
            plt.savefig(save_filename,dpi=DPI)
        else:
            plt.show()
            

    def get_center_info(self, vals):
        labels = np.unique(vals)
        center_info = []
        for label in labels:

            ###
            ### REFERENCE:  https://scikit-image.org/docs/stable/auto_examples/segmentation/plot_regionprops.html
            ###
            CONNECTED_COMPS = True
            if CONNECTED_COMPS:
                # apply connected component analysis to the thresholded image
                masked_vals = np.array(vals == label)
                img = measure.label(masked_vals, background=0)
                regions = measure.regionprops(img)
                components = len(regions)
                if components > 1:
                    #print("Multiple components:")
                    max_area = 0.0
                    
                    for props in regions:
                        #print(props)
                        minr, minc, maxr, maxc = props.bbox
                        area = np.abs(maxr-minr)*np.abs(maxc-minc)
                        if area > max_area:
                            max_area = area
                            center_x, center_y = props.centroid
                else:
                    center_x, center_y = regions[0].centroid
                            
            else:
                mask_x, mask_y = np.where(vals == label)
                center_x = np.mean(mask_x)
                center_y = np.mean(mask_y)
                
            
            center = [int(center_x),int(center_y)]
            center_info.append([label, center])
            #print(center)
        return center_info
        
def recolor_results(FLAGS, config, TARGET_ID=None):
    ### COLOR MAP FROM:  https://github.com/danfenghong/IEEE_TGRS_SpectralFormer
    color_mat = loadmat('./ResultScripts/AVIRIS_colormap.mat')
    color_mat_list = list(color_mat)
    color_matrix = color_mat[color_mat_list[3]] #(17,3)

    cmap_colors = ["white", "yellow", "blue", "red", "green", "pink", "purple", "blue2", "green2", "browngreen", "purple2", "lightblue", "darkblue", "darkgreen", "brown", "green3", "yellow2"]
    c_to_i = dict()
    i_to_c = []
    for k in range(len(cmap_colors)):
        c_to_i[cmap_colors[k]] = k
        i_to_c.append(cmap_colors[k])

    CMAP = colors.ListedColormap(color_matrix)


    PLOT = False
    USE_DISCRETE_CMAP = True
    #USE_DISCRETE_CMAP = FLAGS.USE_DISCRETE_CMAP
    USE_CUSTOM_CMAP = FLAGS.USE_CUSTOM_CMAP
    USE_PALETTE = FLAGS.USE_PALETTE

    RELABEL = True

    USE_LOWEST_LABELS = False

    
    SALINAS = FLAGS.SALINAS
    PAVIA = FLAGS.PAVIA
    NEVADA = FLAGS.NEVADA
    BR1_HiRes = FLAGS.BR1_HiRes
    BR2_HiRes = FLAGS.BR2_HiRes
    BR1_LoRes = FLAGS.BR1_LoRes
    BR2_LoRes = FLAGS.BR2_LoRes
    OF_HiRes = FLAGS.OF_HiRes
    GC_HiRes = FLAGS.GC_HiRes
    OF_LoRes = FLAGS.OF_LoRes
    GC_LoRes = FLAGS.GC_LoRes

    #if (not SALINAS) and (not PAVIA) and (not NEVADA):
    #    SALINAS = True

    if SALINAS:
        prefix = "salinas"
    elif PAVIA:
        prefix = "pavia"
    elif NEVADA:
        prefix = "nevada"
    elif BR1_HiRes:
        prefix = "bosque1_hi"
    elif BR2_HiRes:
        prefix = "bosque2_hi"
    elif BR1_LoRes:
        prefix = "bosque1_lo"
    elif BR2_LoRes:
        prefix = "bosque2_lo"
    elif OF_HiRes:
        prefix = "openfield_hi"
    elif GC_HiRes:
        prefix = "golfcourse_hi"
    elif OF_LoRes:
        prefix = "openfield_lo"
    elif GC_LoRes:
        prefix = "golfcourse_lo"

    print(prefix)
    
    results_folder = 'Results/'

    if SALINAS:
        file_labels = ["Ground Truth", "BIRCH", "HDBSCAN", "VC Clustering", "DBSCAN", "K-Means"]
        files = ["salinasA_gt.csv", "salinasA_BIRCH_labels.csv", "salinasA_HDBSCAN_labels.csv",
                 "salinasA_VSD2_labels.csv","salinasA_DBSCAN_labels.csv",
                 "salinasA_KMeans_labels.csv"]

        #locations = [[5,5], [15,15], [20,20], [25,27], [35,35], [44,44],[52,52], [65,65], [77,77]]
        locations = [[5,5], [20,20], [35,35], [44,44],[52,52], [65,65]]

    elif PAVIA:
        file_labels = ["Ground Truth", "BIRCH", "HDBSCAN", "VC Clustering", "DBSCAN", "K-Means"]
        files = ["pavia_gt.csv", "pavia_BIRCH_labels.csv", "pavia_HDBSCAN_labels.csv",
                "pavia_VSD2_labels.csv","pavia_DBSCAN_labels.csv",
                "pavia_KMeans_labels.csv"]

        #locations = [[325,200],[575,218],[366,36],[328,124],[400,303],[167,55],[28,149]]
        #locations = [[575,218],[325,200],[366,36],[328,124],[400,303],[167,55]]
        #locations = [[575,218],[325,200],[366,36],[400,303],[167,55]]
        locations = [[575,218],[325,200],[400,303],[167,55]]
        #locations = [[325,200],[575,218],[366,36],[328,124],[167,55]]
        #locations = [[575,218]]

    elif NEVADA:
        file_labels = ["BIRCH", "HDBSCAN", "VC Clustering", "DBSCAN", "K-Means"]
        
        files = ["nevada_BIRCH_labels.csv", "nevada_HDBSCAN_labels.csv",
                "nevada_VSD2_labels.csv","nevada_DBSCAN_labels.csv",
                "nevada_KMeans_labels.csv"]

        locations = []

        VC_ID = 2
        vc_labels = load_array(results_folder + files[VC_ID], shape=config['pixels'], dtype=int)        
        NUM_CLUSTERS = len(np.unique(vc_labels))

        
    elif BR1_HiRes:
        file_labels = ["VC","KMeans","BIRCH"]
        files = ["br1_VSD2_labels.csv","br1_KMeans_labels.csv", "br1_BIRCH_labels.csv"]
        locations = []
        
    elif BR1_LoRes:
        file_labels = ["VC","KMeans","BIRCH","HDBSCAN","DBSCAN"]
        files = ["br1_reduced_VSD2_labels.csv","br1_reduced_KMeans_labels.csv","br1_reduced_BIRCH_labels.csv","br1_reduced_HDBSCAN_labels.csv", "br1_reduced_DBSCAN_labels.csv"]

        #locations = []
        locations = [] 
        
    elif BR2_HiRes:
        file_labels = ["VC","KMeans","BIRCH"]
        files = ["br2_VSD2_labels.csv","br2_KMeans_labels.csv", "br2_BIRCH_labels.csv"]

        locations = []

        VC_ID = 0
        vc_labels = load_array(results_folder + files[VC_ID], shape=config['pixels'], dtype=int)        
        NUM_CLUSTERS = len(np.unique(vc_labels))

        
    elif BR2_LoRes:
        file_labels = ["VC","KMeans","BIRCH","HDBSCAN","DBSCAN"]
        files = ["br2_reduced_VSD2_labels.csv","br2_reduced_KMeans_labels.csv","br2_reduced_BIRCH_labels.csv","br2_reduced_HDBSCAN_labels.csv", "br2_reduced_DBSCAN_labels.csv"]

        locations = []
        
    elif GC_LoRes:
        file_labels = ["VC","KMeans","BIRCH","HDBSCAN","DBSCAN"]
        files = ["gc_reduced_VSD2_labels.csv","gc_reduced_KMeans_labels.csv","gc_reduced_BIRCH_labels.csv","gc_reduced_HDBSCAN_labels.csv", "gc_reduced_DBSCAN_labels.csv"]

        locations = []

        VC_ID = 0
        vc_labels = load_array(results_folder + files[VC_ID], shape=config['pixels'], dtype=int)        
        NUM_CLUSTERS = len(np.unique(vc_labels))
        
    elif OF_LoRes:

        file_labels = ["VC","KMeans","BIRCH","HDBSCAN","DBSCAN"]
        files = ["of_reduced_VSD2_labels.csv","of_reduced_KMeans_labels.csv","of_reduced_BIRCH_labels.csv","of_reduced_HDBSCAN_labels.csv", "of_reduced_DBSCAN_labels.csv"]
        locations = []

    elif GC_HiRes:
        file_labels = ["VC","KMeans","BIRCH"]
        files = ["gc_VSD2_labels.csv","gc_KMeans_labels.csv", "gc_BIRCH_labels.csv"]

        #locations = [[3236, 2193]]
        locations = [[1367, 483]]
        #locations = []

        VC_ID = 0
        vc_labels = load_array(results_folder + files[VC_ID], shape=config['pixels'], dtype=int)        
        NUM_CLUSTERS = len(np.unique(vc_labels))
        

    elif OF_HiRes:

        file_labels = ["VC","KMeans","BIRCH"]
        files = ["of_VSD2_labels.csv","of_KMeans_labels.csv", "of_BIRCH_labels.csv"]

        locations = []

        VC_ID = 0
        vc_labels = load_array(results_folder + files[VC_ID], shape=config['pixels'], dtype=int)        
        NUM_CLUSTERS = len(np.unique(vc_labels))
        
    else:
        print("[*] WARNING: please specify dataset") 
        sys.quit()

        
        

        
    #for k, label in enumerate(labels):
    #    print("loc {:}:  {:}".format(k,i_to_c[label]))

    files = [results_folder + fname for fname in files]


    if TARGET_ID is None:
        TARGET_ID = 0
            


    if RELABEL:
        if USE_CUSTOM_CMAP or USE_PALETTE:
            #cmap = create_cmap(style="custom2")
            #cmap = create_cmap(style="normal12")
            #cmap = create_cmap(style="tarnish15")

            if SALINAS:
                #cmap = create_cmap(style="mod_Lin_7")
                #cmap = create_cmap(style="mod_Lin_7_alt")
                cmap = create_cmap(style="mod_Lin_7_alt_alt")                
            elif PAVIA:
                VERSION = 3
                if VERSION == 1:
                    cmap = create_cmap(style="mod_Lin_10")
                elif VERSION == 2:
                    #cmap = create_cmap(style="mod_Lin_10_alt")
                    cmap = create_cmap(style="mod_Lin_10_alt_alt")
                else:
                    #cmap = create_cmap(style="mod_Lin_10_alt")
                    cmap = create_cmap(style="mod_Lin_10_alt_alt_alt")
            elif NEVADA:
                #cmap = create_cmap(style="mod_Lin_3")
                if NUM_CLUSTERS <= 4:
                    cmap = create_cmap(style="mod_Lin_4_swap5")
                elif NUM_CLUSTERS == 5:
                    cmap = create_cmap(style="mod_Lin_5_alt")
                elif NUM_CLUSTERS == 6:
                    cmap = create_cmap(style="mod_Lin_6_alt")
                else:
                    cmap = create_cmap(style="mod_Lin_10")

            elif BR2_HiRes:
                if NUM_CLUSTERS <= 4:
                    cmap = create_cmap(style="mod_Lin_4_swap4")
                    #cmap = create_cmap(style="mod_Lin_4_swap3")
                    #cmap = create_cmap(style="mod_Lin_4_swap2")
                else:
                    #cmap = create_cmap(style="mod_Lin_6_alt")
                    cmap = create_cmap(style="mod_Lin_5_swap_alt")
                    
            elif BR2_LoRes:
                cmap = create_cmap(style="mod_Lin_4_swap3")
            elif BR1_HiRes:
                cmap = create_cmap(style="mod_Lin_4_swap2")
            elif BR1_LoRes:
                #cmap = create_cmap(style="mod_Lin_4_swap")
                cmap = create_cmap(style="mod_Lin_4_swap_new")                

            elif OF_HiRes:
                #cmap = create_cmap(style="mod_Lin_4_swap3")
                if NUM_CLUSTERS <= 4:
                    cmap = create_cmap(style="mod_Lin_4_swap3")
                    #cmap = create_cmap(style="mod_Lin_5_swap")
                else:
                    cmap = create_cmap(style="mod_Lin_6_alt")
                
            elif OF_LoRes:
                #cmap = create_cmap(style="mod_Lin_4_swap")
                cmap = create_cmap(style="mod_Lin_4_swap3")
                #cmap = create_cmap(style="mod_Lin_4_alt")
            elif GC_HiRes:
                if NUM_CLUSTERS <= 5:
                    #cmap = create_cmap(style="mod_Lin_5_swap")
                    #cmap = create_cmap(style="mod_Lin_5_swap_new")
                    cmap = create_cmap(style="mod_Lin_4_swap3")
                else:
                    cmap = create_cmap(style="mod_Lin_10_alt_alt")
            elif GC_LoRes:
                #cmap = create_cmap(style="mod_Lin_4_swap")
                #cmap = create_cmap(style="mod_Lin_4_swap2")
                #cmap = create_cmap(style="mod_Lin_4_swap3")
                if NUM_CLUSTERS <= 4:
                    cmap = create_cmap(style="mod_Lin_4_swap3")
                else:
                    cmap = create_cmap(style="mod_Lin_6_alt")
                
                
                
            #cmap = Paired_12.mpl_colormap
            #cmap = Prism_10.mpl_colormap
            #cmap = GreenOrange_12.mpl_colormap

            if USE_PALETTE:
                if SALINAS:
    
                    cmap = colors.ListedColormap(LinearL_7.mpl_colors)
    
                    """
                    original_colors = LinearL_7.mpl_colors
                    custom_colors = []
                    custom_colors.append(original_colors[0])
                    custom_colors.append( (242/255.0, 194/255.0, 63/255.0)  )
                    for k in range(2,len(original_colors)):
                        custom_colors.append(original_colors[k])
                    cmap = colors.ListedColormap(custom_colors)
                    copiable_colors = [ [int(255.0*c[0]),int(255.0*c[1]),int(255.0*c[2])] for c in custom_colors]
                    print(copiable_colors)
                    """
                    
                elif PAVIA:
                    original_colors = LinearL_10.mpl_colors
                    custom_colors = []
                    custom_colors.append(original_colors[0])
                    custom_colors.append( (242/255.0, 194/255.0, 63/255.0)  )
                    #for k in range(2,len(original_colors)):
                    #    custom_colors.append(original_colors[k])
                    custom_colors.append(original_colors[2])
                    custom_colors.append( (115/255.0, 110/255.0, 109/255.0)  )
                    for k in range(4,len(original_colors)):
                        custom_colors.append(original_colors[k])
                    cmap = colors.ListedColormap(custom_colors)
    
                    
                    copiable_colors = [ [int(255.0*c[0]),int(255.0*c[1]),int(255.0*c[2])] for c in custom_colors]
                    print(copiable_colors)

                elif NEVADA:
                    original_colors = LinearL_5.mpl_colors
                    custom_colors = []
                    custom_colors.append(original_colors[1])
                    custom_colors.append(original_colors[2])
                    custom_colors.append(original_colors[3])
                    cmap = colors.ListedColormap(custom_colors)
    
                    
                    copiable_colors = [ [int(255.0*c[0]),int(255.0*c[1]),int(255.0*c[2])] for c in custom_colors]
                    print(copiable_colors)
                    
                    #cmap = colors.ListedColormap(LinearL_10.mpl_colors)
                    #cmap = Cube1_10.mpl_colormap
                
            labeler = ConsistentLabeler(files[TARGET_ID],files,file_labels, shape=config['pixels'], cmap=cmap, dtype=int, MAKE_CONSECUTIVE=FLAGS.MAKE_CONSECUTIVE)
        elif USE_DISCRETE_CMAP:
            #labeler = ConsistentLabeler(files[0],files,file_labels, shape=original_shape, cmap=CMAP, dtype=int)
            labeler = ConsistentLabeler(files[TARGET_ID],files,file_labels, shape=config['pixels'], cmap='tab20c', dtype=int, MAKE_CONSECUTIVE=FLAGS.MAKE_CONSECUTIVE)
        else:
            labeler = ConsistentLabeler(files[TARGET_ID],files,file_labels, shape=config['pixels'], cmap=None, dtype=int, MAKE_CONSECUTIVE=FLAGS.MAKE_CONSECUTIVE)
        labels = labeler.get_location_labels(locations)

        if USE_LOWEST_LABELS:
            labels = [int(n) for n in range(len(labels))]

        SHOW_CENTERS = False
        labeler.relabel_data(locations, labels)

        if SAVE_PDFs:
            labeler.save_pdfs(FLAGS)
        else:
            if SHOW_INCONSISTENT:
                labeler.plot_results(ORIGINAL=True, SHOW_CENTERS=SHOW_CENTERS, SAVE=FLAGS.SAVE, prefix=prefix)
            labeler.plot_results(SHOW_CENTERS=SHOW_CENTERS, SAVE=FLAGS.SAVE, prefix=prefix)

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--model", default=None, type=int, help="Model to plot")
    parser.add_argument("--model_dir", default=None, type=str, help="Model directory with arrays to plot")
    parser.add_argument("--PAVIA", default=False, action="store_true", help="Animate UQ")
    parser.add_argument("--BR1_HiRes", default=False, action="store_true", help="Animate UQ")
    parser.add_argument("--BR2_HiRes", default=False, action="store_true", help="Animate UQ")    
    parser.add_argument("--BR1_LoRes", default=False, action="store_true", help="Animate UQ")
    parser.add_argument("--BR2_LoRes", default=False, action="store_true", help="Animate UQ")    
    parser.add_argument("--OF_HiRes", default=False, action="store_true", help="Animate UQ")
    parser.add_argument("--GC_HiRes", default=False, action="store_true", help="Animate UQ")    
    parser.add_argument("--OF_LoRes", default=False, action="store_true", help="Animate UQ")
    parser.add_argument("--GC_LoRes", default=False, action="store_true", help="Animate UQ")    
    parser.add_argument("--NEVADA", default=False, action="store_true", help="Animate UQ")    

    parser.add_argument("--USE_DISCRETE_CMAP", default=False, action="store_true", help="Animate UQ")

    #parser.add_argument("--MAKE_CONSECUTIVE", default=False, action="store_true", help="Animate UQ")
    #parser.add_argument("--USE_CUSTOM_CMAP", default=False, action="store_true", help="Animate UQ")
    parser.add_argument("--MAKE_CONSECUTIVE", default=True, action="store_true", help="Animate UQ")
    parser.add_argument("--USE_CUSTOM_CMAP", default=True, action="store_true", help="Animate UQ")

    
    parser.add_argument("--USE_PALETTE", default=False, action="store_true", help="Animate UQ")    
    parser.add_argument("--SALINAS", default=False, action="store_true", help="Animate UQ")
    parser.add_argument("--SAVE", default=False, action="store_true", help="Animate UQ")    
    parser.add_argument("--x_loc", default=0.45, type=float, help="Specify the x-location of sinks")
    FLAGS = parser.parse_args()
    

    config_dict = get_config_dict()
    data_folder = config_dict["data_folder"]

    FLAGS.SALINAS = True
    config = config_dict['SalinasA']

    recolor_results(FLAGS, config, 0)

