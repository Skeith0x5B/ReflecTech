/*
===============================================================================

    ReflecTech
    ==========
    File        :   RtGraphicsDeviceD3D11.h
    Author      :   Jamie Taylor
    Last Edit   :   16/07/13
    Desc        :   D3D11 implementation of the low-level renderer graphics device interface.

===============================================================================
*/


#ifndef RT_GRAPHICS_DEVICE_D3D11_H
#define RT_GRAPHICS_DEVICE_D3D11_H


#include "../../RtCommonHeaders.h"

#include "../LowLevelRenderer/RtGraphicsDevice.h"

// needed to test factory functions
//#include "../../CoreSystems/RtMemoryCommon.h"
#include "../../CoreSystems/RtHeapAllocator.h"
#include "../../CoreSystems/RtDynamicPoolAllocator.h"

// d3d11 specifics
#include <D3D11.h>
#include <D3DX11.h>
#include "../../../../../../Program Files (x86)/Microsoft DirectX SDK (June 2010)/Samples/C++/Effects11/Inc/d3dx11effect.h"
#include <D3Dcompiler.h>
#include <dxerr.h>

// to help cut down on the amount of "if(ptr) ptr->Release( ); ptr = NULL;"
#define SafeRelease( x ) { if( x != NULL ) x->Release( ); x = NULL; }

// d3d error checker borrowed from book
#if defined(DEBUG) | defined(_DEBUG)
    #ifndef HR
    #define HR(x)                                             \
    {                                                         \
        HRESULT hr = (x);                                     \
        if(FAILED(hr))                                        \
        {                                                     \
            DXTrace(__FILE__, (DWORD)__LINE__, hr, #x, true); \
        }                                                     \
    }
    #endif
#else
    #ifndef HR
    #define HR(x) (x)
    #endif
#endif 


/*
===============================================================================

D3D11 powered low level renderer, graphics device class

===============================================================================
*/
class GraphicsDeviceD3D11 : public GraphicsDevice {
public:
                                  GraphicsDeviceD3D11( void );
                                  ~GraphicsDeviceD3D11( void );

                                  // start-up and shutdown the rendering device
    U32                           Startup( handle initialHandle, U32 initialBufferWidth, U32 initialBufferHeight );
    void                          Shutdown( void );

                                  // drawing
    void                          Draw( Mesh *mesh, XMMATRIX *viewMatrix_, XMFLOAT3 *cameraPosition );
    void                          DrawString( const StringDescription &stringDescription, const I8 *string, ... );
    void                          PresentFrame( void );
    void                          SetClearColour( F32 r, F32 g, F32 b, F32 a );
    void                          SetClearColour( F32 *colour );
    void                          ClearScreen( void );

                                  // set/check handle
    void                          SetHandle( handle hWindow );
    handle                        GetHandle( void ) const;

                                  // check/set instance - TEMP - REMOVE? or use a platform neutral instance typedef?
    void                          SetInstance( instance hInst );
    instance                      GetInstance( void ) const;

                                  // check/set buffer dimensions
    U32                           ResizeBuffers( U32 newBufferWidth, U32 newBufferHeight );
    U32                           GetBufferWidth( void ) const;
    U32                           GetBufferHeight( void ) const;
    F32                           GetAspectRatio( void ) const;

                                  // TEMP? Set newly added lighting shader members (added for lighting - directional light)
    void                          SetDirectionalLightShaderParams( const DirectionalLight *directionalLight );

private:
                                  // needed for the window
    instance                      hInst;
    handle                        hWnd;

    D3D_DRIVER_TYPE               driverType;
    U32                           mClientWidth;
    U32                           mClientHeight;
    bool                          use4xMSAA;
      U32                         msaaQuality;

    ID3D11Device                * d3dDevice;
    ID3D11DeviceContext         * immediateContext;
    IDXGISwapChain              * swapChain;
    ID3D11Texture2D             * depthStencilBuffer;
    ID3D11RenderTargetView      * renderTargetView;
    ID3D11DepthStencilView      * depthStencilView;
    D3D11_VIEWPORT                viewport;

    // ===============================================================================

    ID3D11Buffer                * vertexBuffer;
    ID3D11Buffer                * indexBuffer;

    ID3DX11Effect               * mFX;
    ID3DX11EffectTechnique      * mTech;
    ID3DX11EffectMatrixVariable * mfxWorldViewProj;

    ID3D11InputLayout           * inputLayout;

    XMFLOAT4X4                    worldMatrix;
    XMFLOAT4X4                    viewMatrix;
    XMFLOAT4X4                    projectionMatrix;

                                  // render/rasterizer states (may need to rename to rasterizer state)
    ID3D11RasterizerState       * solidRenderStateLeftHanded;
    ID3D11RasterizerState       * solidRenderStateRightHanded;
    ID3D11RasterizerState       * wireFrameRenderStateLeftHanded;
    ID3D11RasterizerState       * wireFrameRenderStateRightHanded;
                                  // stencil states (added for stencil shadows 13/08/13)
    ID3D11DepthStencilState     * shadowingStencilState;
                                  // added for lighting (directional light)
    MATERIAL_RENDER_STATE         currentRenderState;

    ID3DX11EffectMatrixVariable * mfxWorldMatrix;

    ID3DX11EffectVectorVariable * mfxLightAmbientColour;
    ID3DX11EffectVectorVariable * mfxLightDiffuseColour;
    ID3DX11EffectVectorVariable * mfxLightSpecularColour;
    ID3DX11EffectVariable       * mfxLightSpecularPower;
    ID3DX11EffectVectorVariable * mfxLightDirection;

    ID3DX11EffectVectorVariable * mfxCameraPosition;

                                          // added for texturing
    ID3DX11EffectShaderResourceVariable * mfxDiffuseMap;
    ID3D11ShaderResourceView            * mfxDiffuseMapSRV;

    void                          BuildGeometryBuffers( Vertex *meshData, U32 vertexCount, U32 *indexData, U32 indexCount );
    void                          BuildFX( void );
    void                          BuildVertexLayout( void );

                                  // 13/08/13
    bool                          CreateRenderStates( void );
    void                          SetRenderState( MATERIAL_RENDER_STATE renderState );
};


// Factory functions - uses an allocator to create an instance of the graphics device
template<class Allocator>
GraphicsDevice* CreateGraphicsDevice( Allocator &allctr ) {
    GraphicsDevice *temp = reinterpret_cast<GraphicsDeviceD3D11*>( allctr.Allocate( sizeof( GraphicsDeviceD3D11 ) ) );
    allctr.Construct( reinterpret_cast<GraphicsDeviceD3D11*>( temp ) );
    return temp;
}

template<class Allocator>
void DestroyGraphicsDevice( GraphicsDevice *destroyThis, Allocator &allctr ) {
    GraphicsDevice *temp = reinterpret_cast<GraphicsDevice*>( destroyThis );
    allctr.Destruct( temp );
    allctr.DeAllocate( temp );
}

// can't export templates, may have to hardcode these for each allocator type? boilerplate style?
extern "C" GraphicsDevice* CreateGraphicsDeviceFromHeap( HeapAllocator<GraphicsDevice> &allctr );
extern "C" void DestroyGraphicsDeviceFromHeap( GraphicsDevice *p, HeapAllocator<GraphicsDevice> &allctr );


#endif // RT_GRAPHICS_DEVICE_D3D11_H
