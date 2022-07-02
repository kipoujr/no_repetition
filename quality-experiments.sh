for i in 1_4 2 4 7_1 8; do
    echo -e "Starting $i-bottom-k with n=1000000, k=24500, experiments=2000\n"    
    ./run.sh $i BOTTOM_K 0 STRUCTURED 1000000 24500 2000 0 0

    echo -e "Starting $i-bottom-k with n=500000, k=24500, experiments=50000\n"   
    ./run.sh $i BOTTOM_K 0 STRUCTURED 500000 24500 50000 0 0
	
    echo -e "Starting $i-bottom-k-median (random data) with n=500000, k=24500, experiments=30000\n"   
    ./run.sh $i BOTTOM_K 1 RANDOM 500000 24500 30000 0 0


    echo -e "Starting $i-sampling with n=5000000, k=800000, experiments=50000\n"   
    ./run.sh $i SAMPLING 0 STRUCTURED 5000000 800000 50000 0 0
	
    echo -e "Starting $i-sampling with n=250000, k=160000, experiments=50000\n"   
    ./run.sh $i SAMPLING 0 STRUCTURED 250000 160000 50000 0 0
	
    echo -e "Starting $i-sampling-median (random data) with n=250000, k=160000, experiments=30000\n"   
    ./run.sh $i SAMPLING 1 RANDOM 250000 160000 30000 0 0


    #echo -e "Starting $i-sampling on real data unprocessed with n=1815365, k=17500, experiments=10000\n"    
    #./run.sh $i SAMPLING 0 REAL1 1815365 17500 10000 0 0
	#echo -e "Starting $i-sampling-median on real data unprocessed with n=1815365, k=17500, experiments=10000\n"    
    #./run.sh $i SAMPLING 1 REAL1 1815365 17500 10000 0 0
	echo -e "Starting $i-sampling on real data processed with n=648039, k=28000, experiments=30000\n"    
    ./run.sh $i SAMPLING 0 REAL_PROCESSED 648039 28000 30000 0 0
    echo -e "Starting $i-sampling-median on real data processed with n=648039, k=28000, experiments=30000\n"    
    ./run.sh $i SAMPLING 1 REAL_PROCESSED 648039 28000 30000 0 0
	
	#echo -e "Starting $i-sampling on real data unprocessed with n=12775, k=7000, experiments=50000\n"    
    #./run.sh $i SAMPLING 0 REAL2 12775 7000 50000 0 0
    #echo -e "Starting $i-sampling-median on real data unprocessed with n=12775, k=7000, experiments=50000\n"    
    #./run.sh $i SAMPLING 1 REAL2 12775 7000 50000 0 0
	echo -e "Starting $i-bottom-k on real data unprocessed with n=12775, k=7000, experiments=50000\n"    
    ./run.sh $i BOTTOM_K 0 REAL2 12775 7000 50000 0 0
    echo -e "Starting $i-bottom-k-median on real data unprocessed with n=12775, k=7000, experiments=50000\n"    
    ./run.sh $i BOTTOM_K 1 REAL2 12775 7000 50000 0 0
	
	echo -e "Starting $i-sampling on real data unprocessed with n=20000, k=7000, experiments=50000\n"    
    ./run.sh $i SAMPLING 0 ATMOSPHERIC 20000 7000 50000 0 0
    echo -e "Starting $i-sampling-median on real data unprocessed with n=20000, k=7000, experiments=50000\n"    
    ./run.sh $i SAMPLING 1 ATMOSPHERIC 20000 7000 50000 0 0
done