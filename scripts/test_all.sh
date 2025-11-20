#!/bin/bash

# Uso: ./test_all.sh <programma> <cartella>
# Esempio: ./test_all.sh ./mio_programma ./test_files

PROGRAMMA="$1"
CARTELLA="$2"

# Colori ANSI
VERDE="\e[32m"
ROSSO="\e[31m"
RESET="\e[0m"

# test possibili
PREF_VALID="valid_"
PREF_INVALID="invalid_"

# Controllo parametri
if [ -z "$PROGRAMMA" ] || [ -z "$CARTELLA" ]; then
    echo "Uso: $0 <programma> <cartella>"
    exit 1
fi

if [ ! -x "$PROGRAMMA" ]; then
    echo "Errore: $PROGRAMMA non è eseguibile!"
    exit 1
fi

if [ ! -d "$CARTELLA" ]; then
    echo "Errore: $CARTELLA non è una cartella valida!"
    exit 1
fi

# Scorri tutti i file nella cartella
for FILE in "$CARTELLA"/*; do
    if [ -f "$FILE" ]; then
        echo -n "Eseguo $PROGRAMMA con $FILE ... "
        "$PROGRAMMA" "$FILE" 2>/dev/null 1>/dev/null
        RET=$?
        NOME_FILE=$(basename "$FILE")
        if [[ $RET -eq 0 && $NOME_FILE == ${PREF_VALID}* ]]; then
            echo -e "${VERDE}[PASS]${RESET}"
        else
            if [[ $RET -eq 255 && $NOME_FILE == ${PREF_INVALID}* ]]; then
                echo -e "${VERDE}[PASS]${RESET}"
            else
                echo -e "${ROSSO}[FAIL]${RESET} (codice $RET)"
            fi
        fi
    fi
done
