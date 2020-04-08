# author: Karim Alami
#
# created: 22/03/2020
#
# receives tweets data each theta units of times and stores them in parquet format

from pyspark import SparkContext
from pyspark.streaming import StreamingContext
from pyspark.sql import Row, SparkSession
import sys

import twitter_data_schema

# gloqb counter for storage
id_transaction=0

# get Spark Session from Spark running context
def getSparkSessionInstance(sparkConf):
    if ('sparkSessionSingletonInstance' not in globals()):
        globals()['sparkSessionSingletonInstance'] = SparkSession\
            .builder\
            .config(conf=sparkConf)\
            .getOrCreate()
    return globals()['sparkSessionSingletonInstance']

# Convert RDDs of the DStream to DataFrame and store it
def process(time, rdd):
    print("========= %s =========" % str(time))
    global id_transaction
    try:
        # Get the singleton instance of SparkSession
        spark = getSparkSessionInstance(rdd.context.getConf())

        deltaSetDf = spark.createDataFrame(rdd, twitter_data_schema.attributes)
        fileName="receivedData//transaction_%s" % (id_transaction)
        id_transaction=id_transaction+1
        #deltaSetDf.show()
        deltaSetDf.write.save(fileName, format="parquet")
    
    except BaseException as e:
        print("Error in processing RDDs: %s" % str(e))

def Main(): 

    #### receive tuples and store them

    if len(sys.argv) != 3:
        print("Usage: receiver.py <hostname> <port>", file=sys.stderr)
        sys.exit(-1)
    sc = SparkContext(appName="PythonStreamingNetworkWordCount")
    ssc = StreamingContext(sc, 5)
    
    streamed_tuples = ssc.socketTextStream(sys.argv[1], int(sys.argv[2]))
    streamed_tuples = streamed_tuples.map(lambda tweets: tweets.split(","))
    #streamed_tuples.pprint()
    streamed_tuples.foreachRDD(process)

    ssc.start()
    ssc.awaitTermination()

if __name__=='__main__':
    Main()