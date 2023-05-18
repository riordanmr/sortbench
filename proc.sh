awk -f /Users/mrr/Documents/GitHub/sortbench/avesortbench.awk sortbench.csv >sortbenchave.csv
SORTBENCH_RES=sortbench1m-results.csv
grep ",1000000," sortbenchave.csv >$SORTBENCH_RES
wc $SORTBENCH_RES
cat $SORTBENCH_RES
