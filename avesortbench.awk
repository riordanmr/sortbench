# avesortbench.awk - script to compute averages of sort benchmarks
# generated by sortbench.cpp.
# Input lines look like:
# name of sort        ,nrecs, seed,nanosecs ,recs/ns       ,whether sort successful
# ShellSortCiura225Odd,1000000,310,840001057,1190474.692462,true
# ShellSortCiura225Odd,1000001,301,842342466,1187166.788288,true
#
# Output records look like:
# name of sort        ,nrecs  ,ave recs/ns,nRuns,ave deviation,ratio ave dev
# ShellSortCiura225Odd,1000000,40,1192690.9,13758.4,0.01154
# which is a record containing the average for all runs with the same
# algorithm and similar record count.
#
# Data for similar numbers of records (e.g., 1000000 vs. 1000001)
# are collapsed into a single number (the smaller number, like 1000000).
#
# awk -f /Users/mrr/Documents/GitHub/sortbench/avesortbench.awk samplesortdata.csv
# awk -f /Users/mrr/Documents/GitHub/sortbench/avesortbench.awk sortbench.csv
#
# Mark Riordan 04-MAY-2023
BEGIN {
    FS = ","
    nThisType = 0
    sumRecsPerSec = 0
}

# Compute the average, and average deviation, for similar benchmark runs.
# Entry:    sumRecsPerSec   is the sum of the relevant records
#           nThisType       is the number of records
#           aryRecsPerSec   is a 1-based array of recs/sec for the
#                           relevant benchmark runs.
function procRecs() {
    # compute average recs/sec for the type of record we just encountered
    aveRecsPerSec = sumRecsPerSec / nThisType
    sumDevs = 0
    # Compute the average deviation of the prior type of records.
    for(j=1; j<=nThisType; j++) {
        deviation = aryRecsPerSec[j] - aveRecsPerSec
        if(deviation<0) deviation = -deviation
        sumDevs += deviation
    }
    aveDev = sumDevs / nThisType
    ratioAveDev = aveDev / aveRecsPerSec
    print prevName "," prevNRecs "," nThisType "," sprintf("%.1f",aveRecsPerSec) "," sprintf("%.1f",aveDev) "," sprintf("%.5f", ratioAveDev)
}

{
    name = $1
    nRecs = $2
    # Collapse all records with similar nrecs to the same number.
    # The benchmark program can use similar but slightly different record
    # counts to prevent end cases where a particular record count
    # triggers unusual behavior; we'll treat these as having the same count.
    if(length(nRecs)>2) {
        # Also change to a number, as opposed to a string.
        nRecs = 0 + (substr(nRecs, 1, length(nRecs)-1) "0")
    }
    recsPerSec = $5

    # Detect whether the current record starts a new batch of benchmark runs.
    # If so, do calculations on the just-ended batch of runs.
    if(name != prevName || nRecs != prevNRecs) {
        if(prevName != "") {
            procRecs()
            sumRecsPerSec = 0
        }
        nThisType = 0
    }
    
    nThisType++
    
    # This 1-based array contains the recs/sec for all runs with the same
    # algorithm and # of records. It's for computing average deviation.
    aryRecsPerSec[nThisType] = recsPerSec
    sumRecsPerSec += recsPerSec
    
    prevName = name
    prevNRecs = nRecs
}

END {
    # Compute results for the last batch of records.
    procRecs()
}
