# VoroClust

VoroClust is a scalable, density-based clustering algorithm that leverages sphere covers to accurately model the geometric structure of clusters.  The algorithm has one primary parameter, the radius `R` used to construct the sphere cover, and three auxiliary parameters: `detail_ceiling`, `descent_limit`, and `noise_threshold`.  The detail ceiling helps avoid breaking up clusters in high-density regions (e.g., triggered by false peaks due to noise in the empirical density estimates). The descent limit prevents clusters from expanding too far into low density regions, where outliers and anomalies may be present.  Finally, the noise threshold parameter can be specified to adjust the percentage of data that is declared noise.


# Installation

First, download the source code from the repository:
```console
$ git clone https://github.com/sandialabs/VoroClust.git
```

Change into the Python source directory, and install using pip:
```console
$ cd ./VoroClust/python
$ python -m pip install .
```


# Example Usage

```python
from voroclust import voroclust
import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("path/to/data.csv", delimiter=',')
size = data.shape[0]
dimensions = data.shape[1]

vc = voroclust(data.flatten(),
               data_size=size,
               data_dimensions = dimensions,
               radius=0.35,
               detail_ceiling=0.8,
               descent_limit=0.2,
               num_threads=12)

vc.execute()

# Choose between optional post-processing steps
vc.labelByMaxClusters(4)
#vc.labelNoise(.05)

# Retrieve labels and plot results
data_labels = vc.getLabels()
plt.scatter(data[:,0], data[:,1], c=data_labels)
plt.show()
```



## C++ Installation
----- Step 1 -----

To build the VoroClust executable:

```
mkdir build
cd build
cmake ..
make
```


----- Step 2 ----- 

To install the VoroClust python package, navigate to the python folder and run:

python -m pip install .

With Python 3.10+, it should (temporarily) install any missing dependencies
then build and install the voroclust package.

If this fails, you can install manual dependencies with...

----- Step 3 (optional) -----

```
pip install setuptools
pip install wheel
pip install "pybind11[global]"
pip install ninja
pip install cmake
```


----- Step 4 (troubleshooting) -----

If step 2 is still failing, it might be because CMake is failing to find the pybind11 dependency.
This error looks like:

      CMake Error at CMakeLists.txt:26 (find_package):
        Could not find a package configuration file provided by "pybind11" with any
        of the following names:

          pybind11Config.cmake
          pybind11-config.cmake

        Add the installation prefix of "pybind11" to CMAKE_PREFIX_PATH or set
        "pybind11_DIR" to a directory containing one of the above files.  If
        "pybind11" provides a separate development package or SDK, be sure it has
        been installed.

You can locate the pybind11 python install with the command: 

python -m pip show pybind11

Within that directory, navigate to pybind11/share/cmake/pybind11, where it should have the file pybind11Config.cmake.
You can tell CMake exactly where this is by setting the pybind11_DIR variable in python/CMakeLists.txt:

set(pybind11_DIR "path/to/pybind11Config.cmake")
