# MSSD: Multidimensional Skylines Over Streaming Data

### Repository organization

This repository includes MSSD sources as well as state of the art methods to answer subspace skyline queries over streaming data.

### Compile and run

To begin, please clone this repository. This software requires C++ compiler.

```shell 
git clone https://github.com/karimalami7/MSSD.git
``` 

Next, change current directory to ./src and compile the source code

```shell 
cd ./src
./compile.sh
``` 

Then, define the experiment to perform in the file [run.sh](https://github.com/MultiStreamSky/MSSD/blob/master/src/run.sh) and execute it

```shell 
./run.sh
``` 
Arguments to define in the file [run.sh](https://github.com/MultiStreamSky/MSSD/blob/master/src/run.sh) are:

1. Data type:

* INDE: Independent data.

* ANTI: Anti-correlated data.

* CORR: Correlated data.

* other: when using a real dataset.

2. In case of real dataset, input the path of the file. In case of synthetic dataset, define the number of distinct values of each dimension.

3. Omega: size of the sliding window.

4. Experiment lifetime. 

5. Number of dimensions.

6. Number of parallel threads to be run.

7. Batch interval
