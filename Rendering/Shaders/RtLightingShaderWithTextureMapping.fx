/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtLightingShaderWithTextureMapping.fx
    Author        :    Jamie Taylor
    Last Edit    :    08/08/13
    Desc        :    The lighting shader with basic texture mapping logic of added.

===============================================================================
*/


// Nonnumeric values cannot be added to a cbuffer.
//Texture2D textureMaps[16];// : register( t0 );
Texture2D textureMap;

SamplerState anisotropicFilter {
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;

    AddressU = WRAP;
    AddressV = WRAP;
};


/*
================
Constant buffers

Constant buffers must be multiples of 16 (4?) bytes or CreateBuffer( ) will fail.
================
*/
cbuffer cbPerObject {
    float4x4 worldMatrix;
    float4x4 gWorldViewProj; 
};

cbuffer cbLight {
    float4 lightAmbientColour;
    float4 lightDiffuseColour;    
    float4 lightSpecularColour;
    float  lightSpecularPower;
    float3 lightDirection;
};

// We need the camera position to work out where the vertex is being viewed from.
cbuffer cbCamera {
    float3 cameraPosition;
    float  padding;
};

/*
================
Structs
================
*/
struct VertexIn {
    float3 PosL  : POSITION;
    float3 Norm  : NORMAL;
    float4 Color : COLOR;
    float3 Tex     : TEXCOORD0;
};

struct VertexOut {
    float4 PosH             : SV_POSITION;
    float3 Norm             : NORMAL;
    float4 Color         : COLOR;
    float3 Tex             : TEXCOORD0;
    float3 viewDirection : TEXCOORD1; // TEXCOORD1?
};

/*
================
Vertex Shader
================
*/
VertexOut VS( VertexIn vin ) {
    VertexOut vout;
    float4 vertexWorldPosition;
    
    // Texture coordinates
    vout.Tex = vin.Tex;

    // Transform to homogeneous clip space.
    vout.PosH = mul( float4( vin.PosL, 1.0f ), gWorldViewProj );
    
    // Put the normal into world space and normalise, will then be part
    // of the lighting equation in the pixel shader.
    vout.Norm = mul( vin.Norm, (float3x3)worldMatrix );
    vout.Norm = normalize( vout.Norm );

    // Calculate the viewing direction for use in the pixel shader.
    vertexWorldPosition = mul( vin.PosL, worldMatrix );

    // cameraPos - vertexWorldPositon = viewingDirection
    vout.viewDirection = cameraPosition.xyz - vertexWorldPosition.xyz;

    // Normalize and send to pixel shader.
    vout.viewDirection = normalize( vout.viewDirection );
    
    // Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;

    return vout;
}

/*
================
Pixel Shader
================
*/
float4 PS( VertexOut pin ) : SV_Target {
    float4 finalColour;
    float4 specular;
    float3 lightDir;
    float  lightIntensity;


    // Sample the textures.
    float4 textureSample1 = 1.0f;
    textureSample1 = textureMap.Sample( anisotropicFilter, pin.Tex );
    

    // Set the default output color to the ambient light value for all pixels.
    finalColour = lightAmbientColour;

    // Initialize the specular color.
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate( dot( pin.Norm, lightDir ) );

    if( lightIntensity > 0.0f ) {
        // Determine the final diffuse colour based on the diffuse color and the light intensity.
        finalColour += ( lightDiffuseColour * lightIntensity );

        // Saturate the ambient and diffuse colour.
        finalColour = saturate( finalColour );
    }

    finalColour *= textureSample1;

    return finalColour; //pin.Color;
}

/*
================
Techniques
================
*/
technique11 ColorTech {
    pass P0 {
        SetVertexShader( CompileShader( vs_5_0, VS( ) ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS( ) ) );
    }
}
