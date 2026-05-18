#pragma once

#include <compute/texture.h>
#include <image/image.h>

#include <imgui_node_editor.h>

#include <unordered_set>
#include <variant>

namespace imagegraph::graph {
    class Node;
    class OutputPin;

    enum class PinType { Texture, ImageData };
    using Value = std::variant<compute::Texture*, image::Image*>;

    class Pin {
    public:
        explicit Pin(ax::NodeEditor::PinKind, PinType, Node*, const char*);
        virtual ~Pin() = default;

        Pin(const Pin&) = delete;
        Pin& operator=(const Pin&) = delete;

        Pin(Pin&&) = default;
        Pin& operator=(Pin&&) = default;

        virtual void draw() const = 0;

        ax::NodeEditor::PinId id() const;
        ax::NodeEditor::PinKind kind() const;
        PinType type() const;
        Node* owner() const;

    protected:
        ax::NodeEditor::PinId _id;
        ax::NodeEditor::PinKind _kind;
        PinType _type;
        Node* _owner;
        const char* _name;
    };

    class InputPin final : public Pin {
    public:
        explicit InputPin(PinType, Node*, const char* = nullptr);

        void draw() const override;

        Value get_value() const;

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
        explicit OutputPin(PinType, Node*, const char* = nullptr);

        void draw() const override;

        void set_value(Value);
        Value get_value() const;

        void connect(InputPin*);
        void disconnect(InputPin*);

        std::unordered_set<InputPin*> connections() const;

    private:
        std::unordered_set<InputPin*> _connections;
        Value _value;
    };
} // namespace imagegraph::graph
