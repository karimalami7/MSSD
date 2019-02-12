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

./main -f INDE -k 100 -o 43200 -c 2 -d 12 -t 4 -b 1200

./main -f INDE -k 100 -o 43200 -c 2 -d 14 -t 4 -b 1200

./main -f INDE -k 100 -o 43200 -c 2 -d 16 -t 4 -b 1200

./main -f INDE -k 100 -o 86400 -c 2 -d 12 -t 4 -b 1200

./main -f INDE -k 100 -o 86400 -c 2 -d 14 -t 4 -b 1200

./main -f INDE -k 100 -o 86400 -c 2 -d 16 -t 4 -b 1200

./main -f INDE -k 100 -o 432000 -c 2 -d 12 -t 4 -b 12000

./main -f INDE -k 100 -o 432000 -c 2 -d 14 -t 4 -b 12000

./main -f INDE -k 100 -o 432000 -c 2 -d 16 -t 4 -b 12000

./main -f INDE -k 100 -o 864000 -c 2 -d 12 -t 4 -b 12000

./main -f INDE -k 100 -o 864000 -c 2 -d 14 -t 4 -b 12000

./main -f INDE -k 100 -o 864000 -c 2 -d 16 -t 4 -b 12000