/*
===============================================================================

    Reflec-Tech
    ===========
    File        :    RtGraphicsDevice.h
    Author      :    Jamie Taylor
    Last Edit   :    01/07/13
    Desc        :    Defines the basic low-level interface for the renderer.
                     Basic, low-level things like device start-up, shut-down, clear-screen, draw etc...

===============================================================================
*/


#ifndef RT_GRAPHICS_DEVICE_H
#define RT_GRAPHICS_DEVICE_H


#include "../../PlatformIndependenceLayer/RtPlatform.h"
#include "RtRenderingUtils.h"
#include "RtMesh.h"
#include "RtMaterial.h"
#include "RtBitmapFont.h"

#include "RtLights.h"


/*
===============================================================================

Low level renderer, graphics device class

    Make this a singleton?

===============================================================================
*/
class GraphicsDevice {
public:
    virtual             ~GraphicsDevice( void ) { }

                        // start-up and shutdown the rendering device
    virtual U32         Startup( handle initialHandle, U32 initialBufferWidth, U32 intitialBufferHeight ) = 0;
    virtual void        Shutdown( void ) = 0;

                        // drawing
    virtual void        Draw( Mesh *mesh, XMMATRIX *viewMatrix_, XMFLOAT3 *cameraPosition ) = 0;
    virtual void        DrawString( const StringDescription &stringDescription, const I8 *string, ... ) = 0;
    virtual void        PresentFrame( void ) = 0;
    virtual void        SetClearColour( F32 r, F32 g, F32 b, F32 a ) = 0;
    virtual void        SetClearColour( F32 *colour ) = 0;
    virtual void        ClearScreen( void ) = 0;

                        // set/check handle
    virtual void        SetHandle( handle hndl ) = 0;
    virtual handle      GetHandle( void ) const = 0;

                        // this may not be needed on Linux
    virtual void        SetInstance( instance hInst ) = 0;
    virtual instance    GetInstance( void ) const = 0;
    
                        // check/set buffer dimensions
    virtual U32         ResizeBuffers( U32 newBufferWidth, U32 newBufferHeight ) = 0;
    virtual U32         GetBufferWidth( void ) const = 0;
    virtual U32         GetBufferHeight( void ) const = 0;
    virtual F32         GetAspectRatio( void ) const = 0;

                        // TEMP? Set newly added lighting shader members (added for lighting - directional light)
    virtual void        SetDirectionalLightShaderParams( const DirectionalLight *directionalLight ) = 0;

protected:
    bool                isRunning;

    U32                 frameBufferWidth;
    U32                 frameBufferHeight;

    F32                 clearColour[4];

    virtual bool        CreateRenderStates( void ) = 0;
    virtual void        SetRenderState( MATERIAL_RENDER_STATE renderState ) = 0;
};


// Factory functions - an implementation of these must be provided
template<class Allocator>
GraphicsDevice* CreateGraphicsDevice( Allocator &allctr );

template<class Allocator>
void DestroyGraphicsDevice( GraphicsDevice *destroyThis, Allocator &allctr );


#endif // RT_GRAPHICS_DEVICE_H
