/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtGraphicsDeviceD3D11.h
    Author      :    Jamie Taylor
    Last Edit   :    01/07/13
    Desc        :    D3D11 implementation of the low-level renderer graphics device interface.

===============================================================================
*/


#include "RtGraphicsDeviceD3D11.h"


/*
================
GraphicsDeviceD3D11::GraphicsDeviceD3D11
================
*/
GraphicsDeviceD3D11::GraphicsDeviceD3D11( void ) :
    driverType(D3D_DRIVER_TYPE_HARDWARE),
    mClientWidth(640),
    mClientHeight(480),
    use4xMSAA(false),
    hWnd(0),
    msaaQuality(0),
 
    d3dDevice(0),
    immediateContext(0),
    swapChain(0),
    depthStencilBuffer(0),
    renderTargetView(0),
    depthStencilView(0)
{
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    // not fully setup yet - not running
    frameBufferWidth  = mClientWidth;
    frameBufferHeight = frameBufferHeight;

    isRunning = false;
}

/*
================
GraphicsDeviceD3D11::~GraphicsDeviceD3D11
================
*/
GraphicsDeviceD3D11::~GraphicsDeviceD3D11( void ) {
    if( isRunning == true ) {
        Shutdown();
    }
}

/*
================
GraphicsDeviceD3D11::Startup
================
*/
U32 GraphicsDeviceD3D11::Startup( handle initialHandle, U32 initialBufferWidth, U32 initialBufferHeight ) {
    driverType          = D3D_DRIVER_TYPE_HARDWARE,
    frameBufferWidth    = initialBufferWidth,
    frameBufferHeight   = initialBufferHeight,
    use4xMSAA           = false,
    hWnd                = initialHandle;
    msaaQuality         = 0;

    d3dDevice           = NULL;
    immediateContext    = NULL;
    swapChain           = NULL;
    vertexBuffer        = NULL;
    indexBuffer         = NULL;
    depthStencilBuffer  = NULL;
    renderTargetView    = NULL;
    depthStencilView    = NULL;
    inputLayout         = NULL;
    mFX                 = NULL;
    currentRenderState  = MATERIAL_RENDER_STATE::SOLID_LH;
    wireFrameRenderStateLeftHanded = NULL;

    ZeroMemory( &viewport, sizeof( D3D11_VIEWPORT ) );

    XMMATRIX I = XMMatrixIdentity( );
    XMStoreFloat4x4( &worldMatrix, I );
    XMStoreFloat4x4( &viewMatrix, I );
    XMStoreFloat4x4( &projectionMatrix, I );

    // zero out lighting members...

    // zero out texturing members...
    mfxDiffuseMapSRV = NULL;

    // DEVICE_DEBUG
    int createDeviceFlags = 0;
#ifdef RT_DEBUG 
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 

    HRESULT hr = 0;

    // L1) (Luna D3D11 - Page 103) create device and device context
    D3D_FEATURE_LEVEL featureLevel;
    hr = D3D11CreateDevice(
            NULL,                       // default adapter
            D3D_DRIVER_TYPE_HARDWARE,   // WARP for laptop &| onboard, HARDWARE for desktop with graphics card
            NULL,                       // no software driver
            0,                          //createDeviceFlags,
            NULL, 0,                    // only target D3D11, no earlier feature levels
            D3D11_SDK_VERSION,
            &d3dDevice,
            &featureLevel,
            &immediateContext );
    if( FAILED( hr ) ) {
        MessageBox( NULL, "D3D11CreateDevice FAILED.", 0, 0 );
        return hr;
    }

    // ReflecTech D3D renderer only supports D3D11, no earlier feature levels
    if( featureLevel != D3D_FEATURE_LEVEL_11_0 ) {
        MessageBox( NULL, "D3D Feature Level 11_0 not supported.", 0, 0 );
        return hr;
    }

    // L2) Check 4X MSAA support for back buffer format,
    // all D3D11 capable hardware supports at least 4X MSAA, so we're effectively just
    // defaulting to the lowest available level of MSAA for now.
    hr = d3dDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, 4, &msaaQuality );
    RT_ASSERT( this->msaaQuality > 0 );

    // L3) Describe swap chain characteristics
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    swapChainDesc.BufferDesc.Width                   = frameBufferWidth;
    swapChainDesc.BufferDesc.Height                  = frameBufferHeight;
    swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;

    // use MSAA?
    if( this->use4xMSAA == true ) {
        swapChainDesc.SampleDesc.Count     = 4; // setting to msaaQuality causes createSwapChain to fail, investigate;
        swapChainDesc.SampleDesc.Quality = msaaQuality - 1;
    }
    else {
        swapChainDesc.SampleDesc.Count     = 1;
        swapChainDesc.SampleDesc.Quality = 0;
    }

    swapChainDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount  = 1;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed     = true;
    swapChainDesc.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags        = 0;

    // L4) Use the IDXGIFactory instance used to create the device to create the swap chain,
    // trying to use a different IDXGIFactory instance would throw up an error: "IDXGIFactory::CreateSwapChain: 
    // This function is being called with a device from a different IDXGIFactory."
    IDXGIDevice *dxgiDevice = NULL;
    hr = d3dDevice->QueryInterface( __uuidof( IDXGIDevice ), reinterpret_cast<void**>( &dxgiDevice ) );

    IDXGIAdapter *dxgiAdapter = NULL;
    hr = dxgiDevice->GetParent( __uuidof( IDXGIAdapter ), reinterpret_cast<void**>( &dxgiAdapter ) );

    IDXGIFactory *dxgiFactory = NULL;
    hr = dxgiAdapter->GetParent( __uuidof( IDXGIFactory ),  reinterpret_cast<void**>( &dxgiFactory ) );

    // we now have the IDXGIFactory instance used to create the device and can now use it to create
    // the swap chain
    hr = dxgiFactory->CreateSwapChain( d3dDevice, &swapChainDesc, &swapChain );

    SafeRelease( dxgiDevice );
    SafeRelease( dxgiAdapter );
    SafeRelease( dxgiFactory );

    if( FAILED( hr ) ) {
        MessageBox( NULL, "Failed to create swap chain.", 0, 0 );
        return hr;
    }

    // the rest of the setup work needs to be repeated each time we resize the window/buffers
    // so we simply put in a call to resize to finish the setup
    ResizeBuffers( frameBufferWidth, frameBufferHeight );

    // build the render states
    CreateRenderStates();

    // done
    isRunning = false;
    //isRunning = true;
    return 0;
}

/*
================
GraphicsDeviceD3D11::Shutdown
================
*/
void GraphicsDeviceD3D11::Shutdown( void ) {
    SafeRelease( vertexBuffer );
    SafeRelease( indexBuffer );
    SafeRelease( mFX );
    SafeRelease( inputLayout );
    SafeRelease( wireFrameRenderStateLeftHanded );

    SafeRelease( renderTargetView );
    SafeRelease( depthStencilView );
    SafeRelease( swapChain );
    SafeRelease( depthStencilBuffer );

    // release lighting members...

    // release texturing members...

    // Restore all default settings.
    if( immediateContext ) {
        immediateContext->ClearState( );
    }

    SafeRelease( immediateContext );

    // a reference count seems to be getting incremented somewhere, until I find where
    // and put in a matching Release( ) call I'll put it here
    //d3dDevice->Release( ); - commented out when wireframe was removed?
    // now the device can be released and nulled out
    SafeRelease( d3dDevice );

    isRunning = false;
}

/*
================
GraphicsDeviceD3D11::Draw
================
*/
void GraphicsDeviceD3D11::Draw( Mesh *mesh, XMMATRIX *viewMatrix_, XMFLOAT3 *cameraPosition ) {
    // Have to find some alternative to this, building geometry buffers and textures will need to be done for each
    // different mesh, keep it here for now (testing purposes)...
    if( isRunning == false ) {
        BuildFX( );

        // Set texture - need to init effects first since input layout depends on shader signatures - Luna D3D11 book
        // TEXTURE CODE CAN'T STAY HERE, NEEDS TO BE CHANGED FOR EACH SUBSET?
        Material *mat = mesh->GetMaterialData( );
        HRESULT hres = D3DX11CreateShaderResourceViewFromFile( d3dDevice, (LPCSTR)"DefaultGrid_256x256.dds"/*mat[0].diffuseMapName*/, NULL, NULL, &mfxDiffuseMapSRV, NULL );
        hres = mfxDiffuseMap->SetResource( mfxDiffuseMapSRV );

        BuildVertexLayout( );
        isRunning = true;
    }

    Vertex  *vertexData  = mesh->GetVertexData( );
    U32     *indexData   = mesh->GetIndexData( );
    SubMesh *subMeshData = mesh->GetSubMeshData( );
    // constantly rebuilding the buffers like this is murdering the frame-rate, has to be a better way to do this...
    BuildGeometryBuffers( vertexData, mesh->GetVertexCount( ), indexData, mesh->GetIndexCount( ) );

    // clear screen to clear colour before drawing
    //ClearScreen( );

    immediateContext->IASetInputLayout( inputLayout );
    immediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // Set the materials
    U32 materialCount = mesh->GetMaterialCount( );
    for( I32 i=0; i<materialCount; ++i ) {
        I8 str[64] = { '\0' };
        strcpy( str, mesh->GetMaterialData( )[i].diffuseMapName );

        //d3dResult = D3DX11CreateShaderResourceViewFromFile( d3dDevice_,
        //str, 0, 0, &colorMaps_[i], 0 );
    }

    // TEMP, TURN WIREFRAME ON/OFF ---------------------------------------
    if( mesh->GetMaterialData( )[0].renderState != currentRenderState ) {
        currentRenderState = mesh->GetMaterialData( )[0].renderState;
        SetRenderState( currentRenderState );
    }
    // TEMP, TURN WIREFRAME ON/OFF ---------------------------------------*/

    U32 stride = sizeof( Vertex );
    U32 offset = 0;
    immediateContext->IASetVertexBuffers( 0, 1, &vertexBuffer, &stride, &offset );
    immediateContext->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

    // Set matrices and constants
    XMMATRIX P = XMMatrixPerspectiveFovLH( 0.25f*(3.1415926535f) /*0.25f*MathHelper::Pi*/, GetAspectRatio( ), 1.0f, 1000.0f );
    XMStoreFloat4x4( &projectionMatrix, P );

    //XMMATRIX world = XMLoadFloat4x4( &worldMatrix );
    // we use the world matrix from the mesh
    XMMATRIX worldMatrix = *mesh->GetWorldMatrix( );

    // set newly added worldMatrix shader member (added for lighting - directional light)
    HRESULT hr = mfxWorldMatrix->SetMatrix( reinterpret_cast<F32*>( &worldMatrix ) );
    // set newly added camera position shader member (added for lighting - directional light)
    hr = mfxCameraPosition->SetFloatVector( reinterpret_cast<F32*>( const_cast<XMFLOAT3*>( cameraPosition ) ) );

    // we use the view matrix from the camera
    XMMATRIX proj  = XMLoadFloat4x4( &projectionMatrix );
    XMMATRIX worldViewProj = ( worldMatrix ) * ( *viewMatrix_ ) * proj;

    hr = mfxWorldViewProj->SetMatrix( reinterpret_cast<F32*>( &worldViewProj ) );

    D3DX11_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
    // the geo primitive generator makes use of indexing
    U32 subMeshCount = mesh->GetSubMeshCount( );
    if( ( subMeshData[0].startIndex != -1 ) && ( subMeshData[0].indexCount != -1 ) ) {
        for( U32 p = 0; p < techDesc.Passes; ++p ) {
            mTech->GetPassByIndex( p )->Apply( 0, immediateContext );
            // loop through all subsets?
            for( U32 i=0; i<subMeshCount; ++i ) {
                immediateContext->DrawIndexed( subMeshData[0].indexCount, 0, 0 );
            }
        }
    } else { // the current obj/model loading/parsing code however does not (for now) so we use Draw( ) and not DrawIndexed( )
        for( U32 p = 0; p < techDesc.Passes; ++p ) {
            mTech->GetPassByIndex( p )->Apply( 0, immediateContext );
            // loop through all subsets?
            for( U32 i=0; i<subMeshCount; ++i ) {
                immediateContext->Draw( subMeshData[i].vertexCount, subMeshData[i].startVertex );
            }
        }
    }
}

/*
================
GraphicsDeviceD3D11::DrawString
================
*/
void GraphicsDeviceD3D11::DrawString( const StringDescription &stringDescription, const I8 *string, ... ) {
    // ...
}

void GraphicsDeviceD3D11::PresentFrame( void ) {
    HR( swapChain->Present( 0, 0 ) );
    ClearScreen( );
}

/*
================
GraphicsDeviceD3D11::SetClearColor
================
*/
void GraphicsDeviceD3D11::SetClearColour( F32 *colour ) {
    clearColour[0] = colour[0];    
    clearColour[1] = colour[1];
    clearColour[2] = colour[2];
    clearColour[3] = colour[3];
}

void GraphicsDeviceD3D11::SetClearColour( F32 r, F32 g, F32 b, F32 a ) {
    clearColour[0] = r;    
    clearColour[1] = g;
    clearColour[2] = b;
    clearColour[3] = a;
}

/*
================
GraphicsDeviceD3D11::ClearScreen
================
*/
void GraphicsDeviceD3D11::ClearScreen( void ) {
    F32 clear[4] = { 0.0f, 0.0f, 0.25f, 1.0f };
    immediateContext->ClearRenderTargetView( renderTargetView, reinterpret_cast<const float*>( &Colours::DefaultBlue ) );
    immediateContext->ClearDepthStencilView( depthStencilView, ( D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL ), 1.0f, 0 );
}

/*
================
GraphicsDeviceD3D11::SetHandle
================
*/
void GraphicsDeviceD3D11::SetHandle( HWND hWindow ) {
    hWnd = hWindow;
}

/*
================
GraphicsDeviceD3D11::GetHandle
================
*/
handle GraphicsDeviceD3D11::GetHandle( void ) const {
    return hWnd;
}

/*
================
GraphicsDeviceD3D11::SetInstance
================
*/
void GraphicsDeviceD3D11::SetInstance( instance hInstance ) {
    hInst = hInstance;
}

/*
================
GraphicsDeviceD3D11::GetInstance
================
*/
instance GraphicsDeviceD3D11::GetInstance( void ) const {
    return hInst;
}

/*
================
GraphicsDeviceD3D11::ResizeBuffers
================
*/
U32 GraphicsDeviceD3D11::ResizeBuffers( U32 newBufferWidth, U32 newBufferHeight ) {
    HRESULT hr = 0;

    frameBufferWidth = newBufferWidth;
    frameBufferHeight = newBufferHeight;

    // Release the old views before proceeding
    SafeRelease( renderTargetView );
    SafeRelease( depthStencilView );
    SafeRelease( depthStencilBuffer );

    // L5) Create render target view to swap chain back buffer
    // resize specific: resize swap chain and recreate render target view
    hr = swapChain->ResizeBuffers( 1, frameBufferWidth, frameBufferHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0 ); // ensure width and height are being UPDATED!

    ID3D11Texture2D *backBufferTexture = NULL;
    hr = swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBufferTexture ) );
    hr = d3dDevice->CreateRenderTargetView( backBufferTexture, NULL, &renderTargetView );
    SafeRelease( backBufferTexture );

    // L6) Create depth/stencil buffer and associated view
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width      = frameBufferWidth;
    depthStencilDesc.Height     = frameBufferHeight;
    depthStencilDesc.MipLevels  = 1;
    depthStencilDesc.ArraySize  = 1;
    depthStencilDesc.Format     = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // use MSAA?
    if( use4xMSAA == true ) {
        depthStencilDesc.SampleDesc.Count   = 4; // setting to msaaQuality causes createSwapChain to fail, investigate;
        depthStencilDesc.SampleDesc.Quality = msaaQuality - 1;
    }
    else {
        depthStencilDesc.SampleDesc.Count   = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
    }

    depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags      = 0;

    hr = d3dDevice->CreateTexture2D( &depthStencilDesc, NULL, &depthStencilBuffer );
    hr = d3dDevice->CreateDepthStencilView( depthStencilBuffer, NULL, &depthStencilView );

    // L7) Bind render target view and depth/stencil view to the pipleline
    immediateContext->OMSetRenderTargets( 1, &renderTargetView, depthStencilView );

    // set viewport transform
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width    = static_cast<float>( frameBufferWidth );
    viewport.Height   = static_cast<float>( frameBufferHeight );
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // L8) set viewport
    immediateContext->RSSetViewports( 1, &viewport );

    return static_cast<U32>( hr );
}

/*
================
GraphicsDeviceD3D11::GetBufferWidth
================
*/
U32 GraphicsDeviceD3D11::GetBufferWidth( void ) const {
    return frameBufferWidth;
}

/*
================
GraphicsDeviceD3D11::GetBufferHeight
================
*/
U32 GraphicsDeviceD3D11::GetBufferHeight( void ) const {
    return frameBufferHeight;
}

/*
================
GraphicsDeviceD3D11::GetBufferHeight
================
*/
F32    GraphicsDeviceD3D11::GetAspectRatio( void ) const {
    return ( static_cast<F32>( frameBufferWidth ) / static_cast<F32>( frameBufferHeight ) );
}


void GraphicsDeviceD3D11::BuildGeometryBuffers(  Vertex *meshData, U32 vertexCount, U32 *indexData, U32 indexCount  ) {

    if( vertexBuffer ) {
        vertexBuffer->Release( );
    }
    // Create vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.ByteWidth           = ( sizeof( Vertex ) * vertexCount );
    vertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags      = 0;
    vertexBufferDesc.MiscFlags           = 0;
    vertexBufferDesc.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA vertexInitialiseData;

    vertexInitialiseData.pSysMem = meshData;
    HR( d3dDevice->CreateBuffer( &vertexBufferDesc, &vertexInitialiseData, &vertexBuffer ) );
    
    if( indexCount != -1 ) {
        if( indexBuffer ) {
            indexBuffer->Release( );
        }
        // Create the index buffer
        D3D11_BUFFER_DESC indexBufferDesc;
        indexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
        indexBufferDesc.ByteWidth           = ( sizeof( U32 ) * indexCount );
        indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags      = 0;
        indexBufferDesc.MiscFlags           = 0;
        indexBufferDesc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA indexInitialiseData;
        indexInitialiseData.pSysMem = indexData;
        HR( d3dDevice->CreateBuffer( &indexBufferDesc, &indexInitialiseData, &indexBuffer ) );
    }
}

#include <fstream>
#include <vector>
void GraphicsDeviceD3D11::BuildFX( void ) {
    // borrowed from Luna book, go back to run-time compilation at some point?
    std::ifstream fin( "Rendering/Shaders/RtLightingShaderWithTextureMapping.fxo", std::ios::binary );

    fin.seekg( 0, std::ios_base::end );
    U32 size = static_cast<U32>( fin.tellg( ) );
    fin.seekg( 0, std::ios_base::beg );
    std::vector<I8> compiledShader( size );

    fin.read( &compiledShader[0], size );
    fin.close( );

    HR(D3DX11CreateEffectFromMemory( &compiledShader[0], size, 
        0, d3dDevice, &mFX ) );

    mTech            = mFX->GetTechniqueByName("ColorTech");
    mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix( );

    // added for lighting (directional light)
    mfxWorldMatrix         = mFX->GetVariableByName( "worldMatrix" )->AsMatrix( );
    mfxLightAmbientColour  = mFX->GetVariableByName( "lightAmbientColour" )->AsVector( );
    mfxLightDiffuseColour  = mFX->GetVariableByName( "lightDiffuseColour" )->AsVector( );
    mfxLightSpecularColour = mFX->GetVariableByName( "lightSpecularColour" )->AsVector( );
    mfxLightSpecularPower  = mFX->GetVariableByName( "lightSpecularPower" )->AsScalar( );
    mfxLightDirection      = mFX->GetVariableByName( "lightDirection" )->AsVector( );

    mfxCameraPosition      = mFX->GetVariableByName( "cameraPosition" )->AsVector( );

    // added for texturing
    mfxDiffuseMap          = mFX->GetVariableByName( "textureMap" )->AsShaderResource( );
}

void GraphicsDeviceD3D11::BuildVertexLayout( void ) {
    if( this->inputLayout ) {
        inputLayout->Release( );
    }

    // Create the vertex input layout.
    D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        //{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    U32 vertexElementCount = ARRAYSIZE( vertexDesc );

    // Create the input layout
    D3DX11_PASS_DESC passDesc;
    mTech->GetPassByIndex( 0 )->GetDesc( &passDesc );
    HR( d3dDevice->CreateInputLayout( vertexDesc, vertexElementCount, passDesc.pIAInputSignature, 
                                      passDesc.IAInputSignatureSize, &inputLayout) );
}

/*
================
GraphicsDeviceD3D11::CreateRenderStates

Use FrontCounterClockwise || CullMode to counter the rightHanded
mesh property?
================
*/
bool GraphicsDeviceD3D11::CreateRenderStates( void ) {
    D3D11_RASTERIZER_DESC rasterizerDesc;
    D3D11_DEPTH_STENCIL_DESC stencilDesc;

    // left handed solid render state
    ZeroMemory( &rasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );

    rasterizerDesc.FillMode    = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode    = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthClipEnable = true;

    HR( d3dDevice->CreateRasterizerState( &rasterizerDesc, &solidRenderStateLeftHanded ) );


    // right handed slid render state
    ZeroMemory( &rasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );

    rasterizerDesc.FillMode    = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode    = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = true;
    rasterizerDesc.DepthClipEnable = true;

    HR( d3dDevice->CreateRasterizerState( &rasterizerDesc, &solidRenderStateRightHanded ) );

    // left handed wireframe render state
    ZeroMemory( &rasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );

    rasterizerDesc.FillMode    = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode    = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthClipEnable = true;

    HR( d3dDevice->CreateRasterizerState( &rasterizerDesc, &wireFrameRenderStateLeftHanded ) ); 

    // right handed wireframe render state
    ZeroMemory( &rasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );

    rasterizerDesc.FillMode    = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode    = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = true;
    rasterizerDesc.DepthClipEnable = true;

    HR( d3dDevice->CreateRasterizerState( &rasterizerDesc, &wireFrameRenderStateRightHanded ) ); 

    // stencil shadow stenil state
    ZeroMemory( &stencilDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );

    stencilDesc.DepthEnable = true;
    stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    stencilDesc.StencilEnable = true;
    stencilDesc.StencilReadMask = 0xff;
    stencilDesc.StencilWriteMask = 0xff;

    stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
    stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    // ... FINISH...

    return true;
}

/*
================
GraphicsDeviceD3D11::SetRenderState

Set the device render state to that specified by the current material
================
*/
void GraphicsDeviceD3D11::SetRenderState( MATERIAL_RENDER_STATE renderState ) {
    switch( renderState ) {
    case MATERIAL_RENDER_STATE::SOLID_RH:
        immediateContext->RSSetState( solidRenderStateRightHanded );
        break;

    case MATERIAL_RENDER_STATE::WIREFRAME_LH:
        immediateContext->RSSetState( wireFrameRenderStateLeftHanded );
        break;

    default:
        break;
    }
}

/*
================
GraphicsDeviceD3D11::SetDirectionalLightShaderParams

TEMP?
================
*/
void GraphicsDeviceD3D11::SetDirectionalLightShaderParams( const DirectionalLight *directionalLight ) {
    if( isRunning == false ) {
        return;
    }

    HRESULT hr = mfxLightAmbientColour->SetFloatVector( &directionalLight->ambientColour[0] );
    hr = mfxLightDiffuseColour->SetFloatVector( &directionalLight->diffuseColour[0] );
    hr = mfxLightSpecularColour->SetFloatVector( &directionalLight->specularColour[0] );
    hr = mfxLightSpecularPower->SetRawValue( reinterpret_cast<void*>( const_cast<F32*>( &directionalLight->specularPower ) ), 0, sizeof( F32 ) );
    hr = mfxLightDirection->SetFloatVector( &directionalLight->direction[0] );
}

GraphicsDevice* CreateGraphicsDeviceFromHeap( HeapAllocator<GraphicsDevice> &allctr ) {
    return CreateGraphicsDevice( allctr );
}

void DestroyGraphicsDeviceFromHeap( GraphicsDevice *destroyThis, HeapAllocator<GraphicsDevice> &allctr ) {
    DestroyGraphicsDevice( destroyThis, allctr );
}
