#version 420 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloom;


uniform float greyscalePercentage = 1;
	const float offset = 1.0 / 300.0; 
void main()
{
    vec3 col = vec3(texture(screenTexture, TexCoords.st));
	
vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
        1,	2,	1,
		2,	4,	2,
		1,	2,	1
    );
    
    vec3 sampleTex[9];
	   for(int i = 0; i < 9; i++)
		{
			sampleTex[i] = vec3(texture(bloom,TexCoords.st + offsets[i]));
		}
	vec3 fBloom = vec3(0.0f);

	
    for(int i = 0; i < 9; i++)
        fBloom += sampleTex[i] * kernel[i]/16.0f;

	fBloom = vec3(1.0) - exp(-fBloom*1.0);//1 is exposure

	col += fBloom;    

	//this is HDR
	col = vec3(1.0) - exp(-col*1.0);//1 is exposure
	//this is HDR


	float average = 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
	vec3 grey = vec3(average,average,average).xyz;
	vec3 finalColour = mix(grey,col,greyscalePercentage);
    FragColor = vec4(finalColour, 1.0);
}