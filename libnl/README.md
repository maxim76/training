Установка зависимостей (Ubuntu)
$ pip install conan
$ apt install cmake


Сборка
$ conan profile detect --force
$ conan install . --output-folder=build --build=missing
$ cd build
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
(в случае Windows, передать доп. параметр generator, а параметр DCMAKE_BUILD_TYPE не нужен (т.к. Vstudio включает все типы билда сразу)
	cmake .. -G "Visual Studio 15 2017" -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
$ cmake --build . --config Release
