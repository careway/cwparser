#pragma once

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
