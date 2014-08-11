#!/bin/bash

"$@"

if [[ $? -eq 139 ]]; then
    gdb -x gdb_test_commands --args $@
fi
