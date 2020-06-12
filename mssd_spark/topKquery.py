# author: Karim Alami
#
# created: 09/04/2020
#
# Topk query answering through NSCt

import sys
from pyspark.sql import Row, SparkSession
from itertools import combinations, product


# 1/ load all pairs to RDD

def couv_pair(tuple_pair):

	# encode subspace by bit shift
	def subspace_encoding(subspace):
		return sum(map(lambda x: 1<<x,subspace))

	X=tuple_pair[1]["X"]
	Y=tuple_pair[1]["Y"]

	X_comb={ subspace_encoding(subspace) for i in range (1,len(X)+1) for subspace in combinations(X,i) }
	Y_comb={ subspace_encoding(subspace) for i in range (1,len(Y)+1) for subspace in combinations(Y,i) }

	XY_comb={x+y for x,y in product(X_comb,Y_comb)}

	return (tuple_pair[0],XY_comb.union(X_comb))

class NSC:

	def __init__(self, spark_session):
		self.spark_session=spark_session

	def load_pairs(self):
		self.tuple_pair=self.spark_session.read.json("NSCt"+"//transaction_*"+"//buck*"+"//part-*")\
					.rdd.map(lambda t: (t["tuple_id"], t["pairs"]["data"]))\
					.flatMapValues(lambda x: x)

	def tuples_dominance_set(self):
		self.tuple_couv= self.tuple_pair.map(lambda x: couv_pair(x)).reduceByKey(lambda x,y: x.union(y))\
		.map(lambda x: (x[0],len(x[1]))).map(lambda x: Row(tuple_id=x[0],dominance=x[1]))

def Main():

	spark = SparkSession.builder.getOrCreate()
	if len(sys.argv) != 2:
		print("Usage: topKquery.py <K>")
		sys.exit(-1)
	nsc = NSC(spark)
	print("Start loading pairs to RDDs")
	nsc.load_pairs()
	print("Start computing dominance of tuples")
	nsc.tuples_dominance_set()
	print("Creating a dataFrame")
	df = spark.createDataFrame(nsc.tuple_couv)
	df.orderBy('dominance').limit(int(sys.argv[1])).show()

if __name__=="__main__":
	Main()