#First argument: 1_4 or 1_8 for tab1perm, 2 for poly2, 3 for poly3, 4 for multihash, 5_4 or 5_8 for simpletab, 6_4 or 6_8 for twistedtab, 7_4 or 7_8 for mixedtab, 8 for murmur
#Second argument: The source, that is src/sampling.cpp or src/bottom-k.cpp
#Third argument: cardinality of the input
#Fourth argument: A constant c indicating we are allowed space ~c/e^2
#Fifth argument: Number of repetitions
echo -e '\n\n\n' >> times
tmp2=$2
modified2=${tmp2#"src/"}
modified2=${modified2%".cpp"}
echo $1 $modified2 $3 $4 $5 $6 >> times

if [ $1 = '10' ]
then
    g++ -D $6 -D MYHASH10 --std=c++11 -Wl,-z,stack-size=33554432 -O3 $2 src/framework/blake3/blake3.c src/framework/blake3/blake3_dispatch.c src/framework/blake3/blake3_portable.c src/framework/blake3/blake3_sse2_x86-64_unix.S src/framework/blake3/blake3_sse41_x86-64_unix.S src/framework/blake3/blake3_avx2_x86-64_unix.S src/framework/blake3/blake3_avx512_x86-64_unix.S
else
    g++ -D $6 -D MYHASH$1 --std=c++11 -Wl,-z,stack-size=33554432 -O3 $2
fi

(time ./a.out $3 $4 $5) 2>> times
mv tests.txt $1'-'$modified2'-'$3'-'$4'-'$5.txt
mv timesPerExperiment times$1'-'$modified2'-'$3'-'$4'-'$5.txt
