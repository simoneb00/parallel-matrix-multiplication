#!/bin/bash


declare -A processed_configs_sequential
declare -A processed_configs_openmp

# Function that verifies if a configuration has already been processed
function is_configuration_processed_sequential {
    local current_config="$1"
    if [ -n "${processed_configs_sequential["$current_config"]}" ]; then
        return 0
    else
        return 1
    fi
}


function is_configuration_processed_openmp {
    local current_config="$1"
    if [ -n "${processed_configs_openmp["$current_config"]}" ]; then
        return 0
    else
        return 1
    fi
}


if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <config_file>"
    exit 1
fi

config_file=$1

if [ ! -f "$config_file" ]; then
    echo "Config file not found: $config_file"
    exit 1
fi

mapfile -t lines < "$config_file"

for line in "${lines[@]}"; do
    param_count=$(echo "$line" | wc -w)

    if [ "$param_count" -ge 8 ]; then
        params=($line)
        param1=${params[0]}
        param2=${params[1]}
        param3=${params[2]}
        param7=${params[7]}

        ./matrix_generation $param1 $param2 $param3

        n_processes=$(( ${params[3]} * ${params[4]} ))

        current_config_sequential="$param1 $param2 $param3"
        current_config_openmp="$param1 $param2 $param3 $param7"

        if is_configuration_processed_sequential "$current_config_sequential"; then
            echo "This configuration has already been considered: skipping sequential computation"
        else
            ./sequential_computation $param1 $param2 $param3
            processed_configs_sequential["$current_config_sequential"]=1
            last_line=$(tail -n 1 "sequential_execution_data.csv")
            sequential_execution_time=$(echo "$last_line" | awk -F ',' '{print $4}')
            echo "Comparing sequential, time is ${sequential_execution_time}"
            ./comparison ${params[0]} ${params[1]} ${params[2]} ${params[3]} ${params[4]} ${params[5]} ${params[6]} ${params[7]} ${sequential_execution_time} 0.0 sequential

        fi

        echo "Starting MPI computation with ${n_processes} processes, with process grid ${params[3]}x${params[4]} and block ${params[5]}x${params[6]}"
        mpirun -np ${n_processes} ./mpi_computation "${params[@]:0:7}"


        last_line=$(tail -n 1 "mpi_execution_data.csv")
        mpi_execution_time=$(echo "$last_line" | awk -F ',' '{print $8}')

        echo "Comparing MPI, time is ${mpi_execution_time}"
        ./comparison ${params[0]} ${params[1]} ${params[2]} ${params[3]} ${params[4]} ${params[5]} ${params[6]} ${params[7]} ${sequential_execution_time} ${mpi_execution_time} mpi

        if is_configuration_processed_openmp "$current_config_openmp"; then
            echo "This configuration has already been considered: skipping openmp computation"
        else
            ./openmp_computation $param1 $param2 $param3 $param7
            processed_configs_openmp["$current_config_openmp"]=1
            last_line=$(tail -n 1 "openmp_execution_data.csv")
            openmp_execution_time=$(echo "$last_line" | awk -F ',' '{print $5}')

            echo "Comparing OpenMP, time is ${openmp_execution_time}"

            ./comparison ${params[0]} ${params[1]} ${params[2]} ${params[3]} ${params[4]} ${params[5]} ${params[6]} ${params[7]} ${sequential_execution_time} ${openmp_execution_time} openmp
        fi

        echo ""
        echo "-------------------------------------------------------"
        echo ""
    else
        echo "This line does not contain the required parameters: $line"
    fi

done

