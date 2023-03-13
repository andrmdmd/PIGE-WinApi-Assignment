#define NOMINMAX
#include <windows.h>
#include "app.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int show_command)
{
	app app0{ instance };
	return app0.run(show_command);
}
