#include "Main.h"
#include "Driver/driver.h"
#include "Print.hpp"
#include "d3d9_x.h"
#include "xor.hpp"
#include <dwmapi.h>
#include <vector>
#include "Keybind.h"
#include "offsets.h"
#include "xstring"
#include "Options.h"
#include "icon.h"
#include "icons1.h"
#define color1 (WORD)(0x0001 | 0x0000)
#define color2 (WORD)(0x0002 | 0x0000)
#define color3 (WORD)(0x0003 | 0x0000)
#define color4 (WORD)(0x0004 | 0x0000)
#define color5 (WORD)(0x0005 | 0x0000)
#define color6 (WORD)(0x0006 | 0x0000)
#define color7 (WORD)(0x0007 | 0x0000)
#define color8 (WORD)(0x0008 | 0x0000)
#define color9 (WORD)(0x0008 | 0x0000)
#define COLOR(h, c) SetConsoleTextAttribute(h, c);
int bigfovballs = 25;
float fovv_value = bigfovballs;


//static int aimkeypos = 3;
//static int aimbone = 1;

int faken_rot = 0;

float BOG_TO_GRD(float BOG) {
	return (180 / M_PI) * BOG;
}

float GRD_TO_BOG(float GRD) {
	return (M_PI / 180) * GRD;
}


DWORD64 bonearray;
DWORD_PTR Uworld;
DWORD_PTR LocalPawn;
DWORD_PTR PlayerState;
DWORD_PTR Localplayer;
DWORD_PTR Rootcomp;
DWORD_PTR PlayerController;
DWORD_PTR Persistentlevel;
uintptr_t PlayerCameraManager;
//uintptr_t virtualaddy;
Vector3 localactorpos;
Vector3 relativelocation;
Vector3 Relativelocation;


IDirect3DTexture9* logo = nullptr;
IDirect3DTexture9* option1 = nullptr;
IDirect3DTexture9* option2 = nullptr;
IDirect3DTexture9* option3 = nullptr;

//Helpers
ImFont* m_pFont;
ImFont* esp_pFont;
ImFont* verdena_pfont;
ImFont* details;
ImFont* arrow;
ImFont* mainfont;
ImFont* titlefont;
ImFont* icons;
ImVec2 position;
ImDrawList* drawlist;
ImVec2 skeletonpos;
ImDrawList* skeletondraw;
int tabs = 0;

uint64_t TargetPawn;
int localplayerID;


RECT GameRect = { NULL };
D3DPRESENT_PARAMETERS d3dpp;
DWORD ScreenCenterX;
DWORD ScreenCenterY;
Vector3 LocalRelativeLocation; struct FBoxSphereBounds
{
	struct Vector3                                     Origin;                                                   // 0x0000(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	struct Vector3                                     BoxExtent;                                                // 0x0018(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	double                                             SphereRadius;                                             // 0x0030(0x0008) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};
static void xCreateWindow();
static void xInitD3d();
static void xMainLoop();
static void xShutdown();
static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND Window = NULL;
IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;

typedef struct {
	float X, Y, Z;
} FVector;

typedef struct {
	float X, Y;
} FVector2D;

typedef struct {
	float Pitch;
	float Yaw;
	float Roll;
} FRotator;

typedef struct {
	FVector Location;
	float FOV;
	float OrthoWidth;
	float OrthoNearClipPlane;
	float OrthoFarClipPlane;
	float AspectRatio;
} FMinimalViewInfo;

typedef struct {
	float M[4][4];
} FMatrix;

typedef struct {
	FVector ViewOrigin;
	char _padding_0[4];
	FMatrix ViewRotationMatrix;
	FMatrix ProjectionMatrix;
} FSceneViewProjectionData;

std::string encryptString(const std::string& input) { // + 1 ascii
	std::string encrypted;
	for (char c : input) {
		encrypted.push_back(c + 1);
	}
	return encrypted;
}

std::string decryptString(const std::string& encrypted) { // - 1 ascii
	std::string decrypted;
	for (char c : encrypted) {
		decrypted.push_back(c - 1);
	}
	return decrypted;
}


class UClass {
public:
	BYTE _padding_0[0x40];
	UClass* SuperClass;
};

class UObject {
public:
	PVOID VTableObject;
	DWORD ObjectFlags;
	DWORD InternalIndex;
	UClass* Class;
	BYTE _padding_0[0x8];
	UObject* Outer;

	inline BOOLEAN IsA(PVOID parentClass) {
		for (auto super = this->Class; super; super = super->SuperClass) {
			if (super == parentClass) {
				return TRUE;
			}
		}

		return FALSE;
	}
};

void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}


class FUObjectItem {
public:
	UObject* Object;
	DWORD Flags;
	DWORD ClusterIndex;
	DWORD SerialNumber;
	DWORD SerialNumber2;
};

class TUObjectArray {
public:
	FUObjectItem* Objects[9];
};

class GObjects {
public:
	TUObjectArray* ObjectArray;
	BYTE _padding_0[0xC];
	DWORD ObjectCount;
};


template<class T>
struct TArray {
	friend struct FString;

public:
	inline TArray() {
		Data = nullptr;
		Count = Max = 0;
	};

	inline INT Num() const {
		return Count;
	};

	inline T& operator[](INT i) {
		return Data[i];
	};

	inline BOOLEAN IsValidIndex(INT i) {
		return i < Num();
	}

private:
	T* Data;
	INT Count;
	INT Max;
};

struct FString : private TArray<WCHAR> {
	FString() {
		Data = nullptr;
		Max = Count = 0;
	}

	FString(LPCWSTR other) {
		Max = Count = static_cast<INT>(wcslen(other));

		if (Count) {
			Data = const_cast<PWCHAR>(other);
		}
	};

	inline BOOLEAN IsValid() {
		return Data != nullptr;
	}

	inline PWCHAR c_str() {
		return Data;
	}
};

VOID(*FreeInternal)(PVOID) = nullptr;

VOID Free(PVOID buffer) {
	FreeInternal(buffer);
}

#pragma once

namespace detail
{
	extern "C" void* _spoofer_stub();

	template <typename Ret, typename... Args>
	static inline auto shellcode_stub_helper(
		const void* shell,
		Args... args
	) -> Ret
	{
		auto fn = (Ret(*)(Args...))(shell);
		return fn(args...);
	}

	template <std::size_t Argc, typename>
	struct argument_remapper
	{
		template<
			typename Ret,
			typename First,
			typename Second,
			typename Third,
			typename Fourth,
			typename... Pack
		>
		static auto do_call(const void* shell, void* shell_param, First first, Second second,
			Third third, Fourth fourth, Pack... pack) -> Ret
		{
			return shellcode_stub_helper< Ret, First, Second, Third, Fourth, void*, void*, Pack... >(shell, first, second, third, fourth, shell_param, nullptr, pack...);
		}
	};

	template <std::size_t Argc>
	struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>>
	{
		template<
			typename Ret,
			typename First = void*,
			typename Second = void*,
			typename Third = void*,
			typename Fourth = void*
		>
		static auto do_call(
			const void* shell,
			void* shell_param,
			First first = First{},
			Second second = Second{},
			Third third = Third{},
			Fourth fourth = Fourth{}
		) -> Ret
		{
			return shellcode_stub_helper<
				Ret,
				First,
				Second,
				Third,
				Fourth,
				void*,
				void*
			>(
				shell,
				first,
				second,
				third,
				fourth,
				shell_param,
				nullptr
			);
		}
	};
}

template <typename Ret, typename... Args>
static inline auto SpoofCall(Ret(*fn)(Args...), Args... args) -> Ret
{
	static const void* jmprbx = nullptr;
	if (!jmprbx) {
		const auto ntdll = reinterpret_cast<const unsigned char*>(::GetModuleHandleW(NULL));
		const auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(ntdll);
		const auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(ntdll + dos->e_lfanew);
		const auto sections = IMAGE_FIRST_SECTION(nt);
		const auto num_sections = nt->FileHeader.NumberOfSections;

		constexpr char section_name[5]{ '.', 't', 'e', 'x', 't' };
		const auto     section = std::find_if(sections, sections + num_sections, [&](const auto& s) {
			return std::equal(s.Name, s.Name + 5, section_name);
			});

		constexpr unsigned char instr_bytes[2]{ 0xFF, 0x26 };
		const auto              va = ntdll + section->VirtualAddress;
		jmprbx = std::search(va, va + section->Misc.VirtualSize, instr_bytes, instr_bytes + 2);
	}

	struct shell_params
	{
		const void* trampoline;
		void* function;
		void* rdx;
	};

	shell_params p
	{
		jmprbx,
		reinterpret_cast<void*>(fn)
	};

	using mapper = detail::argument_remapper<sizeof...(Args), void>;
	return mapper::template do_call<Ret, Args...>((const void*)&detail::_spoofer_stub, &p, args...);
}

namespace SpoofRuntime {
	inline float acosf_(float x)
	{
		return SpoofCall(acosf, x);
	}

	inline float atan2f_(float x, float y)
	{
		return SpoofCall(atan2f, x, y);
	}
}



#define BONE_HEAD_ID (110)
#define BONE_NECK_ID (66)
#define BONE_CHEST_ID (37)
#define BONE_PELVIS_ID (2)



__forceinline Vector3 GetBoneWithRotation(uintptr_t mesh, int bone_id)
{
	uintptr_t bone_array = read<uintptr_t>(mesh + offsets::BoneArray);
	if (bone_array == NULL) bone_array = read<uintptr_t>(mesh + offsets::BoneArray + 0x10);
	FTransform bone = read<FTransform>(bone_array + (bone_id * 0x60));
	FTransform component_to_world = read<FTransform>(mesh + offsets::ComponetToWorld);
	D3DMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), component_to_world.ToMatrixWithScale());
	return Vector3(matrix._41, matrix._42, matrix._43);
}





D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;


	return matrix;


};



double __fastcall Atan2(double a1, double a2)
{
	double result; // xmm0_8

	result = 0.0;
	if (a2 != 0.0 || a1 != 0.0)
		return atan2(a1, a2);
	return result;
}
double __fastcall FMod(double a1, double a2)
{
	if (fabs(a2) > 0.00000001)
		return fmod(a1, a2);
	else
		return 0.0;
}

double ClampAxis(double Angle)
{
	// returns Angle in the range (-360,360)
	Angle = FMod(Angle, (double)360.0);

	if (Angle < (double)0.0)
	{
		// shift to [0,360) range
		Angle += (double)360.0;
	}

	return Angle;
}

double NormalizeAxis(double Angle)
{
	// returns Angle in the range [0,360)
	Angle = ClampAxis(Angle);

	if (Angle > (double)180.0)
	{
		// shift to (-180,180]
		Angle -= (double)360.0;
	}

	return Angle;
}




FRotator Rotator(fquat* F)
{
	const double SingularityTest = F->z * F->x - F->w * F->y;
	const double YawY = 2.f * (F->w * F->z + F->x * F->y);
	const double YawX = (1.f - 2.f * ((F->y * F->y) + (F->z * F->z)));

	const double SINGULARITY_THRESHOLD = 0.4999995f;
	const double RAD_TO_DEG = 57.295776;
	float Pitch, Yaw, Roll;

	if (SingularityTest < -SINGULARITY_THRESHOLD)
	{
		Pitch = -90.f;
		Yaw = (Atan2(YawY, YawX) * RAD_TO_DEG);
		Roll = NormalizeAxis(-Yaw - (2.f * Atan2(F->x, F->w) * RAD_TO_DEG));
	}
	else if (SingularityTest > SINGULARITY_THRESHOLD)
	{
		Pitch = 90.f;
		Yaw = (Atan2(YawY, YawX) * RAD_TO_DEG);
		Roll = NormalizeAxis(Yaw - (2.f * Atan2(F->x, F->w) * RAD_TO_DEG));
	}
	else
	{
		Pitch = (asin(2.f * SingularityTest) * RAD_TO_DEG);
		Yaw = (Atan2(YawY, YawX) * RAD_TO_DEG);
		Roll = (Atan2(-2.f * (F->w * F->x + F->y * F->z), (1.f - 2.f * ((F->x * F->x) + (F->y * F->y)))) * RAD_TO_DEG);
	}

	FRotator RotatorFromQuat = FRotator{ Pitch, Yaw, Roll };
	return RotatorFromQuat;
}
struct CameraInfo
{
	Vector3 location;
	Vector3 rotation;
	float fov;
};

float fov;

CameraInfo GetCameraInfo()
{
	CameraInfo camera;
	auto location_pointer = read<uintptr_t>(Uworld + 0x120);
	auto rotation_pointer = read<uintptr_t>(Uworld + 0x130);

	struct RotationInfo
	{
		double pitch;
		char pad_0008[24];
		double yaw;
		char pad_0028[424];
		double roll;
	} rotationInfo;

	rotationInfo.pitch = read<double>(rotation_pointer);
	rotationInfo.yaw = read<double>(rotation_pointer + 0x20);
	rotationInfo.roll = read<double>(rotation_pointer + 0x1d0);

	camera.location = read<Vector3>(location_pointer);
	camera.rotation.x = asin(rotationInfo.roll) * (180.0 / M_PI);
	camera.rotation.y = ((atan2(rotationInfo.pitch * -1, rotationInfo.yaw) * (180.0 / M_PI)) * -1) * -1;
	camera.fov = read<float>((uintptr_t)Localplayer + 0x4AC);

	return camera;
}


Vector3 ProjectWorldToScreen(Vector3 WorldLocation)
{
	CameraInfo viewInfo = GetCameraInfo();
	D3DMATRIX tempMatrix = Matrix(viewInfo.rotation);
	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
	Vector3 vDelta = WorldLocation - viewInfo.location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector3((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(viewInfo.fov * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(viewInfo.fov * (float)M_PI / 360.f))) / vTransformed.z, 0);
}



void DrawStrokeText(int x, int y, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);

	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(255, 255, 255, 255)), utf_8_2.c_str());
}


float DrawLobbyText(ImFont* pFont, const std::string& text, float size, ImU32 color, bool center)
{
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int i = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		if (center)
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((textSize.x / 2.0f) + 1, (textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((textSize.x / 2.0f) - 1, (textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((textSize.x / 2.0f) + 1, (textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((textSize.x / 2.0f) - 1, (textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(textSize.x / 2.0f, textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}
		else
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(+1, (textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(-1, (textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(+1, (textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(-1, (textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());


		}

		y = +textSize.y * (i + 1);
		i++;
	}
	return y;
}


float DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center)
{
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int i = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		if (center)
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}
		else
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}

		y = pos.y + textSize.y * (i + 1);
		i++;
	}
	return y;
}

void DrawText1(int x, int y, const char* str, RGBA* color)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}
void DrawOutlineText1(int x, int y, const char* str, RGBA* color)
{
	// Définir la couleur du contour
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 1), IM_COL32(0, 0, 0, 255), str); // Bas-droite
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y + 1), IM_COL32(0, 0, 0, 255), str); // Bas-gauche
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y - 1), IM_COL32(0, 0, 0, 255), str); // Haut-droite
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y - 1), IM_COL32(0, 0, 0, 255), str); // Haut-gauche

	// Définir la couleur du texte principal
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0f, color->G / 255.0f, color->B / 255.0f, color->A / 255.0f)), str);
}

std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}

std::wstring MBytesToWString(const char* lpcszString)
{
	//SPOOF_FUNC;
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}
void DrawString(float fontSize, int x, int y, RGBA* color, bool bCenter, bool stroke, const char* format, ...)
{
	//SPOOF_FUNC;
	va_list args;
	va_start(args, format);

	char buffer[1024];
	vsnprintf_s(buffer, sizeof(buffer), format, args);

	va_end(args);

	std::string text = WStringToUTF8(MBytesToWString(buffer).c_str());

	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - static_cast<int>(textSize.x / 2);
		y = y - static_cast<int>(textSize.y / 2);
	}

	ImU32 textColor = ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0f, color->G / 255.0f, color->B / 255.0f, color->A / 255.0f));

	if (stroke)
	{
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				if (i != 0 || j != 0)
				{
					ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + i, y + j), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
				}
			}
		}
	}

	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), textColor, text.c_str());
}

void LobbyLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(959, 1100), ImVec2(960, 540), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 000.0, color->B / 000.0, color->A / 255.0)), 1.0f);
}

void LobbyBox(float X, float Y, float W, float H, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(815 + 1, 967 + 1), ImVec2(((830 + 275) - 1), ((851 - 625) - 1)), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
	//ImGui::GetOverlayDrawList()->AddRect(ImVec2(150, 60), ImVec2(150 + 63, 60 + 125), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}

void DrawCircle(int x, int y, int radius, RGBA* color, int segments)
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), segments);
}
void DrawBox(float X, float Y, float W, float H, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X + 1, Y + 1), ImVec2(((X + W) - 1), ((Y + H) - 1)), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X, Y), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}
void DrawFilledRect(int x, int y, int w, int h, ImU32 color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}
void DrawNormalBox(int x, int y, int w, int h, int borderPx, ImU32 color)
{

	DrawFilledRect(x + borderPx, y, w, borderPx, color); //top 
	DrawFilledRect(x + w - w + borderPx, y, w, borderPx, color); //top 
	DrawFilledRect(x, y, borderPx, h, color); //left 
	DrawFilledRect(x, y + h - h + borderPx * 2, borderPx, h, color); //left 
	DrawFilledRect(x + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	DrawFilledRect(x + w - w + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	DrawFilledRect(x + w + borderPx, y, borderPx, h, color);//right 
	DrawFilledRect(x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color);//right 
}


void DrawCorneredBox(int X, int Y, int W, int H, const ImU32& color, int thickness) {
	float lineW = (W / 3);
	float lineH = (H / 3);

	//corners
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}

void DrawOutlinedCorneredBox(int X, int Y, int W, int H, const ImU32& color, int thickness, int borderPx) {
	DrawCorneredBox(X, Y, W, H, IM_COL32(0, 0, 0, 255), thickness);

	DrawCorneredBox(X + thickness, Y + thickness, W - 2 * thickness, H - 2 * thickness, color, thickness);
}


void DrawBox(int X, int Y, int W, int H, const ImU32& color, int thickness) {
	float lineW = (W / 1);
	float lineH = (H / 1);
	ImDrawList* Drawlist = ImGui::GetOverlayDrawList();

	//corners
	Drawlist->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::GetColorU32(color), thickness);
	Drawlist->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32(color), thickness);
	Drawlist->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::GetColorU32(color), thickness);
	Drawlist->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::GetColorU32(color), thickness);
	Drawlist->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32(color), thickness);
	Drawlist->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::GetColorU32(color), thickness);
	Drawlist->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
	Drawlist->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}

void Normalbox(int X, int Y, int W, int H, const ImU32& color, int thickness, int borderPx) {
	float lineW = (W / 1);
	float lineH = (H / 1);
	DrawFilledRect(X + borderPx, Y, W, borderPx, ImColor(10, 10, 10, 180));


	//corners
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}





void DrawOutlinedNormalBox(int X, int Y, int W, int H, const ImU32& color, int thickness, int borderPx) {

	Normalbox(X, Y, W, H, IM_COL32(0, 0, 0, 255), thickness, 0);

	Normalbox(X + thickness, Y + thickness, W - 2 * thickness, H - 2 * thickness, color, thickness, 0);
}



typedef struct _FNlEntity
{
	uint64_t Actor;
	int ID;
	uint64_t mesh;
}FNlEntity;

std::vector<FNlEntity> entityList;


struct HandleDisposer
{
	using pointer = HANDLE;
	void operator()(HANDLE handle) const
	{
		if (handle != NULL || handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}
	}
};
using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;
void clear() {
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
	);
	SetConsoleCursorPosition(console, topLeft);
}

static std::uint32_t _GetProcessId(std::string process_name) {
	PROCESSENTRY32 processentry;
	const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));

	if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
		return 0;

	processentry.dwSize = sizeof(MODULEENTRY32);

	while (Process32Next(snapshot_handle.get(), &processentry) == TRUE) {
		if (process_name.compare(processentry.szExeFile) == 0)
			return processentry.th32ProcessID;
	}
	return 0;
}
// random standard header


// PlayerName

auto get_player_name(uintptr_t player_state) -> std::string
{
	auto fstring = read<__int64>(player_state + offsets::PlayerName);
	auto length = read<int>(fstring + 16);

	auto v6 = (__int64)length;

	if (!v6) return std::string(E("BOT / AI"));

	auto IsABot = read<char>(player_state + 0x29A & 0x8);

	if (IsABot) return std::string(E("AI"));


	auto ftext = (PVOID)read<__int64>(fstring + 8);

	wchar_t* buffer = new wchar_t[length];
	mem::read_physical(static_cast<PVOID>(ftext), buffer, length * sizeof(wchar_t));

	char v21;
	int v22;
	int i;

	int v25;
	UINT16* v23;

	v21 = v6 - 1;
	if (!(UINT32)v6)
		v21 = 0;
	v22 = 0;
	v23 = (UINT16*)buffer;
	for (i = (v21) & 3; ; *v23++ += i & 7)
	{
		v25 = v6 - 1;
		if (!(UINT32)v6)
			v25 = 0;
		if (v22 >= v25)
			break;
		i += 3;
		++v22;
	}

	std::wstring username{ buffer };
	delete[] buffer;
	return std::string(username.begin(), username.end());
};

DWORD Menuthread(LPVOID in)
{
	while (1)
	{
		if (MouseController::GetAsyncKeyState(VK_INSERT) & 100) {
			// ShowMenu = !ShowMenu;
		}
		Sleep(1);
	}
}
using namespace std;

void SetConsoleSize(int width, int height)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT rect = { 0, 0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1) };
	COORD size = { static_cast<SHORT>(width), static_cast<SHORT>(height) };
	SetConsoleWindowInfo(consoleHandle, TRUE, &rect);
	SetConsoleScreenBufferSize(consoleHandle, size);
}

std::string random_string(std::string::size_type length)
{
	static auto& chrs = "0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#%^&*()";

	thread_local static std::mt19937 rg{ std::random_device{}() };
	thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

	std::string s;

	s.reserve(length);

	while (length--)
		s += chrs[pick(rg)];

	return s + ".exe";
}
std::string path()
{
	char shitter[_MAX_PATH]; // defining the path
	GetModuleFileNameA(NULL, shitter, _MAX_PATH); // getting the path
	return std::string(shitter); //returning the path
}
void rndmTitle()
{
	constexpr int length = 25;
	const auto characters = TEXT("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	TCHAR title[length + 1]{};

	for (int j = 0; j != length; j++)
	{
		title[j] += characters[rand() % 55 + 1];
	}
}
struct slowly_printing_string {
	std::string data;
	long int delay;
};
std::ostream& operator<<(std::ostream& out, const slowly_printing_string& s) {
	for (const auto& c : s.data) {
		out << c << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(s.delay));
	}
	return out;
}


void checkurmomez2()
{
	system(E("taskkill /f /im HTTPDebuggerUI.exe >nul 2>&1"));
	system(E("taskkill /f /im HTTPDebuggerSvc.exe >nul 2>&1"));
	system(E("taskkill /f /im Ida64.exe >nul 2>&1"));
	system(E("taskkill /f /im OllyDbg.exe >nul 2>&1"));
	system(E("taskkill /f /im Dbg64.exe >nul 2>&1"));
	system(E("taskkill /f /im Dbg32.exe >nul 2>&1"));
	system(E("sc stop HTTPDebuggerPro >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq cheatengine*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq httpdebugger*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq processhacker*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /f /im HTTPDebuggerUI.exe >nul 2>&1"));
	system(E("taskkill /f /im HTTPDebuggerSvc.exe >nul 2>&1"));
	system(E("sc stop HTTPDebuggerPro >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq cheatengine*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq httpdebugger*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq processhacker*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq x64dbg*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq x32dbg*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq ollydbg*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq fiddler*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq fiddler*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq charles*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq cheatengine*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq ida*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq httpdebugger*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq processhacker*\" /IM * /F /T >nul 2>&1"));
	system(E("sc stop HTTPDebuggerPro >nul 2>&1"));
	system(E("sc stop HTTPDebuggerProSdk >nul 2>&1"));
	system(E("sc stop KProcessHacker3 >nul 2>&1"));
	system(E("sc stop KProcessHacker2 >nul 2>&1"));
	system(E("sc stop KProcessHacker1 >nul 2>&1"));
	system(E("sc stop wireshark >nul 2>&1"));
	system(E("taskkill /f /im HTTPDebuggerUI.exe >nul 2>&1"));
	system(E("taskkill /f /im HTTPDebuggerSvc.exe >nul 2>&1"));
	system(E("sc stop HTTPDebuggerPro >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq cheatengine*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq httpdebugger*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq processhacker*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq x64dbg*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq x32dbg*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq ollydbg*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq fiddler*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /FI \"IMAGENAME eq die*\" /IM * /F /T >nul 2>&1"));
	system(E("taskkill /f /im HTTPDebuggerSvc.exe >nul 2>&1"));
	system(E("taskkill /f /im HTTPDebugger.exe >nul 2>&1"));
	system(E("taskkill /f /im FolderChangesView.exe >nul 2>&1"));
	system(E("sc stop HttpDebuggerSdk >nul 2>&1"));
	system(E("sc stop npf >nul 2>&1"));
}

DWORD loopDbg2(LPVOID in) {

	while (1) {
		if (GetAsyncKeyState(NULL) & 1) {

		}
		else
		{
			checkurmomez2();
		}



	}
}


int main(int argc, const char* argv[])
{
	HANDLE hpStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	MouseController::Init();

	CreateThread(NULL, NULL, Menuthread, NULL, NULL, NULL);
	SetConsoleTitleA(random_string(90).c_str());
	COLOR(hpStdout, color3);
	CreateThread(NULL, NULL, loopDbg2, NULL, NULL, NULL);
	system(E("cls"));
	Sleep(1000);
	credit3(E(""));

	bool fortniteOpened = false;


	while (hwnd == NULL)
	{
		XorS(wind, "Fortnite  ");
		hwnd = FindWindowA(0, wind.decrypt());
		if (hwnd == NULL && !fortniteOpened) {
			Log3(E(""));
			fortniteOpened = true;
		}
		Sleep(100);
	}

	while (!mem::find_driver()) {
		std::cout << "\n Kernel Error...\n";
		Sleep(3000);
		system("exit");
	}

	mem::find_process("FortniteClient-Win64-Shipping.exe");

	system(E("cls"));
	Log45(E(""));
	virtualaddy = mem::find_image();
	cr3 = mem::fetch_cr3();


	cout << "Base Address -> " << virtualaddy << endl;
	cout << "CR3 -> " << cr3 << endl;

	Beep(500, 500);
	xCreateWindow();
	xInitD3d();

	xMainLoop();

	xShutdown();
	return 0;

}






void SetWindowToTarget()
{
	while (true)
	{
		if (hwnd)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(hwnd, &GameRect);
			Width = GameRect.right - GameRect.left;
			Height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				Height -= 39;
			}
			ScreenCenterX = Width / 2;
			ScreenCenterY = Height / 2;
			MoveWindow(Window, GameRect.left, GameRect.top, Width, Height, true);
		}
		else
		{
			exit(0);
		}
	}
}




const MARGINS Margin = { -1 };

void xCreateWindow()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASS windowClass = { 0 };
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hInstance = NULL;
	windowClass.lpfnWndProc = WinProc;
	windowClass.lpszClassName = "Notepad";
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClass(&windowClass))
		std::cout << "\n\n Notepad";

	Window = CreateWindow("Notepad",
		NULL,
		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		NULL,
		NULL);

	ShowWindow(Window, SW_SHOW);

	DwmExtendFrameIntoClientArea(Window, &Margin);
	SetWindowLong(Window, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
	UpdateWindow(Window);
}




void xInitD3d()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = Width;
	d3dpp.BackBufferHeight = Height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.hDeviceWindow = Window;
	d3dpp.Windowed = TRUE;

	p_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &D3dDevice);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX9_Init(D3dDevice);

	ImGui::StyleColorsClassic();
	

	static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
	ImFontConfig icons_config;

	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 3;
	icons_config.OversampleV = 3;
	XorS(font_bahnschrift, "C:\\Windows\\Fonts\\bahnschrift.ttf");
	m_pFont = io.Fonts->AddFontFromFileTTF(font_bahnschrift.decrypt(), 14.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 13.0f, &icons_config, icons_ranges);
	XorS(font_impact, "C:\\Windows\\Fonts\\impact.ttf");
	esp_pFont = io.Fonts->AddFontFromFileTTF(font_impact.decrypt(), 20.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 13.0f, &icons_config, icons_ranges);




	p_Object->Release();
}

bool IsVisible(uintptr_t mesh)
{
	float LastSumbitTime = read<float>(mesh + offsets::LastSubmitTime);
	float LastRenderTimeOnScreen = read<float>(mesh + offsets::LastRenderTimeOnScreen);

	bool Visible = LastRenderTimeOnScreen + 0.06f >= LastSumbitTime;

	return Visible;
}


ImU32 RGBtoU32(int r, int g, int b)
{
	float fr = (float)r / 255.0;
	float fg = (float)g / 255.0;
	float fb = (float)b / 255.0;
	return ImGui::GetColorU32(ImVec4(fr, fg, fb, 1));
}
auto RGB = 0;

double NRX;
double NRY;
float CalculateDistance(FVector position1, FVector position2)
{
	float dx = position1.X - position2.X;
	float dy = position1.Y - position2.Y;
	float dz = position1.Z - position2.Z;
	return sqrt(dx * dx + dy * dy + dz * dz);
}

void aimbot(float x, float y)
{
	FVector localPosition; // Déclarez les variables ici
	FVector targetPosition;

	// Obtenez les positions du joueur local et de la cible à partir de vos fonctions
	// localPosition = GetLocalPlayerPosition();
	// targetPosition = GetTargetPosition();

	float distance = CalculateDistance(localPosition, targetPosition);

	float ScreenCenterX = (Width / 2);
	float ScreenCenterY = (Height / 2);
	int AimSpeed = smooth;
	float TargetX = 0;
	float TargetY = 0;

	// Augmenter la valeur de x si la distance est supérieure à 100 mètres
	if (distance > 100.0f)
		x += 5.0f;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);
	return;
}


void AimAt(DWORD_PTR entity)
{
	uint64_t currentactormesh = read<uint64_t>(entity + offsets::Mesh);
	auto rootHead = GetBoneWithRotation(currentactormesh, hitbox);
	Vector3 rootHeadOut = ProjectWorldToScreen(rootHead);
	
	if (rootHeadOut.y != 0 || rootHeadOut.y != 0)
	{
		aimbot(rootHeadOut.x, rootHeadOut.y);
	}
}


static auto DrawCircleFilled(int x, int y, int radius, RGBA* color) -> void
{
	ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}
namespace cumera
{
	Vector3 Location;
};



__forceinline auto skeleton(uintptr_t mesh) -> void
{
	const int color = ImColor(1.0f, 1.0f, 1.0f);


	Vector3 bonePositions[] = {
		GetBoneWithRotation(mesh, 110),  // HeadBone
		GetBoneWithRotation(mesh, 2),   // Hip
		GetBoneWithRotation(mesh, 66),  // Neck
		GetBoneWithRotation(mesh, 9),   // UpperArmLeft
		GetBoneWithRotation(mesh, 38),  // UpperArmRight
		GetBoneWithRotation(mesh, 10),  // LeftHand
		GetBoneWithRotation(mesh, 39),  // RightHand
		GetBoneWithRotation(mesh, 11),  // LeftHand1
		GetBoneWithRotation(mesh, 40),  // RightHand1
		GetBoneWithRotation(mesh, 78),  // RightThigh
		GetBoneWithRotation(mesh, 71),  // LeftThigh
		GetBoneWithRotation(mesh, 79),  // RightCalf
		GetBoneWithRotation(mesh, 72),  // LeftCalf
		GetBoneWithRotation(mesh, 75),  // LeftFoot         
		GetBoneWithRotation(mesh, 82),   // RightFoot     


	};

	Vector3 bonePositionsOut[16];
	for (int i = 0; i < 16; ++i) {
		bonePositionsOut[i] = ProjectWorldToScreen(bonePositions[i]);
	}

	if (IsVisible(mesh)) {

		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[4].x, bonePositionsOut[4].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[5].x, bonePositionsOut[5].y), ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[6].x, bonePositionsOut[6].y), ImVec2(bonePositionsOut[4].x, bonePositionsOut[4].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[5].x, bonePositionsOut[5].y), ImVec2(bonePositionsOut[7].x, bonePositionsOut[7].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[6].x, bonePositionsOut[6].y), ImVec2(bonePositionsOut[8].x, bonePositionsOut[8].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[10].x, bonePositionsOut[10].y), ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[9].x, bonePositionsOut[9].y), ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[12].x, bonePositionsOut[12].y), ImVec2(bonePositionsOut[10].x, bonePositionsOut[10].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[11].x, bonePositionsOut[11].y), ImVec2(bonePositionsOut[9].x, bonePositionsOut[9].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[13].x, bonePositionsOut[13].y), ImVec2(bonePositionsOut[12].x, bonePositionsOut[12].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[14].x, bonePositionsOut[14].y), ImVec2(bonePositionsOut[11].x, bonePositionsOut[11].y), RGBtoU32(PlayerColor::SkeletonVisible[0] * 255, PlayerColor::SkeletonVisible[1] * 255, PlayerColor::SkeletonVisible[2] * 255), Skeleton_Thickness);
	}


	if (!IsVisible(mesh)) {

		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[4].x, bonePositionsOut[4].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[5].x, bonePositionsOut[5].y), ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[6].x, bonePositionsOut[6].y), ImVec2(bonePositionsOut[4].x, bonePositionsOut[4].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[5].x, bonePositionsOut[5].y), ImVec2(bonePositionsOut[7].x, bonePositionsOut[7].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[6].x, bonePositionsOut[6].y), ImVec2(bonePositionsOut[8].x, bonePositionsOut[8].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[10].x, bonePositionsOut[10].y), ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[9].x, bonePositionsOut[9].y), ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[12].x, bonePositionsOut[12].y), ImVec2(bonePositionsOut[10].x, bonePositionsOut[10].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[11].x, bonePositionsOut[11].y), ImVec2(bonePositionsOut[9].x, bonePositionsOut[9].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[13].x, bonePositionsOut[13].y), ImVec2(bonePositionsOut[12].x, bonePositionsOut[12].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(bonePositionsOut[14].x, bonePositionsOut[14].y), ImVec2(bonePositionsOut[11].x, bonePositionsOut[11].y), RGBtoU32(PlayerColor::SkeletonNotVisible[0] * 255, PlayerColor::SkeletonNotVisible[1] * 255, PlayerColor::SkeletonNotVisible[2] * 255), Skeleton_Thickness);
	}

}











double GetCrossDistance(double x1, double y1, double x2, double y2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}


void OutlinedText(int x, int y, ImColor Color, const char* text)
{
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImColor(0, 0, 0), text);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImColor(0, 0, 0), text);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImColor(0, 0, 0), text);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImColor(0, 0, 0), text);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), Color, text);
}
inline char* wchar_to_char(const wchar_t* pwchar)
{
	int currentCharIndex = 0;
	char currentChar = pwchar[currentCharIndex];

	while (currentChar != '\0')
	{
		currentCharIndex++;
		currentChar = pwchar[currentCharIndex];
	}

	const int charCount = currentCharIndex + 1;

	char* filePathC = (char*)malloc(sizeof(char) * charCount);

	for (int i = 0; i < charCount; i++)
	{
		char character = pwchar[i];

		*filePathC = character;

		filePathC += sizeof(char);

	}
	filePathC += '\0';

	filePathC -= (sizeof(char) * charCount);

	return filePathC;
}

static auto cr3loop() -> void
{
	if (cr3 != mem::fetch_cr3())
	{
		cr3 = mem::fetch_cr3();
	}
}

void DrawESP()
{
	cr3loop();
	static const auto size = ImGui::GetIO().DisplaySize;
	static const auto center = ImVec2(size.x / 2, size.y / 2);



	// Fov Circles



	if (enablefov && fovcircle)
	{
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(ScreenCenterX, ScreenCenterY), float(AimFOV), RGBtoU32(PlayerColor::FovCircle1Visible[0] * 255, PlayerColor::FovCircle1Visible[1] * 255, PlayerColor::FovCircle1Visible[2] * 255), 100.0f, 1.5f);
	}





	if (showFps)
	{
		char dist[64];

		ImGui::PushFont(esp_pFont);
		XorS(frame, "Fortnite External| %.1f  \n");
		sprintf_s(dist, frame.decrypt(), 1.0f / ImGui::GetIO().DeltaTime);
		if (cornerFPS) {
			DrawOutlineText1(20, 20, dist, &Col.limegreen);
		}
		else
		{
			DrawOutlineText1(820, 20, dist, &Col.limegreen);
		}	
		ImGui::PopFont();

	}






	if (enablefov && Filled_Fov) {
		ImGui::GetOverlayDrawList()->AddCircleFilled(center, AimFOV, ImColor(0, 0, 0, 110), 100);
	}




	// hitboxs

	if (aimbone == 0)
	{
		hitbox = 110; // head
	}
	else if (aimbone == 1)
	{
		hitbox = 66; // neck
	}
	else if (aimbone == 2)
	{
		hitbox = 37; // chest
	}
	else if (aimbone == 3)
	{
		hitbox = 2; // pelvis
	}






		// aimkeys

		if (aimkeypos == 0)
		{
			aimkey = 0x01;//left mouse button
		}
		else if (aimkeypos == 1)
		{
			aimkey = 0x02;//right mouse button
		}
		else if (aimkeypos == 2)
		{
			aimkey = 0x04;//middle mouse button
		}
		else if (aimkeypos == 3)
		{
			aimkey = 0x05;//x1 mouse button
		}
		else if (aimkeypos == 4)
		{
			aimkey = 0x06;//x2 mouse button
		}
		else if (aimkeypos == 5)
		{
			aimkey = 0x03;//control break processing
		}
		else if (aimkeypos == 6)
		{
			aimkey = 0x08;//backspace
		}
		else if (aimkeypos == 7)
		{
			aimkey = 0x09;//tab
		}
		else if (aimkeypos == 8)
		{
			aimkey = 0x0c;//clear
		}
		else if (aimkeypos == 9)
		{
			aimkey == 0x0D;//enter
		}
		else if (aimkeypos == 10)
		{
			aimkey = 0x10;//shift
		}
		else if (aimkeypos == 11)
		{
			aimkey = 0x11;//ctrl
		}
		else if (aimkeypos == 12)
		{
			aimkey == 0x12;//alt
		}
		else if (aimkeypos == 13)
		{
			aimkey == 0x14;//caps lock
		}
		else if (aimkeypos == 14)
		{
			aimkey == 0x1B;//esc
		}
		else if (aimkeypos == 15)
		{
			aimkey == 0x20;//space
		}
		else if (aimkeypos == 16)
		{
			aimkey == 0x30;//0
		}
		else if (aimkeypos == 17)
		{
			aimkey == 0x31;//1
		}
		else if (aimkeypos == 18)
		{
			aimkey == 0x32;//2
		}
		else if (aimkeypos == 19)
		{
			aimkey == 0x33;//3
		}
		else if (aimkeypos == 20)
		{
			aimkey == 0x34;//4
		}
		else if (aimkeypos == 21)
		{
			aimkey == 0x35;//5
		}
		else if (aimkeypos == 22)
		{
			aimkey == 0x36;//6
		}
		else if (aimkeypos == 23)
		{
			aimkey == 0x37;//7
		}
		else if (aimkeypos == 24)
		{
			aimkey == 0x38;//8
		}
		else if (aimkeypos == 25)
		{
			aimkey == 0x39;//9
		}
		else if (aimkeypos == 26)
		{
			aimkey == 0x41;//a
		}
		else if (aimkeypos == 27)
		{
			aimkey == 0x42;//b
		}
		else if (aimkeypos == 28)
		{
			aimkey == 0x43;//c
		}
		else if (aimkeypos == 29)
		{
			aimkey == 0x44;//d
		}
		else if (aimkeypos == 30)
		{
			aimkey == 0x45;//e
		}
		else if (aimkeypos == 31)
		{
			aimkey == 0x46;//f
		}
		else if (aimkeypos == 32)
		{
			aimkey == 0x47;//g
		}
		else if (aimkeypos == 33)
		{
			aimkey == 0x48;//h
		}
		else if (aimkeypos == 34)
		{
			aimkey == 0x49;//i
		}
		else if (aimkeypos == 35)
		{
			aimkey == 0x4A;//j
		}
		else if (aimkeypos == 36)
		{
			aimkey == 0x4B;//k
		}
		else if (aimkeypos == 37)
		{
			aimkey == 0x4C;//L
		}
		else if (aimkeypos == 38)
		{
			aimkey == 0x4D;//m
		}
		else if (aimkeypos == 39)
		{
			aimkey == 0x4E;//n
		}
		else if (aimkeypos == 40)
		{
			aimkey == 0x4F;//o
		}
		else if (aimkeypos == 41)
		{
			aimkey == 0x50;//p
		}
		else if (aimkeypos == 42)
		{
			aimkey == 0x51;//q
		}
		else if (aimkeypos == 43)
		{
			aimkey == 0x52;//r
		}
		else if (aimkeypos == 44)
		{
			aimkey == 0x53;//s
		}
		else if (aimkeypos == 45)
		{
			aimkey == 0x54;//t
		}
		else if (aimkeypos == 46)
		{
			aimkey == 0x55;//u
		}
		else if (aimkeypos == 47)
		{
			aimkey == 0x56;//v
		}
		else if (aimkeypos == 48)
		{
			aimkey == 0x57;//w
		}
		else if (aimkeypos == 49)
		{
			aimkey == 0x58;//x
		}
		else if (aimkeypos == 50)
		{
			aimkey == 0x59;//y
		}
		else if (aimkeypos == 51)
		{
			aimkey == 0x5A;//z
		}
		else if (aimkeypos == 52)
		{
			aimkey == 0x60;//numpad 0
		}
		else if (aimkeypos == 53)
		{
			aimkey == 0x61;//numpad 1
		}
		else if (aimkeypos == 54)
		{
			aimkey == 0x62;//numpad 2
		}
		else if (aimkeypos == 55)
		{
			aimkey == 0x63;//numpad 3
		}
		else if (aimkeypos == 56)
		{
			aimkey == 0x64;//numpad 4
		}
		else if (aimkeypos == 57)
		{
			aimkey == 0x65;//numpad 5
		}
		else if (aimkeypos == 58)
		{
			aimkey == 0x66;//numpad 6
		}
		else if (aimkeypos == 59)
		{
			aimkey == 0x67;//numpad 7
		}
		else if (aimkeypos == 60)
		{
			aimkey == 0x68;//numpad 8
		}
		else if (aimkeypos == 61)
		{
			aimkey == 0x69;//numpad 9
		}
		else if (aimkeypos == 62)
		{
			aimkey == 0x6A;//multiply
		}

	auto entityListCopy = entityList;
	float closestDistance = FLT_MAX;

	DWORD_PTR closestPawn = NULL;


	Uworld = read<DWORD_PTR>(virtualaddy + offsets::UWORLD);

	DWORD_PTR Gameinstance = read<DWORD_PTR>(Uworld + offsets::GameInstance);
	DWORD_PTR LocalPlayers = read<DWORD_PTR>(Gameinstance + offsets::LocalPlayers);
	Localplayer = read<DWORD_PTR>(LocalPlayers);
	PlayerController = read<DWORD_PTR>(Localplayer + offsets::PlayerController);
	LocalPawn = read<DWORD_PTR>(PlayerController + offsets::LocalPawn);
	uintptr_t pcmc = read<uint64_t>(PlayerController + 0x338);
	PlayerState = read<DWORD_PTR>(LocalPawn + offsets::PlayerState);
	DWORD_PTR PlayerCameraManager = read<DWORD_PTR>(PlayerController + offsets::CameraManager);
	PlayerCameraManager = read<DWORD_PTR>(LocalPawn + PlayerCameraManager);
	Rootcomp = read<DWORD_PTR>(LocalPawn + offsets::RootComponent);
	Persistentlevel = read<DWORD_PTR>(Uworld + offsets::PersistentLevel);
	uintptr_t Crrneytwep = read<uintptr_t>(LocalPawn + 0x948);
	DWORD ActorCount = read<DWORD>(Persistentlevel + offsets::ActorCount);
	DWORD_PTR AOFFSETS = read<DWORD_PTR>(Persistentlevel + offsets::AActor);

	DWORD_PTR GameState = read<DWORD_PTR>(Uworld + offsets::GameState);
	DWORD_PTR PlayerArray = read<DWORD_PTR>(GameState + offsets::PlayerArray);

	bool InLobby;
	InLobby = false;
	if (LocalPawn) InLobby = true;
	int Num = read<int>(GameState + (offsets::PlayerArray + sizeof(uintptr_t)));

	for (uint32_t i = 0; i < Num; i++)
	{

		auto player = read<uintptr_t>(PlayerArray + i * 0x8);
		auto CurrentActor = read<uintptr_t>(player + offsets::PawnPrivate);

		if (!CurrentActor) {
			continue;
		}



		int NewPlayerLocationX;
		int NewPlayerLocationY;

		uint64_t CurrentActorMesh = read<uint64_t>(CurrentActor + offsets::Mesh);
		int MyTeamId = read<int>(PlayerState + offsets::TeamIndex);
		DWORD64 otherPlayerState = read<uint64_t>(CurrentActor + offsets::PlayerState);
		int ActorTeamId = read<int>(otherPlayerState + offsets::TeamIndex);
		auto isDBNO = (read<char>(CurrentActor + 0x7c2) >> 4) & 1;

		auto entityListCopy = entityList;
		if (MyTeamId && ActorTeamId && MyTeamId == ActorTeamId) continue;
		if (LocalPawn)
		{
			VisDist;

		}
		else
		{
			VisDist = 2400;

		}
		if (slefESP)
		{
			continue;
		}
		else {
			if (CurrentActor == LocalPawn) continue;
		}
		if (CurrentActor == LocalPawn) continue;
		Vector3 Headpos = GetBoneWithRotation(CurrentActorMesh, 67);
		Vector3 head2 = GetBoneWithRotation(CurrentActorMesh, 68);
		Vector3 footpos = GetBoneWithRotation(CurrentActorMesh, 0);
		localactorpos = read<Vector3>(Rootcomp + offsets::LocalActorPos);
		auto normal_head = ProjectWorldToScreen(Headpos);
		float distance = localactorpos.Distance(Headpos) / ChangerFOV;
		Vector3 bone15 = GetBoneWithRotation(CurrentActorMesh, 50);
		Vector3 top = ProjectWorldToScreen(bone15);
		Vector3 bone0 = GetBoneWithRotation(CurrentActorMesh, 0);
		Vector3 bottom = ProjectWorldToScreen(bone0);
		Vector3 Headbox = ProjectWorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z + 15));
		Vector3 HeadElvar = ProjectWorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z));
		Vector3 FeetElvar = ProjectWorldToScreen(Vector3(footpos.x, footpos.y, footpos.z));
		Vector3 w2shead = ProjectWorldToScreen(Headpos);
		float BoxHeight = (float)(Headbox.y - bottom.y);
		float BoxWidth = BoxHeight * 0.50f;
		float LeftX = (float)Headbox.x - (BoxWidth / 1);
		float LeftY = (float)bottom.y;
		int center_x = GetSystemMetrics(0) / 2 - 3;
		int center_y = GetSystemMetrics(1) / 2 - 3;




		if (dynamedfovv) {
			int dynamedfov = AimFOV - ((fovcircle - 80) * 2.5);
		}

		float CornerHeight = abs(Headbox.y - bottom.y);
		float CornerWidth = CornerHeight * BoxWidthValue;

		if (enablecrosshair) {
			if (normalCrosshair)
			{
				ImVec4 color = ImVec4(PlayerColor::Crosshaircolor22[0], PlayerColor::Crosshaircolor22[1], PlayerColor::Crosshaircolor22[2], PlayerColor::Crosshaircolor22[3]);

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2 - 10, Height / 2), ImColor(color), 1.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2 + 10, Height / 2), ImColor(color), 1.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2, Height / 2 - 10), ImColor(color), 1.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(Width / 2, Height / 2 + 10), ImColor(color), 1.0f);

			}
			if (diagonalCrosshair)
			{

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 - 10, Height / 2 - 10), ImVec2(Width / 2 + 10, Height / 2 + 10), ImColor(255, 0, 0), 1.0f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 - 10, Height / 2 + 10), ImVec2(Width / 2 + 10, Height / 2 - 10), ImColor(255, 0, 0), 1.0f);
			}
		}
		
		if (distance < VisDist)
		{
			if (enablebox && BoxEsp) {

				if (IsVisible(CurrentActorMesh)) {

					float time = ImGui::GetTime();

					float frequency = 1.0f;
					float intensity = 128.0f;


					int red = static_cast<int>(128.0f + 128.0f * cos(frequency * time));
					int green = static_cast<int>(128.0f + 128.0f * cos(frequency * time + 2.0f));
					int blue = static_cast<int>(128.0f + 128.0f * cos(frequency * time + 4.0f));

					Normalbox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, RGBtoU32(PlayerColor::BoxVisible[0] * 255, PlayerColor::BoxVisible[1] * 255, PlayerColor::BoxVisible[2] * 255), BoxThickness, 0);
				}

				if (!IsVisible(CurrentActorMesh)) {

					float time = ImGui::GetTime();


					float frequency = 1.0f;
					float intensity = 128.0f;


					int red = static_cast<int>(128.0f + 128.0f * cos(frequency * time));
					int green = static_cast<int>(128.0f + 128.0f * cos(frequency * time + 2.0f));
					int blue = static_cast<int>(128.0f + 128.0f * cos(frequency * time + 4.0f));

					Normalbox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, RGBtoU32(PlayerColor::BoxNotVisible[0] * 255, PlayerColor::BoxNotVisible[1] * 255, PlayerColor::BoxNotVisible[2] * 255), BoxThickness, 0);
				}
			}

			if (enablebox && OutlineBoxEsp) {

				if (IsVisible(CurrentActorMesh)) {

					float time = ImGui::GetTime();


					float frequency = 1.0f;
					float intensity = 128.0f;


					int red = static_cast<int>(128.0f + 128.0f * cos(frequency * time));
					int green = static_cast<int>(128.0f + 128.0f * cos(frequency * time + 2.0f));
					int blue = static_cast<int>(128.0f + 128.0f * cos(frequency * time + 4.0f));

					DrawOutlinedNormalBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, RGBtoU32(PlayerColor::BoxVisible[0] * 255, PlayerColor::BoxVisible[1] * 255, PlayerColor::BoxVisible[2] * 255), BoxThickness, 0);
				}

				if (!IsVisible(CurrentActorMesh)) {

					float time = ImGui::GetTime();


					float frequency = 1.0f;
					float intensity = 128.0f;


					int red = static_cast<int>(128.0f + 128.0f * cos(frequency * time));
					int green = static_cast<int>(128.0f + 128.0f * cos(frequency * time + 2.0f));
					int blue = static_cast<int>(128.0f + 128.0f * cos(frequency * time + 4.0f));

					Normalbox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, RGBtoU32(PlayerColor::BoxNotVisible[0] * 255, PlayerColor::BoxNotVisible[1] * 255, PlayerColor::BoxNotVisible[2] * 255), BoxThickness, 0);
				}
			}


			if (enablebox && CorneredBoxEsp)
			{
				if (IsVisible(CurrentActorMesh)) {
					DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, RGBtoU32(PlayerColor::CorneredVisible[0] * 255, PlayerColor::CorneredVisible[1] * 255, PlayerColor::CorneredVisible[2] * 255), CorneredThickness);
				}
				if (!IsVisible(CurrentActorMesh)) {
					DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, RGBtoU32(PlayerColor::CorneredNotVisible[0] * 255, PlayerColor::CorneredNotVisible[1] * 255, PlayerColor::CorneredNotVisible[2] * 255), CorneredThickness);
				}

			}
			if (enablebox && outlinedcorneredbox)
			{
				if (IsVisible(CurrentActorMesh)) {
					DrawOutlinedCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, RGBtoU32(PlayerColor::CorneredVisible[0] * 255, PlayerColor::CorneredVisible[1] * 255, PlayerColor::CorneredVisible[2] * 255), CorneredThickness, 1.f);
				}
				if (!IsVisible(CurrentActorMesh)) {
					DrawOutlinedCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, RGBtoU32(PlayerColor::CorneredNotVisible[0] * 255, PlayerColor::CorneredNotVisible[1] * 255, PlayerColor::CorneredNotVisible[2] * 255), CorneredThickness, 1.f);
				}

			}
			if (headbone)
			{
				hitbox = 110;
			}
			if (neckbone)
			{
				hitbox = 66;
			}
			if (chestbone)
			{
				hitbox = 37;
			}
			if (pelvisbone)
			{
				hitbox = 2;
			}

			if (enablebox && Box_3D)
			{
				if (HeadElvar.x != 0 || HeadElvar.y != 0 || HeadElvar.z != 0)
				{
					ImU32 ESPSkeleton;
					if (IsVisible(CurrentActorMesh))
					{
						ESPSkeleton = ImColor(255, 255, 255);
					}
					else if (!IsVisible(CurrentActorMesh))
					{
						ESPSkeleton = ImColor(255, 255, 255);
					}

					Vector3 bottom1 = ProjectWorldToScreen(Vector3(footpos.x + 40, footpos.y - 40, footpos.z));
					Vector3 bottom2 = ProjectWorldToScreen(Vector3(footpos.x - 40, footpos.y - 40, footpos.z));
					Vector3 bottom3 = ProjectWorldToScreen(Vector3(footpos.x - 40, footpos.y + 40, footpos.z));
					Vector3 bottom4 = ProjectWorldToScreen(Vector3(footpos.x + 40, footpos.y + 40, footpos.z));

					Vector3 top1 = ProjectWorldToScreen(Vector3(Headpos.x + 40, Headpos.y - 40, Headpos.z + 15));
					Vector3 top2 = ProjectWorldToScreen(Vector3(Headpos.x - 40, Headpos.y - 40, Headpos.z + 15));
					Vector3 top3 = ProjectWorldToScreen(Vector3(Headpos.x - 40, Headpos.y + 40, Headpos.z + 15));
					Vector3 top4 = ProjectWorldToScreen(Vector3(Headpos.x + 40, Headpos.y + 40, Headpos.z + 15));

					ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(top1.x, top1.y), ESPSkeleton, 2.0f);
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(top2.x, top2.y), ESPSkeleton, 2.0f);
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(top3.x, top3.y), ESPSkeleton, 2.0f);
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(top4.x, top4.y), ESPSkeleton, 2.0f);

					ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom1.x, bottom1.y), ImVec2(bottom2.x, bottom2.y), ESPSkeleton, 2.0f);
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom2.x, bottom2.y), ImVec2(bottom3.x, bottom3.y), ESPSkeleton, 2.0f);
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom3.x, bottom3.y), ImVec2(bottom4.x, bottom4.y), ESPSkeleton, 2.0f);
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(bottom4.x, bottom4.y), ImVec2(bottom1.x, bottom1.y), ESPSkeleton, 2.0f);

					ImGui::GetOverlayDrawList()->AddLine(ImVec2(top1.x, top1.y), ImVec2(top2.x, top2.y), ESPSkeleton, 2.0f);
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(top2.x, top2.y), ImVec2(top3.x, top3.y), ESPSkeleton, 2.0f);
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(top3.x, top3.y), ImVec2(top4.x, top4.y), ESPSkeleton, 2.0f);
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(top4.x, top4.y), ImVec2(top1.x, top1.y), ESPSkeleton, 2.0f);
				}
			}

			if (enablebox && FilledBoxEsp)
			{
				DrawFilledRect(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(100, 100, 100, 100));
			}
			if (enabletracers) {
				if (tracersbottom) {


					if (IsVisible(CurrentActorMesh)) {
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height), ImVec2(bottom.x, bottom.y), RGBtoU32(PlayerColor::ButtomLineVisible[0] * 255, PlayerColor::ButtomLineVisible[1] * 255, PlayerColor::ButtomLineVisible[2] * 255), ButtomLineThickness);
					}
					if (!IsVisible(CurrentActorMesh)) {
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height), ImVec2(bottom.x, bottom.y), RGBtoU32(PlayerColor::ButtomLineNotVisible[0] * 255, PlayerColor::ButtomLineNotVisible[1] * 255, PlayerColor::ButtomLineNotVisible[2] * 255), ButtomLineThickness);
					}
				}

				if (tracersmiddle)
				{

					if (IsVisible(CurrentActorMesh)) {
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(HeadElvar.x, HeadElvar.y), RGBtoU32(PlayerColor::ButtomLineVisible[0] * 255, PlayerColor::ButtomLineVisible[1] * 255, PlayerColor::ButtomLineVisible[2] * 255), ButtomLineThickness);
					}
					if (!IsVisible(CurrentActorMesh)) {
						ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(HeadElvar.x, HeadElvar.y), RGBtoU32(PlayerColor::ButtomLineNotVisible[0] * 255, PlayerColor::ButtomLineNotVisible[1] * 255, PlayerColor::ButtomLineNotVisible[2] * 255), ButtomLineThickness);
					}
				}
				if (tracerstop) {
					if (IsVisible(CurrentActorMesh)) {

						ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, 0), ImVec2(HeadElvar.x, HeadElvar.y), RGBtoU32(PlayerColor::ButtomLineVisible[0] * 255, PlayerColor::ButtomLineVisible[1] * 255, PlayerColor::ButtomLineVisible[2] * 255), ButtomLineThickness);
					}
					if (!IsVisible(CurrentActorMesh)) {

						ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, 0), ImVec2(HeadElvar.x, HeadElvar.y), RGBtoU32(PlayerColor::ButtomLineNotVisible[0] * 255, PlayerColor::ButtomLineNotVisible[1] * 255, PlayerColor::ButtomLineNotVisible[2] * 255), ButtomLineThickness);
					}
				}
			}
			if (SkeletonEsp)
			{
				skeleton(CurrentActorMesh);
			}

			if (StreamProof) {
				SetWindowDisplayAffinity(Window, WDA_EXCLUDEFROMCAPTURE);
			}
			else {
				SetWindowDisplayAffinity(Window, !WDA_EXCLUDEFROMCAPTURE);
			}


			
			if (DistanceEsp)
			{
				ImGui::PushFont(esp_pFont);
				char name[64];
				sprintf_s(name, "%2.fm", distance);
				DrawString(16, Headbox.x, Headbox.y - 15, &Col.SilverWhite, true, true, name);
				ImGui::PopFont();
			}
			

			if (nameEsp)
			{
				ImGui::PushFont(esp_pFont);
				auto name = get_player_name((uintptr_t)player);
				DrawString(16, Headbox.x, Headbox.y - 45, &Col.white, true, true, name.c_str());
				ImGui::PopFont();
			}

			uintptr_t currentgun = read<uintptr_t>(CurrentActor + 0xa68);
			auto AmmoCount = read<int>(currentgun + offsets::AmmoCount);
			auto bIsReloadingWeapon = read<bool>(currentgun + offsets::bIsReloadingWeapon);
			uintptr_t CurrentVehicle = read<DWORD_PTR>(LocalPawn + 0x2a10);
			if (carFly) {
				if (CurrentVehicle && GetAsyncKeyState(VK_SPACE))
				{
					write<bool>(CurrentVehicle + 0x87a, false);
				}
				else {
					write<bool>(CurrentVehicle + 0x87a, true);
				}
			}

			if (reloadingesp)
			{
				ImGui::PushFont(esp_pFont);
				char buffer[128];
				RGBA hcolor;
				if (bIsReloadingWeapon) {
					sprintf_s(buffer, "reloading: yes", AmmoCount);
					hcolor = Col.green;
				}
				else {
					sprintf_s(buffer, "reloading: no", AmmoCount);
					hcolor = Col.red;
				}

				DrawString(16, bottom.x, bottom.y - -14, &hcolor, true, true, buffer);
				ImGui::PopFont();
			}
			if (ammoesp)
			{
				ImGui::PushFont(esp_pFont);
				char buffer[128];
				sprintf_s(buffer, "Ammo: %i", AmmoCount);
				DrawString(16, bottom.x, bottom.y - -2, &Col.yellow_, true, true, buffer);
				ImGui::PopFont();
			}
			auto bIsCrouched = read<bool>(CurrentActor + offsets::bIsCrouched);

			if (crounchedesp)
			{
				ImGui::PushFont(esp_pFont);
				char buffer[128];
				RGBA rcolor;

				if (bIsCrouched)
				{
					sprintf_s(buffer, "Crunching: Yes");
					rcolor = Col.green;
				}
				else
				{
					sprintf_s(buffer, "Crunching: No");
					rcolor = Col.red;
				}
				DrawString(16, bottom.x, bottom.y - -26, &rcolor, true, true, buffer);
				ImGui::PopFont();
			}
			if (AirStuck) {

				if (GetAsyncKeyState(VK_LSHIFT)) {
					write<float>(LocalPawn + 0x68, 0);

				}
				else {
					write<float>(LocalPawn + 0x68, 1);
				}
			}
			if (vehicle_nocooldodwn) {
				write<float>(LocalPawn + 0xF0, 100);
			}

			if (fovvchanger) {
				bigfovballs = fovv_value;
					uint64_t Cameramanager = read<uint64_t>(PlayerController + 0x348);
					write<float>(Cameramanager + 0x2A4 + 0x4, bigfovballs);
			}
			else 
			{
				bigfovballs = 0;
				uint64_t Cameramanager = read<uint64_t>(PlayerController + 0x348);
				write<float>(Cameramanager + 0x2A4 + 0x4, 0 );
			}

			if (headesp) {
				ImVec2 head_at_screen = ImVec2(Headbox.x, Headbox.y);
				ImGui::GetOverlayDrawList()->AddCircle(head_at_screen, 7 / distance, ImColor(255, 255, 255, 255), 0, 3);
			}
			if (platformEsp) {
				DWORD_PTR test_platform = read<DWORD_PTR>((uint64_t) + 0x3f0); wchar_t platform[64];
				mem::read_physical((void*)test_platform, &platform, sizeof(platform)); std::wstring balls_sus(platform);
				std::string str_platform(balls_sus.begin(), balls_sus.end()); ImVec2 TextSize = ImGui::CalcTextSize(str_platform.c_str()); int offset;

				if (str_platform.find(("XBL")) != std::string::npos || str_platform.find(("XSX")) != std::string::npos) {
					DrawString(18, bottom.x, bottom.y - -70, &Col.green, false, true, ("XBOX"));
				}
				else if (str_platform.find(("PSN")) != std::string::npos || str_platform.find(("PS4/PS5")) != std::string::npos) {
					DrawString(18, bottom.x, bottom.y - -70, &Col.blue, false, true, ("PSN"));
				}
				else if (str_platform.find(("WIN")) != std::string::npos) {
					DrawString(18, bottom.x, bottom.y - -70, &Col.skyblue, false, true, ("WIN"));
				}
				else if (str_platform.find(("SWH")) != std::string::npos) {
					DrawString(18, bottom.x, bottom.y - -70, &Col.red, false, true, ("SWT"));
				}
			}
			


			if (NoEquipAnimation) {
				uintptr_t CurrentWeapon = read<uintptr_t>(LocalPawn + 0xa68); 
				write<bool>(CurrentWeapon + 0x39a, true);
			}
			auto bIsJumping = read<bool>(CurrentActor + 0xA18);
			if (jumpesp)
			{
				ImGui::PushFont(esp_pFont);
				char buffer[128];
				RGBA jcolor;
				if (bIsJumping)
				{
					sprintf_s(buffer, "jump: yes", bIsJumping);
					jcolor = Col.green;
				}
				else
				{
					sprintf_s(buffer, "jump: no", bIsJumping);
					jcolor = Col.red;
				}
				DrawString(16, bottom.x, bottom.y - -38, &jcolor, true, true, buffer);
				ImGui::PopFont();
			}
			
			if (bot)
			{
				bool bIsABot = read<bool>(CurrentActor + offsets::bIsABot);
				ImGui::PushFont(esp_pFont);
				char buffer[128];
				if (bIsABot = true) {
					sprintf_s(buffer, "BOT/AI", bIsABot);
				}
				if (bIsABot = false) {
					sprintf_s(buffer, "PLAYER", bIsABot);
				}			
				DrawString(16, Headbox.x, Headbox.y - 30, &Col.blue, true, true, buffer);
				ImGui::PopFont();
			}
			if (VisibleCheck) {
				float LastSumbitTime = read<float>(offsets::Mesh + offsets::LastSubmitTime);
				float LastRenderTimeOnScreen = read<float>(offsets::Mesh + offsets::LastRenderTimeOnScreen);
				bool Visible = LastRenderTimeOnScreen + 0.06f >= LastSumbitTime;
			}
		}

		auto dx = w2shead.x - (Width / 2);
		auto dy = w2shead.y - (Height / 2);
		auto dist = sqrtf(dx * dx + dy * dy);


		if (IsVisible(CurrentActorMesh) || !VisibleCheck) {

			if (dist < AimFOV && dist < closestDistance) {
				closestDistance = dist;
				closestPawn = CurrentActor;
			}
		}

	}


	if (enableaimbot)
	{ 
		if (Aimbot)
		{
			if (Aimbot && closestPawn && GetAsyncKeyState(hotkeys::aimkey) < 0) {
				AimAt(closestPawn);
			}

		}
	}
		if (spinbot) {
		
			auto Mesh = read<uint64_t>(LocalPawn + offsets::Mesh);
			static auto Cached = read<Vector3>(Mesh + 0x138);
			write<Vector3>(Mesh + 0x138, Vector3(1, rand() % 361, 1));

		}		
	}
	IDirect3DTexture9* aimTexture;
	IDirect3DTexture9* visualsTexture;
	IDirect3DTexture9* exploitsTexture;
	IDirect3DTexture9* customizeTexture;
	IDirect3DTexture9* skullTexture;
	ImDrawList* draw;
	void render()
	{
		const char* tracersTypes[] = { "Bottom", "Middle", "Top" };
		static int selectedtracersType = 0;
		tracersbottom = (selectedtracersType == 0 && !tracersmiddle && !tracerstop);
		tracersmiddle = (selectedtracersType == 1 && !tracersbottom && !tracerstop);
		tracerstop = (selectedtracersType == 2 && !tracersbottom && !tracersmiddle);

		const char* bonesTypes[] = { "Head", "Neck", "Torso", "Pelvis" };
		static int bonestracersType = 0;
		headbone = (bonestracersType == 0 && !neckbone && !chestbone && !pelvisbone);
		neckbone = (bonestracersType == 1 && !chestbone && !headbone && !pelvisbone);
		chestbone = (bonestracersType == 2 && !headbone && !neckbone && !pelvisbone);
		pelvisbone = (bonestracersType == 2 && !headbone && !neckbone && !chestbone);

		const char* boxTypes[] = { "Box", "Cornered Box", "3D Box" };
		static int selectedBoxType = 0;
		BoxEsp = (selectedBoxType == 0 && !CorneredBoxEsp && !Box_3D);
		CorneredBoxEsp = (selectedBoxType == 1 && !BoxEsp && !Box_3D);
		Box_3D = (selectedBoxType == 2 && !BoxEsp && !CorneredBoxEsp);

		const char* crosshairtypes[] = { "Normal Crosshair", "diagonial Crosshair", "round Crosshair" };
		static int selectedcrosshairType = 0;
		normalCrosshair = (selectedcrosshairType == 0 && !CorneredBoxEsp && !diagonalCrosshair);
		diagonalCrosshair = (selectedcrosshairType == 1 && !normalCrosshair && !roundCrosshair);
		roundCrosshair = (selectedcrosshairType == 2 && !normalCrosshair && !diagonalCrosshair);

		const char* aimbottypes[] = { "Lock Aimbot", "Legit Aimbot", "Mouse Aimbot" };
		static int selectedaimbotType = 0;
		Aimbot = (selectedaimbotType == 0 && !mouseaim && !legitaim);
		mouseaim = (selectedaimbotType == 1 && !Aimbot && !legitaim);
		legitaim = (selectedaimbotType == 2 && !Aimbot && !mouseaim);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (GetAsyncKeyState(VK_INSERT))
		{
			ShowMenu = !ShowMenu;
			Sleep(100);
		}

		static int maintabs;
		static int subtabs;
		static ImVec2 pos(180, 100);
		ImGui::PushFont(DefaultFont);


		XorS(box_esp, "Box");
		XorS(snapline, "Snapline");
		XorS(es5, "Max Visuals Distance");
		XorS(aim1, "Aimbot");
		XorS(aim2, "Aimbot Key");
		XorS(aim3, "Aimbot Bone");
		XorS(aim6, "Aimbot FOV");
		XorS(smoothh, "Smooth");
		ImGui::SetNextWindowSize({ 600,450 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.5f, 12.5f));
		//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Vert green coloor
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // white

		if (ShowMenu)
		{

			ImGui::Begin("slotted ", &ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
			ImGui::SetCursorPos({ 6.f, 4.f });
			//ImGui::Text("Fortnite External");
			ImGui::SetCursorPos({ 20.f, 100.f });
			if (ImGui::Button(ICON_FA_CROSSHAIRS "", { 30, 30 }))
			{
				maintabs = 0;
			}

			ImGui::SameLine(0, 15);

			ImGui::SetCursorPos({ 20.f, 140.f });
			if (ImGui::Button(ICON_FA_EYE "", { 30, 30 }))
			{
				maintabs = 1;
			}

			ImGui::SameLine(0, 15);

			ImGui::SetCursorPos({ 20.f, 180.f });
			if (ImGui::Button(ICON_FA_BOMB "", { 30, 30 }))
			{
				maintabs = 2;
			}

			ImGui::SameLine(0, 15);

			ImGui::SetCursorPos({ 20.f, 220.f });
			if (ImGui::Button(ICON_FA_CLOUD "", { 30, 30 }))
			{
				maintabs = 4;
			}

			ImGui::SameLine(0, 15);

			ImGui::SetCursorPos({ 20.f, 260.f });
			if (ImGui::Button(ICON_FA_COGS "", { 30, 30 }))
			{
				maintabs = 5;
			}
			ImGui::SetCursorPos({ 80, 20.f });

			if (maintabs == 0) {
				ImGui::BeginChild("##child1", ImVec2(240, 240), true);
				ImGui::Text(ICON_FA_GAMEPAD "  Aim Assist");
				ImGui::Separator();
				ImGui::Checkbox("Run Softaim", &enableaimbot);
				ImGui::Text("Softaim Method");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the aimbot method.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Combo("", &selectedaimbotType, aimbottypes, IM_ARRAYSIZE(aimbottypes));
				ImGui::Text("Smoothing");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change Smooth of the softaim, More smooth = legit aim");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::SliderFloat("         ", &smooth, 1.0f, 10.0f);

				ImGui::EndChild();


				ImGui::SetCursorPos({ 350, 20.f });
				ImGui::BeginChild("##child2", ImVec2(240, 240), true);
				ImGui::Text(ICON_FA_PENCIL "   Softaim Settings");
				ImGui::Separator();

				ImGui::Text("Aimbot Keybind");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the softaim keybind.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				HotkeyButton(hotkeys::aimkey, ChangeKey, keystatus);

				ImGui::Checkbox("Enable Field of View", &enablefov);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("If the enemies is in the fov you can aimbot then.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				if (enablefov) {
					ImGui::Checkbox("Run Circle Fov", &fovcircle);
					ImGui::SameLine();
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Show an Circle Fov.");
						ImGui::PopTextWrapPos();

						ImGui::EndTooltip();
					}
				}


				if (enablefov) {
					ImGui::Checkbox("Run Filled Circle Fov", &Filled_Fov);
					ImGui::SameLine();
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Show an Filled Circle Fov.");
						ImGui::PopTextWrapPos();

						ImGui::EndTooltip();
					}
				}
				ImGui::Spacing();
				ImGui::Text("Fov Size");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Fov Size.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::SliderFloat("  ", &AimFOV, 25.0f, 400.0f);
				ImGui::Spacing();
				ImGui::EndChild();


				ImGui::SetCursorPos({ 80, 290 });
				ImGui::BeginChild("##child3", ImVec2(240, 135), true);
				ImGui::Text(ICON_FA_CAMERA "   Softaim Bone");
				ImGui::Separator();
				ImGui::Text("Softaim Bone");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the softaim bone, the softaim will lock to the bone.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Combo("", &bonestracersType, bonesTypes, IM_ARRAYSIZE(bonesTypes));

				ImGui::EndChild();


				ImGui::SetCursorPos({ 350, 290 });
				ImGui::BeginChild("##child4", ImVec2(240, 135), true);
				ImGui::Text(ICON_FA_HEART "   Triggerbot");
				ImGui::Separator();
				ImGui::Checkbox("Triggerbot", &triggerbot);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("When the crosshair is on the player, it will automatically shoot for you.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Text("Triggerbot Delay");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Add a delay to the Triggerbot.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::SliderFloat((""), &triggerbotdelay, 0.f, 5);
				ImGui::EndChild();

			}
			//tabs

			if (maintabs == 1) {
				ImGui::SetCursorPos({ 80, 20.f });
				ImGui::BeginChild("##child5", ImVec2(240, 240), true);
				ImGui::Text(ICON_FA_EYE "   Visuals");
				ImGui::Separator();
				ImGui::Checkbox("Run Box ESP", &enablebox);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Enable box esp, makes a  box on every player.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}


				ImGui::Text("Box Type");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Choose Box Type");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Filled Box", &FilledBoxEsp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Enable Filled Box.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Combo("", &selectedBoxType, boxTypes, IM_ARRAYSIZE(boxTypes));
				ImGui::Checkbox("Run Skeleton", &SkeletonEsp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Enable Skeleton esp.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Tracers", &enabletracers);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Enable tracers");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Text("Tracer Type");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Choose Tracer Type");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Combo("              ", &selectedtracersType, tracersTypes, IM_ARRAYSIZE(tracersTypes));
				ImGui::EndChild();

				ImGui::SetCursorPos({ 350, 20.f });
				ImGui::BeginChild("##child6", ImVec2(240, 240), true);
				ImGui::Text(ICON_FA_USERS "   More visuals");
				ImGui::Separator();
				ImGui::Checkbox("Run Distance ESP", &DistanceEsp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Show Distance esp.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Ammo ESP", &ammoesp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Show the ammo of the player");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Reloading ESP", &reloadingesp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Show if the Player Reloading or not.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Crouching ESP", &crounchedesp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Show if the Player Crouching or not.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Jumping ESP", &jumpesp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Show if the Player Jumping or not.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Bot ESP", &bot);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Show if the Player is a Bot/AI or not.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Name ESP", &nameEsp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Shows the name of the player.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Platform ESP", &platformEsp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Show the Player platform.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Weapon ESP", &weaponesp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Shows the Player Weapon.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::EndChild();

				ImGui::SetCursorPos({ 80, 290 });
				ImGui::BeginChild("##child7", ImVec2(240, 140), true);
				ImGui::Text(ICON_FA_CAMERA "   Visuals Options");
				ImGui::Separator();
				ImGui::Checkbox("Run Outline Box ESP", &OutlineBoxEsp);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Enable box esp outlined.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Outline Cornered Box", &outlinedcorneredbox);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Enable Cornered box esp outlined.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}


				ImGui::Text("Render Distance");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Render Distance.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::SliderFloat("  ", &renderdistance, 300.0f, 900.0f);
				ImGui::EndChild();

				ImGui::SetCursorPos({ 350, 290 });
				ImGui::BeginChild("##child8", ImVec2(240, 140), true);
				ImGui::Text(ICON_FA_HEART "   Visual Thickness");
				ImGui::Separator();
				ImGui::Text("Box Thickness");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Thickness of the Box ESP.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::SliderFloat((""), &BoxThickness, 0.f, 5);

				ImGui::Text("Skeleton Thickness");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Thickness of the Skeleton ESP.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::SliderFloat(("   "), &Skeleton_Thickness, 0.f, 5);
				ImGui::EndChild();
			}

			if (maintabs == 2) {
				ImGui::SetCursorPos({ 80, 20.f });
				ImGui::BeginChild("##child1", ImVec2(240, 240), true);
				ImGui::Text(ICON_FA_BOMB "   Exploits");
				ImGui::Separator();
				ImGui::Checkbox("Run Spinbot", &spinbot);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Make your skins spin fast.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Instant Respawn", &InstaRes);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("make your teamate Instant Respawn.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Car Fly", &carFly);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("make your car flying.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run AirStuck [ left shift ]", &AirStuck);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("make your stuck in the air.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::Checkbox("Run Fov Changer", &fovvchanger);
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Changes your field of view.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}

				ImGui::Text("Fov Changer Value");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Fov Value.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}

				ImGui::SliderFloat((""), &fovv_value, 50.f, 120);
				ImGui::EndChild();

			}


			if (maintabs == 4) {
				ImGui::SetCursorPos({ 80, 20.f });
				ImGui::BeginChild("##child1", ImVec2(240, 240), true);
				ImGui::Text(ICON_FA_PENCIL "   Visible Color");
				ImGui::Separator();
				ImGui::Text("Box esp");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Visible Color of the Box ESP.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::ColorEdit3(TEXT(" "), PlayerColor::BoxVisible);


				ImGui::Text("Cornered Box esp");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Visible Color of the Cornered Box ESP.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::ColorEdit3(TEXT("   "), PlayerColor::CorneredVisible);


				ImGui::Text("Skeleton esp");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Visible Color of the Skeleton ESP.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::ColorEdit3(TEXT("       "), PlayerColor::SkeletonVisible);

				ImGui::Text("Tracers ESP");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Visible Color of the Tracers ESP.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::ColorEdit3(TEXT("							"), PlayerColor::ButtomLineVisible);
				ImGui::EndChild();

				ImGui::SetCursorPos({ 350, 20.f });
				ImGui::BeginChild("##child2", ImVec2(240, 240), true);
				ImGui::Text(ICON_FA_PENCIL "   Not Visible Color");
				ImGui::Separator();
				ImGui::Text("Box esp");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Not Visible Color of the Box ESP.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::ColorEdit3(TEXT(" "), PlayerColor::BoxNotVisible);


				ImGui::Text("Cornered Box esp");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Not Visible Color of the Cornered Box ESP.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::ColorEdit3(TEXT("   "), PlayerColor::CorneredNotVisible);


				ImGui::Text("Skeleton esp");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Not Visible Color of the Skeleton ESP.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::ColorEdit3(TEXT("       "), PlayerColor::SkeletonNotVisible);

				ImGui::Text("Tracers ESP");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Not  Visible Color of the Tracers ESP.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::ColorEdit3(TEXT("							"), PlayerColor::ButtomLineNotVisible);
				ImGui::EndChild();

				ImGui::SetCursorPos({ 80, 290 });
				ImGui::BeginChild("##child3", ImVec2(240, 135), true);
				ImGui::Text(ICON_FA_PENCIL "   Misc Color");
				ImGui::Separator();
				ImGui::Text("FOV Color");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the FOV Color.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::ColorEdit3(TEXT("					"), PlayerColor::FovCircle1Visible);
				ImGui::Text("Crosshair Color");
				ImGui::SameLine();
				ImGui::TextDisabled("(?)");
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted("Change the Crosshair Color.");
					ImGui::PopTextWrapPos();

					ImGui::EndTooltip();
				}
				ImGui::ColorEdit3(TEXT("							"), PlayerColor::Crosshaircolor22);
				ImGui::EndChild();


			}
		
				if (maintabs == 5) {
					ImGui::SetCursorPos({ 80, 20.f });
					ImGui::BeginChild("##child1", ImVec2(240, 240), true);
					ImGui::Text(ICON_FA_PENCIL "   Misc Options");
					ImGui::Separator();
					ImGui::Checkbox("Run Crosshair", &enablecrosshair);
					ImGui::SameLine();
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Enables Crosshair");
						ImGui::PopTextWrapPos();

						ImGui::EndTooltip();
					}
					ImGui::Text("Crosshair Type");
					ImGui::SameLine();
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Choose Crosshair Type");
						ImGui::PopTextWrapPos();

						ImGui::EndTooltip();
					}
					ImGui::Combo("				  ", &selectedcrosshairType, crosshairtypes, IM_ARRAYSIZE(crosshairtypes));
					ImGui::Checkbox("Enable StreamProof", &StreamProof);
					ImGui::SameLine();
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Enable StreamProof");
						ImGui::PopTextWrapPos();

						ImGui::EndTooltip();
					}
					ImGui::Checkbox("Corner Watermark", &cornerFPS);
					ImGui::SameLine();
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Makes the Watermark at the Corner instead of middle");
						ImGui::PopTextWrapPos();

						ImGui::EndTooltip();
					}
					ImGui::Checkbox("Run Visible Check", &VisibleCheck);
					ImGui::SameLine();
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Only locks on if the player is visible.");
						ImGui::PopTextWrapPos();

						ImGui::EndTooltip();
					}
					ImGui::EndChild();
				}

				ImGui::End();
			}

			DrawESP();

			ImGui::EndFrame();
			D3dDevice->SetRenderState(D3DRS_ZENABLE, false);
			D3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
			D3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
			D3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);



			if (D3dDevice->BeginScene() >= 0)
			{
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				D3dDevice->EndScene();
			}
			HRESULT result = D3dDevice->Present(NULL, NULL, NULL, NULL);

			if (result == D3DERR_DEVICELOST && D3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			{
				ImGui_ImplDX9_InvalidateDeviceObjects();
				D3dDevice->Reset(&d3dpp);
				ImGui_ImplDX9_CreateDeviceObjects();
			}
		
	}
	
MSG Message = { NULL };
int Loop = 0;
void xMainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, Window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();

		if (hwnd_active == hwnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(Window, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(hwnd, &rc);
		ClientToScreen(hwnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;
		io.ImeWindowHandle = hwnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			d3dpp.BackBufferWidth = Width;
			d3dpp.BackBufferHeight = Height;
			SetWindowPos(Window, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			D3dDevice->Reset(&d3dpp);
		}
		render();
		if (Loop == 0) {

		}
		Loop = Loop + 1;
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DestroyWindow(Window);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		xShutdown();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (D3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			d3dpp.BackBufferWidth = LOWORD(lParam);
			d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = D3dDevice->Reset(&d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void xShutdown()
{
	TriBuf->Release();
	D3dDevice->Release();
	p_Object->Release();

	DestroyWindow(Window);
	UnregisterClass("Bluestacks", NULL);
}
