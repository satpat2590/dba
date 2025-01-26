#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(realpath $HOME/identity/api/lib)
$HOME/identity/api/bin/main