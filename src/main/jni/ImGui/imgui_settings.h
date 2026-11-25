#pragma once

#include "imgui.h"

extern float accentColor[4];

namespace c
{
namespace raspody
{
    inline ImVec4 layout = ImColor(26, 26, 26);
    inline ImVec4 sidebar = ImColor(26, 26, 26);
    inline ImVec4 child = ImColor(26, 26, 26);
    inline ImVec4 widget = ImColor(48, 48, 48);
    inline ImVec4 widget_hover = ImColor(58, 58, 58);
    inline ImVec4 widget_active = ImColor(51, 56, 58);
    inline ImVec4 checkbox_fill = ImColor(48, 48, 48);
    inline ImVec4 checkbox_knob = ImColor(205, 205, 205);
    inline ImVec4 icon = ImColor(167, 167, 167);
    inline ImVec4 accent = ImColor(48, 112, 255);
    inline ImVec4 text = ImColor(167, 167, 167);
    inline ImVec4 text2 = ImColor(205, 205, 205);
    inline ImVec4 text3 = ImColor(167, 167, 167);
    inline ImVec4 text_active = ImColor(255, 255, 255);
    inline ImVec4 popup = ImColor(48, 48, 48);
    inline ImVec4 scroll_bg = ImColor(13, 13, 13);
    inline ImVec4 scroll_thumb = ImColor(87, 87, 87);
    inline ImVec4 separator = ImColor(72, 72, 72);
}

inline float scale = 1.5f;
inline ImVec4 accent = raspody::accent;
inline ImVec4 panel_bg = raspody::child;
inline ImVec4 field_bg = raspody::widget;
inline ImVec4 field_hover = raspody::widget_hover;
inline ImVec4 field_active = raspody::widget_active;

namespace metrics
{
    inline constexpr float check_row = 36.0f;
    inline constexpr float field_row = 78.0f;
    inline constexpr float slider_row = 64.0f;
    inline constexpr float combo_box = 36.0f;
    inline constexpr float slider_track = 12.0f;
    inline constexpr float panel_bottom = 18.0f;
    inline constexpr float widget_text = 1.0f;
    inline constexpr float value_text = 1.0f;
}

namespace child
{
    inline float rounding = 6.0f;
    inline float padding = 8.0f;
}

namespace elements
{
    inline ImVec4 background = raspody::widget;
    inline float rounding = 4.0f;
}

namespace text
{
    inline ImVec4 text_active = raspody::text_active;
    inline ImVec4 text_hov = raspody::text3;
    inline ImVec4 text = raspody::text;
}

namespace button
{
    inline ImVec4 background = raspody::widget;
    inline ImVec4 background_hovered = raspody::widget_hover;
    inline ImVec4 background_active = raspody::widget_active;
}

namespace scrollbar
{
    inline float hitbox_area = 24.0f;
    inline bool left_side = false;
    inline float gutter_spacing = 4.0f;
}
} // namespace c

namespace Theme
{
inline ImVec4 GetAccentVec4(float alpha = 1.0f)
{
    return ImVec4(accentColor[0], accentColor[1], accentColor[2], alpha);
}

inline ImU32 GetAccentTintU32(float strength, float alpha = 1.0f)
{
    const ImVec4 tint(accentColor[0] * strength, accentColor[1] * strength, accentColor[2] * strength, alpha);
    return ImGui::ColorConvertFloat4ToU32(tint);
}

inline void ClampAccent()
{
    accentColor[0] = ImClamp(accentColor[0], 0.0f, 1.0f);
    accentColor[1] = ImClamp(accentColor[1], 0.0f, 1.0f);
    accentColor[2] = ImClamp(accentColor[2], 0.0f, 1.0f);
    accentColor[3] = 1.0f;
}

inline void ApplyThemeState(float uiScale = 1.0f)
{
    c::scale = ImClamp(uiScale, 0.72f, 1.20f);
    c::raspody::accent = GetAccentVec4();
    c::accent = c::raspody::accent;
    c::child::padding = 10.0f / c::scale;

    c::elements::background = c::raspody::widget;
    c::text::text_active = c::raspody::text_active;
    c::text::text_hov = c::raspody::text3;
    c::text::text = c::raspody::text;
    c::panel_bg = c::raspody::child;
    c::field_bg = c::raspody::widget;
    c::field_hover = c::raspody::widget_hover;
    c::field_active = c::raspody::widget_active;
    c::button::background = c::raspody::widget;
    c::button::background_hovered = c::raspody::widget_hover;
    c::button::background_active = c::raspody::widget_active;

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowRounding = 7.0f * c::scale;
    style.ChildRounding = 1.0f * c::scale;
    style.ChildBorderSize = 1.0f * c::scale;
    style.FrameRounding = 1.5f * c::scale;
    style.FrameBorderSize = 1.0f * c::scale;
    style.GrabRounding = c::elements::rounding * c::scale;
    style.PopupRounding = c::elements::rounding * c::scale;
    style.ScrollbarRounding = 4.0f * c::scale;
    style.ScrollbarSize = 18.0f * c::scale;
    style.WindowPadding = ImVec2(6.0f * c::scale, 6.0f * c::scale);
    style.ItemSpacing = ImVec2(2.0f * c::scale, 2.0f * c::scale);
    style.ItemInnerSpacing = ImVec2(4.0f * c::scale, 4.0f * c::scale);
    style.FramePadding = ImVec2(2.0f * c::scale, 2.0f * c::scale);

    style.Colors[ImGuiCol_ScrollbarBg] = c::raspody::scroll_bg;
    style.Colors[ImGuiCol_ScrollbarGrab] = c::raspody::scroll_thumb;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = c::raspody::scroll_thumb;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = c::raspody::widget_active;
    style.Colors[ImGuiCol_WindowBg] = c::raspody::layout;
    style.Colors[ImGuiCol_ChildBg] = c::raspody::child;
    style.Colors[ImGuiCol_FrameBg] = c::field_bg;
    style.Colors[ImGuiCol_FrameBgHovered] = c::field_bg;
    style.Colors[ImGuiCol_FrameBgActive] = c::field_active;
    style.Colors[ImGuiCol_Button] = c::button::background;
    style.Colors[ImGuiCol_ButtonHovered] = c::button::background;
    style.Colors[ImGuiCol_ButtonActive] = c::button::background_active;
    style.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_HeaderHovered] = c::raspody::child;
    style.Colors[ImGuiCol_TabHovered] = c::raspody::child;
    style.Colors[ImGuiCol_PopupBg] = c::panel_bg;
    style.Colors[ImGuiCol_Text] = c::text::text_active;
    style.Colors[ImGuiCol_TextDisabled] = c::raspody::text;
    style.Colors[ImGuiCol_CheckMark] = ImColor(12, 12, 12);
    style.Colors[ImGuiCol_SliderGrab] = ImColor(255, 255, 255);
    style.Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 255, 255);
}
} // namespace Theme
