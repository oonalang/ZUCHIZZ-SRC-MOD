#pragma once

#include <cfloat>
#include <string>

namespace LoginUi {

enum class LoginAction {
    None,
    Paste,
    Submit,
};

struct LoginResult {
    LoginAction action = LoginAction::None;
    bool inputClicked = false;
    bool inputActive = false;
    bool inputHovered = false;
};

inline bool isExiting = false;
inline float exitProgress = 0.0f;
inline float exitOpacity = 1.0f;

inline float Ease(float value)
{
    value = ImClamp(value, 0.0f, 1.0f);
    return value * value * (3.0f - 2.0f * value);
}

inline void BeginExit()
{
    isExiting = true;
    exitProgress = 0.0f;
    exitOpacity = 1.0f;
}

inline bool AdvanceExit()
{
    if (!isExiting)
        return false;

    exitProgress = ImClamp(
        exitProgress + ImGui::GetIO().DeltaTime * 4.8f, 0.0f, 1.0f);
    exitOpacity = 1.0f - Ease(exitProgress);
    if (exitProgress < 1.0f)
        return false;

    isExiting = false;
    return true;
}

inline bool IsExiting()
{
    return isExiting;
}

inline ImU32 Fade(ImU32 color, float multiplier = 1.0f)
{
    const float sourceAlpha = static_cast<float>(
        (color >> IM_COL32_A_SHIFT) & 0xff) / 255.0f;
    const float alpha = ImClamp(
        sourceAlpha * exitOpacity * multiplier * ImGui::GetStyle().Alpha,
        0.0f, 1.0f);
    return (color & ~IM_COL32_A_MASK) |
           (static_cast<ImU32>(alpha * 255.0f + 0.5f) << IM_COL32_A_SHIFT);
}

inline float DrawCenteredText(
    ImDrawList *draw,
    ImFont *font,
    float size,
    ImVec2 center,
    ImU32 color,
    const char *text)
{
    const ImVec2 textSize = XuiTheme::Measure(font, size, text);
    XuiTheme::Text(draw, font, size,
                   ImVec2(center.x - textSize.x * 0.5f,
                          center.y - textSize.y * 0.5f),
                   color, text);
    return textSize.x;
}

inline void DrawAction(ImDrawList *draw,
                       ImFont *textFont,
                       ImFont *iconFont,
                       float textSize,
                       float iconSize,
                       ImVec2 center,
                       ImU32 color,
                       const char *icon,
                       const char *label,
                       float gap)
{
    const ImVec2 iconPx = XuiTheme::Measure(iconFont, iconSize, icon);
    const ImVec2 labelPx = XuiTheme::Measure(textFont, textSize, label);
    const float contentWidth = iconPx.x + gap + labelPx.x;
    const float contentX = center.x - contentWidth * 0.5f;
    Ui::IconCenter(draw, iconFont, iconSize,
                   ImVec2(contentX + iconPx.x * 0.5f, center.y),
                   color, icon);
    XuiTheme::Text(draw, textFont, textSize,
                   ImVec2(contentX + iconPx.x + gap,
                          center.y - labelPx.y * 0.5f),
                   color, label);
}

inline LoginResult Draw(char *key, size_t keySize, const std::string &error)
{
    IM_UNUSED(keySize);
    LoginResult result{};
    ImGuiIO &io = ImGui::GetIO();
    const ImVec2 desired(550.0f, 560.0f);
    const ImVec2 limit(ImMax(320.0f, io.DisplaySize.x - 24.0f),
                       ImMax(320.0f, io.DisplaySize.y - 24.0f));
    const float scale = ImMin(1.0f, ImMin(limit.x / desired.x, limit.y / desired.y));
    const ImVec2 size = desired * scale;
    const ImVec2 pos = io.DisplaySize * 0.5f - size * 0.5f;
    const ImVec2 center = pos + size * 0.5f;

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);
    if (ImGui::Begin("##zuchizzl_login", nullptr,
                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings |
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar |
                          ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                          ImGuiWindowFlags_NoMove)) {
        ImDrawList *draw = ImGui::GetWindowDrawList();
        WindowShell(draw, pos, size, scale);
        const ImVec2 innerMin = pos + ImVec2(8.0f * scale, 8.0f * scale);
        const ImVec2 innerMax = pos + size - ImVec2(8.0f * scale, 8.0f * scale);
        draw->AddRectFilled(innerMin, innerMax,
                            Fade(IM_COL32(26, 26, 26, 255), 0.68f),
                            7.0f * scale);
        draw->AddRect(innerMin, innerMax, Fade(XuiTheme::kWhite, 0.12f),
                      7.0f * scale, 0, 1.0f * scale);

        ImFont *titleFont = XuiFonts::hero ? XuiFonts::hero : ImGui::GetFont();
        ImFont *bodyFont = XuiFonts::small ? XuiFonts::small : ImGui::GetFont();
        ImFont *iconFont = XuiFonts::icons ? XuiFonts::icons : ImGui::GetFont();
        const float logoSide = 180.0f * scale;
        const ImVec2 logoMin(center.x - logoSide * 0.5f, pos.y + 32.0f * scale);
        const ImVec2 logoMax = logoMin + ImVec2(logoSide, logoSide);
        draw->AddImage(GetDonorLogoTexture(), logoMin, logoMax);

        const float formWidth = 305.0f * scale;
        const float inputHeight = 45.0f * scale;
        const ImVec2 formMin(center.x - formWidth * 0.5f, pos.y + 310.0f * scale);
        draw->AddRectFilled(formMin, formMin + ImVec2(formWidth, inputHeight),
                            IM_COL32(245, 245, 245, 235), 2.0f * scale);
        draw->AddRect(formMin, formMin + ImVec2(formWidth, inputHeight),
                      Fade(XuiTheme::kWhite), 2.0f * scale, 0, 1.0f * scale);
        ImGui::SetCursorScreenPos(formMin);
        ImGui::InvisibleButton("##license_input", ImVec2(formWidth, inputHeight));
        result.inputClicked = ImGui::IsItemClicked();
        result.inputActive = ImGui::IsItemActive();
        result.inputHovered = ImGui::IsItemHovered();
        const bool hasKey = key && key[0];
        const char *shown = hasKey ? key : "ENTER LICENSE KEY";
        const ImU32 textColor = hasKey ? IM_COL32(20, 20, 20, 255)
                                       : IM_COL32(100, 100, 100, 255);
        const float shownSize = Ui::FitText(bodyFont, 12.0f * scale, shown,
                                            formWidth - 24.0f * scale);
        XuiTheme::Text(draw, bodyFont, shownSize,
                       ImVec2(formMin.x + 12.0f * scale,
                              formMin.y + (inputHeight - shownSize) * 0.5f),
                       textColor, shown, formMin.x + formWidth - 12.0f * scale);

        const ImVec2 pasteMin(center.x - formWidth * 0.5f, pos.y + 370.0f * scale);
        const ImVec2 pasteSize(formWidth, 45.0f * scale);
        draw->AddRectFilled(pasteMin, pasteMin + pasteSize,
                            IM_COL32(245, 245, 245, 235), 2.0f * scale);
        draw->AddRect(pasteMin, pasteMin + pasteSize,
                      Fade(XuiTheme::kWhite), 2.0f * scale, 0, 1.0f * scale);
        ImGui::SetCursorScreenPos(pasteMin);
        ImGui::InvisibleButton("##license_paste", pasteSize);
        if (ImGui::IsItemClicked())
            result.action = LoginAction::Paste;
        DrawAction(draw, bodyFont, iconFont, 13.0f * scale, 17.0f * scale,
                   pasteMin + pasteSize * 0.5f,
                   IM_COL32(20, 20, 20, 255), ICON_FA_PASTE, "PASTE",
                   7.0f * scale);

        const ImVec2 loginMin(center.x - formWidth * 0.5f, pos.y + 435.0f * scale);
        const ImVec2 loginSize(formWidth, 55.0f * scale);
        ImGui::SetCursorScreenPos(loginMin);
        ImGui::InvisibleButton("##license_submit", loginSize);
        const bool loginHovered = ImGui::IsItemHovered();
        const bool loginHeld = ImGui::IsItemActive();
        if (ImGui::IsItemClicked())
            result.action = LoginAction::Submit;
        const float buttonScale = loginHeld ? 0.975f : 1.0f;
        const ImVec2 buttonCenter = loginMin + loginSize * 0.5f;
        const ImVec2 buttonHalf = loginSize * (0.5f * buttonScale);
        const ImU32 sidebarFill = Fade(IM_COL32(10, 10, 10, 255), 0.58f);
        const ImU32 sidebarHover = Fade(IM_COL32(30, 30, 30, 255), 0.58f);
        draw->AddRectFilled(buttonCenter - buttonHalf, buttonCenter + buttonHalf,
                            loginHovered ? sidebarHover : sidebarFill,
                            buttonHalf.y);
        draw->AddRect(buttonCenter - buttonHalf, buttonCenter + buttonHalf,
                      Fade(XuiTheme::kWhite, 0.16f), buttonHalf.y, 0,
                      1.0f * scale);
        DrawAction(draw, titleFont, iconFont, 14.0f * scale, 17.0f * scale,
                   buttonCenter, Fade(XuiTheme::kWhite), ICON_FA_SIGN_IN_ALT,
                   "LOGIN", 7.0f * scale);

        if (!error.empty() && error != "OK") {
            const float errorSize = Ui::FitText(bodyFont, 10.0f * scale,
                                                error.c_str(), formWidth);
            DrawCenteredText(draw, bodyFont, errorSize,
                             ImVec2(center.x, pos.y + 503.0f * scale),
                             Fade(IM_COL32(238, 90, 90, 255)), error.c_str());
        }
    }
    ImGui::End();
    return result;
}

} // namespace LoginUi
