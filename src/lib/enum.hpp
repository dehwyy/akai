#pragma once
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include "logger.hpp"

using namespace logger;

namespace option {
    /// @brief  Contains value or error
    /// @tparam T ValueType
    template <typename T>
    class Option {
        private:
            T value;
            bool hasValue = false;

        public:
            Option(T value) : value(std::move(value)), hasValue(true) {}
            Option() : hasValue(false) {}

            static Option<T> Some(T&& value) { return Option<T>(std::move(value)); }
            static Option<T> None() { return Option<T>(); }

            bool isSome() const { return this->hasValue; }
            bool isNone() const { return !this->hasValue; }

            void set(T&& newValue) {
                hasValue = true;
                value = std::move(newValue);
            }

            T unwrap() {
                if (!this->hasValue) {
                    throw std::runtime_error("Option has no value");
                }

                this->hasValue = false;
                return std::move(value);
            }

            /// @brief Tries to unwrap inner value. Doesn't consume Self.
            /// @returns <T> or throws an exception.
            /// @throws std::runtime_error if Self has no value.
            /// @throws std::invalid_argument if <T> is not copyable.
            /// @note <Self> can be used after.
            T unwrapRef() const {
                // Check if T is copyable
                if constexpr (!std::is_copy_constructible_v<T>) {
                    throw std::invalid_argument("<T> is not copyable");
                }

                if (!this->hasValue) {
                    throw std::runtime_error("Option has no value");
                }

                return value;  // for copyable types
            }
    };
}  // namespace option

namespace result {
    /// @brief  Contains value or error
    /// @tparam T ValueType
    /// @tparam E ErrorType, default = std::string
    /// @todo Consider if  `get` should return <V, const E*> or <V, E*>
    template <typename T, typename E = std::string>
    class Result {
        private:
            T value;
            E error;
            bool isError = false;

            Result(T value) : value(std::move(value)), isError(false) {}
            Result(E error) : error(error), isError(true) {}

        public:
            template <std::enable_if_t<std::is_move_constructible_v<T>, int> = 0>
            static Result<T, E> Ok(T&& value) {
                return Result<T, E>(std::move(value));
            }

            template <std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
            static Result<T, E> Ok(const T& value) {
                return Result<T, E>(value);
            }

            static Result<T, E> Err(const char* msg) { return Result<T, E>(E(msg)); }
            static Result<T, E> Err(E* error) {
                if (error == nullptr) {
                    throw std::runtime_error("Cannot create Result::Err when 'Error' is nullptr");
                }

                return Result<T, E>(*error);
            }

            /// Returns copy of underlying value.
            /// @returns std::pair<T, E*>
            std::pair<T, E*> get() { return std::make_pair(value, isError ? &error : nullptr); }
    };
}  // namespace result
