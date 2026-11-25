///-- offset updator
///--@Npc00000000000000000000000000000
///--date 2026-11-10
#include "Includes/Logger.h"
#include "Includes/Macros.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "ImGui/Call_ImGui.h"
#include "IL2CppSDKGenerator/BasicStructs/Call_BasicStructs.h"
#include "IL2CppSDKGenerator/IL2Cpp/Call_IL2Cpp.h"
#include "Hacks/Hacks.h"
#include "IL2CppSDKGenerator/KittyMemory/MemoryPatch.h"

#include <chrono>
#include <cfloat>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <EGL/egl.h>
#include <jni.h>
#include <pthread.h>
#include <string>
#include <thread>
#include <unistd.h>

#include "ctorHook/ConstructorHook.hpp"
#include "ctorHook/ctorOffset.h"

bool SnowB = false;
float SnowBsize = 0.0f;
bool isSpeedHackEnabled = false;
float speedHackMultiplier = 1.0f;
bool isJumpAdjustmentEnabled = false;
float jumpHeightMultiplier = 1.0f;
char logintext[4096];
float accentColor[4] = {58.0f / 255.0f, 58.0f / 255.0f, 58.0f / 255.0f, 1.0f};

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>

#include "Includes/Includes.h"
#include "ImGui/imgui_core.h"
#include "ImGui/MenuCore.h"
#include "System/UI/MenuUi.h"
#include "System/UI/LoginUi.h"

JavaVM* jvm = nullptr;
JavaVM* VM = nullptr;

static int g_GlWidth, g_GlHeight;
static bool g_App = false;
static EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface) = nullptr;

static void UpdateWallhackPatch();

struct PatchSet {
    MemoryPatch wall;
} patches;


std::chrono::steady_clock::time_point appStartTime = std::chrono::steady_clock::now();

static bool isLogin = false;

static void UpdateTouchInput(ImGuiIO &io)
{
    if (!m_unity) {
        io.MouseDown[0] = false;
        return;
    }

    auto Input_get_touchCount = (int (*)())(Class_Input_get_touchCount);
    const int touchCount = Input_get_touchCount();
    if (touchCount <= 0) {
        io.MouseDown[0] = false;
        return;
    }

    auto Input_GetTouch = (Touch(*)(uintptr_t, int))(Class_Input_GetTouch);
    auto Input_get_mousePosition = (Vector3(*)(uintptr_t))(Class_Input_get_mousePosition);
    const Touch touch = Input_GetTouch(Config.ImGuiMenu.thiz, 0);
    const Vector3 position = Input_get_mousePosition(Config.ImGuiMenu.thiz);
    io.MousePos = ImVec2(position.x, static_cast<float>(get_height()) - position.y);

    switch (touch.m_Phase)
    {
        case TouchPhase::Began:
        case TouchPhase::Stationary:
        case TouchPhase::Moved:
            io.MouseDown[0] = true;
            break;
        case TouchPhase::Ended:
        case TouchPhase::Canceled:
            io.MouseDown[0] = false;
            break;
        default:
            io.MouseDown[0] = false;
            break;
    }
}

EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
    auto &view = Ui::menu;

    if (old_eglSwapBuffers == nullptr)
        return EGL_FALSE;

    if (dpy == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE)
        return old_eglSwapBuffers(dpy, surface);

    if (eglQuerySurface(dpy, surface, EGL_WIDTH, &g_GlWidth) != EGL_TRUE ||
        eglQuerySurface(dpy, surface, EGL_HEIGHT, &g_GlHeight) != EGL_TRUE ||
        g_GlWidth <= 0 || g_GlHeight <= 0)
        return old_eglSwapBuffers(dpy, surface);

    if (!g_App)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = NULL;
        io.LogFilename = NULL;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.MouseDoubleClickTime = 0.3f;
        io.MouseDragThreshold = 2.f;
        ImGui_ImplOpenGL3_Init("#version 300 es");

        Ui::InitFonts(io);
        Ui::InitDefaults();

        g_App = true;
    }

    ImGuiIO *io = &ImGui::GetIO();
    screenWidth = (float)g_GlWidth;
    screenHeight = (float)g_GlHeight;
    io->DisplaySize = ImVec2((float)g_GlWidth, (float)g_GlHeight);
    UpdateTouchInput(*io);
    fps.update();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImDrawList *draw = ImGui::GetBackgroundDrawList();

    UpdateWallhackPatch();
    DrawESP(ImGui::GetBackgroundDrawList(), screenWidth, screenHeight, get_dpi());
    Overlay::Render(draw, screenWidth, screenHeight);

    Ui::DrawHandle(view);

    if (view.visible && !view.collapsed)
    {
        if (!view.clearReady) {
            Config.ExtraMenu.ClearDisplay = true;
            view.clearReady = true;
        }

        if (!view.loginLoaded && VM != nullptr)
        {
            if (LoadTextFromFile() && logintext[0] != '\0')
            {
                strncpy(view.licenseKey, logintext, sizeof(view.licenseKey) - 1);
                view.licenseKey[sizeof(view.licenseKey) - 1] = '\0';
                view.loginLoaded = true;
            }
        }

        Theme::ClampAccent();
        Theme::ApplyThemeState();

        ImVec2 viewportCenter = ImGui::GetMainViewport()->GetCenter();
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        if (!isLogin)
        {
            const bool loginFinished = LoginUi::AdvanceExit();
            const LoginUi::LoginResult login = LoginUi::Draw(view.licenseKey, sizeof(view.licenseKey), view.loginError);
            if (!LoginUi::IsExiting() && (login.inputClicked || login.inputActive))
                showKeyboard = true;

            if (!LoginUi::IsExiting() && login.action == LoginUi::LoginAction::Paste) {
                const std::string clip = getClipboard();
                if (!clip.empty()) {
                    std::snprintf(view.licenseKey, sizeof(view.licenseKey), "%s", clip.c_str());
                    showKeyboard = false;
                }
            } else if (!LoginUi::IsExiting() && login.action == LoginUi::LoginAction::Submit) {
                view.loginError = Login(view.licenseKey);
                if (view.loginError == "OK") {
                    showKeyboard = false;
                    strncpy(logintext, view.licenseKey, sizeof(logintext) - 1);
                    logintext[sizeof(logintext) - 1] = '\0';
                    SaveLoginTextToFile(view.licenseKey);
                    view.loginLoaded = true;
                    view.loginError.clear();
                    LoginUi::BeginExit();
                }
            }

            if (loginFinished) {
                isLogin = true;
                view.collapsed = true;
                view.visible = false;
                view.closing = false;
                view.closeT = 0.0f;
                view.wheelOpen = false;
                view.wheelAmount = 0.0f;
                view.wasCollapsed = false;
                view.openFromLauncher = false;
                view.anchorMenu = false;
            }

            if (!LoginUi::IsExiting() && showKeyboard && !login.inputActive && !login.inputHovered && ImGui::IsMouseClicked(0)) {
                if (ImGui::GetMousePos().y < Keypad::TopFor(ImGui::GetIO().DisplaySize))
                    showKeyboard = false;
            }
            if (!LoginUi::IsExiting() && showKeyboard)
                Keypad::Render("##Keyboard", view.licenseKey, sizeof(view.licenseKey), &showKeyboard);
        }
        else
        {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, Ui::FadeIn(view));

            const ImVec2 mainWindowSize = Ui::MenuSize(displaySize);
            const float closeEase = view.closing
                ? view.closeT * view.closeT * (3.0f - 2.0f * view.closeT)
                : 0.0f;
            const ImVec2 visibleSize = mainWindowSize * ImLerp(1.0f, 0.90f, closeEase);
            if (!view.menuPositioned) {
                view.menuPos = viewportCenter - visibleSize * 0.5f;
                view.menuPositioned = true;
            }
            ImGui::SetNextWindowPos(view.menuPos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(visibleSize, ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

            ImGui::Begin("spoti.hook", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground);
            {
                NavState nav{
                    view.tabAlpha,
                    view.tabOffset,
                    view.page,
                    view.activeTab,
                    view.collapsed,
                    view.visible,
                    view.closing,
                    view.closeT,
                    view.lastActive,
                    view.handleAlpha,
                    view.press};
                Ui::ApplyPalette();
                Theme::ClampAccent();

                const ImVec2 winSize = ImGui::GetWindowSize();
                const ImVec2 winPos = ImGui::GetWindowPos();
                const float uiScale = ImClamp(winSize.y / Ui::kBaseH, 0.72f, 1.20f);
                Theme::ApplyThemeState(uiScale);
                Ui::ApplyDonorStyle(uiScale);

                const ImVec2 railMin = winPos + ImVec2(Ui::kOuterPad, Ui::kContentY) * uiScale;
                const ImVec2 headerMin = winPos + ImVec2(Ui::kOuterPad, Ui::kHeaderTop) * uiScale;
                const ImVec2 contentMin = winPos + ImVec2(Ui::kContentX, Ui::kContentY) * uiScale;
                const ImVec2 contentSize(
                    ImMax(1.0f, winSize.x - (Ui::kContentX + Ui::kContentRight) * uiScale),
                    ImMax(1.0f, winSize.y - (Ui::kContentY + Ui::kContentBottom) * uiScale));

                ImDrawList *menuDraw = ImGui::GetWindowDrawList();
                WindowShell(menuDraw, winPos, winSize, uiScale);
                RightSurface(menuDraw, contentMin, contentSize, uiScale);

                nav.page = ImClamp(nav.page, 1, SidebarItemCount());
                nav.activeTab = ImClamp(nav.activeTab, 1, SidebarItemCount());
                UpdateMenuDrag(view, winPos, winSize, contentMin, contentMin + contentSize);
                Ui::DrawDonorHeader(nav, headerMin, uiScale);
                if (!view.closing)
                    Ui::DrawDonorRail(nav, railMin, uiScale);

                const float tabSpeed = 8.0f * ImGui::GetIO().DeltaTime;
                nav.tabAlpha = ImClamp(nav.tabAlpha + (nav.page == nav.activeTab ? tabSpeed : -tabSpeed), 0.0f, 1.0f);
                if (nav.page != nav.activeTab && nav.tabAlpha <= 0.02f) {
                    nav.activeTab = nav.page;
                    nav.tabAlpha = 0.0f;
                }
                nav.tabAdd = ImLerp(
                    nav.tabAdd,
                    (1.0f - nav.tabAlpha) * 8.0f * uiScale,
                    ImMin(1.0f, 12.0f * ImGui::GetIO().DeltaTime));

                ImGui::SetCursorScreenPos(contentMin + ImVec2(0.0f, nav.tabAdd));
                char contentId[32] = {};
                std::snprintf(contentId, sizeof(contentId), "##page_%d", nav.activeTab);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGuiWindowFlags pageFlags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar;
                ImGui::BeginChild(contentId, contentSize, false, pageFlags);
                ImGui::PopStyleVar();
                XuiTheme::TouchScroll(contentId);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, nav.tabAlpha * ImGui::GetStyle().Alpha);
                ImGui::PushFont(Fonts::widget ? Fonts::widget : ImGui::GetFont());

                Ui::DrawDonorPage(nav.activeTab, uiScale);


                ImGui::PopFont();
                ImGui::PopStyleVar();
                ImGui::EndChild();

            }
            ImGui::PopStyleVar(2);
            ImGui::End();
            ImGui::PopStyleVar();
        }

        ImGui::PopStyleVar();
    }

    ImGui::EndFrame();
    ImGui::Render();

    ImDrawData *drawData = ImGui::GetDrawData();
    ImGui_ImplOpenGL3_RenderDrawData(drawData);

    return old_eglSwapBuffers(dpy, surface);
}

static void InstallEglHook()
{
    const auto swapBuffers = reinterpret_cast<uintptr_t>(
        DobbySymbolResolver(OBFUSCATE("libunity.so"), OBFUSCATE("eglSwapBuffers")));
    if (swapBuffers <= 0x1028) {
        LOGI("[EGL] libunity.so eglSwapBuffers symbol not found");
        return;
    }

    KittyMemory::ProtectAddr(
        reinterpret_cast<void *>(swapBuffers),
        sizeof(swapBuffers),
        PROT_READ | PROT_WRITE | PROT_EXEC);

    const int registerResult = xhook_register(
        OBFUSCATE(".*libunity\\.so$"),
        OBFUSCATE("eglSwapBuffers"),
        reinterpret_cast<void *>(hook_eglSwapBuffers),
        reinterpret_cast<void **>(&old_eglSwapBuffers));
    const int refresh = xhook_refresh(0);
    LOGI("[EGL] register=%d refresh=%d old=%p",
         registerResult, refresh, reinterpret_cast<void *>(old_eglSwapBuffers));
    if (refresh == 0)
        xhook_clear();
}

static void UpdateWallhackPatch()
{
    static int lastState = -1;
    const int state = Config.ExtraMenu.WallHack ? 1 : 0;
    if (state == lastState || !patches.wall.isValid())
        return;

    const bool applied = state != 0 ? patches.wall.Modify() : patches.wall.Restore();
    if (applied)
        lastState = state;
}

void Anogs_Thread()
{
    InitializeProtection();
}

void* SkinThread(void*)
{
    BP_TH(nullptr);
    return nullptr;
}

void StartCoreThreads();

void Unity_Thread()
{
    while (!m_unity)
    {
        m_unity = Tools::GetBaseAddress("libunity.so");
        sleep(1);
    }
    LOGI("libunity.so: %p", m_unity);

    UpdateAllOffset();
MemoryPatch::createWithHex("libanogs.so", 0x1EE554, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x1F815C, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x203460, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x204218, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x24D638, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x24EDD0, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x257B90, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x28F894, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x2A9338, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x2ABBC8, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x438154, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x44A714, "00 00 80 52 C0 03 5F D6").Modify();
MemoryPatch::createWithHex("libanogs.so", 0x48CF20, "00 00 80 52 C0 03 5F D6").Modify();   

    patches.wall = MemoryPatch::createWithHex("libunity.so", 0x548A67C, "1F 20 03 D5 E0 03 13 AA");

    DobbyHook((void *)getAbsoluteAddress("libunity.so", 0xD5319D4), (void *)&WeaponFireComponent_Instant_CreateBulletLine, (void **)&oWeaponFireComponent_Instant_CreateBulletLine);
    DobbyHook((void *)getAbsoluteAddress("libunity.so", 0xD53DA64), (void *)&WeaponFireComponent_Instant_CreateBulletProjectile, (void **)&oWeaponFireComponent_Instant_CreateBulletProjectile);
    InitializeAllHooks();

    InstallEglHook();
    
  
}

__attribute__((constructor))
void lib_main()
{
    StartCoreThreads();

    pthread_t skin;
    pthread_create(&skin, nullptr, SkinThread, nullptr);
}

void StartCoreThreads()
{
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    static bool started = false;

    pthread_mutex_lock(&lock);
    if (started) {
        pthread_mutex_unlock(&lock);
        return;
    }
    started = true;
    pthread_mutex_unlock(&lock);

    std::thread(Unity_Thread).detach();
    std::thread(Anogs_Thread).detach();
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    jvm = vm;
    VM = vm;

    StartCoreThreads();

    return JNI_VERSION_1_6;
}
