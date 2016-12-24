#ifdef NDEBUG
#include <Windows.h>

int main();

#pragma warning( push )
#pragma warning( disable : 28252 )
#pragma warning( disable : 28253 )
int CALLBACK WinMain(_In_ HINSTANCE, _In_ HINSTANCE, _In_ LPSTR, _In_ int)
{
  return main();
}
#pragma warning( pop ) 

#endif