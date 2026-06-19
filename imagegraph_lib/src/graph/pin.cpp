#define IMGUI_DEFINE_MATH_OPERATORS

#include <imagegraph/graph/ids.h>
#include <imagegraph/graph/node.h>
#include <imagegraph/graph/pin.h>

#include <imgui_internal.h>

#include <algorithm>
#include <cassert>

namespace {
    ImColor get_pin_color(const imagegraph::graph::Pin::Type type) {
        switch (type) {
            default:
            case imagegraph::graph::Pin::Type::Texture:
                return {220, 48, 48};
            case imagegraph::graph::Pin::Type::Mask:
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
                       const imagegraph::graph::Pin::Type type, const bool connected) {
        auto color = get_pin_color(type);
        color.Value.w = ImGui::GetStyle().Alpha;

        draw_icon(drawList, min, max, connected, color, ImColor(32, 32, 32));
    }

    void draw_pin_icon(const ImVec2 size, const imagegraph::graph::Pin::Type type, const bool connected) {
        if (ImGui::IsRectVisible(size)) {
            const auto cursor_pos = ImGui::GetCursorScreenPos();
            const auto draw_list = ImGui::GetWindowDrawList();
            draw_pin_icon(draw_list, cursor_pos, cursor_pos + size, type, connected);
        }
        ImGui::Dummy(size);
    }
} // namespace

namespace imagegraph::graph {
    Pin::Pin(const ax::NodeEditor::PinKind kind, const Type type, Node* owner, const char* name) :
        _id(generate_unique_pin_id()), _kind(kind), _type(type), _owner(owner), _name(name) {}

    ax::NodeEditor::PinId Pin::id() const { return _id; }

    ax::NodeEditor::PinKind Pin::kind() const { return _kind; }

    Pin::Type Pin::type() const { return _type; }

    Node* Pin::owner() const { return _owner; }
} // namespace imagegraph::graph

namespace imagegraph::graph {
    InputPin::InputPin(const Type type, Node* owner, const char* name) :
        Pin(ax::NodeEditor::PinKind::Input, type, owner, name), _output_pin(nullptr) {}

    void InputPin::draw() const {
        constexpr float icon_size = 24.0f;

        ax::NodeEditor::BeginPin(_id, _kind);
        draw_pin_icon(ImVec2(icon_size, icon_size), _type, _output_pin);
        ax::NodeEditor::EndPin();

        if (_name) {
            ImGui::SameLine(0.0f, 0.0f);
            const float text_height = ImGui::GetTextLineHeight();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (icon_size - text_height) * 0.5f);
            ImGui::TextUnformatted(_name);
        }
    }

    compute::Texture* InputPin::texture() const {
        assert(_type == Type::Texture);
        if (_output_pin) {
            return _output_pin->texture();
        }
        return nullptr;
    }

    compute::Mask* InputPin::mask() const {
        assert(_type == Type::Mask);
        if (_output_pin) {
            return _output_pin->mask();
        }
        return nullptr;
    }

    void InputPin::connect(OutputPin* output_pin) {
        assert(output_pin->type() == _type);

        if (_output_pin == output_pin) {
            return;
        }

        if (_output_pin) {
            _output_pin->remove_connection(this);
        }

        _output_pin = output_pin;
        _output_pin->add_connection(this);
        _link_id = generate_unique_link_id();

        _owner->modified();
    }

    void InputPin::disconnect() {
        if (_output_pin) {
            _output_pin->remove_connection(this);
            _output_pin = nullptr;
            _link_id = 0;

            _owner->modified();
        }
    }

    OutputPin* InputPin::output_pin() const { return _output_pin; }

    ax::NodeEditor::LinkId InputPin::link_id() const { return _link_id; }
} // namespace imagegraph::graph

namespace imagegraph::graph {
    OutputPin::OutputPin(const Type type, Node* owner, const char* name) :
        Pin(ax::NodeEditor::PinKind::Output, type, owner, name), _value{} {
        _connections.reserve(8);
    }

    void OutputPin::draw() const {
        constexpr float icon_size = 24.0f;

        if (_name) {
            const float text_height = ImGui::GetTextLineHeight();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (icon_size - text_height) * 0.5f);
            ImGui::TextUnformatted(_name);
            ImGui::SameLine(0.0f, 0.0f);
        }

        ax::NodeEditor::BeginPin(_id, _kind);
        draw_pin_icon(ImVec2(icon_size, icon_size), _type, !_connections.empty());
        ax::NodeEditor::EndPin();
    }

    compute::Texture* OutputPin::texture() const {
        assert(_type == Type::Texture);
        return _value.texture;
    }

    compute::Mask* OutputPin::mask() const {
        assert(_type == Type::Mask);
        return _value.mask;
    }

    void OutputPin::set_texture(compute::Texture* texture) {
        assert(_type == Type::Texture);
        _value.texture = texture;
        for (const auto input_pin: _connections) {
            input_pin->owner()->modified();
        }
    }

    void OutputPin::set_mask(compute::Mask* mask) {
        assert(_type == Type::Mask);
        _value.mask = mask;
        for (const auto input_pin: _connections) {
            input_pin->owner()->modified();
        }
    }

    void OutputPin::add_connection(InputPin* input_pin) {
        assert(input_pin->type() == _type);
        if (std::ranges::find(_connections, input_pin) == _connections.end()) {
            _connections.push_back(input_pin);
        }
    }

    void OutputPin::remove_connection(InputPin* input_pin) {
        assert(input_pin->type() == _type);
        const auto it = std::ranges::find(_connections, input_pin);
        if (it != _connections.end()) {
            *it = _connections.back();
            _connections.pop_back();
        }
    }

    const std::vector<InputPin*>& OutputPin::connections() const { return _connections; }
} // namespace imagegraph::graph
