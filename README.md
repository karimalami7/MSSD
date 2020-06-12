# MSSD: Multidimensional Skylines Over Streaming Data

### Repository organization

This repository includes MSSD sources as well as state of the art methods to answer subspace skyline queries over streaming data.

If you consider **MSSD** in your work, please cite:

    @article{alami2020framework,
      title={A framework for multidimensional skyline queries over streaming data},
      author={Alami, Karim and Maabout, Sofian},
      journal={Data \& Knowledge Engineering},
      pages={101792},
      year={2020},
      publisher={Elsevier}
    }

### Compile and run C++ implementation

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

### Run Spark implementation

## Start services for standalone cluster

```shell
{spark_home}/sbin/start-all.sh
{kafka_home}/bin/zookeeper-server-start.sh config/zookeeper.properties
{kafka_home}/bin/kafka-server-start.sh config/server.properties
```

## Run with twitter API through TCP

```shell
cd twitter_api_pipeline/
python3 twitter_api_pipeline.py
```
```shell
cd mssd_spark/
{spark_home}/bin/spark-submit TcpSocket_receiver.py localhost 9010
```
## Run with data from Kafka

```shell
cd mssd_spark/
python3 kafka_producer.py
{spark_home}/bin/spark-submit --packages org.apache.spark:spark-streaming-kafka-0-8-assembly_2.11:2.4.3 Kafka_receiver.py localhost:9092 twitter_topic
```