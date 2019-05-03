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

for type in INDE 

do

	for d in 8

	do

		for o in  100000

		do

			for b in 1000

			do

			./main -f $type -k 100 -o $o -c 2 -d $d -t 24 -b $b -m 1 #2> "../experiments/m-1-type-$type-dim-$d-o-$o-b-$b-qa"

			done	

		done

	done

done
