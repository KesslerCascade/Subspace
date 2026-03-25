#pragma once

// common structure types whose exact layout is determined at runtime by finding offsets

// Blueprint and derived types
typedef struct Blueprint Blueprint;
typedef struct AugmentBlueprint AugmentBlueprint;
typedef struct DroneBlueprint DroneBlueprint;
typedef struct ItemBlueprint ItemBlueprint;
typedef struct ShipBlueprint ShipBlueprint;
typedef struct WeaponBlueprint WeaponBlueprint;

// CompleteShip derived types
typedef struct CompleteShip CompleteShip;
typedef struct BossShip BossShip;

// Ship derived classes
typedef struct Ship Ship;

// ShipSystem derived classes
typedef struct ShipSystem ShipSystem;
typedef struct DroneSystem DroneSystem;
typedef struct HackingSystem HackingSystem;
typedef struct WeaponSystem WeaponSystem;

// StoreBox derived classes
typedef struct StoreBox StoreBox;
typedef struct AugmentStoreBox AugmentStoreBox;
typedef struct CrewStoreBox CrewStoreBox;
typedef struct DroneStoreBox DroneStoreBox;
typedef struct ItemStoreBox ItemStoreBox;
typedef struct RepairStoreBox RepairStoreBox;
typedef struct SystemStoreBox SystemStoreBox;
typedef struct WeaponStoreBox WeaponStoreBox;

// Non-hierarchical types
typedef struct AchievementTracker AchievementTracker;
typedef struct AnimationTracker AnimationTracker;
typedef struct AugmentEquipBox AugmentEquipBox;
typedef struct BlueprintManager BlueprintManager;
typedef struct CApp CApp;
typedef struct CEvent CEvent;
typedef struct CFPS CFPS;
typedef struct CombatControl CombatControl;
typedef struct CommandGui CommandGui;
typedef struct CreditScreen CreditScreen;
typedef struct CrewMember CrewMember;
typedef struct Drone Drone;
typedef struct Equipment Equipment;
typedef struct EquipmentBox EquipmentBox;
typedef struct EventSystem EventSystem;
typedef struct FTLButton FTLButton;
typedef struct GameOver GameOver;
typedef struct InputEvent InputEvent;
typedef struct Location Location;
typedef struct LocationEvent LocationEvent;
typedef struct MainMenu MainMenu;
typedef struct MouseControl MouseControl;
typedef struct ProjectileFactory ProjectileFactory;
typedef struct ResourceControl ResourceControl;
typedef struct ScoreKeeper ScoreKeeper;
typedef struct ShipEvent ShipEvent;
typedef struct ShipManager ShipManager;
typedef struct ShipObject ShipObject;
typedef struct ShipStatus ShipStatus;
typedef struct SoundControl SoundControl;
typedef struct Sector Sector;
typedef struct StarMap StarMap;
typedef struct TextLibrary TextLibrary;
typedef struct WeaponStoreBox WeaponStoreBox;
typedef struct WorldManager WorldManager;
