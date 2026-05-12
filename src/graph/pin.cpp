#define IMGUI_DEFINE_MATH_OPERATORS

#include <graph/ids.h>
#include <graph/node.h>
#include <graph/pin.h>

#include <imgui_internal.h>

namespace {
    ImColor get_pin_color(const imagegraph::graph::PinType type) {
        switch (type) {
            default:
            case imagegraph::graph::PinType::Texture:
                return {220, 48, 48};
            case imagegraph::graph::PinType::ImageData:
                return {68, 201, 156};
        }
    }

    void draw_icon(ImDrawList* draw_list, const ImVec2& a, const ImVec2& b, const bool filled, const ImU32 color,
                   const ImU32 inner_color) {
        const auto rect = ImRect(a, b);
        const auto rect_center = rect.GetCenter();
        const auto rect_width = rect.GetWidth();
        const auto radius = rect_width * 0.25f;

        if (filled) {
            draw_list->AddCircleFilled(rect_center, radius, color, 16);
        } else {
            if (inner_color & 0xFF000000) {
                draw_list->AddCircleFilled(rect_center, radius, inner_color, 16);
            }
            draw_list->AddCircle(rect_center, radius - 0.5f, color, 16, 2.0f * rect_width / 24.0f);
        }
    }

    void draw_pin_icon(ImDrawList* drawList, const ImVec2& min, const ImVec2& max,
                       const imagegraph::graph::PinType type, const bool connected) {
        auto color = get_pin_color(type);
        color.Value.w = ImGui::GetStyle().Alpha;

        draw_icon(drawList, min, max, connected, color, ImColor(32, 32, 32));
    }

    void draw_pin_icon(const ImVec2 size, const imagegraph::graph::PinType type, const bool connected) {
        if (ImGui::IsRectVisible(size)) {
            const auto cursor_pos = ImGui::GetCursorScreenPos();
            const auto draw_list = ImGui::GetWindowDrawList();
            draw_pin_icon(draw_list, cursor_pos, cursor_pos + size, type, connected);
        }
        ImGui::Dummy(size);
    }
} // namespace

namespace imagegraph::graph {
    Pin::Pin(const ax::NodeEditor::PinKind kind, const PinType type, Node* owner) :
        _id(generate_unique_pin_id()), _kind(kind), _type(type), _owner(owner) {}

    void Pin::draw() const {
        ax::NodeEditor::BeginPin(_id, _kind);
        draw_pin_icon(ImVec2(24, 24), _type, is_connected());
        ax::NodeEditor::EndPin();
    }

    ax::NodeEditor::PinId Pin::id() const { return _id; }

    ax::NodeEditor::PinKind Pin::kind() const { return _kind; }

    PinType Pin::type() const { return _type; }

    Node* Pin::owner() const { return _owner; }
} // namespace imagegraph::graph

namespace imagegraph::graph {
    InputPin::InputPin(const PinType type, Node* owner) :
        Pin(ax::NodeEditor::PinKind::Input, type, owner), _output_pin(nullptr) {}

    Value InputPin::get_value() const {
        if (_output_pin) {
            return _output_pin->get_value();
        }
        return {};
    }

    void InputPin::connect(OutputPin* output_pin) {
        if (_output_pin == output_pin) {
            return;
        }

        if (_output_pin) {
            _output_pin->disconnect(this);
        }

        _output_pin = output_pin;
        _output_pin->connect(this);
        _link_id = generate_unique_link_id();

        _owner->modified();
    }

    void InputPin::disconnect() {
        if (_output_pin) {
            _output_pin->disconnect(this);
            _output_pin = nullptr;
            _link_id = 0;

            _owner->modified();
        }
    }

    bool InputPin::is_connected() const { return _output_pin; }

    OutputPin* InputPin::output_pin() const { return _output_pin; }

    ax::NodeEditor::LinkId InputPin::link_id() const { return _link_id; }
} // namespace imagegraph::graph

namespace imagegraph::graph {
    OutputPin::OutputPin(const PinType type, Node* owner) : Pin(ax::NodeEditor::PinKind::Output, type, owner) {}

    void OutputPin::set_value(const Value value) {
        _value = value;
        for (auto& input_pin: _connections) {
            input_pin->owner()->modified();
        }
    }

    Value OutputPin::get_value() const { return _value; }

    void OutputPin::connect(InputPin* input_pin) {
        if (_connections.insert(input_pin).second) {
            input_pin->connect(this);
        }
    }

    void OutputPin::disconnect(InputPin* input_pin) {
        if (_connections.erase(input_pin)) {
            input_pin->disconnect();
        }
    }

    bool OutputPin::is_connected() const { return !_connections.empty(); }

    std::unordered_set<InputPin*> OutputPin::connections() const { return _connections; }
} // namespace imagegraph::graph
