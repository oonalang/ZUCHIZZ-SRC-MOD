#pragma once

#include "Call_ImGui.h"
#include "Assets/Fonts/fontello_data.h"
#include "Assets/Fonts/fonts_data.h"
#include "imgui_settings.h"
#include "Assets/Fonts/logo_data.h"
#include "../Xui/XuiBoldFont.h"
#include "../Xui/XuiHeroFont.h"
#include "../Xui/XuiIconFont.h"
#include "../Xui/XuiMediumFont.h"
#include "../Xui/XuiRegularFont.h"
#include "../Xui/XuiSemiBoldFont.h"
#include "../System/UI/DonorAssets/DonorFont.h"
#include "../System/UI/DonorAssets/DonorIconFont.h"
#include "../System/UI/DonorAssets/LogoNew.h"
#include "../System/UI/DonorAssets/CodmLogo.h"
#include "stb_image.h"

#include <cfloat>
#include <cstring>
#include <cmath>
#include <string>

inline ImFont *F50 = nullptr;
inline ImFont *F107 = nullptr;
inline ImFont *SOCIAL = nullptr;
inline ImFont *FSidebarIcon = nullptr;
inline ImFont *FSidebarText = nullptr;
inline bool showKeyboard = false;
inline GLuint DonorLogoTexture = 0;
inline GLuint DonorCodmTexture = 0;

namespace Fonts {
inline ImFont *default_font = nullptr;
inline ImFont *inter_semibold = nullptr;
inline ImFont *widget = nullptr;
inline ImFont *button = nullptr;
inline ImFont *auto_techno = nullptr;
inline ImFont *menu_title = nullptr;
inline ImFont *child_title = nullptr;
inline ImFont *donor = nullptr;
inline ImFont *donor_icons = nullptr;
}

namespace XuiFonts {
inline ImFont *hero = nullptr;
inline ImFont *title = nullptr;
inline ImFont *menuBold = nullptr;
inline ImFont *menu = nullptr;
inline ImFont *small = nullptr;
inline ImFont *icons = nullptr;
}

inline ImTextureID CreateDonorTexture(GLuint &texture, const unsigned char *data, int size)
{
    if (texture != 0)
        return (ImTextureID)(intptr_t)texture;

    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char *pixels = stbi_load_from_memory(data, size, &width, &height, &channels, 4);
    if (!pixels)
        return nullptr;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    stbi_image_free(pixels);
    return (ImTextureID)(intptr_t)texture;
}

inline ImTextureID GetDonorLogoTexture()
{
    return CreateDonorTexture(DonorLogoTexture, LogoNew_data, (int)sizeof(LogoNew_data));
}

inline ImTextureID GetDonorCodmTexture()
{
    return CreateDonorTexture(DonorCodmTexture, CodmLogo_data, (int)sizeof(CodmLogo_data));
}

namespace Ui {

enum class TouchOwner {
    None,
    Content,
    Menu,
};

inline TouchOwner touchOwner = TouchOwner::None;

constexpr float kIslandWidthDefault = 129.0f;
constexpr float kIslandHeightDefault = 12.0f;
constexpr float kIslandBottomDefault = 24.0f;

inline float islandWidth = kIslandWidthDefault;
inline float islandHeight = kIslandHeightDefault;
inline float islandBottom = kIslandBottomDefault;

struct MenuState {
    bool clearReady = false;
    bool collapsed = false;
    double lastActive = 0.0;
    float handleAlpha = 1.0f;
    float press = 0.0f;
    float enter = 1.0f;
    float restore = 1.0f;
    bool wheelOpen = false;
    float wheelAmount = 0.0f;
    bool closing = false;
    float closeT = 0.0f;
    ImVec2 launcherPos = ImVec2(0.0f, 0.0f);
    ImVec2 launcherGrab = ImVec2(0.0f, 0.0f);
    bool launcherHeld = false;
    bool launcherDragged = false;
    bool openFromLauncher = false;
    bool anchorMenu = false;
    bool restoring = false;
    bool wasCollapsed = false;
    float open = 1.0f;
    float tabAlpha = 1.0f;
    float tabOffset = 0.0f;
    int page = 2;
    int activeTab = 2;
    bool visible = true;
    bool loginLoaded = false;
    std::string loginError;
    char licenseKey[256] = {};
    ImVec2 menuPos = ImVec2(0.0f, 0.0f);
    ImVec2 menuGrab = ImVec2(0.0f, 0.0f);
    bool menuPositioned = false;
    bool menuHeld = false;
    bool menuMoved = false;
    float islandT = 0.0f;
    float islandOpacity = 1.0f;
    double islandLastInteract = 0.0;
};

inline MenuState menu;

inline float *MakeColor(int red, int green, int blue, int alpha)
{
    return new float[4]{
        red / 255.0f,
        green / 255.0f,
        blue / 255.0f,
        alpha / 255.0f,
    };
}

inline void InitFonts(ImGuiIO &io)
{
    ImFontConfig baseCfg;
    baseCfg.MergeMode = false;
    baseCfg.PixelSnapH = true;
    baseCfg.OversampleH = 1;
    baseCfg.OversampleV = 1;
    baseCfg.FontDataOwnedByAtlas = false;

    Fonts::inter_semibold = io.Fonts->AddFontFromMemoryTTF(
        inter.data(),
        (int)inter.size(),
        13.0f,
        &baseCfg,
        io.Fonts->GetGlyphRangesCyrillic());
    Fonts::default_font = Fonts::inter_semibold;
    io.FontDefault = Fonts::default_font;

    ImFontConfig widgetCfg = baseCfg;
    Fonts::widget = io.Fonts->AddFontFromMemoryTTF(
        inter.data(),
        (int)inter.size(),
        16.0f,
        &widgetCfg,
        io.Fonts->GetGlyphRangesCyrillic());
    if (!Fonts::widget)
        Fonts::widget = Fonts::default_font;
    Fonts::button = Fonts::widget;

    ImFontConfig logoCfg = baseCfg;
    Fonts::auto_techno = io.Fonts->AddFontFromMemoryTTF(
        (void *)logo_data::ttf,
        (int)logo_data::ttf_size,
        28.0f,
        &logoCfg,
        io.Fonts->GetGlyphRangesDefault());
    static const ImWchar iconRanges[] = {0xE000, 0xF8FF, 0};
    ImFontConfig socialCfg = baseCfg;
    SOCIAL = io.Fonts->AddFontFromMemoryTTF(
        (void *)fontello_data::ttf,
        (int)fontello_data::ttf_size,
        22.0f,
        &socialCfg,
        iconRanges);

    ImFontConfig iconCfg = baseCfg;
    F107 = io.Fonts->AddFontFromMemoryCompressedTTF(
        (void *)font_awesome_data1,
        (int)font_awesome_size1,
        22.0f,
        &iconCfg,
        iconRanges);
    ImFontConfig sidebarIconCfg = baseCfg;
    FSidebarIcon = io.Fonts->AddFontFromMemoryCompressedTTF(
        (void *)font_awesome_data1,
        (int)font_awesome_size1,
        26.0f,
        &sidebarIconCfg,
        iconRanges);

    ImFontConfig donorCfg = baseCfg;
    donorCfg.OversampleH = 2.5f;
    donorCfg.OversampleV = 2.5f;
    Fonts::donor = io.Fonts->AddFontFromMemoryTTF(
        (void *)PIRO_data,
        (int)PIRO_size,
        20.0f,
        &donorCfg,
        io.Fonts->GetGlyphRangesChineseFull());

    ImFontConfig donorIconCfg = donorCfg;
    Fonts::donor_icons = io.Fonts->AddFontFromMemoryCompressedTTF(
        (void *)font_awesome_data,
        (int)font_awesome_size,
        23.0f,
        &donorIconCfg,
        iconRanges);
    F50 = Fonts::auto_techno;
    if (!F107)
        F107 = Fonts::inter_semibold;
    if (!FSidebarIcon)
        FSidebarIcon = F107;
    if (!Fonts::donor)
        Fonts::donor = Fonts::widget ? Fonts::widget : Fonts::default_font;
    if (!Fonts::donor_icons)
        Fonts::donor_icons = F107 ? F107 : Fonts::donor;

    ImFontConfig xuiBase = baseCfg;
    xuiBase.OversampleH = 4;
    xuiBase.OversampleV = 4;
    const ImWchar *xuiCyrillic = io.Fonts->GetGlyphRangesCyrillic();
    static const ImWchar xuiIconRange[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    auto AddXui = [&](const void *data, int size, float pixelSize, bool mergeIcons) {
        ImFontConfig cfg = xuiBase;
        ImFont *font = io.Fonts->AddFontFromMemoryCompressedTTF(
            data, size, pixelSize, &cfg, xuiCyrillic);
        if (font && mergeIcons) {
            ImFontConfig merge = xuiBase;
            merge.MergeMode = true;
            merge.GlyphMinAdvanceX = pixelSize * 0.9f;
            io.Fonts->AddFontFromMemoryCompressedTTF(
                XuiIconFont_compressed_data, XuiIconFont_compressed_size,
                pixelSize * 0.9f, &merge, xuiIconRange);
        }
        return font;
    };

    XuiFonts::hero = AddXui(XuiHeroFont_compressed_data, XuiHeroFont_compressed_size, 34.0f, false);
    XuiFonts::title = AddXui(XuiHeroFont_compressed_data, XuiHeroFont_compressed_size, 21.0f, true);
    XuiFonts::menuBold = AddXui(XuiSemiBoldFont_compressed_data, XuiSemiBoldFont_compressed_size, 15.0f, true);
    XuiFonts::menu = AddXui(XuiRegularFont_compressed_data, XuiRegularFont_compressed_size, 14.0f, true);
    XuiFonts::small = AddXui(XuiMediumFont_compressed_data, XuiMediumFont_compressed_size, 12.0f, true);

    ImFontConfig xuiIconCfg = xuiBase;
    xuiIconCfg.GlyphMinAdvanceX = 18.0f;
    XuiFonts::icons = io.Fonts->AddFontFromMemoryCompressedTTF(
        XuiIconFont_compressed_data, XuiIconFont_compressed_size,
        18.0f, &xuiIconCfg, xuiIconRange);

    if (!XuiFonts::menu)
        XuiFonts::menu = Fonts::widget ? Fonts::widget : Fonts::default_font;
    if (!XuiFonts::hero)
        XuiFonts::hero = XuiFonts::menu;
    if (!XuiFonts::title)
        XuiFonts::title = XuiFonts::menu;
    if (!XuiFonts::menuBold)
        XuiFonts::menuBold = XuiFonts::menu;
    if (!XuiFonts::small)
        XuiFonts::small = XuiFonts::menu;
    if (!XuiFonts::icons)
        XuiFonts::icons = F107 ? F107 : XuiFonts::menu;

    Fonts::widget = XuiFonts::menu;
    Fonts::button = XuiFonts::menuBold;
    Fonts::menu_title = XuiFonts::title;
    Fonts::child_title = XuiFonts::small;
    FSidebarText = XuiFonts::menuBold;

    io.Fonts->Build();
    ImGui_ImplOpenGL3_CreateFontsTexture();
}

inline void InitDefaults()
{
    std::memset(&Config, 0, sizeof(sConfig));
    Config.sColorsESPPLAYER.LinePLAYER = MakeColor(255, 0, 0, 255);
    Config.sColorsESPPLAYER.BoxPLAYER = MakeColor(255, 0, 0, 255);
    Config.sColorsESPPLAYER.NamePLAYER = MakeColor(255, 0, 0, 255);
    Config.sColorsESPPLAYER.DistancePLAYER = MakeColor(255, 0, 0, 255);
    Config.sColorsESPPLAYER.HealthPLAYER = MakeColor(255, 0, 0, 255);
    Config.sColorsESPPLAYER.SkeletonPLAYER = MakeColor(255, 0, 0, 255);
    Config.sColorsESPBOT.LineBOT = MakeColor(0, 255, 0, 180);
    Config.sColorsESPBOT.BoxBOT = MakeColor(0, 255, 0, 180);
    Config.sColorsESPBOT.NameBOT = MakeColor(0, 255, 0, 180);
    Config.sColorsESPBOT.HealthBOT = MakeColor(0, 255, 0, 180);
    Config.sColorsESPBOT.DistanceBOT = MakeColor(0, 255, 0, 180);
    Config.sColorsESPBOT.SkeletonBOT = MakeColor(0, 255, 0, 180);
    Config.sColorsESPOTHERS.PovOTHERS = MakeColor(225, 0, 0, 180);

    Config.Aim.AimAssistSize = 0.0f;
    Config.Aim.Cross = 50.0f;
    Config.Aim.Target = EAimTarget::Heads;
    Config.Aim.Trigger = EAimTrigger::None;
    Config.Aim.By = EAim::Distance;
    Config.Bline = 2.0f;
    Config.Pline = 2.0f;
    Config.ExtraMenu.ClearDisplay = true;
}

inline void ResetSettings()
{
    InitDefaults();
    islandWidth = kIslandWidthDefault;
    islandHeight = kIslandHeightDefault;
    islandBottom = kIslandBottomDefault;
    menu.islandT = 0.0f;
    menu.islandOpacity = 1.0f;
    menu.islandLastInteract = ImGui::GetTime();
}

inline void DrawHandle(MenuState &menu)
{
    const ImVec2 display = ImGui::GetIO().DisplaySize;
    const float scale = ImClamp(c::scale, 0.72f, 1.20f);
    const float dt = ImGui::GetIO().DeltaTime;
    const double now = ImGui::GetTime();
    const bool collapsed = menu.collapsed || !menu.visible;
    const bool showIsland = collapsed;
    const float target = showIsland ? 1.0f : 0.0f;
    const float morphBlend = 1.0f - std::exp(-dt * 12.0f);
    menu.islandT = ImLerp(menu.islandT, target, morphBlend);
    menu.handleAlpha = menu.islandT;

    if (menu.islandLastInteract <= 0.0)
        menu.islandLastInteract = now;

    if (!menu.closing && !collapsed)
        menu.wasCollapsed = false;

    menu.wheelOpen = false;
    menu.wheelAmount = 0.0f;
    menu.openFromLauncher = false;
    menu.anchorMenu = false;
    if (!showIsland)
        return;

    const float fullWidth = ImClamp(islandWidth, 72.0f, 280.0f);
    const float fullHeight = ImClamp(islandHeight, 4.0f, 24.0f);
    const float fullBottom = ImClamp(islandBottom, 8.0f, 80.0f);
    const float width = ImLerp(fullWidth * (2.0f / 3.0f), fullWidth, menu.islandT) * scale;
    const float height = ImLerp(fullHeight * (2.0f / 3.0f), fullHeight, menu.islandT) * scale;
    const float centerX = display.x * 0.5f;
    const float centerY = display.y - ImLerp(fullBottom * (2.0f / 3.0f), fullBottom, menu.islandT) * scale;
    const ImVec2 pillMin(centerX - width * 0.5f, centerY - height * 0.5f);
    const ImVec2 pillMax(centerX + width * 0.5f, centerY + height * 0.5f);
    ImDrawList *draw = ImGui::GetForegroundDrawList();
    const bool hit = ImGui::IsMouseHoveringRect(
        pillMin - ImVec2(16.0f * scale, 14.0f * scale),
        pillMax + ImVec2(16.0f * scale, 14.0f * scale), false);
    const ImGuiIO &io = ImGui::GetIO();
    if (hit && (io.MouseDown[0] || ImGui::IsMouseClicked(0))) {
        menu.islandLastInteract = now;
        if (touchOwner == TouchOwner::None)
            touchOwner = TouchOwner::Menu;
    }
    const float idleTarget = now - menu.islandLastInteract >= 5.0 ? 0.25f : 1.0f;
    const float opacityBlend = 1.0f - std::exp(-dt * 8.0f);
    menu.islandOpacity = ImLerp(menu.islandOpacity, idleTarget, opacityBlend);
    const float alpha = ImClamp(menu.islandOpacity, 0.25f, 1.0f);
    const float radius = height * 0.5f;
    draw->AddRectFilled(pillMin, pillMax,
                        IM_COL32(255, 255, 255, (int)(245.0f * alpha)), radius);

    if (hit && ImGui::IsMouseClicked(0) && collapsed) {
        menu.collapsed = false;
        menu.visible = true;
        menu.closing = false;
        menu.closeT = 0.0f;
        menu.open = 0.0f;
        menu.wasCollapsed = false;
        menu.islandT = 0.0f;
        menu.handleAlpha = 0.0f;
        menu.islandLastInteract = now;
    }
}

inline bool MenuDragMoved(const MenuState &state)
{
    return state.menuMoved;
}

inline void UpdateMenuDrag(MenuState &state, const ImVec2 &windowPos,
                           const ImVec2 &windowSize, const ImVec2 &contentMin,
                           const ImVec2 &contentMax)
{
    ImGuiIO &io = ImGui::GetIO();
    if (!io.MouseDown[0]) {
        if (state.menuHeld) {
            state.menuHeld = false;
        } else {
            state.menuMoved = false;
        }
        if (touchOwner == TouchOwner::Menu)
            touchOwner = TouchOwner::None;
        return;
    }

    if (touchOwner == TouchOwner::Content)
        return;

    const bool insideContent = ImGui::IsMouseHoveringRect(contentMin, contentMax, false);
    if (!state.menuHeld && touchOwner == TouchOwner::None && !insideContent &&
        ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
        touchOwner = TouchOwner::Menu;
        state.menuHeld = true;
        state.menuMoved = false;
        state.menuGrab = io.MousePos;
        state.menuPos = windowPos;
    }

    if (!state.menuHeld || !ImGui::IsMousePosValid())
        return;

    const ImVec2 delta = io.MousePos - state.menuGrab;
    state.menuGrab = io.MousePos;
    if (std::fabs(delta.x) >= 2.0f || std::fabs(delta.y) >= 2.0f)
        state.menuMoved = true;
    if (!state.menuMoved)
        return;

    state.menuPos += delta;
    const ImVec2 display = io.DisplaySize;
    state.menuPos.x = ImClamp(state.menuPos.x, 0.0f, ImMax(0.0f, display.x - windowSize.x));
    state.menuPos.y = ImClamp(state.menuPos.y, 0.0f, ImMax(0.0f, display.y - windowSize.y));
}

inline float FadeIn(MenuState &menu)
{
    const float dt = ImGui::GetIO().DeltaTime;
    if (menu.closing) {
        menu.closeT = ImClamp(menu.closeT + dt * 11.5f, 0.0f, 1.0f);
        const float eased = menu.closeT * menu.closeT * (3.0f - 2.0f * menu.closeT);
        if (menu.closeT >= 1.0f) {
            menu.closing = false;
            menu.closeT = 0.0f;
            menu.collapsed = true;
            menu.visible = false;
            menu.open = 0.0f;
            menu.wasCollapsed = true;
            menu.wheelOpen = true;
            menu.wheelAmount = 1.0f;
        }
        return 1.0f - eased;
    }
    menu.open = ImClamp(menu.open + dt * 5.0f, 0.0f, 1.0f);
    const float eased = menu.open * menu.open * (3.0f - 2.0f * menu.open);
    return 0.2f + 0.8f * eased;
}

} // namespace Ui
