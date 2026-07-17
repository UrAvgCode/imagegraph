#pragma once

#include <imagegraph/graph/node.h>

#include <memory>
#include <string_view>
#include <typeindex>
#include <unordered_map>

namespace imagegraph::nodes {
    class Registry {
    public:
        Registry();

        std::unique_ptr<graph::Node> create(const char*) const;

        const char* type(const graph::Node&) const;

        const char* label(const char*) const;

        std::span<const char* const> categories() const;

        std::span<const char* const> category_types(const char*) const;

    private:
        using Factory = std::unique_ptr<graph::Node> (*)();

        struct Entry {
            const char* label;
            Factory factory;
        };

        template<typename NodeType>
        void register_node(const char*, const char* label, const char* category);

        std::unordered_map<std::string_view, Entry> _entries;
        std::unordered_map<std::type_index, const char*> _types;

        std::vector<const char*> _categories;
        std::unordered_map<std::string_view, std::vector<const char*>> _category_types;
    };
} // namespace imagegraph::nodes
