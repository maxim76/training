if not exist build mkdir build
cd build
cmake -G "Visual Studio 16" .. -DZMQLIB_DIR=C:\Repos\vcpkg\packages\zeromq_x64-windows-static
