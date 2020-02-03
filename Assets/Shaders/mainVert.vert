#version 420 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uvs;
layout (location = 2) in vec3 norms;
layout (location = 3) in vec3 tangs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 tViewDir;
out vec2 texCoords;
out mat3 TBN;

void main()
{
	texCoords = uvs;

	mat4 temp = view*model;
    vec4 temp4 = temp*(vec4(pos,1.0));
	tViewDir = -temp4.xyz;
	fragPos = vec3(model * vec4(pos, 1.0));

	vec3 T = normalize(vec3(model* vec4(tangs,0.0)));
    vec3 N = normalize(vec3(model* vec4(norms,0.0)));
    vec3 B = normalize(cross(T,N));
    TBN = mat3(T,B,N);

	gl_Position = projection * view * model * vec4(pos, 1.0);
}