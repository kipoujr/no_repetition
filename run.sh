# First argument: 1_4 or 1_8 for tab1perm, 2 for poly2, 3 for poly3, 4 for multihash, 5_4 or 5_8 for simpletab, 6_4 or 6_8 for twistedtab, 7_4 or 7_8 for mixedtab, 8 for murmur
# Second argument: Which algorithm to use. It should be either SAMPLING or BOTTOM_K
# Third argument: Whether the median trick should be used. Set 1 for true and 0 for false.
# Fourth argument: Which data set to use. It should be either STRUCTURED, RANDOM, or REAL.
# Fifth argument: Cardinality of the input
# Sixth argument: A constant c indicating we are allowed space ~c/e^2
# Seventh argument: Number of repetitions
# Eighth argument: Whether the experiment should be timed. Set 1 for true and 0 for false. Default is 1.
# Ninth argument: Whether indidividual experiments should be timed. Set 1 for true and 0 for false. Default is 0.

echo -e '\n\n\n' >> res/times

if [ $3 = '1' ]
then
    alg_name=$2-'median'
else
    alg_name=$2
fi

name=$alg_name'-'$4'-'$5'-'$6'-'$7'-'$1

echo $1 $alg_name $4 $5 $6 $7 >> res/times

count_time=${8:-1}
individual_time=${9:-0}

if [ $1 = '10' ]
then
    #g++ -DMYHASH10 -DALG_$2 -DMEDIAN=$3 -D$4_DATA -DCOUNT_TIME=$count_time -DINDIVIDUAL_TIME=$individual_time -o a.out --std=c++11 -Wl,-z,stack-size=33554432 -O3 src/test.cpp src/framework/blake3/blake3.c src/framework/blake3/blake3_dispatch.c src/framework/blake3/blake3_portable.c src/framework/blake3/blake3_sse2_x86-64_unix.S src/framework/blake3/blake3_sse41_x86-64_unix.S src/framework/blake3/blake3_avx2_x86-64_unix.S src/framework/blake3/blake3_avx512_x86-64_unix.S
    g++ -DMYHASH10 -DALG_$2 -DMEDIAN=$3 -D$4_DATA -DCOUNT_TIME=$count_time -DINDIVIDUAL_TIME=$individual_time -o a.out --std=c++11  -Wl,--stack,33554432 -O3 src/test.cpp src/framework/blake3/blake3.c src/framework/blake3/blake3_dispatch.c src/framework/blake3/blake3_portable.c src/framework/blake3/blake3_sse2_x86-64_unix.S src/framework/blake3/blake3_sse41_x86-64_unix.S src/framework/blake3/blake3_avx2_x86-64_unix.S src/framework/blake3/blake3_avx512_x86-64_unix.S
else
    #g++ -DMYHASH$1 -DALG_$2 -DMEDIAN=$3 -D$4_DATA -DCOUNT_TIME=$count_time -DINDIVIDUAL_TIME=$individual_time -o a.out --std=c++11 -Wl,-z,stack-size=33554432 -O3 src/test.cpp
	g++ -DMYHASH$1 -DALG_$2 -DMEDIAN=$3 -D$4_DATA -DCOUNT_TIME=$count_time -DINDIVIDUAL_TIME=$individual_time -o a.out --std=c++11  -Wl,--stack,33554432 -O3 src/test.cpp
fi

if [ $count_time = '1' ]
then
    (./a.out $5 $6 $7) 1>> res/times
else
    ./a.out $5 $6 $7
fi

mv tests.txt res/$name.txt

if [ $individual_time = '1' ]
then
    mv timesPerExperiment res/$name'-times.txt'
fi
