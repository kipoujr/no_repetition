for i in 1_4 2 4 7_1 8; do
    echo -e "Starting $i-bottom-k with n=50000000, k=3500, experiments=10\n"    
    ./run.sh $i BOTTOM_K 0 STRUCTURED 50000000 3500 10 1 0
	echo -e "Starting $i-bottom-k with n=50000000, k=3500, experiments=10 (median trick)\n"
	./run.sh $i BOTTOM_K 1 STRUCTURED 50000000 3500 10 1 0

	echo -e "Starting $i-sampling with n=1000000000, k=800000, experiments=10\n"   
    ./run.sh $i SAMPLING 0 STRUCTURED 1000000000 800000 10 1 0
	echo -e "Starting $i-sampling with n=5000000, k=800000, experiments=10 (median trick)\n"   
    ./run.sh $i SAMPLING 1 STRUCTURED 5000000 800000 10 1 0
done