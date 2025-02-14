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

namespace cwparser
{
namespace _
{

	/**
	 *  Trivial types
	 */

	template <typename T>
	typename std::enable_if<std::is_integral<T>::value, T>::type
	inline get_from_string(std::string str)
	{
		if (str[1] != 'x' && str[1] != 'X')
			return std::stol(str);
		else
			return std::stol(str, nullptr, 16);
	}

	template <typename T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type
	inline get_from_string(std::string str)
	{
		return std::stod(str);
	}

	template <typename T>
	typename std::enable_if<std::is_same<T, std::string>::value, T>::type
	inline get_from_string(std::string str)
	{
		auto first = str.find('"'), second = str.find('"', first + 1);
		if (first != std::string::npos && second != std::string::npos)
			return str.substr(first + 1, second - 1);
		return str;
	}

	/**
	 * @brief Parse string to tuple c++11 compatible
	 */
	template<typename Tuple, size_t Index = 0>
	typename std::enable_if<Index == std::tuple_size<Tuple>::value, void>::type
	parse_string_to_tuple(std::stringstream &ss, Tuple &tuple) {}

	template<typename Tuple, size_t Index = 0>
	typename std::enable_if<Index < std::tuple_size<Tuple>::value, void>::type
	parse_string_to_tuple(std::stringstream &ss, Tuple &tuple) {
		std::string token;
		while (ss.peek() == ' ') ss.get();
		if (ss.peek() == '"') {
			ss.get(); // Skip opening quote
			std::getline(ss, token, '"'); // Read until closing quote
			if (ss.peek() == ' ') ss.get(); // Skip space after quote
		} else {
			ss >> token;
		}
		std::get<Index>(tuple) = _::get_from_string<typename std::decay<decltype(std::get<Index>(tuple))>::type>(token);
		parse_string_to_tuple<Tuple, Index + 1>(ss, tuple);
	}

	template <typename T>
	typename std::enable_if<is_specialization_of<std::tuple, T>::value, T>::type
	inline get_from_string(const std::string &value)
	{

		std::stringstream ss(value);
		T tuple;
	    parse_string_to_tuple(ss, tuple);

		return tuple;
	}


	/**
	 *  get_from_string recursive types
	 */
	template <typename T>
	typename std::enable_if<is_specialization_of<std::vector, T>::value && !is_specialization_of<std::vector, typename T::value_type>::value, T>::type
	inline get_from_string(const std::string &str)
	{
		size_t first = str.find_first_of("["), second = str.find_last_of("]");
		if (first == std::string::npos || second == std::string::npos)
			throw std::runtime_error("Unmatched brackets");
			
		first += 1;
		if (first == second)
			return T{};

		T ret;

		while (true)
		{
			size_t coma = str.find(",", first);
			ret.emplace_back(get_from_string<typename std::decay<typename T::value_type>::type>(
				str.substr(first, (coma == std::string::npos) ? second : coma)));
			first = coma + 1;
			if (coma == std::string::npos)
				break;
		}

		return ret;
	};

	template <typename T>
	typename std::enable_if<is_specialization_of<std::vector, T>::value && is_specialization_of<std::vector, typename T::value_type>::value, T>::type
	inline get_from_string(const std::string &str)
	{

		size_t first = str.find_first_of("["), second = str.size() - 1;
		if (first == std::string::npos)
			throw std::runtime_error("Error on format");

		T ret;
		size_t sub_first = first + 1, sub_second = sub_first;
		for (size_t idx = first + 1, level = 1; idx < str.size() && level > 0; idx++)
		{
			switch (str[idx])
			{
			case '[':
				level++;
				if (level == 2)
					sub_first = idx;
				break;
			case ']':
				level--;
				if (level == 1)
				{
					sub_second = idx;
					ret.emplace_back(get_from_string<typename std::decay<typename T::value_type>::type>(
						str.substr(sub_first, (sub_second - sub_first) + 1)));
				}
				break;
			default:
				break;
			}
		}

		return ret;

		return ret;
	};

	/**
	 * Tools
	 */
	size_t inline countLeadingSpaces(const std::string &str)
	{
		auto it = str.find_first_not_of(" \t");
		size_t ret = 0;
		if(it == std::string::npos)
			it = str.size();
		for(int i = 0; i < it ; i++)
		{
			ret += str[i] == '\t'? 4 : 1;
		}
		return ret;
	}

	std::string inline trim(const std::string &str)
	{
		const auto start = str.find_first_not_of(" \t");
		if (start == std::string::npos)
			return "";
		const auto end = str.find_last_not_of(" \t");
		return str.substr(start, end - start + 1);
	}

} // namespace _

#ifndef __cplusplus
#elif __cplusplus > 201703L

#include <optional>
#else 
#include "ctm_optional.hpp"
#endif

class Node
{
	#ifndef __cplusplus
	#elif __cplusplus > 201703L
	template<typename T>
	using optional = std::optional<T>;
	#endif
public:
	std::map<std::string, std::string> properties;
	std::map<std::string, std::shared_ptr<Node>> children;
	static constexpr Node *end = nullptr;

	template <typename T>
	optional<T> get(const std::string &key) const
	{
		auto it = properties.find(key);
		if (it != properties.end())
		{
			return _::get_from_string<T>(it->second);
		}
		return optional<T>{};
	}

	template <typename T>
	std::unordered_map<std::string, T>
	getAll()
	{
		if (properties.size() > 0)
		{
			std::string value;
			std::unordered_map<std::string, T> result;
			for (const auto it : properties)
			{
				const auto &key = it.first;
				const auto &value = it.second;
				if (value.empty())
					continue;
				result.emplace(key, _::get_from_string<T>(value));
			}
			return result;
		}
		return std::unordered_map<std::string, T>{};
	}

	void setValue(const std::string &key, const std::string &value)
	{
		properties[key] = value;
	}

	// Add operator[] for chained access
	Node &operator[](const std::string &name)
	{
		auto it = children.find(std::string(name));
		return (it != children.end()) ? *it->second : *end;
	}
	// Overload for string literals
	inline Node &operator[](const char *name)
	{
		return operator[](std::string(name));
	}

	// Add bool operator for null checking
	operator bool() const
	{
		return this != end;
	}
};

class cwparser
{
public:
	cwparser() = default;

	bool parse(const std::string &filename)
	{
		nodes.clear();
		std::ifstream file(filename);
		if (!file.is_open())
		{
			std::cerr << "Failed to open file: " << filename << std::endl;
			return false;
		}

		std::string line;
		std::vector<std::pair<std::string, std::shared_ptr<Node>>> nodeStack;
		auto current_node = std::make_shared<Node>();
		nodes[""] = current_node;

		while (std::getline(file, line))
		{

			// Count leading spaces to determine level
			size_t indent = _::countLeadingSpaces(line);
			line = _::trim(line);

			if (line.empty() || line[0] == '#')
				continue;

			// Parse key-value pairs
			size_t delimiter = line.find(':');
			if (delimiter != std::string::npos && !nodeStack.empty())
			{
				std::string key =
					_::trim(line.substr(0, delimiter));
				std::string value =
					_::trim(line.substr(delimiter + 1));
				parseValue(*current_node, key, value);
				continue;
			}
			// Pop stack until we're at the right level
			while (!nodeStack.empty() && nodeStack.size() > (indent / 4))
			{
				nodeStack.pop_back();
			}

			// Check for node header [nodeX]
			if (line[0] == '[' && line.back() == ']')
			{
				std::string nodeName = line.substr(1, line.length() - 2);
				auto newNode = std::make_shared<Node>();

				if (nodeStack.empty())
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

	Node &operator[](const std::string &nodePath)
	{
		auto it = nodes.find(nodePath);
		if (it == nodes.end())
			return *Node::end;
		return *(it->second.get());
	}

	// Overload for string literals
	Node &operator[](const char *nodeName)
	{
		return operator[](std::string(nodeName));
	}

private:
	std::map<std::string, std::shared_ptr<Node>> nodes;

	void
	parseValue(Node &node, const std::string &key, const std::string &value)
	{
		node.setValue(key, value);
	}
};
} // namespace cwparser