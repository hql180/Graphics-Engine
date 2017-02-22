#include "Application.h"
#include "MyApplication.h"

int main()
{
	auto app = new MyApplication();
	// window title, dimensions and if it is fullscreen
	app->run("AIE", 1280, 720, false);
	delete app;

    return 0;
}

