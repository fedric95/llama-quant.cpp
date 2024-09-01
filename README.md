[ IN CONSTRUCTION ]

from: https://huggingface.co/datasets/ikawrakow/validation-datasets-for-llama.cpp/blob/main/README.md

## JSON -> Binary

g++ -o convert convert.cpp
wget https://huggingface.co/datasets/ikawrakow/validation-datasets-for-llama.cpp/resolve/main/mmlu-validation.bin
./convert mmlu-validation.bin mmlu-validation.json

## Binary -> JSON

g++ -o encode encode.cpp
./encode mmlu-validation.json mmlu-validation_2.bin

## Measure performance

./perplexity -m model -bf custom-test.bin --multiple choice
