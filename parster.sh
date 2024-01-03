#!/bin/bash

# Path to webserv executable
WEBSERV_EXEC="./webserv"

# Directory paths
VALID_DIR="config-files/valid"
INVALID_DIR="config-files/invalid"

# Tmp folder to store results
TMP_DIR="./results-parster"
mkdir -p "$TMP_DIR"

# Colors
RED=$(tput bold; tput setaf 1)
GREEN=$(tput bold; tput setaf 2)
BLUE=$(tput bold; tput setaf 4)
RESET=$(tput sgr0)

# Run webserv with given config file
# Store detailed output to a tmp result file
# Display result (OK/KO) on console
run_webserv() {
	config_file="$1"
	result_file="$TMP_DIR/$(basename "$config_file").result"
	valgrind -s --leak-check=full --show-leak-kinds=all --track-fds=yes $WEBSERV_EXEC "$config_file" > "$result_file" 2>&1

	exit_status="${PIPESTATUS[0]}"

	if [ $exit_status -eq 0 ]; then
		actual_output="${GREEN}OK${RESET}"
	elif [ $exit_status -eq 1 ]; then
		actual_output="${RED}KO${RESET}"
	else
		actual_output="${RED}CRASH${RESET}"
	fi
	# printf "%-45s %-30s %-10s\n" "$(basename "$config_file")" "$actual_output" "$result_file"
	printf "%-45s %-30s %-10s\n" "$(basename "$config_file")" "$actual_output" "$(basename "$result_file")"
}

# Print header
print_header() {
	input_file="$1"
	printf "%-54s %-15s %-10s\n" "${BLUE}$input_file config files" "Result" "Output file${RESET}"
	printf "%-54s %-15s %-10s\n" "${BLUE}--------------------" "------" "-----------${RESET}"
}

# Check for webserv executable and run
if [ ! -x "$WEBSERV_EXEC" ]; then
	echo "The webserv executable ('$WEBSERV_EXEC') was not found or is not executable."
	exit 1
else
	printf "\n"

	print_header "Valid"
	for valid_config in $VALID_DIR/*; do
		run_webserv "$valid_config"
	done

	printf "\n"

	print_header "Invalid"
	for invalid_config in $INVALID_DIR/*; do
		run_webserv "$invalid_config"
	done
fi

printf "\n%-s\n" "${GREEN}Script completed.${RESET}"
