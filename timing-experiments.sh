for i in 1_4 2 4 7_1 8; do
    echo -e "Starting $i-bottom-k with n=50000000, k=3500, experiments=10\n"    
    ./run.sh $i BOTTOM_K 0 STRUCTURED 50000000 3500 10 1 0
	echo -e "Starting $i-bottom-k with n=50000000, k=3500, experiments=10 (median trick)\n"
	./run.sh $i BOTTOM_K 1 STRUCTURED 50000000 3500 10 1 0

	echo -e "Starting $i-sampling with n=1000000000, k=800000, experiments=10\n"   
    ./run.sh $i SAMPLING 0 STRUCTURED 1000000000 800000 10 1 0
	echo -e "Starting $i-sampling with n=5000000, k=800000, experiments=10 (median trick)\n"   
    ./run.sh $i SAMPLING 1 STRUCTURED 5000000 800000 10 1 0

	

	echo -e "Starting $i-sampling on real data processed with n=648039, k=28000, experiments=10\n"    
    ./run.sh $i SAMPLING 0 REAL_PROCESSED 648039 28000 10 1 0
    echo -e "Starting $i-sampling-median on real data processed with n=648039, k=28000, experiments=10\n" 
    ./run.sh $i SAMPLING 1 REAL_PROCESSED 648039 28000 10 1 0

	echo -e "Starting $i-bottom-k on real data unprocessed with n=12775, k=7000, experiments=10\n"  
    ./run.sh $i BOTTOM_K 0 REAL2 12775 7000 10 1 0
    echo -e "Starting $i-bottom-k-median on real data unprocessed with n=12775, k=7000, experiments=10\n"   
    ./run.sh $i BOTTOM_K 1 REAL2 12775 7000 10 1 0
	
	echo -e "Starting $i-sampling on real data unprocessed with n=20000, k=7000, experiments=10\n"    
    ./run.sh $i SAMPLING 0 ATMOSPHERIC 20000 7000 10 1 0
    echo -e "Starting $i-sampling-median on real data unprocessed with n=20000, k=7000, experiments=10\n"  
    ./run.sh $i SAMPLING 1 ATMOSPHERIC 20000 7000 10 1 0
done