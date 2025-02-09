#include "config_parser.hpp"
#include <fstream>
#include <cstdio>
#include <iostream>
#include <functional>
#include <vector>

// Simple test framework
class TestFramework {
private:
    struct TestCase {
        std::string name;
        std::function<bool()> test;
    };
    std::vector<TestCase> tests;
    int passed = 0;
    int failed = 0;

    void printResult(const std::string& name, bool success) {
        if (success) {
            std::cout << "\033[32m[PASS]\033[0m " << name << std::endl;
            passed++;
        } else {
            std::cout << "\033[31m[FAIL]\033[0m " << name << std::endl;
            failed++;
        }
    }

public:
    void addTest(const std::string& name, std::function<bool()> test) {
        tests.push_back({name, test});
    }

    bool runTests() {
        for (const auto& test : tests) {
            try {
                bool result = test.test();
                printResult(test.name, result);
            } catch (const std::exception& e) {
                std::cout << "\033[31m[EXCEPTION]\033[0m in " << test.name << ": " << e.what() << std::endl;
                failed++;
            }
        }
        
        std::cout << "\nTest Summary:\n";
        std::cout << "Passed: " << passed << "\n";
        std::cout << "Failed: " << failed << "\n";
        std::cout << "Total:  " << tests.size() << "\n";
        
        return failed == 0;
    }
};

// Test fixture class
class ConfigParserTest {
protected:
    ConfigParser parser;
    const std::string test_file = "test_config.txt";

    void setUp() {
        const char* config_content = R"(
[system]
    threads: 4
    memory_limit: 1024
    debug_mode: true
    hex_value: 0xFF

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
    point1: 100 200 300
    point2: 150 250 350

[types_test]
    string_value: "Hello World"
    int_value: 42
    float_value: 3.14159
    hex_number: 0xAB
    vector_nums: [1.0, 2.0, 3.0, 4.0]
    2d_vector: [[1, 2], [3, 4], [5, 6]]
    tuple_value: 1 3.14 "hello"

[malformed]
    empty: [[]]
    missingbr: [[1,2,3][1,3]
)";
        std::ofstream config_file(test_file);
        config_file << config_content;
        config_file.close();
    }

    void tearDown() {
        std::remove(test_file.c_str());
    }

public:
    bool testParseFile() {
        setUp();
        bool result = parser.parse(test_file) && !parser.parse("nonexistent_file.txt");
        tearDown();
        return result;
    }

    bool testSystemSettings() {
        setUp();
        bool success = true;
        
        success &= parser.parse(test_file);
        auto& system = parser["system"];
        success &= bool(system);

        auto threads = system.get<int>("threads");
        success &= threads.has_value() && *threads == 4;

        auto memory = system.get<int>("memory_limit");
        success &= memory.has_value() && *memory == 1024;

        tearDown();
        return success;
    }

    bool testGraphicsSettings() {
        setUp();
        bool success = true;

        success &= parser.parse(test_file);
        auto& graphics = parser["graphics"];
        success &= bool(graphics);

        auto resolution = graphics.get<std::vector<int>>("resolution");
        success &= resolution.has_value() && 
                  (*resolution)[0] == 1920 && 
                  (*resolution)[1] == 1080;

        auto refresh = graphics.get<int>("refresh_rate");
        success &= refresh.has_value() && *refresh == 60;

        tearDown();
        return success;
    }

    bool testNetworkSettings() {
        setUp();
        bool success = true;

        success &= parser.parse(test_file);
        auto& server = parser["network"]["server"];
        success &= bool(server);

        auto host = server.get<std::string>("host");
        success &= host.has_value() && *host == "localhost";

        auto port = server.get<int>("port");
        success &= port.has_value() && *port == 8080;

        tearDown();
        return success;
    }

    bool testCoordinates() {
        setUp();
        bool success = true;

        success &= parser.parse(test_file);
        auto& coords = parser["coordinates"];
        success &= bool(coords);

        using Point3D = std::tuple<int, int, int>;
        auto point1 = coords.get<Point3D>("point1");
        success &= point1.has_value() && 
                  std::get<0>(*point1) == 100 &&
                  std::get<1>(*point1) == 200 &&
                  std::get<2>(*point1) == 300;

        auto all_points = coords.getAll<Point3D>();
        success &= all_points.size() == 2 &&
                  std::get<0>(all_points["point2"]) == 150;

        tearDown();
        return success;
    }

    bool testDataTypes() {
        setUp();
        bool success = true;

        success &= parser.parse(test_file);
        auto& types = parser["types_test"];
        success &= bool(types);

        auto str = types.get<std::string>("string_value");
        success &= str.has_value() && *str == "Hello World";

        auto hex = types.get<int>("hex_number");
        success &= hex.has_value() && *hex == 0xAB;

        auto vec = types.get<std::vector<double>>("vector_nums");
        success &= vec.has_value() && vec->size() == 4 && (*vec)[0] == 1.0;

        auto vec2d = types.get<std::vector<std::vector<int>>>("2d_vector");
        success &= vec2d.has_value() && vec2d->size() == 3 && 
                  (*vec2d)[0].size() == 2 && (*vec2d)[0][0] == 1 && (*vec2d)[0][1] == 2;

        using MixedTuple = std::tuple<int, double, std::string>;
        auto tuple = types.get<MixedTuple>("tuple_value");
        success &= tuple.has_value() &&
                  std::get<0>(*tuple) == 1 &&
                  std::get<1>(*tuple) == 3.14 &&
                  std::get<2>(*tuple) == "hello";

        tearDown();
        return success;
    }

    bool testErrorHandling() {
        setUp();
        bool success = true;

        success &= parser.parse(test_file);
        success &= !parser["nonexistent"];
        success &= !parser["system"].get<int>("nonexistent").has_value();
        success &= parser["system"].get<std::string>("threads").has_value();

        tearDown();
        return success;
    }

    bool testNodeTraversal() {
        setUp();
        bool success = true;

        success &= parser.parse(test_file);
        success &= bool(parser["network"]);
        success &= bool(parser["network"]["server"]);
        success &= !parser["network"]["nonexistent"];
        success &= !parser["network"]["server"]["nonexistent"];

        tearDown();
        return success;
    }


    bool testNodeMalformed(){
        setUp();
        bool success = true;

        success &=parser.parse(test_file);
        success &= bool(parser["malformed"].get<std::vector<std::vector<int>>>("empty"));
        try { 
            parser["malformed"].get<std::vector<int>>("missingbr");
            success &= false;
        }
        catch (std::exception e) {
            success &= true;
        }
        
        tearDown();
        return success;
    }
};

int main() {
    TestFramework framework;
    ConfigParserTest tests;

    framework.addTest("Parse File", [&]() { return tests.testParseFile(); });
    framework.addTest("System Settings", [&]() { return tests.testSystemSettings(); });
    framework.addTest("Graphics Settings", [&]() { return tests.testGraphicsSettings(); });
    framework.addTest("Network Settings", [&]() { return tests.testNetworkSettings(); });
    framework.addTest("Coordinates", [&]() { return tests.testCoordinates(); });
    framework.addTest("Data Types", [&]() { return tests.testDataTypes(); });
    framework.addTest("Error Handling", [&]() { return tests.testErrorHandling(); });
    framework.addTest("Node Traversal", [&]() { return tests.testNodeTraversal(); });
    framework.addTest("Malformed", [&]() { return tests.testNodeMalformed(); });

    return framework.runTests() ? 0 : 1;
} 