############# SAMPLING BASED METHOD ###################
szSmpl=1000000000
szBot=50000000
szMed=5000000
szHeavy=5000000
sz1=648039
#sz1=1000
sz2=1815365
#sz2=1000
expSmpl=100
expBot=100
expMed=100
expHeavy=100
expData=10000
expDataPre=30000

for i in 1_4 2 4 ; do
    echo -e "Starting $i-sampling-median with n=$szMed, k=100k0, experiments=$expMed\n"    
    bash run.sh $i src/median-sampling.cpp $szMed 100 $expMed COUNT_TIME;

    echo -e "Starting $i-bottom-k-median with n=$szMed, k=k0, experiments=$empMed\n"
    bash run.sh $i src/median-bk.cpp $szMed 1 $expMed COUNT_TIME;
    
    echo -e "Starting $i-sampling-median on real data, with n=$sz1, k=40k0, experiments=$exp2\n"
    bash run.sh $i src/realData-sampling-median.cpp $sz1 40 $expDataPre COUNT_TIME;

    echo -e "Starting $i-sampling on real data, with n=$sz2, k=25k0, experiments=$exp2\n"
    bash run.sh $i src/realData-sampling-median.cpp $sz2 25 $expData COUNT_TIME;
done;
    
for i in 1_4 2 3 4 6_4 7 8 ; do
    echo -e "Starting $i-sampling with n=$szSmpl, k=100k0, experiments=$expSmpl\n"    
    bash run.sh $i src/sampling.cpp $szSmpl 100 $expSmpl COUNT_TIME;

    echo -e "Starting $i-bottom-k with n=$szBot, k=k0, experiments=$expBot\n"   
    bash run.sh $i src/bottom-k.cpp $szBot 1 $expBot COUNT_TIME;    
    
    echo -e "Starting $i-sampling-median on real data, with n=$sz1, k=40k0, experiments=$exp2\n"
    bash run.sh $i src/realData-sampling.cpp $sz1 40 $expDataPre COUNT_TIME;

    echo -e "Starting $i-sampling on real data, with n=$sz2, k=40k0, experiments=$exp2\n"
    bash run.sh $i src/realData-sampling.cpp $sz2 25 $expData COUNT_TIME;
done;


#Costly poly<100>
echo -e "Starting Poly100-sampling, with n=$szHeavy, k=40k0, experiments=$expHeavy\n"
bash run.sh 9 src/sampling.cpp $szHeavy 20 $expHeavy COUNT_TIME;

#Super costly blake3
echo -e "Starting Blake3-sampling, with n=$szHeavy, k=40k0, experiments=$expHeavy\n"
bash run.sh 10 src/sampling.cpp $szHeavy 20 $expHeavy COUNT_TIME;
