#pragma once

#include "Call_ImGui.h"
#include "imgui_settings.h"

#include <cstring>
#include <string>

extern ImFont *F107;
extern ImFont *FSidebarIcon;
extern ImFont *FSidebarText;

struct NavState {
    float &tabAlpha;
    float &tabAdd;
    int &page;
    int &activeTab;
    bool &windowCollapsed;
    bool &isMenuVisible;
    bool &isClosing;
    float &closeT;
    double &collapseBarLastActiveTime;
    float &collapseBarOpacityAnim;
    float &collapseBarPressAnim;
};

namespace Ui {

inline void IconCenter(ImDrawList *draw, ImFont *font, float size, const ImVec2 &center, ImU32 color, const char *icon)
{
    if (!draw || !font || !icon || !icon[0])
        return;
    const ImVec2 text = font->CalcTextSizeA(size, FLT_MAX, 0.0f, icon);
    draw->AddText(font, size, center - text * 0.5f, color, icon);
}

inline void CollapseMenu(NavState &state)
{
    if (state.isClosing)
        return;
    state.isClosing = true;
    state.closeT = 0.0f;
    state.collapseBarLastActiveTime = ImGui::GetTime();
    state.collapseBarOpacityAnim = 1.0f;
    state.collapseBarPressAnim = 0.0f;
}

constexpr float kCollapseBgAlpha = 0.90f;

inline ImU32 CollapseFill(bool active, bool hovered, float opacity = kCollapseBgAlpha)
{
    if (active) {
        const ImVec4 &accent = c::raspody::accent;
        return ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, opacity * 0.8888889f));
    }
    const ImVec4 dark = hovered
        ? ImVec4(30.0f / 255.0f, 31.0f / 255.0f, 37.0f / 255.0f, opacity)
        : ImVec4(19.0f / 255.0f, 20.0f / 255.0f, 23.0f / 255.0f, opacity);
    return ImGui::GetColorU32(dark);
}

inline ImU32 CollapseBorder(bool active, float opacity = 1.0f)
{
    if (active) {
        const ImVec4 &accent = c::raspody::accent;
        return ImGui::GetColorU32(ImVec4(accent.x, accent.y, accent.z, opacity));
    }
    return IM_COL32(199, 211, 220, (int)(155.0f * opacity));
}

} // namespace Ui

struct SidebarItem {
    const char *icon;
    const char *label;
};

inline constexpr SidebarItem kSidebarItems[] = {
    {ICON_FA_HOME, "Home"},
    {ICON_FA_EYE, "Visual"},
    {ICON_FA_MAGIC, "Misc"},
    {ICON_FA_USER_SECRET, "SkinHacks"},
    {ICON_FA_COGS, "Settings"},
};

inline int SidebarItemCount()
{
    return IM_ARRAYSIZE(kSidebarItems);
}

inline void WindowShell(ImDrawList *draw, const ImVec2 &windowPos, const ImVec2 &windowSize, float uiScale)
{
    const ImVec2 shellMax(windowPos.x + windowSize.x, windowPos.y + windowSize.y);
    const float radius = 18.0f * uiScale;
    draw->AddRectFilled(windowPos, shellMax, IM_COL32(38, 38, 38, 220), radius);
    draw->AddRect(windowPos, shellMax, IM_COL32(255, 255, 255, 20), radius, 0, 1.0f * uiScale);
}

inline void RightSurface(ImDrawList *draw, const ImVec2 &min, const ImVec2 &size, float uiScale)
{
    (void)draw;
    (void)min;
    (void)size;
    (void)uiScale;
}

inline bool SidebarButton(
    ImDrawList *draw,
    const char *id,
    const char *icon,
    const char *label,
    const ImVec2 &buttonMin,
    const ImVec2 &buttonSize,
    bool active,
    float uiScale)
{
    ImGui::SetCursorScreenPos(buttonMin);
    ImGui::InvisibleButton(id, buttonSize);
    const bool hovered = ImGui::IsItemHovered();
    const bool held = ImGui::IsItemActive();
    const bool clicked = ImGui::IsItemClicked();
    const ImVec2 buttonMax(buttonMin.x + buttonSize.x, buttonMin.y + buttonSize.y);
    const ImVec2 fillMin(buttonMin.x, buttonMin.y + 5.0f * uiScale);
    const ImVec2 fillMax(buttonMax.x, buttonMax.y - 5.0f * uiScale);
    const float rounding = 8.0f * uiScale;
    if (held || (hovered && !active))
        draw->AddRectFilled(fillMin, fillMax, ImGui::GetColorU32(c::raspody::widget_active), rounding);

    ImFont *iconFont = FSidebarIcon ? FSidebarIcon : (F107 ? F107 : ImGui::GetFont());
    const float iconSize = iconFont->FontSize;
    const ImVec2 iconText = iconFont->CalcTextSizeA(iconSize, FLT_MAX, 0.0f, icon);
    const ImVec2 center(buttonMin.x + 30.0f * uiScale, buttonMin.y + buttonSize.y * 0.5f);
    const ImU32 iconColor = active
        ? ImGui::GetColorU32(c::raspody::text_active)
        : ImGui::GetColorU32(c::raspody::text3);
    draw->AddText(iconFont, iconSize, center - iconText * 0.5f, iconColor, icon);
    ImFont *textFont = FSidebarText ? FSidebarText : ImGui::GetFont();
    const float textSize = textFont->FontSize;
    const ImVec2 textPx = textFont->CalcTextSizeA(textSize, FLT_MAX, 0.0f, label);
    draw->AddText(textFont, textSize, ImVec2(buttonMin.x + 64.0f * uiScale, buttonMin.y + (buttonSize.y - textPx.y) * 0.5f), iconColor, label);
    return clicked;
}

inline std::string MaskKey(const char *key)
{
    if (!key || !key[0])
        return "Unknown";
    const std::string raw(key);
    if (raw.size() <= 4)
        return raw;
    const size_t left = ImMin((size_t)3, raw.size() / 2);
    const size_t right = ImMin((size_t)2, raw.size() - left);
    return raw.substr(0, left) + "******" + raw.substr(raw.size() - right);
}
