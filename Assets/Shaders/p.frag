#version 420 core
		out vec4 FragColor;
  		
		in vec2 TexCoords;
		
		uniform sampler2D screenTexture;
		uniform sampler2D bloom;
		uniform sampler3D LUT1;
		uniform sampler3D LUT2;
		uniform sampler3D LUT3;
		uniform sampler3D LUT4;

		uniform int use1;
		uniform int use2;
		uniform int use3;
		uniform int useBloom;

		const float offset = 1.0 / 500.0; 

		//kernel code is from learnopengl, as for bloom we used a combination of our own findings and learnopengl's tutorial
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

			// for(int k = 0; k < 2; k++)
				 for(int i = 0; i < 9; i++)
					fBloom += sampleTex[i] * kernel[i]/16.0f;

			//this is HDR
			fBloom = vec3(1.0) - exp(-fBloom*1.0f);//1 is exposure

			col += fBloom*useBloom;    

			//this is HDR
			col = vec3(1.0) - exp(-col*1.0f);//1 is exposure
			vec4 Lcol1 = texture(LUT1,col)*use1;
			vec4 Lcol2 = texture(LUT2,col)*use2;
			vec4 Lcol3 = texture(LUT3,col)*use3;
			vec4 fColour = Lcol1 + Lcol2 + Lcol3;	
			if(fColour == vec4(0.0f,0.0f,0.0f,0.0f))
				fColour = texture(LUT4,col);

		    FragColor = fColour;

		}