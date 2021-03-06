// SKCDropDash.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "..\skc-mod-loader\SKCModLoader\SKCModLoader.h"
#include "Trampoline.h"
#include "resource.h"

HMODULE moduleHandle;

VoidFunc(Play_Sound_2, 0x41A82D);
VoidFunc(SonicKnux_DoLevelCollision, 0x43795B);
VoidFunc(Player_TouchFloor, 0x4380BA);
VoidFunc(loc_4381DD, 0x4381DD);
VoidFunc(BubbleShield_Bounce, 0x4382B7);
DataPointer(Controller, Ctrl_1_logical, 0x8FFF602);
DataPointer(char, Reverse_gravity_flag, 0x8FFF7C6);

void Sonic_ReleaseDropDash()
{
	Object *a0 = reg_a0.Object;
	if (a0->double_jump_property && !(a0->status & 2))
	{
		a0->double_jump_property = 0;
		if (Ctrl_1_logical.Held & 8)
			a0->status &= ~1;
		if (Ctrl_1_logical.Held & 4)
			a0->status |= 1;
		short dashspeed, maxspeed;
		if (Super_Sonic_Knux_flag)
		{
			dashspeed = 0xC00;
			maxspeed = 0xD00;
		}
		else
		{
			dashspeed = 0x800;
			maxspeed = 0xC00;
		}
		if (a0->status & 1)
		{
			if (a0->x_vel <= 0)
			{
				a0->ground_vel = (a0->ground_vel >> 2) - dashspeed;
				if (a0->ground_vel < -maxspeed)
					a0->ground_vel = -maxspeed;
				goto LABEL_25;
			}
			if (a0->angle)
			{
				a0->ground_vel = (a0->ground_vel >> 1) - dashspeed;
				goto LABEL_25;
			}
			dashspeed = -dashspeed;
		}
		else
		{
			if (a0->x_vel >= 0)
			{
				a0->ground_vel = (a0->ground_vel >> 2) + dashspeed;
				if (a0->ground_vel > maxspeed)
					a0->ground_vel = maxspeed;
				goto LABEL_25;
			}
			if (a0->angle)
			{
				a0->ground_vel = dashspeed + (a0->ground_vel >> 1);
				goto LABEL_25;
			}
		}
		a0->ground_vel = dashspeed;
	LABEL_25:
		a0->status |= 4;
		a0->y_radius = 14;
		a0->x_radius = 7;
		a0->anim = 2;
		a0->prev_anim = 0;
		a0->y_pos += 5;
		if (Reverse_gravity_flag)
			a0->y_pos -= 10;
		reg_d0.Word = 0xFFB6u;
		Play_Sound_2();
	}
}

char djp;
void DoLevelCollision_DropDash()
{
	djp = reg_a0.Object->double_jump_property;
	SonicKnux_DoLevelCollision();
	reg_a0.Object->double_jump_property = djp;
	Sonic_ReleaseDropDash();
}

void TouchFloor_DropDash()
{
	if (reg_a0.Object->character_id || !reg_a0.Object->double_jump_property)
		Player_TouchFloor();
	else
	{
		loc_4381DD();
		reg_a0.Object->double_jump_property = 1;
		Sonic_ReleaseDropDash();
	}
}

void Sonic_RollJump()
{
	reg_a0.Object->y_radius = 14;
	reg_a0.Object->x_radius = 7;
}

Trampoline *instatramp;
void Sonic_InstaAndShieldMoves()
{
	if (reg_a0.Object->double_jump_property)
	{
		// Sonic_DropDash_CheckRelease:
		if (!(Ctrl_1_logical.Held & 0x70))
		{
			reg_a0.Object->double_jump_flag = -1;
			reg_a0.Object->double_jump_property = 0;
			reg_a0.Object->prev_anim = 0;
		}
	}
	else if (reg_a0.Object->double_jump_flag >= 2)
	{
		//Sonic_DropDash_Charge:
		if (Ctrl_1_logical.Held & 0x70 && ++reg_a0.Object->double_jump_flag > 21)
		{
			reg_a0.Object->double_jump_property = 1;
			reg_a0.Object->prev_anim = 0;
			reg_d0.Word = 0xFFABu;
			Play_Sound_2();
		}
	}
	else
		((void (*)())instatramp->Target())();
}

void Sonic_DropDash()
{
	reg_a0.Object->double_jump_flag = 2;
}

Trampoline *touchfloortramp;
__declspec(naked) void Player_TouchFloor_Check_Spindash_r()
{
	__asm
	{
		mov al, [edi+38h]
		test al,al
		jnz skip
		mov al, [edi+25h]
		jz skip
		jmp loc_4381DD
skip:
	}
	((void(*)())touchfloortramp->Target())();
	__asm { ret }
}

void BubbleShield_CheckBounce()
{
	if (!reg_a0.Object->double_jump_property)
		BubbleShield_Bounce();
}

#pragma warning(suppress : 838 309)
char AniSonic_DropDash[] = { 0, 0xBF, 0xC1, 0xC0, 0xC2, 0xBF, 0xC5, 0xC0, 0xC6, 0xBF, 0xC7, 0xC0, 0xDA, 0xBF, 0xDB, 0xC0, 0xDC, 0xFF };
Trampoline *anitramp;
void DropDashAnim()
{
	if (reg_a0.Object->double_jump_property)
		reg_a1.Unknown = AniSonic_DropDash;
	((void(*)())anitramp->Target())();
}

Trampoline *arttramp;
intptr_t ArtUnc_Sonic_Extra;
void DropDashArt()
{
	if (reg_a0.Unknown == (void*)0x8FFB000 && reg_a0.Object->double_jump_property)
		reg_d6.Long = ArtUnc_Sonic_Extra;
	((void(*)())arttramp->Target())();
}

Trampoline *tunneltramp;
void FixWaterTunnel()
{
	reg_a0.Object->double_jump_property = 0;
	((void(*)())tunneltramp->Target())();
}

Trampoline *bubbletramp;
void FixAirBubble()
{
	reg_a1.Object->double_jump_property = 0;
	((void(*)())bubbletramp->Target())();
}

Trampoline *floortramp;
void FixTouchFloor()
{
	reg_a0.Object->double_jump_property = 0;
	((void(*)())floortramp->Target())();
}

extern "C"
{
	__declspec(dllexport) void Init(const wchar_t *path, const HelperFunctions &helperFunctions)
	{
		WriteCall((void*)0x433B8F, DoLevelCollision_DropDash);
		WriteCall((void*)0x434F67, DoLevelCollision_DropDash);
		WriteCall((void*)0x8240ED, TouchFloor_DropDash);
		WriteJump((void*)0x43699A, Sonic_RollJump);
		instatramp = new Trampoline(0x436A55, 0x436A5B, Sonic_InstaAndShieldMoves);
		WriteData((void*)0x436AAC, 0x90u, 0xC);
		WriteData((int*)0x436ACC, 0x436C50 - 0x436ACA - 6);
		WriteJump((void*)0x436C50, Sonic_DropDash);
		touchfloortramp = new Trampoline(0x4380AF, 0x4380B7, Player_TouchFloor_Check_Spindash_r);
		WriteCall((void*)0x4382A6, BubbleShield_CheckBounce);
		anitramp = new Trampoline(0x438C68, 0x438C6E, DropDashAnim);
		HRSRC hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_SONICEXTRA), L"BIN");
		ArtUnc_Sonic_Extra = (intptr_t)LockResource(LoadResource(moduleHandle, hres));
		WriteData((intptr_t*)0x439E6C, ArtUnc_Sonic_Extra);
		arttramp = new Trampoline(0x439E5F, 0x439E65, DropDashArt);
		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_SONICMAP), L"BIN");
		void *tmp = LockResource(LoadResource(moduleHandle, hres));
		WriteData((void**)0x434119, tmp);
		WriteData((void**)0x436FA9, tmp);
		WriteData((void**)0x4592DE, tmp);
		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_SONICDPLC), L"BIN");
		tmp = LockResource(LoadResource(moduleHandle, hres));
		WriteData((void**)0x439DBB, tmp);
		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_SUPERSONICMAP), L"BIN");
		tmp = LockResource(LoadResource(moduleHandle, hres));
		WriteData((void**)0x4308E3, tmp);
		WriteData((void**)0x436C9F, tmp);
		WriteData((void**)0x79AA01, tmp);
		hres = FindResource(moduleHandle, MAKEINTRESOURCE(IDR_SUPERSONICDPLC), L"BIN");
		tmp = LockResource(LoadResource(moduleHandle, hres));
		WriteData((void**)0x439DCE, tmp);
		tunneltramp = new Trampoline(0x41C5D7, 0x41C5DD, FixWaterTunnel);
		bubbletramp = new Trampoline(0x764753, 0x764759, FixAirBubble);
		floortramp = new Trampoline(0x43822E, 0x438236, FixTouchFloor);
	}

	__declspec(dllexport) ModInfo SKCModInfo = { ModLoaderVer };
}