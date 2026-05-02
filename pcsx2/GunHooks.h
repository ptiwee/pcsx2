#pragma once

#include <libserialport.h>

namespace GunHooks
{
	struct GunInfo
	{
		sp_port *port;
		u8 lives;

		u32 shootAddr;
		u32 feedbackAddr;
		u32 lifeAddr;

		char name[255];
	};

	extern void InitGunHooks();
	extern void ApplyVsyncGunHooks();
	extern void UnloadGunHooks();
} // namespace GunHooks
