#!/bin/bash

# Directory paths
VALID_DIR="config-files/valid"
INVALID_DIR="config-files/invalid"

# Create a tmp folder in the current working directory if it doesn't exist
TMP_DIR="./tmp"
mkdir -p "$TMP_DIR"

# Colors
RED=$(tput bold; tput setaf 1)
GREEN=$(tput bold; tput setaf 2)
BLUE=$(tput bold; tput setaf 4)
RESET=$(tput sgr0)

# Path to webserv executable
WEBSERV_EXEC="./webserv"

# Check for the existence of the webserv executable
if [ ! -x "$WEBSERV_EXEC" ]; then
    echo "The webserv executable ('$WEBSERV_EXEC') was not found or is not executable."
    exit 1
fi

# Function to run webserv with a given config file
run_webserv() {
    config_file="$1"
    result_file="$TMP_DIR/$(basename "$config_file").result"
    $WEBSERV_EXEC "$config_file" > "$result_file" 2>&1
    if [ $? -eq 0 ]; then
        actual_output="${GREEN}OK${RESET}"
    else
        actual_output="${RED}KO${RESET}"
    fi
	printf "%-45s %-30s %-10s\n" "$(basename "$config_file")" "$actual_output" "$result_file"
}

print_header() {
	input_file="$1"
	printf "%-54s %-15s %-10s\n" "${BLUE}$input_file config files" "Result" "Output file${RESET}"
	printf "%-54s %-15s %-10s\n" "${BLUE}--------------------" "------" "-----------${RESET}"
}

# Check for command-line argument
if [ $# -eq 0 ]; then
	printf "\n"

	print_header "Valid"
    for valid_config in $VALID_DIR/*.conf; do
        run_webserv "$valid_config"
    done

	printf "\n"

	print_header "Invalid"
    for invalid_config in $INVALID_DIR/*.conf; do
        run_webserv "$invalid_config"
    done
fi

printf "\n%-s\n" "${GREEN}Script completed.${RESET}"
