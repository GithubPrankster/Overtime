#version 330 core
out vec4 FragColor;

in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec3 fragPos;

uniform sampler2D diffuse;
uniform sampler2D specularMap;
uniform sampler2D depthMap;
uniform vec3 cool;
 
uniform int numLights;
uniform vec3 lightPos[16];
uniform vec3 lightColor[16];
uniform float strength;

uniform vec3 viewPos;
uniform float specularStrength;

uniform mat4 lightSpaceMatrix;

vec3 lightCalculation(vec3 lightPos, vec3 lightDir,vec3 lightColor, vec3 normalThing){
	//Diffuse shade
	float diff = max(dot(normalThing, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	vec3 viewDir    = normalize(viewPos - fragPos);
	
	//Calculate energy conservation and specularity. Apparently these lifeless blocks have energy!
	float pi = 3.14159265, shine = 32.0;
	float specularConservation = ( 8.0 + shine) / ( 8.0 * pi); 
	vec3 halfwayDir = normalize(lightDir + viewDir); 
	float spec = specularConservation * pow(max(dot(normalThing, halfwayDir), 0.0), shine);

	vec3 specular = specularStrength * spec * lightColor * vec3(texture(specularMap, vertexTexCoord));
	
	//Attenuation calculations...
	float distance    = length(lightPos - fragPos);
	float attenuation = 1.0 / (1.0f + 0.09f * distance + 0.032f * (distance * distance));   
	
	//Multiply everything by it. So light actually behaves like uh, light.
	diffuse  *= attenuation;
	specular *= attenuation; 
	
	//Oh right, return result.
	vec3 result = diffuse + specular;
	return result;
}

void main()
{
	//Main color ting
	vec3 color = texture(diffuse, vertexTexCoord).rgb;
	vec3 normalFrag = normalize(vertexNormal);
	vec3 ambient = strength * vec3(0.6);
	vec3 fullResult;
	
	for(int i = 0; i < numLights; i++){
		vec3 lightDir = normalize(lightPos[i]- fragPos);
		vec3 fullLight = lightCalculation(lightPos[i], lightDir, lightColor[i], normalFrag);
		fullResult += fullLight;
	}
	
	//vec3 dirMethod = (vec3(0.4,0.6,0.9) * max(dot(normalFrag, vec3(0.3,0.4,0.1)), 0));
	vec3 result = ((ambient + fullResult) * color);
	
	FragColor = vec4(cool, 1.0) * vec4(result, 1.0);
}  