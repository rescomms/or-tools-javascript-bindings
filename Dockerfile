# syntax=docker/dockerfile:1

# If you need help, visit the Dockerfile reference guide at
# https://docs.docker.com/go/dockerfile-reference/

################################################################################
# Pick a base image to serve as the foundation for the other build stages in
# this file.
FROM ubuntu:24.04 as base

################################################################################
# Install emscripten
FROM base as emscripten
RUN git clone --depth 1 --branch 4.0.5 https://github.com/emscripten-core/emsdk.git && cd emsdk
RUN ./emsdk install latest
RUN ./emsdk activate latest
RUN source ./emsdk_env.sh && cd ..

################################################################################
# Build OR-Tools
FROM emscripten as build-ortools
RUN git clone --depth 1 --branch v9.12 https://github.com/google/or-tools.git && cd or-tools
RUN emcmake cmake -S. -Bbuild -DBUILD_DEPS:BOOL=ON -DBUILD_TESTING=OFF \
-DBUILD_FLATZINC:BOOL=OFF \
-DBUILD_EXAMPLES:BOOL=OFF -DUSE_COINOR:BOOL=OFF -DUSE_HIGHS:BOOL=OFF \
-DUSE_SCIP:BOOL=OFF
RUN cmake --build build

################################################################################
# Install built OR-Tools
FROM build-ortools as install-ortools
RUN cmake --build build --config Release --target install -v

################################################################################
# Create a final stage for running your application.
#
# The following commands copy the output from the "build" stage above and tell
# the container runtime to execute it when the image is run. Ideally this stage
# contains the minimal runtime dependencies for the application as to produce
# the smallest image possible. This often means using a different and smaller
# image than the one used for building the application, but for illustrative
# purposes the "base" image is used here.
FROM base AS final

# Copy the executable from the "build" stage.
COPY --from=build /bin/hello.sh /bin/

# What the container should run when it is started.
ENTRYPOINT [ "/bin/hello.sh" ]
