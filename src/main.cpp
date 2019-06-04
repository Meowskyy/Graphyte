#include "Graphyte\Graphyte.h"

#include <windows.h>
#include <cstdio>


int main()
{
	// This removes the console from the application
	// FreeConsole();

	// Init
	Graphyte::GraphyteEditor graphyte;

	// Run the application
	graphyte.run();

	// If there are no errors return 0
    return 0;
}