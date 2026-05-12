#pragma once

#include <image/image.h>
#include <image/texture.h>

#include <imgui_node_editor.h>

#include <unordered_set>
#include <variant>

namespace imagegraph::graph {
    class Node;
    class OutputPin;

    enum class PinType { Texture, ImageData };
    using Value = std::variant<image::Texture*, image::Image*>;

    class Pin {
    public:
        explicit Pin(ax::NodeEditor::PinKind, PinType, Node*);
        virtual ~Pin() = default;

        Pin(const Pin&) = delete;
        Pin& operator=(const Pin&) = delete;

        Pin(Pin&&) = default;
        Pin& operator=(Pin&&) = default;

        void draw() const;

        ax::NodeEditor::PinId id() const;
        ax::NodeEditor::PinKind kind() const;
        PinType type() const;
        Node* owner() const;

        virtual bool is_connected() const = 0;

    protected:
        ax::NodeEditor::PinId _id;
        ax::NodeEditor::PinKind _kind;
        PinType _type;
        Node* _owner;
    };

    class InputPin final : public Pin {
    public:
        explicit InputPin(PinType, Node*);

        Value get_value() const;

        void connect(OutputPin*);
        void disconnect();
        bool is_connected() const override;

        OutputPin* output_pin() const;
        ax::NodeEditor::LinkId link_id() const;

    private:
        OutputPin* _output_pin;
        ax::NodeEditor::LinkId _link_id;
    };

    class OutputPin final : public Pin {
    public:
        explicit OutputPin(PinType, Node*);

        void set_value(Value);
        Value get_value() const;

        void connect(InputPin*);
        void disconnect(InputPin*);
        bool is_connected() const override;

        std::unordered_set<InputPin*> connections() const;

    private:
        std::unordered_set<InputPin*> _connections;
        Value _value;
    };
} // namespace imagegraph::graph
