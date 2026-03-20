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
RUN git clone --depth 1 --branch 5.0.3 https://github.com/emscripten-core/emsdk.git
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
-DUSE_SCIP:BOOL=OFF -DCMAKE_C_FLAGS="-pthread -sMEMORY64=1 -D_MIPS_SZPTR=64" -DCMAKE_CXX_FLAGS="-pthread -sMEMORY64=1 -D_MIPS_SZPTR=64"
RUN cmake --build build

################################################################################
# Install built OR-Tools
FROM build-ortools AS install-ortools
RUN cmake --build build --config Release --target install -v
WORKDIR ..

################################################################################
# Build bindings
FROM install-ortools AS bindings
RUN source ./emsdk/emsdk_env.sh && \
npm install -g typescript
COPY . .
RUN source ./emsdk/emsdk_env.sh && \
emcmake cmake -S . -B build -DCMAKE_C_FLAGS="-sMEMORY64=1 -D_MIPS_SZPTR=64" -DCMAKE_CXX_FLAGS="-sMEMORY64=1 -D_MIPS_SZPTR=64" -Dortools_DIR=/wrkdir/emsdk/upstream/emscripten/cache/sysroot/lib/cmake/ortools
RUN source ./emsdk/emsdk_env.sh && \
cmake --build build

################################################################################
# Copy built bindings to host
VOLUME /wrkdir/result

CMD ["cp", "./build/bindings.js", "./build/bindings.d.ts", "/wrkdir/result/"]
