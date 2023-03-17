#define NOMINMAX
#include <windows.h>
#include "app.h"

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int show_command) // tutorial 2048
{
	app app0{ instance };
	return app0.run(show_command);
}
