# or-tools-javascript-bindings
Javascript bindings for Google's OR-Tools C++ library

## Building
```
# Build and install OR-Tools in Docker, and build the bindings
docker build -t bindings .
# Copy the bindings files into ./build on the host machine
docker run -v ./build:/wrkdir/result bindings
```
