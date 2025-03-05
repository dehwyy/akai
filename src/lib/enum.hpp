#pragma once
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

template <typename T, typename E = std::string>
class Result {
    private:
        T value;
        E error;
        bool isError = false;

        Result(T value) : value(value), isError(false) {}
        Result(E error) : error(error), isError(true) {}

    public:
        static Result<T, E> Ok(T value) { return Result<T, E>(value); }
        static Result<T, E> Err(const char* msg) { return Result<T, E>(E(msg)); }
        static Result<T, E> Err(E* error) {
            if (error == nullptr) {
                throw std::runtime_error("Cannot create Result::Err when 'Error' is nullptr");
            }

            return Result<T, E>(*error);
        }

        T& unwrapRefValue() { return value; }
        std::pair<T, E*> unwrapRef() { return std::make_pair(value, isError ? &error : nullptr); }
};

template <typename T>
class Option {
    private:
        T value;
        bool hasValue = false;

    public:
        Option(T value) : value(std::move(value)), hasValue(true) {}
        Option() : hasValue(false) {}

        static Option<T> TryFrom(std::function<T()> callback) {
            try {
                return Option<T>(callback());
            } catch (const std::exception& err) {
                std::cout << "Exception in Option<T>::From: " << err.what() << std::endl;
                return Option<T>();
            }
        }

        static Option<T> Some(T value) {
            return value == nullptr ? Option<T>::None() : Option<T>(std::move(value));
        }
        static Option<T> None() { return Option<T>(); }

        void set(T value) {
            hasValue = true;
            this->value = std::move(value);
        }

        /// @brief Grants non-null value. object cannot be used after this call
        T unwrap() {
            if (!this->hasValue) {
                throw std::runtime_error("Option has no value");
            }

            this->hasValue = false;
            return std::move(value);
        }

        /// @brief `Unwrap` and set `None`.
        T unwrapSwap() {
            auto v = this->unwrap();
            this->hasValue = false;

            return std::move(v);
        }

        T unwrapRef() const {
            if (!this->hasValue) {
                throw std::runtime_error("Option has no value");
            }

            return std::move(value);
        }

        bool isSome() const { return this->hasValue; }
        bool isNone() const { return !this->isSome(); }
};
