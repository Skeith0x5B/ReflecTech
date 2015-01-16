/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    Texture Mapping Shader for 3D Objects

	- Modified/updated/upgraded by Jamie Taylor
*/


Texture2D colorMaps[6] : register( t0 );
SamplerState colorSampler : register( s0 );


cbuffer cbChangesEveryFrame : register( b0 )
{
    matrix worldMatrix;
};

cbuffer cbNeverChanges : register( b1 )
{
    matrix viewMatrix;
};

cbuffer cbChangeOnResize : register( b2 )
{
    matrix projMatrix;
};


struct VS_Input
{
    float4 pos  : POSITION;
    float2 tex0 : TEXCOORD0;
	// add another texture field here so the top and bottom faces of the barrel model
	// can have a separate texture/material?
};

struct PS_Input
{
    float4 pos  : SV_POSITION;
    float2 tex0 : TEXCOORD0;
	// add another texture field here so the top and bottom faces of the barrel model
	// can have a separate texture/material?
};


PS_Input VS_Main( VS_Input vertex )
{
    PS_Input vsOut = ( PS_Input )0;
    vsOut.pos = mul( vertex.pos, worldMatrix );
    vsOut.pos = mul( vsOut.pos, viewMatrix );
    vsOut.pos = mul( vsOut.pos, projMatrix );
    vsOut.tex0 = vertex.tex0;
	//vsOut.tex1 = vertex.tex1

    return vsOut;
}


float4 PS_Main( PS_Input frag ) : SV_TARGET
{
	float4 color1;
	float4 color2;
	float4 color3 = 1;
	float4 color4 = 1;
	float4 color5 = 1;
	float4 color6 = 1;
	float4 finalColor;

	// sample the first texture
	color1 = colorMaps[0].Sample( colorSampler, frag.tex0 );

	// sample the second texture
	color2 = colorMaps[1].Sample( colorSampler, frag.tex0 );

	// sample the third texture
	color3 = colorMaps[2].Sample( colorSampler, frag.tex0 );

	// use the tex1 field to set these?
	color4 = colorMaps[3].Sample( colorSampler, frag.tex0 );
	color5 = colorMaps[4].Sample( colorSampler, frag.tex0 );
	//color6 = colorMaps[5].Sample( colorSampler, frag.tex0 );

	// calculate and return the final blended pixel color
	finalColor = color1;// * 2.0;// color2 * color3 * color4 * color5 * color6 * 2.0;
	// saturate the final color
	finalColor = saturate(finalColor);

    return finalColor; //colorMaps[0].Sample( colorSampler, frag.tex0 );
}