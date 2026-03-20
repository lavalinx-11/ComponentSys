#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>

#include <string>
#include "Scenes/SceneManager.h"
#include "Engine/Debug.h"

  
int main(int argc, char* args[]) {
	Debug::DebugInit("GameEngineLog.txt");
	
	SceneManager* gsm = new SceneManager();
	if (gsm->Initialize("Game Engine", 1920, 1080) ==  true) {
		gsm->Run();
	} 
	delete gsm;
	_CrtDumpMemoryLeaks();
	exit(0);
}