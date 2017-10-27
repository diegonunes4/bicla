**BIsect Command Line Arguments parser**

(c) BISECT UNIPESSOAL LDA



# Build

## Setup conan remotes

```
> conan remote add bintray https://api.bintray.com/conan/uilianries/conan
```

## Windows
```
> md build
> cd build
> conan install .. -s arch=x86_64 -s build_type=Release -s os=Windows -s compiler="Visual Studio" -s compiler.version=15 -s compiler.runtime=MD --build missing
> cmake ..
> cmake --build .
```

