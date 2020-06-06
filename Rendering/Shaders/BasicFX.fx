/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtPassThroughShader.fx
    Author        :    Jamie Taylor
    Last Edit    :    30/06/13
    Desc        :    Basic shader, geometry passes through unchagned.

===============================================================================
*/


/*
===============================================================================

Constant buffers

===============================================================================
*/
//cbuffer cbUpdatedPerFrame {
    
//};

cbuffer cbUpdatedPerObject {
    float4x4 gWorldViewProjMatrix;
};

//cbuffer cbUpdatedRarely {

//};


/*
===============================================================================

Structures

===============================================================================
*/
struct VertexShaderInput {
    // map to input layout described in C++ code
    float3 pos    : POSITION;
    float4 colour : COLOUR;
};

struct VertexShaderOutput {
    float4 posH      : SV_POSITION;
    float4 colour : COLOUR;
};


/*
===============================================================================

Vertex Shader

===============================================================================
*/
VertexShaderOutput VertexShaderMain( VertexShaderInput vsIn ) {
    VertexShaderOutput vsOut;

    // just transform to projection space
    vsOut.posH = mul( float4( vsIn.pos, 1.0f ), gWorldViewProjMatrix );

    // just pass colour along to pixel shader
    vsOut.colour = vsIn.colour;

    return vsOut;
}

/*
===============================================================================

Pixel Shader

===============================================================================
*/
float4 PixelShaderMain( VertexShaderOutput psIn ) : SV_TARGET {
    // just pass on the vertex colour unchanged
    return psIn.colour;
}
