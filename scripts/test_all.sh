#!/bin/bash

# Usage: ./test_all.sh <program> <folder>
# Example: ./test_all.sh ./my_program ./test_files

PROGRAMMA="$1"
CARTELLA="$2"

# ANSI colors
GREEN="\e[32m"
RED="\e[31m"
RESET="\e[0m"

# possible tests
PREF_VALID="valid_"
PREF_INVALID="invalid_"

# Parameter check
if [ -z "$PROGRAMMA" ] || [ -z "$CARTELLA" ]; then
    echo "Usage: $0 <program> <folder>"
    exit 1
fi

if [ ! -x "$PROGRAMMA" ]; then
    echo "Error: $PROGRAMMA is not executable!"
    exit 1
fi

if [ ! -d "$CARTELLA" ]; then
    echo "Error: $CARTELLA is not a valid folder!"
    exit 1
fi

# Scroll through all files in the folder
for FILE in "$CARTELLA"/*; do
    if [ -f "$FILE" ]; then
        echo -n "Running $PROGRAMMA with $FILE ... "
        "$PROGRAMMA" "$FILE" 2>/dev/null 1>/dev/null
        RET=$?
        NOME_FILE=$(basename "$FILE")
        if [[ $RET -eq 0 && $NOME_FILE == ${PREF_VALID}* ]]; then
            echo -e "${GREEN}[PASS]${RESET}"
        else
            if [[ $RET -eq 255 && $NOME_FILE == ${PREF_INVALID}* ]]; then
                echo -e "${GREEN}[PASS]${RESET}"
            else
                echo -e "${RED}[FAIL]${RESET} (code $RET)"
            fi
        fi
    fi
done
