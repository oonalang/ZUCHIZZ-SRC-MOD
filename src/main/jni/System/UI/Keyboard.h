#pragma once

#include "../../ImGui/imgui.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>

namespace Keypad {

struct AnimState {
    float hover = 0.0f;
    float press = 0.0f;
    float pressTime = 0.0f;
};

struct State {
    bool visible = false;
    bool enabled = false;
    float shownAt = 0.0f;
    float progress = 0.0f;
    float lastFrame = 0.0f;
};

inline std::map<std::string, State> states;

constexpr float kBaseW = 1120.0f;
constexpr float kBaseH = 338.0f;
constexpr ImU32 kPanel = IM_COL32(25, 25, 28, 242);
constexpr ImU32 kPanelEdge = IM_COL32(50, 50, 60, 230);
constexpr ImU32 kKey = IM_COL32(33, 33, 40, 255);
constexpr ImU32 kKeyActive = IM_COL32(39, 39, 49, 255);
constexpr ImU32 kAccent = IM_COL32(192, 195, 255, 255);
constexpr ImU32 kText = IM_COL32(255, 255, 255, 255);
constexpr ImU32 kMuted = IM_COL32(110, 110, 129, 255);

enum class KeyTone {
    Normal,
    Action,
    Accent,
};

inline float ScaleFor(const ImVec2 &display)
{
    return ImMax(0.62f, ImMin(1.0f, ImMin((display.x - 24.0f) / kBaseW, (display.y * 0.46f) / kBaseH)));
}

inline float TopFor(const ImVec2 &display)
{
    const float scale = ScaleFor(display);
    return display.y - (kBaseH + 14.0f) * scale;
}

inline size_t SafeLen(const char *text, size_t cap)
{
    if (!text || cap == 0)
        return 0;
    size_t length = 0;
    while (length < cap && text[length] != '\0')
        ++length;
    return length;
}

inline void Append(char *buffer, size_t cap, const char *text)
{
    if (!buffer || !text || cap == 0)
        return;
    const size_t length = SafeLen(buffer, cap);
    if (length >= cap - 1)
        return;
    const size_t count = std::min(std::strlen(text), cap - length - 1);
    std::memcpy(buffer + length, text, count);
    buffer[length + count] = '\0';
}

inline void Backspace(char *buffer, size_t cap)
{
    const size_t length = SafeLen(buffer, cap);
    if (buffer && length > 0)
        buffer[length - 1] = '\0';
}

inline float FrameDelta(float now, float &last)
{
    float delta = now - last;
    if (last <= 0.0f || delta <= 0.0f || delta > 0.1f)
        delta = 1.0f / 60.0f;
    last = now;
    return delta;
}

inline float Ease(float value)
{
    value = ImClamp(value, 0.0f, 1.0f);
    return value * value * (3.0f - 2.0f * value);
}

inline ImU32 Alpha(ImU32 color, float alpha)
{
    const int source = (color >> IM_COL32_A_SHIFT) & 0xFF;
    const int value = (int)(source * ImClamp(alpha, 0.0f, 1.0f));
    return (color & ~IM_COL32_A_MASK) | ((ImU32)value << IM_COL32_A_SHIFT);
}

inline bool DrawKey(
    ImDrawList *draw,
    ImFont *font,
    const std::string &id,
    const char *label,
    const ImVec2 &min,
    const ImVec2 &size,
    float scale,
    float alpha,
    float now,
    float delta,
    bool enabled,
    KeyTone tone,
    std::map<std::string, AnimState> &animations)
{
    AnimState &anim = animations[id];
    const ImVec2 max = min + size;
    const bool hovered = ImGui::IsMouseHoveringRect(min, max);
    const bool clicked = hovered && enabled && ImGui::IsMouseClicked(0);
    const float target = hovered ? 1.0f : 0.0f;
    anim.hover += (target - anim.hover) * ImMin(1.0f, delta * 14.0f);
    if (clicked) {
        anim.press = 1.0f;
        anim.pressTime = now;
    } else if (now - anim.pressTime > 0.22f) {
        anim.press = 0.0f;
    } else {
        anim.press = 1.0f - (now - anim.pressTime) / 0.22f;
    }

    ImU32 fill = kKey;
    ImU32 edge = kPanelEdge;
    ImU32 text = kText;
    if (tone == KeyTone::Action) {
        fill = IM_COL32(30, 30, 36, 255);
        text = kAccent;
    } else if (tone == KeyTone::Accent) {
        fill = IM_COL32(35, 36, 54, 255);
        edge = kAccent;
    }
    if (anim.hover > 0.18f || anim.press > 0.05f)
        fill = kKeyActive;

    const float rounding = 6.0f * scale;
    const ImVec2 pressInset = ImVec2(2.4f, 2.4f) * (scale * anim.press);
    const ImVec2 drawMin = min + pressInset;
    const ImVec2 drawMax = max - pressInset;
    draw->AddRectFilled(drawMin, drawMax, Alpha(fill, alpha), rounding);
    draw->AddRect(drawMin, drawMax, Alpha(edge, alpha * (tone == KeyTone::Accent ? 0.92f : 0.72f)), rounding, 0, 1.0f * scale);
    if (anim.press > 0.0f) {
        const float release = 1.0f - anim.press;
        const ImVec2 pulse = ImVec2(3.0f, 3.0f) * (scale * release);
        draw->AddRect(drawMin - pulse, drawMax + pulse, Alpha(kAccent, alpha * anim.press * 0.58f), rounding + pulse.x, 0, 1.5f * scale);
        draw->AddRectFilled(drawMin, drawMax, Alpha(kAccent, alpha * anim.press * 0.18f), rounding);
    }

    const float textSize = font->FontSize * (tone == KeyTone::Normal ? 1.0f : 0.88f) * scale;
    const ImVec2 textPx = font->CalcTextSizeA(textSize, FLT_MAX, 0.0f, label);
    const ImVec2 drawSize = drawMax - drawMin;
    draw->AddText(font, textSize, drawMin + (drawSize - textPx) * 0.5f + ImVec2(0.0f, 1.2f * scale * anim.press), Alpha(text, alpha), label);
    return clicked;
}

inline void Render(const char *id, char *buffer, size_t cap, bool *show)
{
    if (!id || !buffer || cap == 0 || !show)
        return;

    static std::map<std::string, AnimState> animations;
    const std::string keyboardId(id);
    State &state = states[keyboardId];
    const float now = (float)ImGui::GetTime();
    const bool stale = now - state.lastFrame > 0.25f;

    if (*show && (!state.visible || stale)) {
        state.visible = true;
        state.enabled = false;
        state.shownAt = now;
        state.progress = 0.0f;
        state.lastFrame = now;
    } else if (!*show) {
        state = {};
        return;
    }
    if (!state.visible)
        return;

    const float delta = FrameDelta(now, state.lastFrame);
    state.progress += (1.0f - state.progress) * ImMin(1.0f, delta * 9.0f);
    if (state.progress > 0.995f)
        state.progress = 1.0f;
    state.enabled = now - state.shownAt > 0.10f;

    ImGuiIO &io = ImGui::GetIO();
    const float scale = ScaleFor(io.DisplaySize);
    const float eased = Ease(state.progress);
    const ImVec2 size(kBaseW * scale, kBaseH * scale);
    const float bottom = 14.0f * scale;
    const ImVec2 pos(
        (io.DisplaySize.x - size.x) * 0.5f,
        io.DisplaySize.y - size.y - bottom + (1.0f - eased) * 34.0f * scale);

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin(
        id,
        nullptr,
        ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings);

    ImDrawList *draw = ImGui::GetWindowDrawList();
    ImFont *font = ImGui::GetFont();
    const float alpha = eased;
    auto S = [&](float value) { return value * scale; };
    draw->AddRectFilled(pos, pos + size, Alpha(kPanel, alpha), S(9.0f));
    draw->AddRect(pos, pos + size, Alpha(kPanelEdge, alpha), S(9.0f), 0, S(1.0f));

    const ImVec2 innerMin = pos + ImVec2(S(18.0f), S(12.0f));
    const ImVec2 innerMax = pos + size - ImVec2(S(18.0f), S(12.0f));
    draw->AddRectFilled(innerMin + ImVec2(0.0f, S(5.0f)), innerMin + ImVec2(S(44.0f), S(8.0f)), Alpha(kAccent, alpha), S(1.5f));
    char countText[24] = {};
    std::snprintf(countText, sizeof(countText), "%zu CHARS", SafeLen(buffer, cap));
    const float countSize = font->FontSize * 0.76f * scale;
    const ImVec2 countPx = font->CalcTextSizeA(countSize, FLT_MAX, 0.0f, countText);
    draw->AddText(font, countSize, ImVec2(innerMax.x - S(42.0f) - countPx.x, innerMin.y + S(2.0f)), Alpha(kMuted, alpha), countText);

    const ImVec2 closeMin(innerMax.x - S(28.0f), pos.y + S(9.0f));
    if (DrawKey(draw, font, keyboardId + "_close", "X", closeMin, ImVec2(S(28.0f), S(28.0f)), scale, alpha, now, delta, state.enabled, KeyTone::Action, animations))
        *show = false;
    draw->AddLine(ImVec2(innerMin.x, pos.y + S(45.0f)), ImVec2(innerMax.x, pos.y + S(45.0f)), Alpha(kPanelEdge, alpha * 0.72f), S(1.0f));

    const float contentW = innerMax.x - innerMin.x;
    const float gap = S(8.0f);
    const float keyW = (contentW - gap * 9.0f) / 10.0f;
    const float keyH = S(44.0f);
    const float rowGap = S(7.0f);
    auto rowX = [&](int count) {
        return innerMin.x + (contentW - count * keyW - (count - 1) * gap) * 0.5f;
    };
    auto drawRow = [&](const char *chars, float y) {
        const int count = (int)std::strlen(chars);
        const float startX = rowX(count);
        for (int i = 0; i < count; ++i) {
            const char key[2] = {chars[i], '\0'};
            const ImVec2 keyMin(startX + i * (keyW + gap), y);
            if (DrawKey(draw, font, keyboardId + "_" + key, key, keyMin, ImVec2(keyW, keyH), scale, alpha, now, delta, state.enabled, KeyTone::Normal, animations))
                Append(buffer, cap, key);
        }
    };

    float y = pos.y + S(48.0f);
    drawRow("1234567890", y);
    y += keyH + rowGap;
    drawRow("QWERTYUIOP", y);
    y += keyH + rowGap;
    drawRow("ASDFGHJKL", y);
    y += keyH + rowGap;

    const char *lastRow = "-ZXCVBNM";
    const int lastCount = (int)std::strlen(lastRow) + 1;
    const float lastX = rowX(lastCount);
    for (int i = 0; i < lastCount - 1; ++i) {
        const char key[2] = {lastRow[i], '\0'};
        const ImVec2 keyMin(lastX + i * (keyW + gap), y);
        if (DrawKey(draw, font, keyboardId + "_" + key, key, keyMin, ImVec2(keyW, keyH), scale, alpha, now, delta, state.enabled, KeyTone::Normal, animations))
            Append(buffer, cap, key);
    }
    const ImVec2 delMin(lastX + (lastCount - 1) * (keyW + gap), y);
    if (DrawKey(draw, font, keyboardId + "_del", "DEL", delMin, ImVec2(keyW, keyH), scale, alpha, now, delta, state.enabled, KeyTone::Action, animations))
        Backspace(buffer, cap);

    y += keyH + rowGap;
    const float clearW = keyW * 2.0f + gap;
    const float spaceW = keyW * 4.0f + gap * 3.0f;
    const float doneW = keyW * 2.0f + gap;
    const float actionW = clearW + spaceW + doneW + gap * 2.0f;
    float actionX = innerMin.x + (contentW - actionW) * 0.5f;
    if (DrawKey(draw, font, keyboardId + "_clear", "CLEAR", ImVec2(actionX, y), ImVec2(clearW, keyH), scale, alpha, now, delta, state.enabled, KeyTone::Action, animations))
        buffer[0] = '\0';
    actionX += clearW + gap;
    if (DrawKey(draw, font, keyboardId + "_space", "SPACE", ImVec2(actionX, y), ImVec2(spaceW, keyH), scale, alpha, now, delta, state.enabled, KeyTone::Normal, animations))
        Append(buffer, cap, " ");
    actionX += spaceW + gap;
    if (DrawKey(draw, font, keyboardId + "_done", "DONE", ImVec2(actionX, y), ImVec2(doneW, keyH), scale, alpha, now, delta, state.enabled, KeyTone::Accent, animations))
        *show = false;

    ImGui::End();
    ImGui::PopStyleVar(2);
}

} // namespace Keypad
