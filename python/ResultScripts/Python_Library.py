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
import numpy as np
import os

import matplotlib.pyplot as plt
from matplotlib import colors
import matplotlib as mpl

import scipy
import scipy.io
from scipy.io import loadmat

from functools import reduce



def main():
    factors = get_factors(123456)
    print(factors)

    filename = "salinas_gt.npy"
    vals = load_array(filename)
    print(vals)

    color_matrix = get_color_matrix(style="custom2")
    #cmap = create_cmap(color_matrix=color_matrix)
    cmap = create_cmap(style="custom2")

    plt.imshow(vals, cmap=cmap)
    plt.colorbar()
    plt.show()


def load_array(filename, shape=None, dtype=None):

    if filename[-4:] == ".csv":
        vals = np.loadtxt(filename,dtype=dtype)
    elif filename[-4:] == ".npy":
        #vals = np.load(filename,dtype=dtype)
        vals = np.load(filename)
    elif filename[-4:] == ".mat":
        vals = scipy.io.loadmat(filename)

    if shape is not None:
        vals = np.reshape(vals,shape)

    return vals
    

def get_factors(n):    
    return set(reduce(list.__add__, 
                ([i, n//i] for i in range(1, int(n**0.5) + 1) if n % i == 0)))


def get_reshapes(vals):
    factors = get_factors(vals.size)
    reshapes = []
    for f in factors:
        reshapes.append([f,int(vals.size/f)])        
    return reshapes



"""

----------------------  DISPLAY/PROGRESS UTILITIES  ----------------------

"""

    
class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[32m'
    WARNING = '\033[91m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def warning_string(string):
    return bcolors.FAIL + string + bcolors.ENDC


def show_terminal_colors():
    color_pairs = []
    color_pairs.append(["HEADER", bcolors.HEADER])
    color_pairs.append(["OKBLUE", bcolors.OKBLUE])
    color_pairs.append(["OKCYAN", bcolors.OKCYAN])
    color_pairs.append(["OKGREEN", bcolors.OKGREEN])
    color_pairs.append(["WARNING", bcolors.WARNING])
    color_pairs.append(["FAIL", bcolors.FAIL])
    #color_pairs.append(["ENDC", bcolors.ENDC])
    color_pairs.append(["BOLD", bcolors.BOLD])
    color_pairs.append(["UNDERLINE", bcolors.UNDERLINE])

    test_string = "/\\/\\/\\/\\|  Test string for viewing terminal color options  |/\\/\\/\\/\\"

    for label, tcolor in color_pairs:
        print("\nbcolors.{:} + string + bcolors.ENDC".format(label))
        print(tcolor + test_string + bcolors.ENDC)


    print(" ")




"""

-------------------------  COLOR MAP UTILITIES  -------------------------

"""

def create_cmap(style=None, color_matrix=None, filename='./AVIRIS_colormap.mat'):

    if style is not None:
        color_matrix = get_color_matrix(style=style)
        
    if color_matrix is None:
        color_matrix = load_array(filename)
        
    color_mat_list = list(color_matrix)
    cmap = colors.ListedColormap(color_matrix)
    return cmap


def get_color_matrix(style="custom1"):

    if style == "custom1":
        # REFRENCE: https://colorbrewer2.org
        color_mat = np.array([[240,240,240],
                              [55,126,184],
                              [77,175,74],
                              [152,78,163],
                              [255,127,0],
                              [255,255,51],
                              [166,86,40],
                              [247,129,191],
                              [153,153,153],
                              [141,211,199],
                              [255,255,179],
                              [190,186,218],
                              [251,128,114],
                              [128,177,211],
                              [253,180,98],
                              [179,222,105],
                              [252,205,229]
                              ])
    elif style == "custom2":
        color_mat = np.array([[189,189,189],
                              [2,56,88],
                              [253,187,132],
                              [247,252,185],
                              [173,221,142],
                              [54,144,192],
                              [239,101,72],
                              [250,159,181],
                              [221,52,151],
                              [4,90,141],
                              [122,1,119],
                              [35,132,67],
                              [179,0,0],
                              [191,129,45],
                              [82,82,82]
                              ])
    elif style == "normal12":
        # REFERENCE: http://tsitsul.in/blog/coloropt/
        color_mat = np.array([[235, 172, 35], [184, 0, 88], [0, 140, 249], [0, 110, 0], [0, 187, 173], [209, 99, 230], [178, 69, 2], [255, 146, 135], [89, 84, 214], [0, 198, 248], [135, 133, 0], [0, 167, 108], [189, 189, 189]])

    elif style == "tarnish15":
        # REFERENCE: http://tsitsul.in/blog/coloropt/
        color_mat = np.array([[39, 77, 82], [199, 162, 166], [129, 139, 112], [96, 78, 60], [140, 159, 183], [121, 104, 128], [202, 202, 202],
                              [140,81,10], [217,240,211], [69,117,180], [197,27,125], [241,163,64], [27,158,119], [84,39,136], [120,120,120], [244,109,67]])

    elif style == "mod_Lin_3":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        #color_mat = np.array([[27, 57, 127], [0, 142, 63], [231, 247, 139]])
        color_mat = np.array([[27, 57, 127], [0, 142, 63], [247, 227, 139]])

    elif style == "mod_Lin_3_alt":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[27, 57, 127], [0, 142, 63], [172, 198, 0]])

    elif style == "mod_Lin_4":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[27, 57, 127], [0, 142, 63], [247, 227, 139], [145, 92, 54]])

    elif style == "mod_Lin_4_alt":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        #dark blue, green, light yellow, brown
        color_mat = np.array([[65, 65, 87], [0, 142, 63], [247, 227, 139], [145, 92, 54]])

    elif style == "mod_Lin_4_swap":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[0, 142, 63], [65, 65, 87], [247, 227, 139], [145, 92, 54]])



    elif style == "mod_Lin_4_swap_new":
        #dark blue, green, light yellow, brown
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[65, 65, 87], [0, 142, 63], [247, 227, 139], [145, 92, 54]])

    elif style == "mod_Lin_4_swap2":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[65, 65, 87], [145, 92, 54], [247, 227, 139], [0, 142, 63]])

    elif style == "mod_Lin_4_swap3":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[0, 142, 63], [145, 92, 54], [247, 227, 139], [65, 65, 87]])

    elif style == "mod_Lin_4_swap4":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[145, 92, 54], [0, 142, 63], [247, 227, 139], [65, 65, 87]])
    
    elif style == "mod_Lin_4_swap5":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[145, 92, 54], [247, 227, 139], [65, 65, 87], [0, 142, 63]])

    elif style == "mod_Lin_5_alt":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[65, 65, 87], [0, 142, 63], [247, 227, 139], [145, 92, 54], [255, 255, 255]])

    elif style == "mod_Lin_5_swap":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        #color_mat = np.array([[0, 142, 63], [65, 65, 87], [247, 227, 139], [145, 92, 54], [79, 188, 0]])
        color_mat = np.array([[0, 142, 63], [247, 227, 139], [255, 255, 255], [145, 92, 54], [65, 65, 87]])

    elif style == "mod_Lin_5_swap_alt":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[145, 92, 54], [247, 227, 139], [0, 142, 63], [65, 65, 87], [255, 255, 255]])

    elif style == "mod_Lin_5_swap_new":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        #color_mat = np.array([[0, 142, 63], [65, 65, 87], [247, 227, 139], [145, 92, 54], [79, 188, 0]])
        color_mat = np.array([[247, 227, 139], [0, 142, 63], [255, 255, 255], [65, 65, 87], [145, 92, 54]])

        # DEBUG COLORS
        #color_mat = np.array([[0, 142, 63], [247, 227, 139], [255, 0, 0], [145, 92, 54], [65, 65, 87]])
        #color_mat = np.array([[0, 142, 63], [247, 227, 139], [255, 0, 0], [145, 92, 54]])

    elif style == "mod_Lin_6_alt":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[65, 65, 87], [0, 142, 63], [247, 227, 139], [145, 92, 54], [255, 255, 255], [0, 0, 0]])
        
        
    elif style == "mod_Lin_7":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[4, 4, 4], [242, 194, 63], [10, 85, 122], [0, 142, 63], [79, 188, 0], [226, 207, 138], [255, 255, 255]])


    elif style == "mod_Lin_7_alt":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[4, 4, 4], [242, 194, 63], [10, 85, 122], [0, 142, 63], [84, 61, 99], [226, 207, 138], [255, 255, 255]])

    elif style == "mod_Lin_7_alt_alt":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        #[145, 92, 54]
        color_mat = np.array([[4, 4, 4], [242, 194, 63], [10, 85, 122], [0, 142, 63], [145, 92, 54], [226, 207, 138], [255, 255, 255]])

        
    elif style == "mod_Lin_10":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[4, 4, 4], [242, 194, 63], [31, 49, 121], [115, 110, 109], [0, 123, 86], [0, 160, 45], [79, 188, 0], [197, 201, 0], [247, 219, 195], [255, 255, 255]])

    elif style == "mod_Lin_10_alt":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        #color_mat = np.array([[4, 4, 4], [242, 194, 63], [31, 49, 121], [10, 85, 122], [0, 123, 86], [0, 160, 45], [79, 188, 0], [197, 201, 0], [247, 219, 195], [255, 255, 255]])
        color_mat = np.array([[4, 4, 4], [242, 194, 63], [59, 54, 92], [115, 110, 109], [0, 123, 86], [99, 194, 168], [163, 113, 140], [227, 155, 116], [212, 69, 47], [163, 186, 89]])  #[255, 255, 255]])

    elif style == "mod_Lin_10_alt_alt":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[4, 4, 4], [242, 194, 63], [31, 49, 121], [115, 110, 109], [0, 123, 86], [250, 0, 0], [79, 188, 0], [197, 201, 0], [247, 219, 195], [255, 255, 255]])
        #color_mat = np.array([[4, 4, 4], [242, 194, 63], [59, 54, 92], [115, 110, 109], [0, 123, 86], [99, 194, 168], [163, 113, 140], [227, 155, 116], [212, 69, 47], [163, 186, 89]])  #[255, 255, 255]])

    elif style == "mod_Lin_10_alt_alt_alt":
        # REFERENCE: https://jiffyclub.github.io/palettable/mycarta/
        color_mat = np.array([[4, 4, 4], [242, 194, 63], [31, 49, 121], [115, 110, 109], [0, 123, 86], [82, 186, 209], [79, 188, 0], [197, 201, 0], [247, 219, 195], [255, 255, 255]])
        
        
    color_mat = color_mat / 255.0
        

    return color_mat



def add_suplot_colorbar(fig, im, plot_adjust=0.8, position=None, y_shift=0.0, x_shift=0.0):
    # Shift plot by e.g. 80% ~ plot_adjust = 0.8
    fig.subplots_adjust(right=plot_adjust)
    if position is None:
        ###
        ###  position  =  [x_loc, y_loc, x_thick, y_thick]
        ###
        y_thick = 0.75
        y_loc = (1.0 - y_thick) / 2.0 + y_shift

        x_thick = 0.035
        left_x_pad = 0.05

        x_loc = plot_adjust + left_x_pad + x_shift
        threshold = 0.945
        if x_loc + x_thick > threshold:
            x_loc = threshold - x_thick + xshift
        
        position = [x_loc, y_loc, x_thick, y_thick]
        
    cbar_ax = fig.add_axes(position)
    fig.colorbar(im, cax=cbar_ax)



if __name__ == "__main__":
    main()
