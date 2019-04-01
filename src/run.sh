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
#
#	-m : method    NSCt=1   DBSky=2

for type in INDE ANTI

do
	for dim in 12

	do

		./main -f $type -k 100 -o 43200 -c 2 -d $dim -t 24 -b 1200 -m 1

	done

done
