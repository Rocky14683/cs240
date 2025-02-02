#!/bin/sh

HW=${HW:-hw2}
RUNS=${RUNS:-1000}
EXPECTED_SCORE=${EXPECTED_SCORE:-100}

if ! [ -x "${HW}_test" ]
then
    echo "Cannot find ${HW}_test executable." >&2
    exit 1
fi

if ! mv test_data_files test_data_files_backup
then
    echo "Error: Cannot back up test_data_files." >&2
    exit 1
fi
tmpfile=$(mktemp)

for i in $(seq 1 $RUNS)
do
    printf "\rRunning %d/%d..." "$i" "$RUNS" >&2
    mkdir -p test_data_files
    "./${HW}_test" >"$tmpfile" 2>&1
    score="$?"
    if [ "$score" -lt "$EXPECTED_SCORE" ]
    then
        echo >&2
        echo >&2
        cat "$tmpfile"
        printf "\n\nFailed at run %d/%d. See output above.\n" "$i" "$RUNS" >&2
        test -d "test_data_files_run_$i" && rm -rf "test_data_files_run_$i"
        mv test_data_files "test_data_files_run_$i"
        printf "Data directory saved as test_data_files_run_%d.\n" "$i" >&2
        break
    fi
    rm -rf test_data_files
done

if [ "$i" = "$RUNS" ]
then
    echo >&2
    echo "All tests successful." >&2
fi

rm -f "$tmpfile"
mv test_data_files_backup test_data_files
