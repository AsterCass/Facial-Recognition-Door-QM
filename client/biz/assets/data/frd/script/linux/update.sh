#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Param error"
    exit 1
fi

FILE="$1"
URL="$2"

wget -O "$FILE" "$URL"

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
            TMP_FILE="${FILE%.gz}"
            gzip -d -c "$FILE" > "$TMP_FILE"
            tar -xf "$TMP_FILE" -C "$DIR"
            rm "$TMP_FILE"
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

chmod 755 /data/frd/bin/*

/etc/init.d/S99zplay stop
reboot

exit 0