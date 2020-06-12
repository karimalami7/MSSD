# author: Karim Alami
#
# created: 22/03/2020
#
# (i) consumes data each theta units of times from Kafka, (ii) updates the structure and (iii) stores everything in parquet format

from pyspark import SparkContext
from pyspark.streaming import StreamingContext
from pyspark.sql import Row, SparkSession
from pyspark.streaming.kafka import KafkaUtils
import sys

import twitter_data_schema
import NSC_UpdateProcess

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

        print("New transaction id is: ", id_transaction)

        deltaSetDf = spark.createDataFrame(rdd, twitter_data_schema.attributes)
        fileName="receivedData//transaction_%s" % (id_transaction)
        #deltaSetDf.show()
        deltaSetDf.write.save(fileName, format="parquet")

        print("Start loading data to RDDs")
        nsc_update_session= NSC_UpdateProcess.NSC_update_process(id_transaction, spark)
        print("Computing Pairs for Newly inserted tuples")
        nsc_update_session.computePairs_Ntuple() # compute pairs of newly inserted tuples wrt valid tuples
        print("Computing Pairs for Valid tuples")
        nsc_update_session.computePairs_VTuple() # compute pairs of valid tuple wrt new tuples
    
    except BaseException as e:
        print("Error in processing RDDs: %s" % str(e))
    
    id_transaction=id_transaction+1

def Main(): 

    #### receive tuples and store them

    if len(sys.argv) != 3:
        print("Usage: receiver.py <kafka_port> <topic>")
        sys.exit(-1)
    sc = SparkContext(appName="MSSD with kafka stream")
    sc.addPyFile("NSC_UpdateProcess.py")
    ssc = StreamingContext(sc, 5)
    

    brokers, topic = sys.argv[1:]
    streamed_tuples = KafkaUtils.createDirectStream(ssc, [topic], {"metadata.broker.list": brokers})
    streamed_tuples = streamed_tuples.map(lambda tweets: tweets[1].split(","))
    #streamed_tuples.pprint()
    streamed_tuples.foreachRDD(process)

    ssc.start()
    ssc.awaitTermination()

if __name__=='__main__':
    Main()