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
RUN git clone --depth 1 --branch 7.0.1 https://github.com/emscripten-core/emsdk.git
WORKDIR /wrkdir/emsdk
RUN ./emsdk install latest
RUN ./emsdk activate latest
# We want to use static libs, and bzip2 is the only dependency or-tools doesn't build from source
# Use one supplied by emscripten and build it here
RUN source ./emsdk_env.sh && embuilder build bzip2
WORKDIR /wrkdir

################################################################################
# Build OR-Tools
FROM emscripten AS build-ortools
RUN git clone --depth 1 --branch v9.13 https://github.com/google/or-tools.git
WORKDIR /wrkdir/or-tools
# To use static libs, patch the or-tools CMakeLists.txt files to have BUILD_SHARED_LIBS=OFF
# This needs to be done even though we supply -DBUILD_SHARED_LIBS=OFF from the command line
RUN grep -rn "BUILD_SHARED_LIBS" CMakeLists.txt cmake/ && \
    find CMakeLists.txt cmake/ -type f -exec sed -i '/BUILD_SHARED_LIBS/s/ON/OFF/g' {} +
RUN source ../emsdk/emsdk_env.sh && \
    emcmake cmake -S. -Bbuild \
    -DBUILD_DEPS:BOOL=ON \
    -DBUILD_TESTING=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_FLATZINC:BOOL=OFF \
    -DBUILD_EXAMPLES:BOOL=OFF \
    -DUSE_COINOR:BOOL=OFF \
    -DUSE_HIGHS:BOOL=OFF \
    -DUSE_SCIP:BOOL=OFF \
    -DCMAKE_C_FLAGS="-pthread" \
    -DCMAKE_CXX_FLAGS="-pthread"
RUN source ../emsdk/emsdk_env.sh && \
    cmake --build build --config Release --target install

################################################################################
# Build bindings
FROM build-ortools AS bindings
WORKDIR /wrkdir
RUN source ./emsdk/emsdk_env.sh && \
    npm install -g typescript
COPY . .
RUN source ./emsdk/emsdk_env.sh && \
    emcmake cmake -S . -B build
RUN source ./emsdk/emsdk_env.sh && \
    cmake --build build

################################################################################
# Copy built bindings to host
VOLUME /wrkdir/result

CMD ["cp", "./build/bindings.js", "./build/bindings.d.ts", "/wrkdir/result/"]
