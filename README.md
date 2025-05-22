# or-tools-javascript-bindings
Javascript bindings for Google's OR-Tools C++ library

## Building
Run the following commands after changing `bindings.cpp` or `CMakeLists.txt` to build the bindings. The built bindings will be stored into `./build`. 

```
# Build and install OR-Tools in Docker, and build the bindings
docker build -t bindings .
# Copy the bindings files into ./build on the host machine
docker run -v ./build:/wrkdir/result --rm bindings
```

## Development
For info on OR-Tools visit their [site](https://developers.google.com/optimization) or [source code](https://github.com/google/or-tools). 

Refer to the [Embind documentation](https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html) to understand how the bindings work. 

Refer to [Emscripten compiler settings](https://emscripten.org/docs/tools_reference/settings_reference.html) to set settings in `CMakeLists.txt`.

It might be helpful to install OR-Tools to your local machine to allow IDE integration in `bindings.cpp`.
