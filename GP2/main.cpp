//Are we on the windows platform?
#ifdef WIN32
//if so include the header for windows applications
#include <Windows.h>
#endif
#include "GameApplication.h"

//Check to see if we are on windows
#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
//we are on some other platform
int main(int argc, char **argv)
#endif
{
	CGameApplication * pApp=new CGameApplication(); //entry point of the application allocates a pointer to the Game Application class
	
	if(!pApp->init()) //checking to see if the pointer is not initialized
	{
		if(pApp) //will delete the pointer if it is not initialized
		{
			delete pApp;
			pApp=NULL;
			return 1;
		}
	}
	pApp->run(); //will run if the pointer is initialized
	
	if(pApp) // deletes the pointer
	{
		delete pApp;
		pApp=NULL;
	}
	return 0;
}

