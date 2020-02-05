#version 420 core
layout (location = 0) out vec4 outColour;
layout (location = 1) out vec4 outBright;

 struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normalMap;
    sampler2D emissionMap;
    float     shininess;
}; 

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform int numLights;
#define MAX_LIGHTS 10
uniform PointLight pointLight[MAX_LIGHTS];
uniform Material material;
in vec3 fragPos; 
in vec2 texCoords;
in vec3 tViewDir;
in mat3 TBN;
in mat4 outView;

uniform int ourambient;
uniform int ourspecular;
uniform int ourdiffuse;
uniform int ourrim;
uniform int ourdiffuseRampShading;
uniform int ourspecularRampShading;
uniform int ourwarm;
uniform int ourcool;
uniform int ourcustom;

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 vD);  

vec4 calculateRimLight(PointLight light,vec3 fragPos,vec3 norm);
void main()
{
	
	vec3 norm = texture(material.normalMap,texCoords).rgb;
	norm = normalize(norm*2.0-1.0);
	norm = normalize(TBN*norm);
	vec3 fViewDir = normalize(tViewDir);


	vec3 light = vec3(0.0);
	    for(int i = 0 ; i < numLights;i++){
	        light += calculatePointLight(pointLight[i],norm,fragPos,fViewDir);
            light += vec3(calculateRimLight(pointLight[i],fragPos,norm)).xyz * ourrim;
        }

	vec3 emission = texture(material.emissionMap,texCoords).rgb;
	light += emission;

	outColour = vec4(light, 1.0);

    float brightness = dot(outColour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.5f)//bloom threshold
        outBright = vec4(outColour.rgb, 1.0);
    else
        outBright = vec4(0.0, 0.0, 0.0, 1.0);

}

vec4 calculateRimLight(PointLight light,vec3 fragPos,vec3 norm){
    //rim lighting
    float rim = 1.0f - max(dot(norm,normalize(tViewDir)),0.0f);
    rim = smoothstep(0.9f,1.0f,rim);
    rim = rim / 5.0f;//scale it so it doesn't make everything white
    
    float distance = length(light.position - fragPos);
   	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    rim*=attenuation;

    return vec4(light.diffuse*rim,1.0f); 
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos,vec3 vD) {

  	vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0f);

    //TODO
    //texture1D yeet;
    //texture1D temp =(yeet,newDiff);


    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(vD, reflectDir), 0.0f), material.shininess);
    
    // attenuation
   	float distance = length(light.position - fragPos);
   	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

    ambient*=attenuation;
    diffuse*=attenuation;
    specular*=attenuation;

    return ((ambient*ourambient) + (diffuse*ourdiffuse) + (specular*ourspecular));
}