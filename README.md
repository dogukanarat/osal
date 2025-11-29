# libscaffold

A scaffold template for creating CMake-based C libraries with proper installation support.

## Overview

This is a ready-to-use scaffold for creating C libraries with CMake. It includes:

- ✅ Proper directory structure (`src/`, `include/`)
- ✅ Comprehensive CMakeLists.txt with install commands
- ✅ Example source and header files
- ✅ CMake package configuration for `find_package()` support
- ✅ Intelligent rename script to customize the library name

## Directory Structure

```
libscaffold/
├── include/libscaffold/     # Public headers
│   ├── libscaffold.h        # Main API header
│   └── libscaffoldTypes.h   # Common types and definitions
├── src/                     # Implementation
│   ├── libscaffold.c        # Core implementation
│   └── libscaffoldInternal.c # Internal utilities
├── cmake/                   # CMake modules
│   └── libscaffoldConfig.cmake.in  # Package config template
├── CMakeLists.txt          # Build configuration
├── rename.sh               # Rename script
└── README.md               # This file
```

## Building

### Basic Build

```bash
mkdir build
cd build
cmake ..
make
```

### Build Options

```bash
# Build shared library instead of static
cmake -DBUILD_SHARED_LIBS=ON ..

# Enable tests (if you add them later)
cmake -DBUILD_TESTS=ON ..

# Enable examples (if you add them later)
cmake -DBUILD_EXAMPLES=ON ..
```

## Installation

### System-Wide Installation

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

This will install:
- Library files to `/usr/local/lib/`
- Header files to `/usr/local/include/libscaffold/`
- CMake config files to `/usr/local/lib/cmake/libscaffold/`

### Custom Installation Prefix

```bash
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ..
make
make install
```

## Using the Installed Library

### In Your CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.14)
project(MyProject)

# Find the installed library
find_package(libscaffold REQUIRED)

# Create your executable
add_executable(myapp main.c)

# Link against the library
target_link_libraries(myapp PRIVATE libscaffold::libscaffold)
```

### In Your C Code

```c
#include <libscaffold/libscaffold.h>
#include <stdio.h>

int main(void) {
    // Initialize the library
    if (libscaffoldInit() != LIBSCAFFOLD_SUCCESS) {
        fprintf(stderr, "Failed to initialize library\n");
        return 1;
    }
    
    // Get version
    printf("Library version: %s\n", libscaffoldGetVersion());
    
    // Use the library
    char input[] = "Hello, World!";
    char output[256];
    size_t outputSize = sizeof(output);
    
    LibScaffoldResult result = libscaffoldProcess(
        input, strlen(input),
        output, &outputSize
    );
    
    if (result == LIBSCAFFOLD_SUCCESS) {
        printf("Processed %zu bytes\n", outputSize);
    }
    
    // Cleanup
    libscaffoldCleanup();
    
    return 0;
}
```

## Renaming the Scaffold

To customize this scaffold for your own library, use the provided rename script:

### Preview Changes (Dry Run)

```bash
./rename.sh mylib --dry-run
```

This will show you all the changes that would be made without actually modifying any files.

### Apply Rename

```bash
./rename.sh mylib
```

The script will:
- ✅ Preserve case conventions:
  - `libscaffold` → `mylib` (lowercase)
  - `LibScaffold` → `Mylib` (PascalCase)
  - `LIBSCAFFOLD` → `MYLIB` (UPPERCASE)
- ✅ Rename all files and directories
- ✅ Update all file contents
- ✅ Preserve underscores in names (e.g., `my_lib` stays as `my_lib`)

### After Renaming

1. Review the changes
2. Update this README with your library's specific information
3. Implement your actual library functionality
4. Add tests and examples as needed

## Features

### CMake Installation Support

The scaffold includes complete installation support:

- **Library Installation**: Installs both static and shared libraries
- **Header Installation**: Preserves directory structure
- **CMake Package Config**: Enables `find_package()` support
- **Version Management**: Automatic version file generation
- **Component Support**: Separate Runtime and Development components

### Compiler Support

- C99 standard
- GCC/Clang: Full warnings enabled (`-Wall -Wextra -Wpedantic`)
- MSVC: Warning level 4 (`/W4`)
- Symbol visibility control for shared libraries

## Customization

After renaming, you can customize:

1. **Add more source files**: Update `CMakeLists.txt` `target_sources()`
2. **Add dependencies**: Use `find_package()` and `target_link_libraries()`
3. **Add compile definitions**: Use `target_compile_definitions()`
4. **Add tests**: Create a `test/` directory and enable `BUILD_TESTS`
5. **Add examples**: Create an `example/` directory and enable `BUILD_EXAMPLES`

## License

Customize this section with your own license information.

## Authors

Your name and contributors.
