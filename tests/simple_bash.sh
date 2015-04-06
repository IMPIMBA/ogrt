#!/bin/bash

echo "Bash script with PID: $$"
ps e
bash -l -c "/bin/ps"
