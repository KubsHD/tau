#include "Asset.h"

#include <cstdlib>
#include <cstdio>

const char* Asset::get_real_path(const char* vpath)
{
#if DEBUG
#if APPLE
	const char* path_prefix = "./../../game/data/";
#else
	const char* path_prefix = "../../../../game/data/";
#endif

#else
#if VITA
	path_prefix = "app0:data/";
#elif XBOX
	char* base = SDL_WinRTGetFSPathUTF8(SDL_WINRT_PATH_INSTALLED_LOCATION);
	int size = snprintf(NULL, 0, "%s/data/", base);
	char* buf = malloc(size + 1);
	sprintf(buf, "%s/data/", base);
	path_prefix = buf;
#elif APPLE

	char* base = SDL_GetBasePath();
	int size = snprintf(NULL, 0, "%sdata/", base);
	char* buf = (char*)malloc(size + 1);
	sprintf(buf, "%sdata/", base);
	path_prefix = buf;
#else
	path_prefix = "data/";
#endif
	printf("asset: path: %s\n", path_prefix);
#endif

	size_t needed = snprintf(NULL, 0, "%s%s", path_prefix, vpath) + 1;

	char* tmp = (char*)calloc(needed, 1);

	snprintf(tmp, needed, "%s%s", path_prefix, vpath);

	return tmp;
}

const char* Asset::get_real_path(std::string vpath)
{
	return get_real_path(vpath.c_str());
}

