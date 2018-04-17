# **BICLA** - **BI**SECT **C**ommand **L**ine **A**rguments Parser

## Build

### Setup conan remotes

```
> conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
```

### Windows
```
> md build
> cd build
> cmake .. -DBICLA_BUILD_TESTS=1 -G "Visual Studio 15 2017 Win64"
> cmake --build .
```

Open the solution file build\bicla.sln
