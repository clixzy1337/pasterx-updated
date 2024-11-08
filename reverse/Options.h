#pragma once
// DYNAMIC FOV
bool dynamicEsp = false;
float decreaseStep = 0.1f;
bool isButtonPressed = false;
int steps = 300;

// AIM
bool Aimbot = true;
bool enableaimbot = false;
bool mouseaim = false;
bool legitaim = false;
bool trigerbot = false;
bool enablefov = false;
bool fovcircle = false;
bool Filled_Fov = false;
bool dynamedfovv = false;
bool triggerbot = false;
bool vehicle_fly = false;
// ESP
bool lobby = false;
bool Esp = true;
bool slefESP = false;
bool BoxEsp = true;
bool fbox = false;
bool OutlineBoxEsp = false;
bool D3_Box = false;
bool CorneredBoxEsp = false;
bool enablebox = false;
bool enabletracers = false;
bool FilledBoxEsp = false;
bool JustBox = false;
bool DistanceEsp = false;
bool Name_Esp = false;
bool playerFly = false;
bool SkeletonEsp = true;
bool platformEsp = false;
bool outlinedbox = false;
bool outlinedcorneredbox = false;
bool weaponesp = false;
bool VisibleCheck = false;
float NotVisColor[3] = { 0.92f, 0.10f, 0.14f }; 
float VisColor[3] = { 144.00f, 0.00f, 255.00f };  


bool reloadEsp = false;
bool crounchedesp = false;
bool jumpesp = false;
bool bot = false;
bool nameEsp = false;
bool Box_3D = false; 

bool tracersbottom = false; 
bool tracersmiddle = false;
bool tracerstop = true;


// MISC
bool ShowMenu = true;
bool particles = true;
bool StreamProof = false;
bool enablecrosshair = false;
bool normalCrosshair = false;
bool diagonalCrosshair = false;
bool roundCrosshair = false;
bool vehicle_nocooldodwn = false;
bool teamate = false;
bool worldgrv = false;
bool draw_username = false;
bool draw_held_weapon = false;
bool VehicleESP = false;
bool showFps = false;

// EXPLOITS
bool spinbot = false;
bool InstaRes = false;
bool NoBloom = false;
bool bullettp = false;
bool instant_reload = false;
int fovchangervalue = 80;

// ADDITIONAL VARIABLES
float BoxWidthValue = 0.550;
float ChangerFOV = 80;
int aimkeypos = 3;
int aimbone = 0;
float smooth = 3.0f;
float VisDist = 2400.f;
float AimFOV = 300;
float triggerbotdelay = 1;
int aimkey;
int hitbox;
bool headbone = false;
float renderdistance = 600;
bool neckbone = false;
bool chestbone = false;
bool pelvisbone = false;
bool reloadingesp = false;
bool fovvchanger = false;
bool ammoesp = false;
bool NoEquipAnimation = false;
bool headesp = false;
bool squarefov = false;
bool filledsquarefov = false;
bool AirStuck = false;
bool carFly = false;
int hitboxpos = 0;
bool cornerFPS = true;
float CrossHair[3];
float CrossThickness = 1.5f;
float Skeleton_Thickness = 2.5f;
float BoxThickness = 2.5f;
float CorneredThickness = 1.5f;
float ButtomLineThickness = 1.5f;
float boxchick[3] = { 1.f, 1.f, 1.f };
float boxcolor[3] = { 1.f, 1.f, 1.f };
namespace PlayerColor {
	float SkeletonVisible[4] = { 0.5f, 0.0f, 0.5f, 1.0f };
	float SkeletonNotVisible[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float BoxVisible[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float BoxNotVisible[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float CorneredVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float CorneredNotVisible[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float ButtomLineVisible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float ButtomLineNotVisible[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float FovCircle1Visible[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float Crosshaircolor22[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
}