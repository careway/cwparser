# Config Parser

A flexible C++ configuration parser that supports hierarchical structures, various data types, and intuitive access patterns.

## Features

- Hierarchical configuration structure with nested nodes
- Support for multiple data types:
  - Basic types (int, double, string)
  - Multidimensional Vectors (e.g., `[1.0, 2.0, 3.0]`, `[[1,2]]`)
  - Space-separated tuples (e.g., `1 3.14 "hello"`)
  - Hex numbers (e.g., `0xFF`)
- Type-safe value retrieval using templates
- Easy node access using operator[]
- Optional value returns to handle missing data
- Support for quoted strings

## Usage

### Basic Configuration File Format

```ini
[node_name]
    key1: value1
    key2: value2

[parent_node]
    [child_node]
        key3: value3
        key4: "quoted string"
```

### Reading Configuration

```cpp
ConfigParser config;
if (config.parse("config.txt")) {
    // Access nodes using operator[]
    auto& node = config["node_name"];
    
    // Access nested nodes
    auto& child = config["parent_node"]["child_node"];
    
    // Check if node exists
    if (node) {
        // Node exists and can be used
    }
}
```

### Reading Different Types

```cpp
// Basic types with optional returns
auto int_val = node.get<int>("int_key");
auto double_val = node.get<double>("double_key");
auto string_val = node.get<std::string>("string_key");

if (int_val.has_value()) {
    int value = *int_val;
}

// Hex numbers
// In config: hex_value: 0xFF
auto hex = node.get<int>("hex_value"); // Returns 255

// Vector parsing
// In config: vector_key: [1.0, 2.0, 3.0]
auto vec = node.get<std::vector<double>>("vector_key");

// Tuple parsing
// In config: point: 100 200 300
auto point = node.get<std::tuple<int, int, int>>("point");

// Mixed type tuples
// In config: mixed: 1 3.14 "hello"
auto mixed = node.get<std::tuple<int, double, std::string>>("mixed");
```

### Bulk Reading Properties

```cpp
// Get all properties of a specific type
auto all_points = node.getAll<std::tuple<int, int, int>>();
// Returns std::unordered_map<std::string, tuple>
```

## Example Configuration

```ini
[system]
    threads: 4
    memory_limit: 1024
    debug_mode: true
    hex_value: 0xFF

[graphics]
    resolution: [1920, 1080]
    refresh_rate: 60
    vsync: true
    2dvec : [[1,2],[3,4]]

[network]
    [server]
        host: localhost
        port: 8080
        max_connections: 100

[coordinates]
    point1: 100 200 300
    point2: 150 250 350
```

## Example Usage

```cpp
ConfigParser config;
if (config.parse("settings.cfg")) {
    // Read system settings
    if (auto threads = config["system"].get<int>("threads")) {
        std::cout << "Threads: " << *threads << "\n";
    }
    
    // Read graphics settings
    if (auto res = config["graphics"].get<std::vector<int>>("resolution")) {
        std::cout << "Resolution: " << (*res)[0] << "x" << (*res)[1] << "\n";
    }
    
    // Read 2d coordinates settings
    if (auto res = config["graphics"].get<std::vector<std::vector<int>>>("2dvec")) {
        for( auto idx0 = 0 ; idx0 < (*res).size(); idx0++ )
        {    
            for( auto idx1 = 0 ; idx1 < (*res)[idx0]; idx1++ )
                std::cout << "[" << idx0 << " , " << idx1 << " ] ->" << (*res)[idx0][idx1]
        
        }
    }
    
    // Read coordinates
    using Point3D = std::tuple<int, int, int>;
    if (auto point = config["coordinates"].get<Point3D>("point1")) {
        auto [x, y, z] = *point;
        std::cout << "Point: " << x << ", " << y << ", " << z << "\n";
    }
    
    // Bulk read all points
    auto all_points = config["coordinates"].getAll<Point3D>();
    for (const auto& [name, point] : all_points) {
        auto [x, y, z] = point;
        std::cout << name << ": " << x << ", " << y << ", " << z << "\n";
    }
}
```

## Format Rules

- Indentation must use 4 spaces per level
- Node names are enclosed in square brackets: `[node_name]`
- Properties use colon as separator: `key: value`
- Vectors must be enclosed in square brackets and comma-separated: `[1, 2, 3]`
- Tuples must be separated by spaces. (This will change to brackets and comma-sparated)
- Strings can be quoted other wise they are space separated: `"Hello World"` or `Hello World`
- Hex numbers start with 0x: `0xFF`
- Comments start with '#' (must be on their own line)



## Requirements

- C++17 or later
- Standard Template Library (STL)
