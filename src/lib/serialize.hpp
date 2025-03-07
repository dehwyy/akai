#pragma once
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <variant>
#include <vector>
#include "smart_pointers.hpp"

namespace serialize {
    using V = std::variant<int, double, std::string, bool>;

    class Serializable {
        public:
            virtual ~Serializable() = default;
            virtual std::string String() = 0;
    };

    class Serializer {
        private:
            std::string title;
            std::unordered_map<std::string, V> content;

            friend class SerializerBuilder;

        public:
            std::string String() {
                std::stringstream ss;

                ss << '"' << title << "\": {\n";
                int i = 0;
                for (auto& [key, value] : content) {
                    std::visit([&ss, &key](const auto& v) { ss << "\t" << '"' << key << "\": " << v; },
                               value);
                    // ss << "\t" << '"' << key << "\": \"" << value << "\"";

                    if (i++ < content.size() - 1) {
                        ss << ",";
                    }

                    ss << std::endl;
                }

                ss << "};" << std::endl;

                return ss.str();
            }
    };

    class SerializerBuilder {
        private:
            std::string title;
            std::unordered_map<std::string, V> content;

        public:
            SerializerBuilder() = default;

            void addField(const std::string& name, V value) { content[name] = value; }

            SerializerBuilder* withField(const std::string& name, V value) {
                content[name] = value;
                return this;
            }

            SerializerBuilder* withTitle(const std::string& title) {
                this->title = title;
                return this;
            }

            Box<Serializer> build() {
                auto serializer = NewBox<Serializer>();

                serializer->title = title;
                serializer->content = content;

                return serializer;
            };
    };

}  // namespace serialize
