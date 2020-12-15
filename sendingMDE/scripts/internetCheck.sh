#!/bin/bash

wget -q --tries=3 --output-document=/dev/null --timeout=10 http://google.com
if [[ $? -eq 0 ]]; then
        echo "1"
else
        echo "0"
fi

