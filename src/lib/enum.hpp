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
            explicit Option(T&& value) : value(std::forward<T>(value)), hasValue(true) {}
            Option() : hasValue(false) {}

            static Option<T> Some(T&& value) { return Option<T>(std::forward<T>(value)); }
            static Option<T> None() { return Option<T>(); }

            bool isSome() const { return this->hasValue; }
            bool isNone() const { return !this->hasValue; }

            void set(T&& newValue) {
                hasValue = true;
                value = std::forward<T>(newValue);
            }
            void reset() { hasValue = false; }

            /// @brief Unwraps inner value. Consumers Self.
            /// @throws std::runtime_error if Self has no value.
            /// @return <T> inner value.
            T unwrap() {
                if (!this->hasValue) {
                    throw std::runtime_error("Option has no value");
                }

                this->hasValue = false;
                return std::move(value);
            }

            /// @brief Tries to unwrap inner value. Doesn't consume Self.
            /// @returns <T> inner value.
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
                return Result<T, E>(std::forward<T>(value));
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
            /// @throws std::invalid_argument if <T> is not copyable
            std::pair<T, E*> get() {
                if constexpr (!std::is_copy_constructible_v<T>) {
                    throw std::invalid_argument("<T> is not copyable");
                }

                return std::make_pair(std::move(value), isError ? &error : nullptr);
            }
    };
}  // namespace result
