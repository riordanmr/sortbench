# mkmdtable.awk - Create a Markdown table summarizing the results
# of sorting benchmark runs.
# The input file comes from running run.sh, then
#   sort -t, -n -k4 -r sortbench1m-md5rng-seed9200-results.csv
# to sort the runs in decreasing order of performance.
# Input consists of lines like:
# ShellSortCiura225,1000000,1000,1240309.2,6588.4,0.00531
#
# Usage:  sort -t, -n -k4 -r sortbench1m-md5rng-seed9200-results.csv | awk -f mkmdtable.awk 
#
# Mark Riordan 2023-05-17
BEGIN {
    FS = ","
}

{
    aryRecs[NR] = $0
}

END {
    print "| Gap sequence | Recs/sec | Ave dev | Rel Perf |"
    print "|---------------|----------|---------|---------|"
    split(aryRecs[NR], fields)
    worstPerf = fields[4]
    for(j=1; j<=NR; j++) {
        split(aryRecs[j], fields)
        gapName = substr(fields[1],10)
        recsPerSec = fields[4]
        relPerf = recsPerSec / worstPerf
        aveDevPct = fields[6] * 100
        print "| " gapName " | " recsPerSec " | " aveDevPct "% | " sprintf("%.4f",relPerf) " | "
    }
}
