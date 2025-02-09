#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "ctm_tt.hpp"

namespace _config_parser
{
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
get_from_string(std::string str)
{
	if(str[1] != 'x' && str[1] != 'X')
		return std::stol(str);
	else
		return std::stol(str, nullptr, 16);
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
get_from_string(std::string str)
{
	return std::stod(str);
}

template <typename T>
typename std::enable_if<std::is_same<T, std::string>::value, T>::type
get_from_string(std::string str)
{
	return str;
}

template <typename T>
typename std::enable_if<is_specialization_of<std::tuple, T>::value, T>::type
get_from_string(const std::string& value)
{

	std::stringstream ss(value);
	T tuple;
	std::apply(
		[&ss](auto&... args) {
			std::string token;
			((
				 // Handle quoted strings
				 [&]() {
					 while(ss.peek() == ' ')
						 ss.get();
					 if(ss.peek() == '"')
					 {
						 ss.get(); // Skip opening quote
						 std::getline(ss,
									  token,
									  '"'); // Read until closing quote
						 if(ss.peek() == ' ')
							 ss.get(); // Skip space after quote
					 }
					 else
					 {
						 ss >> token;
					 }
					 args = _config_parser::get_from_string<
						 std::decay_t<decltype(args)>>(token);
				 }()),
			 ...);
		},
		tuple);

	return tuple;
}
template <typename T>
typename std::enable_if<is_specialization_of<std::vector, T>::value, T>::type
get_from_string(const std::string& str)
{
	auto first = str.find("["), second = str.find("]");
	if(first == std::string::npos || second == std::string::npos)
		throw std::runtime_error(" Malformed [");
	first += 1;
	T ret;

	while(true)
	{
		size_t coma = str.find(",", first);
		ret.emplace_back(std::stod(
			str.substr(first, (coma == std::string::npos) ? second : coma)));
		first = coma + 1;
		if(coma == std::string::npos)
			break;
	}

	return ret;
};

size_t countLeadingSpaces(const std::string& str)
{
	return str.find_first_not_of(" \t");
}

std::string trim(const std::string& str)
{
	const auto start = str.find_first_not_of(" \t");
	if(start == std::string::npos)
		return "";
	const auto end = str.find_last_not_of(" \t");
	return str.substr(start, end - start + 1);
}

} // namespace _config_parser

class Node
{
public:
	std::map<std::string, std::string> properties;
	std::map<std::string, std::shared_ptr<Node>> children;
	static constexpr Node* end = nullptr;

	template <typename T>
	using is_vector = std::is_same<
		T,
		std::vector<typename T::value_type, typename T::allocator_type>>;

	template <typename T>
	std::optional<T> get(const std::string& key) const
	{
		auto it = properties.find(key);
		if(it != properties.end())
		{
			return _config_parser::get_from_string<T>(it->second);
		}
		return std::optional<T>{};
	}

	template <typename T>
	std::unordered_map<std::string, T>
	getAllLike()
	{
		if(properties.size() > 0)
		{
			std::string value;
			std::unordered_map<std::string, T> result;
			for(const auto& [key, value] : properties)
			{
				if(value.empty())
					continue;
				result.emplace(key, _config_parser::get_from_string<T>(value));
			}
			return result;
		}
		return std::unordered_map<std::string, T>{};
	}

	void setValue(const std::string& key, const std::string& value)
	{
		properties[key] = value;
	}

	// Add operator[] for chained access
	Node& operator[](const std::string_view& name)
	{
		auto it = children.find(std::string(name));
		return (it != children.end()) ? *it->second : *end;
	}
	// Overload for string literals
	inline Node& operator[](const char* name)
	{
		return operator[](std::string(name));
	}

	// Add bool operator for null checking
	operator bool() const
	{
		return this != end;
	}
};

class ConfigParser
{
public:
	ConfigParser() = default;

	bool parse(const std::string& filename)
	{
		nodes.clear();
		std::ifstream file(filename);
		if(!file.is_open())
		{
			std::cerr << "Failed to open file: " << filename << std::endl;
			return false;
		}

		std::string line;
		std::vector<std::pair<std::string, std::shared_ptr<Node>>> nodeStack;
		auto current_node = std::make_shared<Node>();
		nodes[""] = current_node;

		while(std::getline(file, line))
		{

			// Count leading spaces to determine level
			size_t indent = _config_parser::countLeadingSpaces(line);
			line = _config_parser::trim(line);

			if(line.empty() || line[0] == '#')
				continue;

			// Parse key-value pairs
			size_t delimiter = line.find(':');
			if(delimiter != std::string::npos && !nodeStack.empty())
			{
				std::string key =
					_config_parser::trim(line.substr(0, delimiter));
				std::string value =
					_config_parser::trim(line.substr(delimiter + 1));
				parseValue(*current_node, key, value);
				continue;
			}
			// Pop stack until we're at the right level
			while(!nodeStack.empty() && nodeStack.size() > (indent / 4))
			{
				nodeStack.pop_back();
			}

			// Check for node header [nodeX]
			if(line[0] == '[' && line.back() == ']')
			{
				std::string nodeName = line.substr(1, line.length() - 2);
				auto newNode = std::make_shared<Node>();

				if(nodeStack.empty())
				{
					// Root level node
					nodes[nodeName] = newNode;
					current_node = newNode;
				}
				else
				{
					// Child node
					nodeStack.back().second->children[nodeName] = newNode;
					current_node = newNode;
				}

				nodeStack.push_back({nodeName, current_node});
				continue;
			}
		}

		return true;
	}

	Node& operator[](const std::string_view& nodePath)
	{
		auto it = nodes.find(std::string(nodePath));
		if(it == nodes.end())
			return *Node::end;
		return *(it->second.get());
	}

	// Overload for string literals
	Node& operator[](const char* nodeName)
	{
		return operator[](std::string(nodeName));
	}

private:
	std::map<std::string, std::shared_ptr<Node>> nodes;

	void
	parseValue(Node& node, const std::string& key, const std::string& value)
	{
		node.setValue(key, value);
	}
};
