#pragma once

#include <imagegraph/compute/mask.h>
#include <imagegraph/compute/texture.h>

#include <imgui_node_editor.h>

#include <vector>

namespace imagegraph::graph {
    class Node;
    class OutputPin;

    class Pin {
    public:
        enum class Type { Texture, Mask };

        explicit Pin(ax::NodeEditor::PinKind, Type, Node*, const char*);
        virtual ~Pin() = default;

        Pin(const Pin&) = delete;
        Pin& operator=(const Pin&) = delete;

        Pin(Pin&&) = default;
        Pin& operator=(Pin&&) = default;

        virtual void draw() const = 0;

        ax::NodeEditor::PinId id() const;
        ax::NodeEditor::PinKind kind() const;
        Type type() const;
        Node* owner() const;

    protected:
        union Value {
            compute::Texture* texture;
            compute::Mask* mask;
        };

        ax::NodeEditor::PinId _id;
        ax::NodeEditor::PinKind _kind;
        Type _type;
        Node* _owner;
        const char* _name;
    };

    class InputPin final : public Pin {
    public:
        explicit InputPin(Type, Node*, const char* = nullptr);

        void draw() const override;

        compute::Texture* texture() const;
        compute::Mask* mask() const;

        void connect(OutputPin*);
        void disconnect();

        OutputPin* output_pin() const;
        ax::NodeEditor::LinkId link_id() const;

    private:
        OutputPin* _output_pin;
        ax::NodeEditor::LinkId _link_id;
    };

    class OutputPin final : public Pin {
    public:
        explicit OutputPin(Type, Node*, const char* = nullptr);

        void draw() const override;

        compute::Texture* texture() const;
        compute::Mask* mask() const;

        void set_texture(compute::Texture*);
        void set_mask(compute::Mask*);

        const std::vector<InputPin*>& connections() const;

    private:
        friend void InputPin::connect(OutputPin*);
        friend void InputPin::disconnect();

        void add_connection(InputPin*);
        void remove_connection(InputPin*);

        std::vector<InputPin*> _connections;
        Value _value;
    };
} // namespace imagegraph::graph
