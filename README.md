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
