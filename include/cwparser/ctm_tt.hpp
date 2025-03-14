#pragma once
#include <type_traits>
/**
 * Custom type traits implementation
 */

template <template <typename...> class T, typename U>
struct is_specialization_of: std::false_type {};

template <template <typename...> class T, typename... Us>
struct is_specialization_of<T, T<Us...>>: std::true_type {};

template <typename T>
using is_vector = std::is_same< T, 
                  std::vector<typename T::value_type,
                  typename T::allocator_type>>;


// Helper template to detect std::get
template <typename T, typename = void>
struct has_get : std::false_type {};

template <typename T>
struct has_get<T, std::void_t<decltype(std::get<0>(std::declval<T>()))>> : std::true_type {};


