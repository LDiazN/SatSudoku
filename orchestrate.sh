
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

# Whenever not an integer it leads to zero (wait forever)
TIMEOUT=$(($2)) 

# Validate solver is either ZCHAFF or SAT
if [ $3 != "ZCHAFF" -a $3 != "SAD" ]; then
    echo -e "Invalid solver. Options are 'ZCHAFF' and 'SAD'.\nExiting"
    exit -1
fi

SOLVER=$3

## 

make -C `pwd` && make -C zchaff64


# Manpages lie, word splitting is indeed performed on command substituion even
# if done with backtics (`). The translation (tr), is used to adjust this flaw
INSTANCES=`cat $INPUT_PATH | tr ' ' '*'`

echo "Getting instances from $INPUT_PATH"

SUDOKU_BUFFER=`mktemp`
SAT_BUFFER=`mktemp`
SOLUTION_BUFFER=`mktemp`

# TODO explain
run_solver() {
    if [[ $SOLVER = "ZCHAFF" ]] ; then
        (./zchaff64/zchaff $SAT_BUFFER $TIMEOUT) > $SOLUTION_BUFFER #| tee $SOLUTION_BUFFER 
    elif [[ $SOLVER = "SAD" ]] ; then
        (timeout --preserve-status -s SIGUSR1 "$TIMEOUT"s ./SatSudoku --solve < $SAT_BUFFER) > $SOLUTION_BUFFER #| tee $SOLUTION_BUFFER 
    fi
}

let "idx = -1" 
for sudoku in $INSTANCES; do
    ((++idx))
    echo -e "--> Results for sudoku instance at line $idx\n"

    echo $sudoku | tr '*' ' ' > "$SUDOKU_BUFFER"

    (./SatSudoku --toSAT < $SUDOKU_BUFFER) > $SAT_BUFFER 

    if [ $? -ne 0 ]; then
        echo "Sudoku transformation went wrong for $idx instance. Skipping display.." && continue
    fi

    run_solver 

    TMP=$?

    if [ $TMP -eq 42 ]; then
        echo -e "Solver timed out. Skipping.." && continue
    elif [ $TMP -ne 0 ]; then
        echo -e "Problem during solving $?. Skipping.." && continue
    fi

    ./SatSudoku --toSudoku < $SUDOKU_BUFFER
    #cat $SUDOKU_BUFFER $SOLUTION_BUFFER | ./SATSudoku --toSudoku
    # ! hard to catch errors this way
done

rm $SUDOKU_BUFFER $SAT_BUFFER $SOLUTION_BUFFER