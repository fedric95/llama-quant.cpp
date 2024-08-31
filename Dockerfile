ARG CUDA_IMAGE="12.1.1-devel-ubuntu22.04"
FROM nvidia/cuda:${CUDA_IMAGE}

RUN apt-get update && apt-get upgrade -y \
    && apt-get install -y git build-essential \
    python3 python3-pip gcc wget

# Install depencencies
RUN python3 -m pip install --upgrade pip cmake setuptools

COPY . .

RUN chmod +x ./setup.sh && ./setup.sh
