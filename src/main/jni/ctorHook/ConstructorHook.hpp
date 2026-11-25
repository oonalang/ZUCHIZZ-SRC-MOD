#pragma once
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include <array>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <algorithm>
#include "Hacks/StructGame/StructSDK.h"

#ifdef __cplusplus
extern "C" {
#endif

std::string ToLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

void hookConstructor(void *targetFunc, std::vector<void *> &container);

#ifdef __cplusplus
}
#endif

extern JavaVM* VM;
inline void* (*UnityComponent_GetGameObject)(void* component) = nullptr;
inline void* (*UnityTransform_GetGameObject)(void* transform) = nullptr;
inline void (*UnityGameObject_SetActive)(void* gameObject, bool active) = nullptr;

extern uintptr_t Item2InventoryAddress;
extern uintptr_t WeaponConfAddress;
extern uintptr_t WeaponConfExtraAddress;
extern uintptr_t WeaponFireEffectAddress;
extern uintptr_t CharacterModelAddress;
extern uintptr_t WeaponAssetGroupAddress;
extern uintptr_t WeaponPartAssetAddress;
extern uintptr_t WeaponSkinAddress;
extern uintptr_t AttachmentConfAddress;
extern uintptr_t AttachmentGroupNewCtorAddress;
extern uintptr_t AttachmentGroupNewItemCtorAddress;
extern uintptr_t AttachmentGroupNewGetColorIdAddress;
extern uintptr_t BRWeaponAttachItemDataCtorAddress;
extern uintptr_t AttachItemCtorAddress;
extern uintptr_t AttachItemSetDataAddress;
extern uintptr_t AttachItemSetMythicSkinIdAddress;
extern uintptr_t AttachItemCopyEquippedAddress;
extern uintptr_t AttachItemGetModelAttachIdAddress;
extern uintptr_t AttachSkinDataCtorAddress;
extern uintptr_t LoadoutAttachItemSetDataAddress;
extern uintptr_t WeaponInitPartAddress;
extern uintptr_t BRBagWeaponAvatarEquipAttachmentAddress;
extern uintptr_t BRBagWeaponAvatarShouldIgnoreAddress;
extern uintptr_t MythicArmorConfigAddress;
extern uintptr_t MythicSightConfigAddress;
extern uintptr_t KillEffectItemConfConfigAddress;
extern uintptr_t ItemResourceAddress;
extern uintptr_t RoleConfAddress;
extern uintptr_t RoleSkinAddress;
extern uintptr_t RolePackConfAddress;
extern uintptr_t WeaponConfName;
extern uintptr_t GetNameRoleSkin;
extern uintptr_t GetRoleConfName;

extern std::vector<void *> itemInventoryInstance;
extern std::vector<void *> weaponExtraInstance;
extern std::vector<void *> weaponFireEffectInstance;
extern std::vector<void *> weaponConfInstance;
extern std::vector<void *> weaponAssetGroupInstance;
extern std::vector<void *> weaponPartAssetInstance;
extern std::vector<void *> attachmentConfInstance;
extern std::vector<void *> mythicArmorInstance;
extern std::vector<void *> mythicSightInstance;
extern std::vector<void *> killEffectItemInstance;
extern std::vector<void *> weaponSkinConfigInstance;
extern std::vector<void *> itemResourceConfigInstance;
extern std::vector<void *> CharacterModelConfigInstance;
extern std::vector<void *> RoleConfConfigInstance;
extern std::vector<void *> RoleSkinConfigInstance;
extern std::vector<void *> RolePackConfConfigInstance;

extern std::unordered_map<std::string, bool> sBool;
extern std::mutex g_skinDataMutex;
extern std::mutex g_effectMutex;
extern std::unordered_map<int, int> activeKillEffects;

extern std::unordered_map<int, int> activeBulletTrackEffects;
extern std::unordered_map<int, int> activeWeaponFireEffects;
struct TargetChar {
    std::string name;
    int traitor1p;
    int traitor3p;
    int itemID;
    int roleID;
    int rolepackID;
};
extern std::vector<TargetChar> g_targetCharacters;
extern int g_selectedTargetCharIndex;

int (*orig_GetCurrentWeaponKillEffect)(Weapon);
int _GetCurrentWeaponKillEffect(Weapon weapon) {
    Pawn* localPawn = GamePlay::get_LocalPawn();
    if (!Tools::IsPtrValid(localPawn)) return 0;

    Weapon* currentWeapon = localPawn->get_CurrentWeapon();
    if (!Tools::IsPtrValid(currentWeapon)) return 0;

    int currentID = currentWeapon->get_WeaponID();

    std::lock_guard<std::mutex> lock(g_effectMutex);
    auto it = activeKillEffects.find(currentID);
    if (it != activeKillEffects.end()) {
        return it->second;
    }

    return 0;
}

int (*orig_GetCurrentBulletTrackEffect)(Weapon);
int _GetCurrentBulletTrackEffect(Weapon weapon) {
    Pawn* localPawn = GamePlay::get_LocalPawn();
    if (!Tools::IsPtrValid(localPawn)) return 0;

    Weapon* currentWeapon = localPawn->get_CurrentWeapon();
    if (!Tools::IsPtrValid(currentWeapon)) return 0;

    int currentID = currentWeapon->get_WeaponID();

    std::lock_guard<std::mutex> lock(g_effectMutex);
    auto it = activeBulletTrackEffects.find(currentID);
    if (it != activeBulletTrackEffects.end()) {
        return it->second;
    }

    return 0;
}

int (*orig_GetCurrentWeaponFireEffect)(Weapon);
int _GetCurrentWeaponFireEffect(Weapon weapon) {
    Pawn* localPawn = GamePlay::get_LocalPawn();
    if (!Tools::IsPtrValid(localPawn)) return 0;

    Weapon* currentWeapon = localPawn->get_CurrentWeapon();
    if (!Tools::IsPtrValid(currentWeapon)) return 0;

    int currentID = currentWeapon->get_WeaponID();

    std::lock_guard<std::mutex> lock(g_effectMutex);
    auto it = activeWeaponFireEffects.find(currentID);
    if (it != activeWeaponFireEffects.end()) {
        return it->second;
    }

    return 0;
}

struct itemInfo {
    std::string itemName;
    std::array<int, 4> Item2Inventory;
    std::array<int, 5> WeaponConf;
    std::array<int, 5> WeaponExtra;
    std::array<int, 3> WeaponAsset;
    std::array<int, 4> WeaponAttach;
    std::array<int, 3> ItemResInt;
    std::array<uint16_t, 4> ItemRes;
};
extern std::vector<itemInfo> itemData;

struct charInfo {
    std::string charName;
    int roleId;
    std::array<int, 12> charModel;
    std::array<int, 3> charRes;
    std::array<uint16_t, 4> charRes2;
    std::array<int, 8> charRole;
    std::array<void *, 1> charRole2;
    std::array<int, 6> charPack;
};
extern std::vector<charInfo> charData;

extern char searchQuery[256];
extern std::string ToLower(std::string str);
extern uintptr_t getRealOffset(uintptr_t offset);

struct RoleSkinFields {
    uint8_t ClothType;
    uint8_t ColorID;
    uint8_t GoGetPos;
    uint8_t HatSubMode;
    bool HideInBag;
    uint8_t IsCanBeUsedForAllRole;
    bool IsCloseScreenSaver;
    uint8_t IsDisassemble;
    uint8_t SecondTab;
    bool ShowInBag;
    bool ShowRare;
    int BRBagAssetID;
    int BRClothAssetID;
    int BRHatAssetID;
    int BRLeftArmAssetID;
    int BRSuitAssetID;
    int BRWingAssetID;
    int BagAssetID;
    int ReservedSkinId;
    int ClothAssetID;
    int FxAssetIDHandGestureUI;
    int FxAssetIDShow;
    int FxAssetID_1P;
    int FxAssetID_3P;
    int FxAssetID_UI;
    int HatAssetID;
    uint ID;
    int LeftArm1PAssetID;
    int LeftArmAssetID;
    int ShowBagAssetID;
    int ShowClothAssetID;
    int ShowHatAssetID;
    int SuitAssetID;
    int WingAssetID;
};
extern RoleSkinFields* roleskinFields;

struct CharacterModelFields {
    uint8_t BindEffect1PType;
    bool Female;
    uint8_t pad_12[2];
    uint AvatarAnimationType;
    int BRBagModel;
    int BRHeadModel;
    int BRLobby;
    int BRModel;
    int BindEffect1P;
    int ChangeClipEffect1P;
    int DefaultModelID;
    int Guarder1P;
    int Guarder3P;
    int GuarderBagModel;
    int GuarderHeadModel;
    int GuarderLobby;
    int HeadIgnoreGUPSkin;
    int SpReceiveAnimType;
    int Traitor1P;
    int Traitor3P;
    float* BagOffset;
    string* Desc;
    float* ReservedOffset;
    int* VestOtherModelID;
    uintptr_t ItemID;
    uintptr_t LitePackDefault;
};
extern CharacterModelFields* characterfields;

struct RolePackFields {
    int mMatchRoomEasterEggType;
    uint mMatchRoomEasterEggCharacterID;
    void *mMatchRoomEasterEggActionName;
    bool DisableCustomize;
    bool EntryDefaultAppearance;
    bool SkipWinnerCircleAnimation;
    bool UseDynamicLobbyScene;
    bool UseVFWFeature;
    char pad[3];
    int AirItem1;
    int AnimationWhenSell;
    int CabinIndivID;
    int DeadBoxID;
    int DynamicRoomAssetID;
    int DynamicRoomUnlockItemID;
    int EntryAnimID;
    int EntryUIEffectAssetID;
    int EntryVideoAnimID;
    float EntryVideoPlayTime;
    int EntryVideoTagAnimID;
    int GestureId;
    int HandEffectUI;
    int HomeEasterEggAssetID;
    float HomeEasterEggCoolingTime;
    int HomeEasterEggType;
    int KillStreakSkinID;
    int LoadingFrame;
    int LobbySceneType;
    int MVPIndivID;
    int MatchRoomSort;
    int OpenSceneIndivID;
    int PendantShowID;
    int PetID;
    int PetSwitchEffectAssetID;
    int RoleFeatureType;
    uint RolePackID;
    int SkillSkinID;
    int SkillSkinUI;
    int SpecialSceneFrameGroupAssetID;
    uint TODSceneID;
    int VTOLShowAssetID;
    void *AirPoseBR;
    void *EntryAnim;
    void *EntryIdleAnim;
    void* EntryIdleItems;
    void* EntryItems;
    void* EntryItemsAnim;
    void* EntryItemsIdleAnim;
    void *EntryUIAnimationStateName;
};
extern RolePackFields* packfields;

struct RoleConfFields {
    uint BaseCharacterID;
    int roleLeftArmID;
    int roleFinalSuitID;
    int roleBasicHologramID;
    uint8_t ArchiveSwitch;
    uint8_t ArmorType;
    uint8_t ColorID;
    uint8_t ColorSubID;
    uint8_t DefaultPerk_1;
    uint8_t DefaultPerk_2;
    uint8_t DefaultPerk_3;
    bool DefaultShow;
    uint8_t DefaultSkill_1;
    bool ExternalUnVisible;
    bool IsVFW;
    uint8_t Loadout_Magnifier_Flag;

    uint8_t RoleLvGroupID;
    bool ShowInBag;
    uint8_t ShowOrder;
    bool ShowRare;
    int BlockAppearance;
    int ID;
    int JetPackInfoID;
    int KeepInCatalog;
    float RTPCMaxValue;
    int RolePackID;
    void *AudioSwitch;
    void *BRBankNameEX;
    void *BasicAudioBank;
    void *BasicAudioBank2;
    void *BasicAudioBank3;
    void *DefaultAudioBank;
    void *DefaultAudioBank2;
    void *DefaultAudioEventEx;
    void *FoleyAudioBank;
    void *FoleyAudioNameEx;
    void *LOCID_Desc;
    void *LOCID_Name;
    void *LitePackageKeyName;
    void *MPBankNameEx;
    void *PVEBankNameEX;
    void *RTPCName;
    void *RoleSkinBox;
    void *SpecialAudioBank;
    void *SpecialAudioBank2;
    void *SpecialAudioEventEx;
    void *SpecialTips;
    void *WinnerCircle;
};
extern RoleConfFields* roleFields;

struct ItemResourceFields {
    uint16_t UIMiniSpriteName;
    uint16_t UISmallSpriteName;
    uint16_t UISpriteName;
    uint16_t UISquareSpriteName;
    int FxAssetID;
    int ID;
    int InventoryModelID;
    int ModelAssetIDRaw;
    uint64_t AvatarModelID;
};
extern ItemResourceFields* itemFields;

struct WeaponConfFields {
    uint8_t Accuracy;
    bool CanEquipOptic;
    uint8_t ColorID;
    uint8_t ColorSubID;
    uint8_t Control;
    uint8_t Damage;
    bool ExternalUnVisible;
    uint8_t FireRate;
    uint8_t LOCID_HowToGet;
    uint8_t MaxAttach;
    uint8_t MaxSkill;
    uint8_t Mobility;
    uint8_t Range;
    bool ShowRare;
    uint8_t SuperShowGroup;
    uint8_t WeaponBluePrintType;
    uint8_t WinnerCircleAnimID;
    uint BaseWeaponId;
    int DefWeaponSkinID;
    int DefaultKillBrocast;
    uint ID;
    int IsDouble;
    int WinnerCircleWeaponMountType;
    void *ChangeColorStr;
    void *EnableCamoBeta;
    void *LOCID_Desc;
    void *LOCID_Name;
    void *ModelRotation;
    void *UIProperty;
    void *UIPropertyUnlockLevel;
};
extern WeaponConfFields* weaponconfFields;

static_assert(offsetof(RolePackFields, RolePackID) == 0x80,
              "Role pack ID offset changed");
static_assert(offsetof(ItemResourceFields, FxAssetID) == 0x8,
              "Item resource asset offset changed");
static_assert(offsetof(ItemResourceFields, AvatarModelID) == 0x18,
              "Item resource model offset changed");
static_assert(offsetof(WeaponConfFields, LOCID_Desc) == 0x40,
              "Weapon config description offset changed");
static_assert(offsetof(WeaponConfFields, LOCID_Name) == 0x48,
              "Weapon config name offset changed");

struct WeaponSkinFields {
    uint8_t ColorID;
    uint8_t GoGetPos;
    bool ReservedCleanFlag;
    uint8_t SkinType;
    uint BRWeaponID;
    int BrSkinWeaponID;
    int CamoID;
    int ColorSubID;
    uint ID;
    uint WeaponID;
    void *LOCID_Describe;
    void *LOCID_HowToGet;
    void *LOCID_Name;
    Array<int> *SkinID1P;
    Array<int> *SkinID3P;
};
extern WeaponSkinFields* weaponskinFields;

static_assert(offsetof(WeaponSkinFields, ID) == 0x14,
              "Weapon skin ID offset changed");
static_assert(offsetof(WeaponSkinFields, LOCID_Name) == 0x30,
              "Weapon skin name offset changed");

struct Item2InventoryFields {
    int ItemID;
    int WeaponAssetGroupID;
    int WeaponIconID;
    int AnimationTemplateItemID;
    int FallbackWeaponID;
    String* AssetName;
    void* ConfigFilePathH;
    void* ConfigFilePath_BRH;
    void* ConfigFilePath_DMZH;
    void* ConfigFilePath_PVEH;
    void* WeaponNameH;
};
extern Item2InventoryFields* item2Fields;

struct WeaponAssetGroupFields {
    int FireEffectGroupID;
    int Id;
};
extern WeaponAssetGroupFields* weaponAssetFields;

struct WeaponFireEffectFields {
    int AssetIdFor1P;
    int AssetIdFor3P;
    int AssetIdForBulletSmoke1P;
    int AssetIdForBulletSmoke3P;
    int AssetIdForBulletTrack1P;
    int AssetIdForBulletTrack3P;
    int AssetIdForCharging1P;
    int AssetIdForCrossHair;
    int AssetIdForDecalEffect;
    int AssetIdForExtra1P;
    int AssetIdForExtraBulletSmoke1P;
    int AssetIdForExtraBulletSmoke3P;
    int AssetIdForHip1P;
    int AssetIdForHip3P;
    int AssetIdForHitEnemy;
    int AssetIdForInspectionScreenEffect;
    int AssetIdForSecondBulletSmoke1P;
    int AssetIdForSecondBulletSmoke3P;
    int AssetIdForSecondHip1P;
    int AssetIdForSecondHip3P;
    int AssetIdForShellDrop1P;
    int AssetIdForShellDrop3P;
    int AttachAssetIdFor1P;
    int AttachAssetIdFor3P;
    int AttachAssetIdForHip1P;
    int AttachAssetIdForHip3P;
    int DefaultKillEffectLevelUI;
    float HitEffectDelayDestroyTime;
    int HitEffectType;
    int Id;
};
extern WeaponFireEffectFields* weaponfireFields;

struct WeaponConfExtraFields {
    uint8_t DefaultMythicArmor;
    uint8_t DefaultMythicSig;
    uint8_t GoGetPos;
    uint8_t SecondTab;
    uint8_t ThirdTab;
    int DefaultDeadReplayEffectId;
    int DefaultKillEffectId;
    int FireEffectGroupID;
    uint ID;
};
extern WeaponConfExtraFields* weaponextraFields;

struct MythicArmorFields {
    int AssetID;
    int ColorID;
    int ColorSubID;
    int GoGetPos;
    uint ID;
    int MythicArmorLevel;
    int SecondTab;
    int ThirdTab;
};
extern MythicArmorFields* mythicarmorFields;

struct MythicSightFields {
    bool ShowInBag;
    char pad_11[3];
    int AssetID;
    int ColorID;
    int ColorSubID;
    uint ID;
    int RelateMythicSight;
    void* LOCID_Desc;
    void* LOCID_Name;
    Array<int> **WeaponCanEquip;
};
extern MythicSightFields* mythicsightFields;

struct KillEffectItemFields {
    Array<int> **RealAssetIDs;
    bool IsAutoDemolition;
    bool ShowInBag;
    int ColorID;
    int ColorSubID;
    uint CurrencyID;
    int GoGetPos;
    uint ID;
    int SecondTab;
    int SellPrice;
    int SellProtection;
    int ThirdTab;
    Array<int> **AssetID;
    Array<int> **AssetID_CN;
    Array<int> **AssetID_GA;
    Array<int> **AssetID_KR;
    Array<int> **AssetID_VNG;
    string* GoGetUrl;
    Array<int> **KillCount;
    void* LOCID_Desc;
    void* LOCID_HowToGet;
    void* LOCID_Name;
    Array<int> **WeaponCanEquip;
};
extern KillEffectItemFields* killeffectFields;

