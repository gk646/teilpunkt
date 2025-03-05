#!/bin/bash

#
# SPDX-License-Identifier: GPL-3.0-only
#

#
# SPDX-License-Identifier: Apache License 2.0
#

# Highlight text in green for better visibility
highlight() {
    echo -e "\033[1;32m$1\033[0m"
}

# Display errors in red
error() {
    echo -e "\033[1;31m$1\033[0m" >&2
}

# Prompt user for input
prompt() {
    read -p "$1" INPUT
    echo "$INPUT"
}

# Prompt for essential variables (non-empty values)
set_essential_var() {
    while true; do
        read -r -p "$1: " INPUT
        if [[ -n "$INPUT" ]]; then
            echo "$INPUT"
            return 0
        else
            error "$2 cannot be empty. Please try again."
        fi
    done
}

# Securely prompt for sensitive information (hidden input)
secret_prompt() {
    local SECRET SECRET_CONFIRM
    while true; do
        read -r -s -p "$1: " SECRET
        echo ""  # Ensure a new line after user input
        read -r -s -p "Confirm $1: " SECRET_CONFIRM
        echo ""  # Ensure a new line after confirmation input
        if [[ "$SECRET" == "$SECRET_CONFIRM" && -n "$SECRET" ]]; then
            echo "$SECRET"  # Return the secret to be assigned
            return 0
        else
            error "Entries do not match or are empty. Please try again."
        fi
    done
}

# Confirm an action with a Yes/No prompt
confirm() {
    read -r -p "$1 [y/N]: " CONFIRM
    case "$CONFIRM" in
        [yY][eE][sS]|[yY]) return 0;;
        *) return 1;;
    esac
}

# Safely create a file if it doesn't exist
create_file() {
    local FILE_PATH="$1"
    if [[ -e "$FILE_PATH" ]]; then
        error "File '$FILE_PATH' already exists."
        return 1
    else
        touch "$FILE_PATH"
        highlight "File '$FILE_PATH' created successfully."
        return 0
    fi
}

# Delete a directory if it exists, with confirmation
delete_directory() {
    local DIR_PATH="$1"
    if [[ -d "$DIR_PATH" ]]; then
        if confirm "Are you sure you want to delete '$DIR_PATH' and its contents?"; then
            rm -rf "$DIR_PATH"
            highlight "Directory '$DIR_PATH' deleted successfully."
        else
            highlight "Deletion cancelled."
        fi
    else
        error "Directory '$DIR_PATH' does not exist."
    fi
}

# START SETUP PROCESS

highlight "Thank you for choosing teilpunkt!"
echo "This script will guide you through setting up your instance."
echo "Except for the instance secret, all settings can later be configured in the admin panel."

# Set instance name (required)
INSTANCE_NAME=$(set_essential_var "Choose an instance name" "Instance name")
echo -n "Instance name: "
highlight "$INSTANCE_NAME"

# Set instance secret (hidden input)
INSTANCE_SECRET=$(secret_prompt "Enter instance secret")

highlight "Setup completed successfully!"