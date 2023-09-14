#!/bin/bash

# Directory paths
VALID_DIR="config-files/valid"
INVALID_DIR="config-files/invalid"

# Path to webserv executable
WEBSERV_EXEC="./webserv"

# Function to run webserv with a given config file
run_webserv() {
    config_file="$1"
    echo -e "\e[34m\n\tRunning webserv with config file: $config_file\e[0m"
    $WEBSERV_EXEC "$config_file"
}

# Check for command-line argument
if [ $# -eq 0 ]; then
    echo -e "\e[32m\n[Valid config files]\n\e[0m"
    for valid_config in $VALID_DIR/*.conf; do
        run_webserv "$valid_config"
    done

    echo -e "============================================================\n"

    echo -e "\033[0;31m [Invalid config files]\n\e[0m"
    for invalid_config in $INVALID_DIR/*.conf; do
        run_webserv "$invalid_config"
    done
elif [ "$1" == "valid" ]; then
    echo -e "\e[32m\n[Valid config files]\n\e[0m"
    for valid_config in $VALID_DIR/*.conf; do
        run_webserv "$valid_config"
    done
elif [ "$1" == "invalid" ]; then
    echo -e "\033[0;31m [Invalid config files]\n\e[0m"
    for invalid_config in $INVALID_DIR/*.conf; do
        run_webserv "$invalid_config"
    done
else
    echo "Usage: $0 [valid|invalid]"
    exit 1
fi

echo "Script completed."
