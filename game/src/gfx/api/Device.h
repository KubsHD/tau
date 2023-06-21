#pragma once

#include <core/Types.h>

struct STexture {
	// todo
};

class Device
{
public:
	Device();
	~Device();
	
	spt::ref<STexture> create_texture(TextureCreateDesc desc);
	void create_shader();
};