BIsect Command Line Arguments parser
(c) BISECT UNIPESSOAL LDA

conan remote add bintray https://api.bintray.com/conan/uilianries/conan

= Windows =
md build-release
cd build-release
conan install .. -s arch=x86_64 -s build_type=Release -s os=Windows -s compiler="Visual Studio" -s compiler.version=15 -s compiler.runtime=MD --build missing
cmake .. -G "Visual Studio 15 2017 Win64"
cmake --build .

