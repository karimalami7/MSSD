# author: Karim Alami
#
# created: 23/03/2020
#
# NSCt update Process

import sys
from pyspark.sql import Row, SparkSession


def compute_pairs(pair_of_tuples):
	x=pair_of_tuples[0]
	y=pair_of_tuples[1]
	dim_dominated=list()
	dim_equal=list()
	if x[0] != y[0]:
		for i in range(0, len(x[1])):
			if x[1][i]>y[1][i]:
				dim_dominated.append(i)
			if x[1][i]==y[1][i]:
				dim_equal.append(i)
		return (x[0],{"X":dim_dominated,"Y":dim_equal})


#This part is specific to each dataset, the output is: (tuple_id, [list of values wrt which the skyline is computed])
def Data_DfToTuple(t):
	return (t["id"],[t["#followers"], t["#following"], t["#lists"],\
					t["#likes"], t["#tweets"]])

class NSC_update_process:

	def __init__(self, new_transaction_id, spark_session):
		self.new_transaction_id=new_transaction_id
		self.spark_session= spark_session
		self.new_tuples_rdd=self.load_data_to_rdd(self.new_transaction_id)
		self.valid_tuples_rdd=[self.load_data_to_rdd(i) for i in range (0,self.new_transaction_id+1)]

	def load_data_to_rdd(self,transaction_id):
		transaction_path="receivedData//transaction_"+str(transaction_id)
		return self.spark_session.read.parquet(transaction_path+"//part-*").rdd.map(Data_DfToTuple)

	def pairs_computation(self, transactionP, transactionP_id, transactionS, transactionS_id):

		# Pairs computation
		tuple_pairs_rdd=transactionP.cartesian(transactionS).map(compute_pairs).filter(lambda item: item!=None).groupByKey()
		
		# Transformation to dataFrame and storage
		tuple_pairs_rdd_row=tuple_pairs_rdd.map(lambda p: Row(tuple_id=p[0], pairs=p[1]))
		tuple_pairs_df=self.spark_session.createDataFrame(tuple_pairs_rdd_row)
		tuple_pairs_df.repartition(1).write.save("NSCt//transaction_"+str(transactionP_id)+"//buck"+str(transactionS_id), format="json")

	def computePairs_Ntuple(self):

		for i in range(0,self.new_transaction_id+1):
			self.pairs_computation(self.new_tuples_rdd, self.new_transaction_id, self.valid_tuples_rdd[i], i)

	def computePairs_VTuple(self):

		for i in range(0,self.new_transaction_id):
			self.pairs_computation(self.valid_tuples_rdd[i], i, self.new_tuples_rdd, self.new_transaction_id)


def Main():

	if len(sys.argv) != 2:
		print("Usage: NSC_UpdateProcess.py <new_transaction_id>")
		sys.exit(-1)

	print("New transaction id is: ", sys.argv[1])
	spark = SparkSession.builder.getOrCreate()
	print("Start loading data to RDDs")
	nsc_update_session= NSC_update_process(int(sys.argv[1]), spark)
	print("Computing Pairs for Newly inserted tuples")
	nsc_update_session.computePairs_Ntuple() # compute pairs of newly inserted tuples wrt valid tuples
	print("Computing Pairs for Valid tuples")
	nsc_update_session.computePairs_VTuple() # compute pairs of valid tuple wrt new tuples


if __name__=="__main__":
	Main()