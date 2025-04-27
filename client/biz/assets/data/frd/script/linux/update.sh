#!/bin/bash

usage() {
    echo "Usage: $0 <Compress File>"
    echo "Support formate: zip, gz, tar, tar.gz, tgz"
    exit 1
}


if [ $# -ne 1 ]; then
    usage
fi

FILE="$1"

if [ ! -f "$FILE" ]; then
    echo "Error: File '$FILE' not exist"
    exit 1
fi

#DIR=$(dirname "$FILE")
DIR="/"

case "$FILE" in
    *.zip)
        echo "Zip file: $FILE"
        unzip -o "$FILE" -d "$DIR"
        ;;
    *.gz)
        if [[ "$FILE" == *.tar.gz || "$FILE" == *.tgz ]]; then
            echo "Tar.gz file: $FILE"
            tar -xzf "$FILE" -C "$DIR"
        else
            BASENAME=$(basename "$FILE" .gz)
            gunzip -c "$FILE" > "$DIR/$BASENAME"
        fi
        ;;
    *.tar)
        echo "Tar file: $FILE"
        tar -xf "$FILE" -C "$DIR"
        ;;
    *)
        echo "Not support format"
        exit 1
        ;;
esac

exit 0