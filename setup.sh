#!/bin/bash

highlight() {
    echo -e "\033[1;32m$1\033[0m"
}

error() {
    echo -e "\033[1;31m$1\033[0m" >&2
}

prompt() {
    read -p "$1" INPUT
    echo "$INPUT"
}

set_essential_var() {
    while true; do
        read -p "$1" INPUT
        if [[ -n "$INPUT" ]]; then
            echo "$INPUT"
            return 0
        else
            echo "$2 cannot be empty. Please try again." >&2
        fi
    done
}

confirm() {
    read -p "$1 [y/N]:" CONFIRM
    case "$CONFIRM" in
        [yY][eE][sS]|[yY])
            return 0;;
        *)
            return 1;;
    esac
}

# START

echo "Thank you for choosing teilpunkt!"
echo "This script will automatically setup your instance"
echo "All options can later be configured in the admin panel"

INSTANCE_NAME=$(set_essential_var "Choose an instance name: " "Instance name")
echo -n "Instance name:"
highlight "$INSTANCE_NAME."