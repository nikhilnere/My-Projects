clear
javac com/sharedmemory/SharedMemorySort.java

rm dataset
rm splits/merge_9_0
echo "Generating Dataset"
./gensort -a 100000000 dataset
echo "============================================"
echo " "

echo "Starting SharedMemory code with 1 thread"
java com.sharedmemory.SharedMemorySort dataset 1
echo "--------------------"
echo "Running valsort on the result"
./valsort splits/merge_9_0
echo "--------------------"
echo "First 10 lines :"
head -10 splits/merge_9_0
echo "--------------------"
echo "Last 10 lines :"
tail -10 splits/merge_9_0
rm splits/merge_9_0
echo "============================================"
echo " "

echo "Starting SharedMemory code with 2 threads"
java com.sharedmemory.SharedMemorySort dataset 2
echo "--------------------"
echo "Running valsort on the result"
./valsort splits/merge_9_0
echo "--------------------"
echo "First 10 lines :"
head -10 splits/merge_9_0
echo "--------------------"
echo "Last 10 lines :"
tail -10 splits/merge_9_0
rm splits/merge_9_0
echo "============================================"
echo " "

echo "Staring sharedMemory code with 4 threads"
java com.sharedmemory.SharedMemorySort dataset 4
echo "--------------------"
echo "Running valsort on the result" 
./valsort splits/merge_9_0
echo "--------------------"
echo "First 10 lines :"
head -10 splits/merge_9_0
echo "--------------------"
echo "Last 10 lines :"
tail -10 splits/merge_9_0
rm splits/merge_9_0
echo "============================================"
echo " "

echo "Staring sharedMemory code with 6 threads"
java com.sharedmemory.SharedMemorySort dataset 6
echo "--------------------"
echo "Running valsort on the result" 
./valsort splits/merge_9_0
echo "--------------------"
echo "First 10 lines :"
head -10 splits/merge_9_0
echo "--------------------"
echo "Last 10 lines :"
tail -10 splits/merge_9_0
rm splits/merge_9_0
echo "============================================"
echo " "

echo "Staring sharedMemory code with 8 threads"
java com.sharedmemory.SharedMemorySort dataset 8
echo "--------------------"
echo "Running valsort on the result"
./valsort splits/merge_9_0
echo "--------------------"
echo "First 10 lines :"
head -10 splits/merge_9_0
echo "--------------------"
echo "Last 10 lines :"
tail -10 splits/merge_9_0
rm splits/merge_9_0
echo "============================================"
echo " "

exit 0

