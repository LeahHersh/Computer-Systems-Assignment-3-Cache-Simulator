# Initialize the field of input
f1=(1 4 16 64)
f2=(1 4 16 64)
f3=(4 32 128)
f6=("lru" "fifo")
f7=("gcc.trace" "swim.trace")
ctt=1

# SOURCE: https://www.freecodecamp.org/news/bash-array-how-to-declare-an-array-of-strings-in-a-bash-script/

# Make sure csim exists
make csim

# Clear result.out
echo "CSF Project 3 Results" > result.out

# Loop through the arguments
for fd1 in ${f1[@]}; do
for fd2 in ${f2[@]}; do
for fd3 in ${f3[@]}; do
for fd6 in ${f6[@]}; do
for fd7 in ${f7[@]}; do
    echo "test case " $ctt " ==> ./csim " $fd1 $fd2 $fd3 " write-allocate write-back " $fd6 " < " $fd7 " <=="
    echo "test case " $ctt " ==> ./csim " $fd1 $fd2 $fd3 " write-allocate write-back " $fd6 " < " $fd7 " <==" >> result.out
    ./csim $fd1 $fd2 $fd3 write-allocate write-back $fd5 $fd6 < $fd7 &>> result.out

    ctt=$((ctt+1))

    echo "=============="
    echo "" >> result.out

    echo "test case " $ctt " ==> ./csim " $fd1 $fd2 $fd3 " write-allocate write-through " $fd6 " < " $fd7 " <=="
    echo "test case " $ctt " ==> ./csim " $fd1 $fd2 $fd3 " write-allocate write-through " $fd6 " < " $fd7 " <==" >> result.out
    ./csim $fd1 $fd2 $fd3 write-allocate write-through $fd5 $fd6 < $fd7 &>> result.out

    ctt=$((ctt+1))

    echo "=============="
    echo "" >> result.out

    echo "test case " $ctt " ==> ./csim " $fd1 $fd2 $fd3 " no-write-allocate write-through " $fd6 " < " $fd7 " <=="
    echo "test case " $ctt " ==> ./csim " $fd1 $fd2 $fd3 " no-write-allocate write-through " $fd6 " < " $fd7 " <==" >> result.out
    ./csim $fd1 $fd2 $fd3 no-write-allocate write-through $fd5 $fd6 < $fd7 &>> result.out

    echo "=============="
    echo "" >> result.out

    ctt=$((ctt+1))
done
done
done
done
done

echo "All results completed!"
