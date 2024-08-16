#!/bin/bash
set -e

if [[ ! -v LLAMA_CPP_VERSION ]]; then
    echo "LLAMA_CPP_VERSION is missing"
    exit 1
else
    echo "LLAMA_CPP_VERSION: $LLAMA_CPP_VERSION"
fi

if [[ ! -v LLAMA_CPP_CUDA ]]; then
    echo "LLAMA_CPP_CUDA is missing"
    exit 1
else
    echo "LLAMA_CPP_CUDA: $LLAMA_CPP_CUDA"
fi



git clone https://github.com/ggerganov/llama.cpp
cd llama.cpp
git reset --hard $LLAMA_CPP_VERSION
make GGML_CUDA=$LLAMA_CPP_CUDA
cd ..
