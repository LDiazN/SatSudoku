#!/bin/bash

# Validate commandline
if [ $# -ne 3 ] ; then 
    echo -e "Invalid commandline.\nUsage: ./orchestrate <in-path> <timeout> <solver>"
    echo -e "\t<in-path>: File to draw instances from."
    echo -e "\t<timeout>: Integer number of seconds to wait for solver. Zero timeout means wait forever."
    echo -e "\t<solver>: either ZCHAFF for the known solver or SAD for our solution."

    exit -1
fi

# Validate in-path existence
if [ ! -f $1 ] ; then 
    echo -e "Instances file doesn't exist.\nExiting.."
    exit -1
fi

INPUT_PATH=$1
TIMEOUT=$(($2))  # Whenever not an integer it leads to zero (wait forever)

SOLVER=$3
TMP=0

# Exit on compile error
if [[ $SOLVER = "ZCHAFF" ]]; then
    make -C zchaff64 && make -C `pwd`
    TMP=$(($?))
elif [[ $SOLVER = "SAD" ]]; then
    make -C `pwd` 
    TMP=$(($?))
else 
    echo -e "Invalid solver. Options are 'ZCHAFF' and 'SAD'.\nExiting"
    exit -1
fi

if [ $TMP -ne 0 ] ; then
    echo "ERROR: Problem building project. Stopping";
    exit -1 
fi

clear # Clean display

# Manpages lie, word splitting is indeed performed on command substituion even
# if done with backtics (`). The translation (tr), is used to adjust this flaw
INSTANCES=`cat $INPUT_PATH | tr ' ' '*'`

# Temporary files for orchestration
SUDOKU_BUFFER=`mktemp`
SAT_BUFFER=`mktemp`
SOLUTION_BUFFER=`mktemp`
OUT_FILE=sat.log

echo "" > $OUT_FILE

echo "Getting instances from $INPUT_PATH" >> $OUT_FILE

# Run propper solver depending on input 
run_solver() {
    if [[ $SOLVER = "ZCHAFF" ]] ; then
        (./zchaff64/zchaff $SAT_BUFFER $TIMEOUT) |& tee -a $OUT_FILE 
    elif [[ $SOLVER = "SAD" ]] ; then
        (timeout --preserve-status -s SIGUSR1 "$TIMEOUT"s ./SatSudoku --solve < $SAT_BUFFER) > $SOLUTION_BUFFER  
    fi

}

let "idx = -1" 
for sudoku in $INSTANCES; do
    ((++idx))
    echo -e "--> Results for sudoku instance at line $idx" | tee -a $OUT_FILE

    echo $sudoku | tr '*' ' ' > "$SUDOKU_BUFFER"

    (./SatSudoku --toSAT < $SUDOKU_BUFFER) > $SAT_BUFFER 

    if [ $? -ne 0 ]; then
        echo "ERROR: Sudoku transformation went wrong for instance No $idx. Skipping." | tee -a $OUT_FILE
        continue
    fi

    run_solver 
    TMP=$(($?))

    if [ $TMP -eq 42 ]; then
        echo -e "WARNING: Solver timed out. Skipping $idx instance."  | tee -a $OUT_FILE
        continue
    elif [ $TMP -ne 0 ]; then
        echo -e "ERROR: Something went wrong while solving. Skipping $idx instance." | tee -a $OUT_FILE
        continue
    fi

    cat $SUDOKU_BUFFER $SOLUTION_BUFFER | ./SatSudoku --toSudoku |& tee -a $OUT_FILE

    if [ $? -ne 0 ]; then
        echo -e "ERROR: Something went wrong while display solution. Skipping $idx instance." | tee -a $OUT_FILE 
        continue
    fi

done

rm $SUDOKU_BUFFER $SAT_BUFFER $SOLUTION_BUFFER