#include "stdafx.h"

#include "DemoApp.h"


INT WINAPI WinMain( HINSTANCE instance, HINSTANCE, LPSTR cmdlne, INT show )
{
    CDemoApp app;
    if( FAILED( app.Create( instance ) ) )
        return 0;
    return app.Run();
}
