#pragma once

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>

namespace XuiTheme {

constexpr float kTextScale = 1.2f;
constexpr ImU32 kElevated = IM_COL32(48, 48, 48, 255);
constexpr ImU32 kHighlight = IM_COL32(56, 56, 56, 255);
constexpr ImU32 kMuted = IM_COL32(167, 167, 167, 255);
constexpr ImU32 kWhite = IM_COL32(255, 255, 255, 255);

inline float Clamp(float value)
{
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

inline ImU32 Fade(ImU32 color, float mul = 1.0f)
{
    const float src = static_cast<float>((color >> IM_COL32_A_SHIFT) & 0xff) / 255.0f;
    const float alpha = Clamp(src * ImGui::GetStyle().Alpha * mul);
    return (color & ~IM_COL32_A_MASK)
        | (static_cast<ImU32>(alpha * 255.0f + 0.5f) << IM_COL32_A_SHIFT);
}

inline ImVec2 Measure(ImFont *font, float size, const char *text)
{
    ImFont *use = font ? font : ImGui::GetFont();
    return use->CalcTextSizeA(size * kTextScale, FLT_MAX, 0.0f, text);
}

inline void Text(ImDrawList *draw, ImFont *font, float size, ImVec2 pos,
                 ImU32 color, const char *text, float clipRight = 0.0f)
{
    ImFont *use = font ? font : ImGui::GetFont();
    const float textSize = size * kTextScale;
    if (clipRight > 0.0f) {
        const ImVec4 clip(pos.x, pos.y - 4.0f, clipRight, pos.y + textSize + 8.0f);
        draw->AddText(use, textSize, pos, color, text, nullptr, 0.0f, &clip);
    } else {
        draw->AddText(use, textSize, pos, color, text);
    }
}

struct TouchScrollState {
    ImGuiID windowId = 0;
    bool dragging = false;
    bool moved = false;
    int axis = 0;
    float startX = 0.0f;
    float startY = 0.0f;
    float lastY = 0.0f;
};

inline std::unordered_map<ImGuiID, TouchScrollState> touchScrollStates;
inline ImGuiID touchScrollActiveWindow = 0;
inline int touchScrollFrame = -1;
inline bool touchScrollMovedFrame = false;

inline void TouchScroll(const char *id)
{
    ImGuiIO &input = ImGui::GetIO();
    const ImGuiID windowId = ImGui::GetID(id);
    TouchScrollState &state = touchScrollStates[windowId];
    state.windowId = windowId;

    const int frame = ImGui::GetFrameCount();
    if (touchScrollFrame != frame) {
        touchScrollFrame = frame;
        touchScrollMovedFrame = false;
    }

    if (!input.MouseDown[0]) {
        state.dragging = false;
        state.axis = 0;
        state.moved = false;
        touchScrollActiveWindow = 0;
        if (Ui::touchOwner == Ui::TouchOwner::Content)
            Ui::touchOwner = Ui::TouchOwner::None;
        return;
    }

    if (touchScrollActiveWindow == 0 &&
        Ui::touchOwner != Ui::TouchOwner::Menu &&
        !state.dragging && ImGui::IsWindowHovered()) {
        touchScrollActiveWindow = windowId;
        Ui::touchOwner = Ui::TouchOwner::Content;
        state.dragging = true;
        state.moved = false;
        state.axis = 0;
        state.startX = input.MousePos.x;
        state.startY = input.MousePos.y;
        state.lastY = input.MousePos.y;
    }

    if (touchScrollActiveWindow != windowId || !state.dragging ||
        !ImGui::IsMousePosValid())
        return;

    const float deltaY = input.MousePos.y - state.lastY;
    if (state.axis == 0) {
        const float totalX = input.MousePos.x - state.startX;
        const float totalY = input.MousePos.y - state.startY;
        if (std::max(std::fabs(totalX), std::fabs(totalY)) >= 2.0f) {
            state.axis = std::fabs(totalY) > std::fabs(totalX) ? 1 : -1;
            state.moved = state.axis == 1;
        }
    }
    if (state.axis == 1 && std::fabs(deltaY) >= 0.5f) {
        ImGui::SetScrollY(std::max(0.0f,
                                   std::min(ImGui::GetScrollMaxY(),
                                            ImGui::GetScrollY() - deltaY)));
        state.moved = true;
        touchScrollMovedFrame = true;
    }
    state.lastY = input.MousePos.y;
}

inline bool TouchScrollMoved()
{
    return touchScrollMovedFrame;
}

} // namespace XuiTheme


namespace Ui {

    inline int palette = 0;

constexpr float kBaseW = 880.0f;
constexpr float kBaseH = 560.0f;
constexpr float kOuterPad = 35.0f;
constexpr float kHeaderTop = 24.0f;
constexpr float kContentX = 163.0f;
constexpr float kContentY = 116.0f;
constexpr float kContentRight = 23.0f;
constexpr float kContentBottom = 30.0f;

inline std::string FooterKey()
{
    std::string value = usedKey;
    if (value.empty() || value == " ")
        value = logintext;
    if (value.empty())
        return "Unavailable";
    if (value.size() <= 13)
        return value;
    return value.substr(0, 3) + "..." + value.substr(value.size() - 3);
}

inline float ContentWidth()
{
    return ImGui::GetContentRegionAvail().x;
}

inline float FitText(ImFont *font, float size, const char *text, float maxWidth)
{
    if (!font || !text || maxWidth <= 0.0f)
        return size;
    const float width = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text).x;
    return width > maxWidth ? size * maxWidth / width : size;
}

inline ImVec2 MenuSize(const ImVec2 &display)
{
    const ImVec2 desired(kBaseW, kBaseH);
    const ImVec2 limit(ImMax(360.0f, display.x - 32.0f),
                       ImMax(300.0f, display.y - 14.0f));
    const float fit = ImMin(1.0f, ImMin(limit.x / desired.x, limit.y / desired.y));
    return desired * fit;
}

inline void ApplyPalette()
{
    static const ImVec4 colors[] = {
        ImVec4(58.0f / 255.0f, 58.0f / 255.0f, 58.0f / 255.0f, 1.0f),
        ImVec4(48.0f / 255.0f, 112.0f / 255.0f, 255.0f / 255.0f, 1.0f),
        ImVec4(27.0f / 255.0f, 210.0f / 255.0f, 203.0f / 255.0f, 1.0f),
        ImVec4(29.0f / 255.0f, 185.0f / 255.0f, 84.0f / 255.0f, 1.0f),
        ImVec4(255.0f / 255.0f, 164.0f / 255.0f, 22.0f / 255.0f, 1.0f),
        ImVec4(255.0f / 255.0f, 74.0f / 255.0f, 79.0f / 255.0f, 1.0f),
        ImVec4(241.0f / 255.0f, 52.0f / 255.0f, 172.0f / 255.0f, 1.0f),
        ImVec4(255.0f / 255.0f, 205.0f / 255.0f, 0.0f / 255.0f, 1.0f),
    };
    const ImVec4 &color = colors[ImClamp(palette, 0, IM_ARRAYSIZE(colors) - 1)];
    accentColor[0] = color.x;
    accentColor[1] = color.y;
    accentColor[2] = color.z;
    accentColor[3] = color.w;
    c::raspody::accent = color;
    c::accent = color;
}

inline void ApplyDonorStyle(float scale)
{
    ImGuiStyle &style = ImGui::GetStyle();
    const float s = ImClamp(scale, 0.72f, 1.20f);
    // Match the donor's final ImGui::GetStyle().ScaleAllSizes(3.0f) pass.
    const float k = 3.0f * s;
    const ImVec4 white(1.0f, 1.0f, 1.0f, 1.0f);
    const ImVec4 dark(0.02f, 0.02f, 0.02f, 0.48f);
    const ImVec4 child(0.04f, 0.04f, 0.04f, 0.58f);
    const ImVec4 popup(0.06f, 0.06f, 0.06f, 0.78f);
    const ImVec4 border(0.12f, 0.12f, 0.12f, 0.32f);
    const ImVec4 control(61.0f / 255.0f, 61.0f / 255.0f, 61.0f / 255.0f, 1.0f);
    const ImVec4 frame = control;
    const ImVec4 hovered(0.19f, 0.19f, 0.19f, 1.0f);
    const ImVec4 pressed(0.20f, 0.22f, 0.23f, 1.0f);

    c::raspody::layout = dark;
    c::raspody::sidebar = child;
    c::raspody::child = child;
    c::raspody::widget = frame;
    c::raspody::widget_hover = hovered;
    c::raspody::widget_active = pressed;
    c::raspody::checkbox_fill = control;
    c::button::background = frame;
    c::button::background_hovered = hovered;
    c::button::background_active = pressed;

    style.WindowPadding = ImVec2(6.0f, 6.0f) * k;
    style.FramePadding = ImVec2(2.0f, 2.0f) * k;
    style.ItemSpacing = ImVec2(2.0f, 2.0f) * k;
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f) * k;
    style.IndentSpacing = 21.0f * k;
    style.ScrollbarSize = 0.0f;
    style.GrabMinSize = 1.0f * k;
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f * k;
    style.FrameBorderSize = 0.0f;
    style.WindowRounding = 6.0f * k;
    style.ChildRounding = 6.0f * k;
    style.FrameRounding = 1.5f * k;
    style.ScrollbarRounding = 4.0f * k;
    style.SelectableTextAlign = ImVec2(0.5f, 0.5f);

    ImVec4 *colors = style.Colors;
    colors[ImGuiCol_Text] = white;
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);
    colors[ImGuiCol_WindowBg] = dark;
    colors[ImGuiCol_ChildBg] = child;
    colors[ImGuiCol_PopupBg] = popup;
    colors[ImGuiCol_Border] = border;
    colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0.24f);
    colors[ImGuiCol_FrameBg] = frame;
    colors[ImGuiCol_FrameBgHovered] = hovered;
    colors[ImGuiCol_FrameBgActive] = pressed;
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = frame;
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = frame;
    colors[ImGuiCol_ButtonHovered] = hovered;
    colors[ImGuiCol_ButtonActive] = pressed;
    colors[ImGuiCol_Header] = ImVec4(0, 0, 0, 0.52f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0, 0, 0, 0.36f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.0f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.12f, 0.12f, 0.52f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = pressed;
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_CheckMark] = c::raspody::accent;
    colors[ImGuiCol_SliderGrab] = control;
    colors[ImGuiCol_SliderGrabActive] = control;
    colors[ImGuiCol_NavHighlight] = c::raspody::accent;
    colors[ImGuiCol_NavWindowingHighlight] = c::raspody::accent;
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(c::raspody::accent.x, c::raspody::accent.y, c::raspody::accent.z, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(c::raspody::accent.x, c::raspody::accent.y, c::raspody::accent.z, 0.35f);
}

inline ImFont *DonorTextFont()
{
    return Fonts::donor ? Fonts::donor : ImGui::GetFont();
}

inline ImFont *DonorIconFont()
{
    return Fonts::donor_icons ? Fonts::donor_icons : (F107 ? F107 : ImGui::GetFont());
}

inline bool DonorToggle(const char *label, bool *value, float scale = 1.0f)
{
    if (!label || !value)
        return false;
    const float h = 23.0f * scale;
    const float w = 46.0f * scale;
    const float r = h * 0.5f;
    const ImVec2 rowStart = ImGui::GetCursorScreenPos();
    const float rowWidth = ImGui::GetContentRegionAvail().x;
    const ImVec2 pos(rowStart.x + ImMax(0.0f, rowWidth - w), rowStart.y);
    ImGui::SetCursorScreenPos(pos);
    ImGui::InvisibleButton(label, ImVec2(w, h));
    const bool clicked = ImGui::IsItemClicked() && !XuiTheme::TouchScrollMoved();
    if (clicked)
        *value = !*value;

    ImGuiContext &g = *GImGui;
    float t = *value ? 1.0f : 0.0f;
    if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
        const float active = ImClamp(g.LastActiveIdTimer / 0.50f, 0.0f, 1.0f);
        t = *value ? active : 1.0f - active;
    }
    const ImU32 bg = ImGui::GetColorU32(ImLerp(c::raspody::checkbox_fill,
                                              c::raspody::accent, t));
    ImDrawList *draw = ImGui::GetWindowDrawList();
    draw->AddRectFilled(pos, pos + ImVec2(w, h), bg, r);
    draw->AddCircleFilled(ImVec2(pos.x + r + t * (w - r * 2.0f), pos.y + r),
                          ImMax(1.0f, r - 1.5f * scale), IM_COL32(255, 255, 255, 255));
    ImGui::SetCursorScreenPos(rowStart);
    ImGui::Text("%s", label);
    ImGui::SetCursorScreenPos(ImVec2(rowStart.x, rowStart.y + h + ImGui::GetStyle().ItemSpacing.y));
    return clicked;
}

inline bool DonorCombo(const char *label, int *value, const char *const items[], int count,
                       float scale = 1.0f)
{
    const std::string comboId = std::string("##donor_combo_") + (label ? label : "item");
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f * scale);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f * scale);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, c::raspody::checkbox_fill);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, c::raspody::widget_hover);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, c::raspody::widget_active);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.46f, 0.46f, 0.46f, 0.78f));
    ImGui::SetNextItemWidth(ImMax(1.0f, ImGui::GetContentRegionAvail().x));
    const bool changed = ImGui::Combo(comboId.c_str(), value, items, count);
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(2);
    return changed;
}

inline void DonorInfoRow(const char *icon, const char *label,
                         const std::string &value, float scale)
{
    ImGui::PushFont(DonorIconFont());
    ImGui::Text("%s", icon);
    ImGui::PopFont();
    ImGui::SameLine(0.0f, 8.0f * scale);
    ImGui::Text("%s%s", label, value.c_str());
}

inline bool DonorSliderFloat(const char *label, float *value, float minValue,
                             float maxValue, const char *format, float height,
                             float scale)
{
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems || !value)
        return false;

    ImGuiContext &g = *GImGui;
    const ImGuiStyle &style = g.Style;
    const ImGuiID id = window->GetID(label);
    if (!format)
        format = "%.3f";

    const ImVec2 labelSize = ImGui::CalcTextSize(label, nullptr, true);
    char valueBuf[64] = {};
    const char *valueEnd = valueBuf + ImGui::DataTypeFormatString(
        valueBuf, IM_ARRAYSIZE(valueBuf), ImGuiDataType_Float, value, format);
    const ImVec2 valueSize = ImGui::CalcTextSize(valueBuf, valueEnd);
    const float textHeight = ImMax(labelSize.y, valueSize.y) + style.ItemInnerSpacing.y;
    const float width = ImGui::GetContentRegionAvail().x;
    const float frameHeight = height > 0.0f
        ? height * scale
        : labelSize.y + style.FramePadding.y * 2.0f;
    const ImRect frameBb(window->DC.CursorPos + ImVec2(0.0f, textHeight),
                         window->DC.CursorPos + ImVec2(width, textHeight + frameHeight));
    const ImRect totalBb(window->DC.CursorPos, frameBb.Max);

    ImGui::ItemSize(totalBb, style.FramePadding.y);
    if (!ImGui::ItemAdd(totalBb, id, &frameBb))
        return false;

    const bool hovered = ImGui::ItemHoverable(frameBb, id, 0);
    bool tempInput = ImGui::TempInputIsActive(id);
    if (!tempInput) {
        const bool clicked = hovered && g.IO.MouseClicked[0];
        if (clicked || g.NavActivateId == id) {
            ImGui::SetActiveID(id, window);
            ImGui::SetFocusID(id, window);
            ImGui::FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            if (clicked && g.IO.KeyCtrl)
                tempInput = true;
        }
    }

    if (tempInput)
        return ImGui::TempInputScalar(frameBb, id, label, ImGuiDataType_Float, value,
                                      format, &minValue, &maxValue);

    const ImU32 frameColor = ImGui::GetColorU32(
        g.ActiveId == id ? ImGuiCol_FrameBgActive
                         : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImGui::RenderNavHighlight(frameBb, id);
    ImGui::RenderFrame(frameBb.Min, frameBb.Max, frameColor, true, style.FrameRounding);

    const float t = ImClamp((*value - minValue) / (maxValue - minValue), 0.0f, 1.0f);
    const float fillWidth = ImMax(1.0f, t * (frameBb.GetWidth() - 4.0f));
    const float fillPad = 2.0f * scale;
    const ImRect fillBb(frameBb.Min + ImVec2(fillPad, fillPad),
                        frameBb.Min + ImVec2(fillPad + fillWidth,
                                            fillPad + frameBb.GetHeight() - fillPad * 2.0f));
    window->DrawList->AddRectFilled(fillBb.Min, fillBb.Max,
                                    ImGui::GetColorU32(c::raspody::accent),
                                    style.GrabRounding);

    ImRect grabBb;
    const bool changed = ImGui::SliderBehavior(frameBb, id, ImGuiDataType_Float,
                                               value, &minValue, &maxValue,
                                               format, 0, &grabBb);
    if (changed)
        ImGui::MarkItemEdited(id);
    if (grabBb.Max.x > grabBb.Min.x) {
        const float adjust = (frameHeight - grabBb.GetHeight()) * 0.5f;
        grabBb.Min.y -= adjust;
        grabBb.Max.y += adjust;
        window->DrawList->AddRectFilled(
            grabBb.Min, grabBb.Max,
            ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive
                                                : ImGuiCol_SliderGrab),
            style.GrabRounding);
    }

    if (labelSize.x > 0.0f)
        window->DrawList->AddText(ImVec2(frameBb.Min.x, totalBb.Min.y),
                                  ImGui::GetColorU32(ImGuiCol_Text), label);
    window->DrawList->AddText(ImVec2(frameBb.Max.x - valueSize.x, totalBb.Min.y),
                               ImGui::GetColorU32(ImGuiCol_Text), valueBuf, valueEnd);
    return changed;
}

inline void DonorTitle(const char *title, float width, float scale)
{
    ImGui::SetWindowFontScale(1.50f);
    ImGui::SetCursorPosX((width - ImGui::CalcTextSize(title).x) * 0.5f);
    ImGui::Text("%s", title);
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::SetWindowFontScale(1.0f);
    (void)scale;
}

inline void BeginDonorPage(const char *id, const char *title, float width, float height, float scale)
{
    const float radius = 18.0f * scale;
    const ImVec2 outerPad(18.0f * scale, 18.0f * scale);
    const ImVec2 bodyPad(7.5f * scale, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, outerPad);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, radius);
    const ImGuiWindowFlags pageFlags = ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_AlwaysUseWindowPadding;
    ImGui::BeginChild(id, ImVec2(width * scale, height * scale), false, pageFlags);
    DonorTitle(title, width * scale, scale);
    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const ImVec2 bodySize(ImMax(1.0f, avail.x), ImMax(1.0f, avail.y));
    const std::string bodyId = std::string(id) + "##body";
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, bodyPad);
    const ImGuiWindowFlags bodyFlags = ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysUseWindowPadding;
    ImGui::BeginChild(bodyId.c_str(), bodySize, false, bodyFlags);
    XuiTheme::TouchScroll(bodyId.c_str());
}

inline void EndDonorPage()
{
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
}

inline void DrawDonorHeader(NavState &state, const ImVec2 &min, float scale)
{
    ImDrawList *draw = ImGui::GetWindowDrawList();
    struct Info { const char *icon; const char *name; const char *desc; };
    static constexpr Info info[] = {
        {ICON_FA_HOME, "HOME", "Developer Info & Updates"},
        {ICON_FA_EYE, "VISUAL", "See Enemies Through Walls"},
        {ICON_FA_MAGIC, "MISC", "Recoil control & more"},
        {ICON_FA_USER_SECRET, "SKINHACKS", "Unlock Your Favorite Skins"},
        {ICON_FA_COGS, " SETTINGS", "User Information & Key System"},
    };
    const int index = ImClamp(state.activeTab - 1, 0, IM_ARRAYSIZE(info) - 1);
    const float gap = 6.0f * scale;
    const float brandW = 270.0f * scale;
    const float infoW = 305.0f * scale;
    const float fpsW = 75.0f * scale;
    const float closeW = 155.0f * scale;
    const float h = 77.0f * scale;
    const ImVec2 brand(min.x, min.y);
    const ImVec2 banner(brand.x + brandW + gap, min.y);
    const ImVec2 fps(banner.x + infoW + gap, min.y);
    const ImVec2 close(fps.x + fpsW + gap, min.y);
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushFont(DonorTextFont());
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 18.0f * scale);
    ImGui::SetCursorScreenPos(brand);
    ImGui::BeginChild("##donor_brand", ImVec2(brandW, h), false, flags);
    const ImVec2 image(50.0f * scale, 50.0f * scale);
    ImGui::SetWindowFontScale(1.30f);
    const float textW = ImGui::CalcTextSize("ZUCHIZZHAX MOD").x;
    ImGui::SetWindowFontScale(1.0f);
    ImGui::SetCursorPos(ImVec2((brandW - image.x - textW - 10.0f * scale) * 0.5f,
                               (h - image.y) * 0.5f));
    ImGui::Image(GetDonorLogoTexture(), image);
    ImGui::SameLine();
    ImGui::SetWindowFontScale(1.30f);
    ImGui::SetCursorPosY((h - ImGui::GetTextLineHeight()) * 0.5f);
    ImGui::Text("ZUCHIZZHAX MOD");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();

    ImGui::SetCursorScreenPos(banner);
    ImGui::BeginChild("##donor_indicator", ImVec2(infoW, h), false, flags);
    ImGui::PushFont(DonorIconFont());
    ImGui::SetWindowFontScale(2.40f);
    ImGui::SetCursorPos(ImVec2(18.0f * scale,
                               (h - ImGui::GetTextLineHeight()) * 0.5f));
    ImGui::Text("%s", info[index].icon);
    ImGui::PopFont();
    ImGui::SetWindowFontScale(1.70f);
    ImGui::SetCursorPos(ImVec2(75.0f * scale, 12.0f * scale));
    ImGui::Text("%s", info[index].name);
    ImGui::SetWindowFontScale(0.70f);
    ImGui::SetCursorPos(ImVec2(75.0f * scale, 48.0f * scale));
    ImGui::Text("%s", info[index].desc);
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();

    ImGui::SetCursorScreenPos(fps);
    ImGui::BeginChild("##donor_fps", ImVec2(fpsW, h), false, flags);
    char fpsText[16] = {};
    std::snprintf(fpsText, sizeof(fpsText), "%d", (int)ImGui::GetIO().Framerate);
    ImGui::SetCursorPos(ImVec2((fpsW - ImGui::CalcTextSize("FPS").x) * 0.5f, 12.0f * scale));
    ImGui::Text("FPS");
    ImGui::SetCursorPos(ImVec2((fpsW - ImGui::CalcTextSize(fpsText).x) * 0.5f, 42.0f * scale));
    ImGui::Text("%s", fpsText);
    ImGui::EndChild();

    ImGui::SetCursorScreenPos(close);
    const bool closePressed = ImGui::InvisibleButton("##donor_close", ImVec2(closeW, h))
        && !MenuDragMoved(menu);
    const bool closeHovered = ImGui::IsItemHovered();
    const bool closeHeld = ImGui::IsItemActive();
    const ImVec4 closeFill = closeHeld
        ? ImVec4(0.10f, 0.10f, 0.10f, 0.64f)
        : closeHovered
            ? ImVec4(0.07f, 0.07f, 0.07f, 0.58f)
            : ImVec4(0.02f, 0.02f, 0.02f, 0.48f);
    draw->AddRectFilled(close, close + ImVec2(closeW, h),
                        ImGui::GetColorU32(closeFill), 18.0f * scale);
    draw->AddRect(close, close + ImVec2(closeW, h),
                  IM_COL32(255, 255, 255, 20), 18.0f * scale, 0, 1.0f * scale);
    const ImVec2 closeText = ImGui::CalcTextSize("CLOSE");
    draw->AddText(DonorTextFont(), ImGui::GetFontSize(),
                  ImVec2(close.x + (closeW - closeText.x) * 0.5f,
                         close.y + (h - closeText.y) * 0.5f),
                  IM_COL32(255, 255, 255, 255), "CLOSE");
    if (closePressed)
        CollapseMenu(state);
    ImGui::PopStyleVar();
    ImGui::PopFont();
}

inline void DrawDonorRail(NavState &state, const ImVec2 &min, float scale)
{
    const float outerW = 114.0f * scale;
    const float outerH = 414.0f * scale;
    const float inset = 13.0f * scale;
    const float rowH = 70.0f * scale;
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    ImGui::PushFont(DonorIconFont());
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 18.0f * scale);
    ImGui::SetCursorScreenPos(min);
    ImGui::BeginChild("##donor_rail", ImVec2(outerW, outerH), false, flags);
    const ImVec2 railPos = ImGui::GetWindowPos();
    ImGui::SetWindowFontScale(1.4f);
    const ImVec2 outerSize(outerW, outerH);
    const ImVec2 innerSize(outerSize.x - inset * 2.0f, outerSize.y - inset * 2.0f);
    const ImVec2 innerPos((outerSize.x - innerSize.x) * 0.5f,
                          (outerSize.y - innerSize.y) * 0.5f);

    ImGui::SetCursorPos(innerPos);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.05f, 0.05f, 0.05f, 0.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.19f, 0.19f, 0.19f, 0.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.22f, 0.23f, 0.00f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, c::raspody::child);
    ImGui::BeginChild("##donor_rail_inner", innerSize, false, flags);
    ImGui::Spacing();
    ImVec2 rows[5] = {};
    for (int i = 0; i < SidebarItemCount(); ++i) {
        ImGui::PushID(i);
        const ImVec2 rowPos = ImGui::GetCursorScreenPos();
        rows[i] = rowPos;
        const float rowWidth = ImGui::GetContentRegionAvail().x;
        const bool clicked = ImGui::InvisibleButton("##nav", ImVec2(rowWidth, rowH));
        const ImVec2 center = rowPos + ImVec2(rowWidth * 0.5f, rowH * 0.5f);
        IconCenter(ImGui::GetWindowDrawList(), DonorIconFont(), 35.0f * scale, center,
                   IM_COL32(255, 255, 255, 255), kSidebarItems[i].icon);
        if (clicked && !state.isClosing && !MenuDragMoved(menu))
            state.page = i + 1;
        ImGui::PopID();
    }
    ImGui::EndChild();
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar();

    static float indicatorY = 0.0f;
    const int active = ImClamp(state.page - 1, 0, SidebarItemCount() - 1);
    const float target = rows[active].y - railPos.y;
    indicatorY += (target - indicatorY) * 0.18f;
    if (std::fabs(target - indicatorY) < 0.5f)
        indicatorY = target;
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
    ImGui::SetCursorPos(ImVec2(0.0f, indicatorY));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::BeginChild("##donor_rail_indicator", ImVec2(innerPos.x, rowH), false, flags);
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopFont();
}

inline void DrawDonorVisualPage(float scale)
{
    constexpr float w = 340.0f;
    constexpr float h = 414.0f;
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(255, 255, 255, 255));
    ImGui::PushFont(DonorTextFont());
    ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
    BeginDonorPage("##donor_esp_page", "ESP MENU", w, h, scale);
    ImGui::PushID("donor_esp");
    DonorToggle("Line", &Config.ESPMenu.isPlayerLine, scale);
    DonorToggle("Box", &Config.ESPMenu.Box, scale);
    DonorToggle("Health", &Config.ESPMenu.Health, scale);
    DonorToggle("Name", &Config.ESPMenu.Name, scale);
    DonorToggle("Skeleton", &Config.ESPMenu.Skeleton, scale);
    DonorToggle("Distance", &Config.ESPMenu.Distance, scale);
    DonorToggle("Count", &Config.ESPMenu.Count, scale);
    DonorToggle("Alert", &Config.ESPMenu.Alert, scale);
    DonorToggle("Aimline", &Config.ESPMenu.Aimline, scale);
    DonorToggle("Wallhack", &Config.ExtraMenu.WallHack, scale);
    DonorToggle("Crosshair", &Config.ESPMenu.Crosshair, scale);
    ImGui::Spacing();
    static const char *boxStyles[] = {"Rounded", "Corner", "3D"};
    static const char *healthStyles[] = {"Default", "Indicator"};
    int box = ImClamp((int)Config.ESPMenu.BoxType, 0, IM_ARRAYSIZE(boxStyles) - 1);
    int health = ImClamp((int)Config.ESPMenu.HealthPosition, 0, IM_ARRAYSIZE(healthStyles) - 1);
    DonorCombo("Box Style", &box, boxStyles, IM_ARRAYSIZE(boxStyles), scale);
    DonorCombo("Health Style", &health, healthStyles, IM_ARRAYSIZE(healthStyles), scale);
    Config.ESPMenu.BoxType = (EspBoxType)box;
    Config.ESPMenu.HealthPosition = (EspHealthPosition)health;
    ImGui::PopID();
    EndDonorPage();

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(3.0f * scale, 0.0f));
    ImGui::SameLine();
    BeginDonorPage("##donor_aim_page", "AIMBOT MENU", w, h, scale);
    ImGui::PushID("donor_aim");
    ImGui::Spacing();
    DonorToggle("Aimbot 360", &Config.Aim.Aimbot360, scale);
    DonorToggle("Bullet Track", &Config.Aim.AimSilent, scale);
    static bool visibleCheck = false;
    static bool hideFov = false;
    DonorToggle("Visible Check", &visibleCheck, scale);
    DonorToggle("Hide FOV", &hideFov, scale);
    ImGui::Spacing();
    static int aimStrength = 0;
    static const char *strengths[] = {"Off", "Level1", "Level2", "Level3", "Max"};
    static const char *triggers[] = {"None", "Shooting", "Scoping"};
    static const char *targets[] = {"Distance", "FOV"};
    static const char *positions[] = {"Head", "Chest", "Body"};
    aimStrength = ImClamp((int)(Config.Aim.AimAssistSize / 25.0f + 0.5f), 0, 4);
    if (DonorCombo("Aim Strength", &aimStrength, strengths, IM_ARRAYSIZE(strengths), scale))
        Config.Aim.AimAssistSize = aimStrength * 25.0f;
    int trigger = ImClamp((int)Config.Aim.Trigger, 0, IM_ARRAYSIZE(triggers) - 1);
    if (DonorCombo("Trigger", &trigger, triggers, IM_ARRAYSIZE(triggers), scale))
        Config.Aim.Trigger = (EAimTrigger)trigger;
    int target = Config.Aim.By == EAim::Crosshair ? 1 : 0;
    if (DonorCombo("Target", &target, targets, IM_ARRAYSIZE(targets), scale))
        Config.Aim.By = target == 1 ? EAim::Crosshair : EAim::Distance;
    int position = ImClamp((int)Config.Aim.Target, 0, IM_ARRAYSIZE(positions) - 1);
    if (DonorCombo("Position", &position, positions, IM_ARRAYSIZE(positions), scale))
        Config.Aim.Target = (EAimTarget)position;
    ImGui::SliderFloat("FOV", &Config.Aim.size, 0.0f, 500.0f, "%.3f");
    Config.Aim.Cross = Config.Aim.size;
    ImGui::PopID();
    EndDonorPage();
    ImGui::PopFont();
    ImGui::PopStyleColor();
}

inline void DonorButtonToggle(const char *label, bool &value, float width, float scale)
{
    const ImVec2 size(width, 33.0f * scale);
    const ImU32 color = value ? IM_COL32(7, 242, 31, 255) : IM_COL32(255, 0, 0, 255);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    if (ImGui::Button(label, size))
        value = !value;
    ImGui::PopStyleColor(3);
}

inline void DonorButtonRow(const char *leftLabel, bool &leftValue,
                           const char *rightLabel, bool &rightValue, float scale)
{
    const float gap = ImGui::GetStyle().ItemSpacing.x;
    const float rowWidth = ImMax(1.0f, ImGui::GetContentRegionAvail().x);
    const float buttonWidth = ImMax(1.0f, (rowWidth - gap) * 0.5f);
    DonorButtonToggle(leftLabel, leftValue, buttonWidth, scale);
    ImGui::SameLine(0.0f, gap);
    DonorButtonToggle(rightLabel, rightValue, buttonWidth, scale);
}

inline void DrawDonorMiscPage(float scale)
{
    constexpr float w = 340.0f;
    constexpr float h = 414.0f;
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(255, 255, 255, 255));
    ImGui::PushFont(DonorTextFont());
    ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
    BeginDonorPage("##donor_memory_page", "MEMORY MENU", w, h, scale);
    ImGui::PushID("donor_memory");
    DonorToggle("Hitbox", &Config.ExtraMenu.Hit, scale);
    DonorToggle("No Recoil", &Config.ExtraMenu.Recoil, scale);
    DonorToggle("No Spread", &Config.ExtraMenu.Spread, scale);
    DonorToggle("No Shake", &Config.ExtraMenu.Shake, scale);
    DonorToggle("Firerate", &Config.ExtraMenu.Fire, scale);
    DonorToggle("Fast Reload", &Config.ExtraMenu.Reload, scale);
    DonorToggle("Fast Scope", &Config.ExtraMenu.Scope, scale);
    DonorToggle("Quick Switch", &Config.ExtraMenu.Switch, scale);
    DonorToggle("Weapon Kinetic", &Config.ExtraMenu.Kinetic, scale);
    DonorToggle("No Overheat", &Config.ExtraMenu.Rpd, scale);
    DonorToggle("Unlock Blueprint", &Config.ExtraMenu.UnlockBlueprint, scale);
    DonorToggle("No Parachute", &Config.ExtraMenu.Parachute, scale);
    DonorToggle("Fast Armor Plate", &Config.ExtraMenu.FastArmorPlate, scale);
    DonorToggle("Fast Dive", &Config.ExtraMenu.Diving, scale);
    DonorToggle("Anti Flashbang", &Config.ExtraMenu.Flash, scale);
    ImGui::PopID();
    EndDonorPage();
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(3.0f * scale, 0.0f));
    ImGui::SameLine();
    BeginDonorPage("##donor_quick_page", "QUICK HACKS", w, h, scale);
    ImGui::PushID("donor_quick");
    DonorButtonRow("RECOIL", Config.ExtraMenu.Recoil,
                   "SPREAD", Config.ExtraMenu.Spread, scale);
    DonorButtonRow("RELOAD", Config.ExtraMenu.Reload,
                   "SHAKE", Config.ExtraMenu.Shake, scale);
    DonorButtonRow("SCOPE", Config.ExtraMenu.Scope,
                   "SWITCH", Config.ExtraMenu.Switch, scale);
    ImGui::SliderFloat("Snowboard", &SnowBsize, 0.0f, 100.0f, "%.0f%%");
    ImGui::SliderFloat("High Jump", &jumpHeightMultiplier, 1.0f, 10.0f, "%.2fx");
    ImGui::PopID();
    EndDonorPage();
    ImGui::PopFont();
    ImGui::PopStyleColor();
}

inline void DrawDonorHomePage(float scale)
{
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(255, 255, 255, 255));
    ImGui::PushFont(DonorTextFont());
    ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
    ImGui::BeginChild("##donor_version", ImVec2(276.0f * scale, 106.0f * scale));
    const ImVec2 logo(65.0f * scale, 65.0f * scale);
    const float textW = ImGui::CalcTextSize("1.6.57").x;
    ImGui::SetCursorPos(ImVec2((276.0f * scale - logo.x - textW - 20.0f * scale) * 0.5f,
                               (106.0f * scale - logo.y) * 0.5f));
    ImGui::Image(GetDonorCodmTexture(), logo);
    ImGui::SameLine();
    ImGui::SetWindowFontScale(1.80f);
    ImGui::Text("1.6.57");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::EndChild();
    ImGui::SameLine();
    BeginDonorPage("##donor_changelog", "CHANGELOGS", 410.0f, 414.0f, scale);
    ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), " V58 UI UPDATE");
    ImGui::Text("  - Added sticky child headers");
    ImGui::Text("  - Added touch scrolling for page bodies");
    ImGui::Text("  - Updated translucent black panels");
    ImGui::TextColored(ImVec4(0, 1, 1, 1), " CONTROLS");
    ImGui::Text("  - Matched toggle, combo, and slider styling");
    ImGui::Text("  - Improved sidebar and header alignment");
    ImGui::TextColored(ImVec4(0.6f, 0.85f, 1.0f, 1.0f), " LAYOUT");
    ImGui::Text("  - Added child edge padding");
    ImGui::Text("  - Restored rounded menu surfaces");
    EndDonorPage();
    ImGui::SetCursorPos(ImVec2(0.0f, 114.0f * scale));
    BeginDonorPage("##donor_about", "ABOUT US", 276.0f, 303.0f, scale);
    ImGui::Text(" Developer: ZuchizzHax");
    EndDonorPage();
    ImGui::PopFont();
    ImGui::PopStyleColor();
}

inline void DrawDonorSkinPage(float scale)
{
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(255, 255, 255, 255));
    ImGui::PushFont(DonorTextFont());
    ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
    BeginDonorPage("##donor_skin_page", "SKINHACKS", 683.0f, 414.0f, scale);
    static int tab = 0;
    if (ImGui::Button("WEAPONS", ImVec2(120.0f * scale, 34.0f * scale))) tab = 0;
    ImGui::SameLine();
    if (ImGui::Button("CHARACTERS", ImVec2(140.0f * scale, 34.0f * scale))) tab = 1;
    ImGui::Separator();
    SkinContent(tab);
    EndDonorPage();
    ImGui::PopFont();
    ImGui::PopStyleColor();
}

inline void DrawDonorSettingsPage(float scale)
{
    ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(255, 255, 255, 255));
    ImGui::PushFont(DonorTextFont());
    ImGui::SetCursorPos(ImVec2(0.0f, 0.0f));
    BeginDonorPage("##donor_config_page", "CONFIGS", 276.0f, 414.0f, scale);
    if (ImGui::Button("SAVE", ImVec2(120.0f * scale, 34.0f * scale))) SaveConfig();
    ImGui::SameLine();
    if (ImGui::Button("LOAD", ImVec2(120.0f * scale, 34.0f * scale))) LoadConfig();
    if (ImGui::Button("RESET", ImVec2(ImMax(1.0f, ImGui::GetContentRegionAvail().x),
                                       34.0f * scale)))
        ResetSettings();
    EndDonorPage();
    ImGui::SameLine();
    BeginDonorPage("##donor_key_page", "KEY SYSTEM", 410.0f, 414.0f, scale);
    DonorInfoRow(ICON_FA_USER, "User Key : ", FooterKey(), scale);
    ImGui::Separator();
    const std::string remaining = getExpiryCountdown();
    DonorInfoRow(ICON_FA_CLOCK, "Remaining Time : ", remaining, scale);
    ImGui::Separator();
    DonorInfoRow(ICON_FA_STICKY_NOTE, "Note : ", "Playsafe to avoid ban", scale);
    ImGui::Separator();
    DonorInfoRow(ICON_FA_TELEGRAM, "Owner : ", "ZENNHAXX", scale);
    EndDonorPage();
    ImGui::PopFont();
    ImGui::PopStyleColor();
}

inline void DrawDonorPage(int page, float scale)
{
    switch (page) {
    case 1: DrawDonorHomePage(scale); break;
    case 2: DrawDonorVisualPage(scale); break;
    case 3: DrawDonorMiscPage(scale); break;
    case 4: DrawDonorSkinPage(scale); break;
    case 5: DrawDonorSettingsPage(scale); break;
    default: DrawDonorVisualPage(scale); break;
    }
}

} 
