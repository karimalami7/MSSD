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
#	-d : d = space or number of attributes
#
#	-t : NBTHREARDS = number of parallel threads to be run.
#
# 	-b : batch interval

./main -f INDE -k 100 -o 43200 -c 2 -d 8 -t 4 -b 600
