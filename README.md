# AnalysisCoreCpp

A lightweight C++ library providing core utility functions for data analysis (no ROOT required). The cxxopts dependency for command-line argument parsing is found or fetched automatically.

## Features

- Input/output utilities
- Statistical utilities
- Error handling and custom exceptions
- Stream state management

## Usage

### As an External Dependency (Prebuilt)

If you already built this project, downstream projects can use the existing build artifacts via CMake package config (no `add_subdirectory()` required).

1. **Build AnalysisCoreCpp once**
    ```
    mkdir build 
    cd build/
    cmake ..
    make
    ```

    To use a local cxxopts package, pass its config directory with `-Dcxxopts_DIR=/path/to/cxxopts/lib/cmake/cxxopts`.

2. **Set an environment variable to the generated package config directory**
    ```bash
    export AnalysisCoreCpp_DIR="/path/to/AnalysisCoreCpp/build/cmake"
    ```

3. **In your consumer CMakeLists.txt:**
    ```cmake
    if(DEFINED ENV{AnalysisCoreCpp_DIR})
      list(PREPEND CMAKE_PREFIX_PATH "$ENV{AnalysisCoreCpp_DIR}")
    endif()

    find_package(AnalysisCoreCpp CONFIG REQUIRED)
    target_link_libraries(your_target PRIVATE AnalysisCoreCpp::AnalysisCoreCpp)
    ```

### As an External Dependency (Source Tree)

If you prefer building it as part of your project, use `add_subdirectory()`:

```cmake
add_subdirectory($ENV{AnalysisCoreCpp_SRC_DIR})
target_link_libraries(your_target PRIVATE AnalysisCoreCpp)
```

4. **Include the headers in your code:**
    ```cpp
    #include "io_utils.h"
    #include "stat_utils.h"
    #include "InputParser.h"

    // Use the utilities
    auto parser = InputParser(argc, argv);
    ```

The `AnalysisCoreCpp` library will automatically add the correct include path for you, so you do **not** need to copy headers or manually set include directories.

## Testing

GTest is only required when configuring with BUILD_TESTS=ON.
Tests are in the `tests/` directory. To build and run them:

```bash
mkdir build
cd build
cmake -DBUILD_TESTS=ON .. 
make tests
ctest
```

## License

This project is licensed under the GNU General Public License (GPL). See the [LICENSE](LICENSE) file for details.
