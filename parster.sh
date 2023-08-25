#!/bin/bash

# Directory paths
VALID_DIR="config-files/valid"
INVALID_DIR="config-files/invalid"

# Path to webserv executable
WEBSERV_EXEC="./webserv"

# Function to run webserv with a given config file
run_webserv() {
    config_file="$1"
    echo -e "\e[34mRunning webserv with config file: $config_file\e[0m"
    $WEBSERV_EXEC "$config_file"
}

# Process valid config files
echo -e "\e[32m\n[Valid config files]\n\e[0m"
for valid_config in $VALID_DIR/*.conf; do
    run_webserv "$valid_config"
done

echo -e "============================================================\n"

# Process invalid config files
echo -e "\033[0;31m [Invalid config files]\n\e[0m"
for invalid_config in $INVALID_DIR/*.conf; do
    run_webserv "$invalid_config"
done

echo "Script completed."
