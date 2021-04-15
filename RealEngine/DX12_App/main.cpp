/*
Полезные тулзы:
    - Генераторы/редакторы мешей:
        3D Studio Max   (http://usa.autodesk.com/3ds-max/)
        LightWave 3D    (https://www.lightwave3d.com/)
        Maya            (http://usa.autodesk.com/maya/)
        Softimage|XSI   (http://www.softimage.com)
        Blender         (http://www.blender.org/)

*/

#include "DX12_App.h"

#include "DxException.h"

#include "LogError.h"
#include "LogInfo.h"

#include <memory>
#include <string>


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd )
{
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );         // Enable run-time memory check for debug builds.
#endif
    DX12_App dx12App( hInstance );
    try
    {
        if( !dx12App.Initialize() )
            return -1;

        dx12App.Run();

        dx12App.Deinitialize();
        return 0;
    }
    catch( DxException &e )
    {
        MessageBox( 0, e.ToString().c_str(), L"HR Failed", MB_OK );
        return -1;
    }
    catch( ... )
    {
        MessageBox( 0, L"Something unhandeled", L"HR Failed", MB_OK );
        return -1;
    }
}