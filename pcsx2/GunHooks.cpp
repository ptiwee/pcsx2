#define _PC_

#include "Memory.h"
#include "GunHooks.h"

namespace GunHooks
{
	static GunInfo guns[] = {
		{nullptr, 0, 0x003CC0C8, 0x003CC148, 0x006618A0, "FIRECon P1"},
		{nullptr, 0, 0x003CC178, 0x003CC1F8, 0x00661900, "FIRECon P2"}
	};
} // namespace GunHooks

void GunHooks::ReconnectGunHooks()
{
	struct sp_port **ports;
	sp_list_ports(&ports);

	UnloadGunHooks();

	for (GunInfo& gun : guns) {
		for (int i=0; ports[i]; i++) {
			std::string desc = sp_get_port_description(ports[i]);
			if (desc.starts_with(gun.name)) {
				sp_copy_port(ports[i], &gun.port);
			}
		}

		if(sp_open(gun.port, SP_MODE_WRITE) == SP_OK) {
			sp_set_baudrate(gun.port, 115200);
			sp_set_bits(gun.port, 8);
			sp_set_parity(gun.port, SP_PARITY_NONE);
			sp_set_stopbits(gun.port, 1);
			sp_set_flowcontrol(gun.port, SP_FLOWCONTROL_NONE);

			sp_nonblocking_write(gun.port, "S", 1);
			sp_nonblocking_write(gun.port, "M0x1", 4);
		}
	}

	sp_free_port_list(ports);
}

void GunHooks::ApplyVsyncGunHooks()
{
	for (GunInfo& gun : guns) {
		// Gun recoil
		if (memRead8(gun.shootAddr) == 0x01 &&
				memRead32(gun.feedbackAddr) != 0xFFFFFFFF) {
			sp_nonblocking_write(gun.port, "F0x2x1", 6);
		}

		// Rumble on hit
		u8 lives = memRead8(gun.lifeAddr);

		if (lives < gun.lives &&
				memRead32(gun.feedbackAddr) != 0xFFFFFFFF)
			sp_nonblocking_write(gun.port, "F1x2x1", 6);

		gun.lives = lives;
	}
}

void GunHooks::UnloadGunHooks() {
	for (GunInfo& gun : guns) {
		sp_nonblocking_write(gun.port, "E", 1);
		sp_close(gun.port);
		sp_free_port(gun.port);
		gun.port = nullptr;
	}
}
