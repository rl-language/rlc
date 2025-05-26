#!/bin/bash

# This script merges multiple benchmark .csv files into 1
# in order to call plot.py on the output and see on the same 
# graph one different curve per benchmark file

# It replaces the name column with the .csv file name so 
# choose meaningful names

# Replace part of first column with filename without extension
replace_name(){
    tail -n +2 "$current_file" | awk -F, -v filename=$filename_noext '{
        # Replace part of first field up to and including "_rlc" with filename (without .csv)
        # Use sub() to replace the pattern in the first field
        if (index($1, "_rlc") > 0) {
            # Replace everything up to and including "_rlc" with filename
            sub(/^.*_rlc/, filename, $1);
        } else {
            # If "_rlc" is not found, just prepend the filename
            $1 = filename"_"$1;
        }
        
        # Remove quotes
        gsub(/"/, "", $1);
        
        # Output first field
        printf "%s", $1;
        
        # Output remaining fields with commas
        for (i=2; i<=NF; i++) {
            printf ",%s", $i;
        }
        printf "\n";
    }' >> "$filename_noext"_renamed.csv
}



# Check if at least one argument is provided
if [ $# -lt 2 ]; then
    echo "Usage: $0 file1.csv file2.csv [file3.csv ...]"
    echo "This script processes multiple CSV files by:"
    echo "  1. Keeping the header from the first file, removing headers from others"
    echo "  2. Replacing the first column with the filename"
    echo "  3. Merging all files into one output file"
    exit 1
fi

# Setup variables
header=$(head -1 $1)

echo "Processing CSV files..."


# Process each file
for (( i=1; i<=$#; i++ )); do
    current_file="${!i}"
    filename=$(basename "$current_file")
    
    echo "- Processing $filename"

    # Remove file extension
    filename_noext="${filename%.csv}"
    renamed_file="$filename_noext"_renamed.csv
    
    echo $header > $renamed_file

    replace_name

    # First file: Keep header 
    if [ $i -eq 1 ]; then
        tail -n +2 $renamed_file |awk -F, -v header=$header '
            BEGIN{i = 0; file = "bench_"i; print header >> file}
            /RMS/ {change_file = 1}
            /./{print >> file}
            {if (change_file == 1){
                close(file);
                i++;
                file = "bench_"i ;
                {if (getline != 0){print header >> file}}
                change_file = 0;
                next;
            }}
        '
    else  
        tail -n +2 $renamed_file |awk -F, '
            BEGIN{i = 0; file = "bench_"i}
            /RMS/ {change_file = 1}
            /./{print >> file}
            {if (change_file == 1){
                close(file);
                i++;
                file = "bench_"i ;
                change_file = 0;
                next;
            }}
        '
    fi



done

echo
echo "Cleaning intermediate files"
rm *_renamed.csv


echo
echo "Done!"

# echo
# echo "Entering venv"
# source ../.venv/bin/activate || exit 1

# echo
# echo "Opening plotter for every file"
# for bench in ./bench_* ; do
#     python3 plot.py -f $benc &
# done