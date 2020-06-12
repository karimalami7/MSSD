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
# 	-b : batch interval
#
#	-m : method    MSSD=1   DBSky=2


#experiments with real data

./main -f other -k /home/kalami/twitter_files/processed_data/ -d 5 -o 30 -b 1 -m 1

# experiments with synthetic data

# for type in INDE 
# do
# 	for d in 8
# 	do
# 		for o in  100000
# 		do
# 			for b in 1000
# 			do
# 			#./main -f $type -k 100 -d $d -o $o -c 2 -b $b -m 2
# 			done	
# 		done
# 	done
# done
