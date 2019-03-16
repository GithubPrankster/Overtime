#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float exposure;

void main()
{         
    
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
	
	//Luma exposure correction
	vec3 luma = vec3(0.2126, 0.7152, 0.0722);
	float lumaExposure = exposure * dot(luma, hdrColor);

	// Exposure-based result 
	vec3 result = vec3(1.0) - exp(-hdrColor * lumaExposure);
	// Gamma correction      
	result = pow(result, vec3(1.0 / gamma));
	
	FragColor = vec4(result, 1.0);

}