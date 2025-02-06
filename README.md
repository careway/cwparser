# Config Parser

This is a config parser, not that much to say about this. 


## Instructions to use

```cpp

ConfigParser parser;              // Declaration of parser
parser.parse(argc[1]);            // Parse file
    


```



# Config Parser

A flexible C++ configuration parser that supports hierarchical structures, various data types, and intuitive access patterns.

## Features

- Hierarchical configuration structure with nested nodes
- Support for multiple data types (integers, floating-point, strings, vectors, tuples)
- Hex number parsing support
- Easy access using operator[]
- Type-safe value retrieval
- Vector and tuple parsing capabilities

## Usage

### Basic Configuration File Format

```yaml
[node_name]
key1: value1
key2: value2
[parent_node]
    [child_node]
    key3: value_int value_float "this is a string"
```

### Reading Configuration

```cpp
ConfigParser config;
config.parse("config.txt");

// Access nodes using operator[]
auto& node = config["node_name"];

// Access nodes using nested operator[]
auto& child = config["parent_node"]["child_node"];


// Check if node exists
if (node) {
    // Node exists
}

```

### Reading Different Types

```cpp
// Basic types
auto int_value = node.get<int>("key1"); // Returns std::optional<int>
auto double_value = node.get<double>("key2"); // Returns std::optional<double>
auto string_value = node.get<std::string>("key3"); // Returns std::optional<string>
// Hex numbers
// In config: hex_value: 0xFF
auto hex = node.get<int>("hex_value"); // Will parse as hexadecimal
// Vector parsing
// In config: vector_key: [1.0, 2.0, 3.0]
auto vector = node.get<std::vector<double>>("vector_key");
// Tuple parsing
// In config: tuple_key: 1 3.14 hello
auto tuple = node.get<std::tuple<int, double, std::string>>("tuple_key");

```


### Bulk Reading Tuples

```cpp
// Read all properties 
using TupleType = std::tuple<int, double, std::string>;
auto all_tuples = node.getAllLike<TupleType>();
// Config format for bulk reading:
// prop1: 1 2.5 hello
// prop2: 2 3.7 world
// Will return vector of tuples

```
  
## Error Handling

- The parser returns `std::optional` for all get operations
- Node access returns a null node (checkable via boolean operator) if not found
- File parsing returns bool indicating success/failure

## Example Configuration

```yaml
[system]
threads: 4
memory_limit: 1024
debug_mode: true
[graphics]
resolution: [1920, 1080]
refresh_rate: 60
vsync: true
[network]
    [server]
    host: localhost
    port: 8080
    max_connections: 100
[coordinates]
point1: 100 200 300 # Tuple format: x y z
point2: 150 250 350

``` 
## Example Code

```cpp
ConfigParser config;
if (config.parse("settings.cfg")) {
    // Read system settings
    if (auto threads = config["system"].get<int>("threads")) {
        std::cout << "Threads: " << threads << "\n";
    }
    // Read resolution
    if (auto res = config["graphics"].get<std::vector<int>>("resolution")) {
        std::cout << "Resolution: " << (res)[0] << "x" << (res)[1] << "\n";
    }
    // Read coordinates as tuples
    using Point3D = std::tuple<int, int, int>;
    auto points = config["coordinates"].getAllLike<Point3D>();
    for (const auto& point : points) {
        auto [x, y, z] = point;
        std::cout << "Point: " << x << ", " << y << ", " << z << "\n";
    }
}

``` 


## Notes

- Indentation in the config file must use spaces (4 spaces per level)
- Comments start with '#'
- Node names are enclosed in square brackets
- Properties use colon ':' as separator
- Vector values must be enclosed in square brackets
- Tuple values must be space-separated

## Requirements

- C++17 or later
- Standard Template Library (STL)