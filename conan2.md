# How to configure stuff using `conan2`:

They way to configure stuff with conan2 has changed a bit. Do this instead of
what you might have been doing previously:

```sh
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="conan/conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=Debug
```
