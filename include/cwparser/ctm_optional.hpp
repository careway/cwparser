#pragma once

// Detectar si C++17 está activo
#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)

#include <optional>

namespace custom {
    // ESTRATEGIA 1: Si estamos en C++17, 'custom::optional' es un alias directo de 'std::optional'.
    // Esto garantiza 100% de compatibilidad binaria y cero overhead.
    template <typename T>
    using optional = std::optional<T>;
    
    using bad_optional_access = std::bad_optional_access;
}

#else 

#include <utility>
#include <stdexcept>
#include <type_traits>

// Intentar incluir la cabecera estándar por si se mezcla con código C++17 
// o se usa la versión experimental de C++14 (<experimental/optional>)
#if __has_include(<optional>)
#include <optional>
#define CUSTOM_HAS_STD_OPTIONAL 1
#endif

namespace custom {

    class bad_optional_access : public std::exception {
    public:
        const char* what() const noexcept override { return "bad optional access"; }
    };

    template<typename T>
    class optional {
    public:
        optional() noexcept : has_value_(false) {}
    
        optional(const T& value) : has_value_(true) {
            ::new (static_cast<void*>(storage_)) T(value);
        }
    
        optional(T&& value) : has_value_(true) {
            ::new (static_cast<void*>(storage_)) T(std::move(value));
        }
    
        optional(const optional& other) : has_value_(other.has_value_) {
            if (has_value_) {
                ::new (static_cast<void*>(storage_)) T(other.get_value());
            }
        }
    
        optional(optional&& other) noexcept(std::is_nothrow_move_constructible<T>::value) 
            : has_value_(other.has_value_) 
        {
            if (has_value_) {
                ::new (static_cast<void*>(storage_)) T(std::move(other.get_value()));
                other.reset();
            }
        }

        // ESTRATEGIA 2: Interoperabilidad nativa con std::optional si existe en el entorno mixto
#if defined(CUSTOM_HAS_STD_OPTIONAL)
        // Permitir construir custom::optional a partir de un std::optional estándar
        optional(const std::optional<T>& other) : has_value_(other.has_value()) {
            if (has_value_) {
                ::new (static_cast<void*>(storage_)) T(other.value());
            }
        }

        optional(std::optional<T>&& other) : has_value_(other.has_value()) {
            if (has_value_) {
                ::new (static_cast<void*>(storage_)) T(std::move(other.value()));
                other.reset();
            }
        }

        // Permitir conversión implícita de custom::optional a std::optional
        operator std::optional<T>() const {
            if (has_value_) {
                return std::optional<T>(get_value());
            }
            return std::optional<T>();
        }
#endif

        ~optional() { reset(); }
    
        optional& operator=(const optional& other) {
            if (this != &other) {
                reset();
                if (other.has_value_) {
                    ::new (static_cast<void*>(storage_)) T(other.get_value());
                    has_value_ = true;
                }
            }
            return *this;
        }
    
        optional& operator=(optional&& other) noexcept(std::is_nothrow_move_assignable<T>::value) {
            if (this != &other) {
                reset();
                if (other.has_value_) {
                    ::new (static_cast<void*>(storage_)) T(std::move(other.get_value()));
                    has_value_ = true;
                    other.reset();
                }
            }
            return *this;
        }

        T& operator*() noexcept { return get_value(); }
        const T& operator*() const noexcept { return get_value(); }
        T* operator->() noexcept { return &get_value(); }
        const T* operator->() const noexcept { return &get_value(); }
    
        explicit operator bool() const noexcept { return has_value_; }
        bool has_value() const noexcept { return has_value_; }
    
        T& value() {
            if (!has_value_) throw bad_optional_access();
            return get_value();
        }
    
        const T& value() const {
            if (!has_value_) throw bad_optional_access();
            return get_value();
        }
    
        void reset() noexcept {
            if (has_value_) {
                get_value().~T();
                has_value_ = false;
            }
        }

        // Operadores de comparación
        bool operator==(const optional& other) const {
            if (has_value_ != other.has_value_) return false;
            if (!has_value_) return true;
            return get_value() == other.get_value();
        }
        bool operator!=(const optional& other) const { return !(*this == other); }

    private:
        T& get_value() noexcept { return *reinterpret_cast<T*>(storage_); }
        const T& get_value() const noexcept { return *reinterpret_cast<const T*>(storage_); }

        bool has_value_;
        alignas(T) unsigned char storage_[sizeof(T)];
    };
}
#endif
