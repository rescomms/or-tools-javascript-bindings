# syntax=docker/dockerfile:1

# If you need help, visit the Dockerfile reference guide at
# https://docs.docker.com/go/dockerfile-reference/

################################################################################
# Pick a base image to serve as the foundation for the other build stages in
# this file.
FROM ubuntu:24.04 AS base

SHELL ["/bin/bash", "-c"]

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    git \
    python3 \
    xz-utils \
    cmake

################################################################################
# Install emscripten
FROM base AS emscripten
WORKDIR /wrkdir
RUN git clone --depth 1 --branch 4.0.5 https://github.com/emscripten-core/emsdk.git
WORKDIR emsdk
RUN ./emsdk install latest
RUN ./emsdk activate latest
WORKDIR ..

################################################################################
# Build OR-Tools
FROM emscripten AS build-ortools
RUN git clone --depth 1 --branch v9.12 https://github.com/google/or-tools.git
WORKDIR or-tools
RUN source ../emsdk/emsdk_env.sh && \
emcmake cmake -S. -Bbuild -DBUILD_DEPS:BOOL=ON -DBUILD_TESTING=OFF \
-DBUILD_FLATZINC:BOOL=OFF \
-DBUILD_EXAMPLES:BOOL=OFF -DUSE_COINOR:BOOL=OFF -DUSE_HIGHS:BOOL=OFF \
-DUSE_SCIP:BOOL=OFF
RUN cmake --build build

################################################################################
# Install built OR-Tools
FROM build-ortools AS install-ortools
RUN cmake --build build --config Release --target install -v
WORKDIR ..

################################################################################
# Build bindings
FROM install-ortools AS bindings
COPY . .
RUN source ./emsdk/emsdk_env.sh && \
npm install -g typescript
RUN source ./emsdk/emsdk_env.sh && \
emcmake cmake -S . -B build
RUN source ./emsdk/emsdk_env.sh && \
cmake --build build

################################################################################
# Copy built bindings to host
VOLUME /wrkdir/result

CMD ["cp", "./build/bindings.js", "./build/bindings.d.ts", "/wrkdir/result/"]
