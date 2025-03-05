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

        std::pair<T&, E*> unwrapRef() const { return std::make_pair(value, isError ? &error : nullptr); }
};

template <typename T>
class Option {
    private:
        T value;
        bool hasValue = false;

    public:
        Option(T value) : value(value), hasValue(true) {}
        Option() : value(nullptr), hasValue(false) {}

        ~Option() {
            if (this->hasValue && this->value != nullptr) {
                delete this->value;
                this->value = nullptr;
            }
        }

        Option(Option&& other) {}
        Option operator=(Option&& other) {}

        static Option<T> FromPtr(T* value) {
            if (value == nullptr) {
                return Option<T>();
            }

            return Option(value);
        }
        static Option<T> TryFrom(std::function<T()> callback) {
            try {
                return Option<T>(callback());
            } catch (const std::exception& err) {
                std::cout << "Exception in Option<T>::From: " << err.what() << std::endl;
                return Option<T>();
            }
        }

        static Option<T> Some(T value) {
            return value == nullptr ? Option<T>::None() : Option<T>(value);
        }
        static Option<T> None() { return Option<T>(); }

        /// @brief Grants non-null value. object cannot be used after this call
        T unwrap() {
            if (!this->hasValue || this->value == nullptr) {
                throw std::runtime_error("Option has no value");
            }

            return std::move(value);
        }

        T& unwrapRef() const {
            if (!this->hasValue || this->value == nullptr) {
                throw std::runtime_error("Option has no value");
            }

            return value;
        }

        bool is_some() const { return this->hasValue && this->value != nullptr; }
        bool is_none() const { return !this->is_some() }
};
