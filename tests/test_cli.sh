#!/bin/bash

command="./hypercli"

if [ ! -f "$command" ]; then
    echo -e "Error: File '$command' not found.\nTest failed."
    exit 1
fi

num_right=0
total=0
line="________________________________________________________________________"

run_test() {
    (( ++total ))
    echo -n "Running test $total:"
    expected=$2
    received=$( $command $1 2>&1 | tr -d '\r' )
    if [ "$expected" = "$received" ]; then
        printf "\e[1;32m Success\e[0m\n"
        (( ++num_right ))
    else
        printf "\e[1;31m Failure\e[0m\n\nExpected$line\n\e[1;32m$expected\n\e[0mReceived$line\n\e[1;31m$received\e[0m\n"
    fi
}

printf "\n"


# Test usage
run_test "" "${command} --action [info|deploy|destroy|updatefdb] --organization <org> --vmid <vmid>"
run_test "--help" "${command} --action [info|deploy|destroy|updatefdb] --organization <org> --vmid <vmid>"

# Test action
run_test "--action" "Error: Bad input for action."
run_test "--action blah" "Error: Bad input for action."
run_test "--action blah1 --organization 123" "Error: Bad input for action."
run_test "--action blah2 --organization 123 --vmid 1234" "Error: Bad input for action."
run_test "--action info" "Error: Bad input for organization."
run_test "--action deploy" "Error: Bad input for organization."
run_test "--action destroy" "Error: Bad input for organization."
run_test "--action updatefdb" "Error: Bad input for organization."

# Test organization
run_test "--action info --organization" "Error: Bad input for organization."
run_test "--action info --organization 123!" "Error: Bad input for organization."
run_test "--action info --organization 123#" "Error: Bad input for organization."
run_test "--action info --organization 12@3" "Error: Bad input for organization."
run_test "--action info --organization 123(" "Error: Bad input for organization."
run_test "--action info --organization '123 123'" "Error: Bad input for organization."
run_test "--action info --organization 123" "Error: Bad input for vmid."

# Test vmid
run_test "--action info --organization 123 --vmid" "Error: Bad input for vmid."
run_test "--action info --organization 123 --vmid 123!" "Error: Bad input for vmid."
run_test "--action info --organization 123 --vmid 123@" "Error: Bad input for vmid."
run_test "--action info --organization 123 --vmid 123#" "Error: Bad input for vmid."
run_test "--action info --organization 123 --vmid 123$" "Error: Bad input for vmid."
run_test "--action info --organization 123 --vmid 12%3" "Error: Bad input for vmid."
run_test "--action info --organization 123 --vmid 12&3" "Error: Bad input for vmid."
run_test "--action info --organization 123 --vmid '123 abc'" "Error: Bad input for vmid."

echo -e "\nTotal tests run: $total"
echo -e "Number correct : $num_right"
echo -n "Percent correct: "
echo "scale=2; 100 * $num_right / $total" | bc