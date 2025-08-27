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

from voroclust import voroclust
import numpy as np
import matplotlib.pyplot as plt

###print full documentation
help(voroclust)

data = np.loadtxt("path/to/data.csv", delimiter=',')
size = data.shape[0]
dimensions = data.shape[1]

vc = voroclust(data.flatten(),
               data_size=size,
               data_dimensions = dimensions,
               radius=.35,
               detail_ceiling=.8,
               descent_limit=.2,
               num_threads=12,
               data_tree_filename="")

###skips sphere generation, to allow faster tuning of secondary parameters (radius is ignored)
#vc.loadSpheres("spheres.bin")

vc.execute()

###Choice of post-processing steps
vc.labelByMaxClusters(4)
#vc.labelNoise(.05)

###optionally, write some data to files which can be loaded on subsequent runs to save time
#vc.writeDataTree("tree.bin")
#vc.writeSpheres("spheres.bin")

data_labels = vc.getLabels()

###plotting results if it's just a 2D dataset
plt.scatter(data[:,0], data[:,1], c=data_labels)
plt.show()

###for higher dimensional data, results can be plotted by reshaping and using labels to color pixels of an image
# pixel_rows=628
# pixel_columns=557
# labels_reshaped = np.reshape(data_labels, (pixel_rows, pixel_columns))  
# plt.imshow(labels_reshaped, interpolation="none")
# plt.show()
