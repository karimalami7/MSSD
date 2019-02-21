#/bin/bash
###############################
#
#   -f = ANTI, INDE, CORR, other
#
#	-k : if  -f == other : -k = dataset path
# 			  if  -f != other : -k = distinct values 
#	
#	-o : omega = lifetime 
#
#	-c : cycles = generations
#
#	-d : space or number of attributes
#
#	-t : NBTHREARDS = number of parallel threads to be run.
#
# 	-b : batch interval

./main -f INDE -k 100 -o 100000 -c 2 -d 6 -t 12 -b 1000

