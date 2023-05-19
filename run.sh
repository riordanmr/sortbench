rm sortbench.csv.old2
mv sortbench.csv.old1 sortbench.csv.old2
mv sortbench.csv sortbench.csv.old1
export BENCHARGS="-loopct:1000 -seed:9200 "
echo $BENCHARGS
time /Users/mrr/Library/Developer/Xcode/DerivedData/sortbench-ctcrgrohjviqkchcipkjtnucyeou/Build/Products/Release/sortbench $BENCHARGS
echo $BENCHARGS
./proc.sh
