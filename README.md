# AnalysisCoreCpp

A lightweight C++ library providing core utility functions for data analysis, with no external dependencies (no ROOT required).

## Features

- Input/output utilities
- Statistical utilities
- Error handling and custom exceptions
- Stream state management

## Usage

### As an External Dependency

You can add AnalysisCore to your project without copying headers manually by using CMake’s `add_subdirectory()` 

1. **Add AnalysisCoreCpp to your project**  
   - Clone the repo

2. **Set the ${AnalysisCoreCpp_DIR} environment variable**
    ```bash
    export AnalysisCoreCpp_DIR="/path/to/AnalysisCoreCpp"
    ```

3. **In your CMakeLists.txt:**
    ```cmake
    add_subdirectory($ENV{AnalysisCoreCpp_DIR})
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

Tests are in the `tests/` directory. To build and run them:

```bash
mkdir build
cd build
cmake ..
make tests
ctest
```

## License

This project is licensed under the GNU General Public License (GPL). See the [LICENSE](LICENSE) file for details.
