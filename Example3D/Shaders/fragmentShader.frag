#version 410

in vec4 vPosition;
in vec4 vWorldNormal;
in vec2 vUV;
in vec3 vTangent;
in vec3 vBiTangent;
out vec4 fragColor;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform float lightIntensity;
uniform vec3 cameraPos;
uniform float specPow;
uniform sampler2D tex;
uniform sampler2D normal;
uniform sampler2D specular;


void main()
{
	mat3 TBN = mat3(normalize(vTangent),
					normalize(vBiTangent),
					normalize(vWorldNormal));

	// normal in local tangent-normal space
	vec3 N = texture(normal, vUV).rgb * 2 - 1;
	
	// normal in worldspace after being perturbed by the normal maop
	vec3 localNormal = normalize(TBN * N);

	float d = max(0,dot(localNormal, normalize(lightDirection))) + 0.1;
	//float d = max(0,dot(normalize(vWorldNormal.xyz), lightDirection)) + 0.1;
	// getting vector to camera

	//vec2 uv = vWorldNormal.xy;
	vec4 texColour = texture(tex, vUV);

	vec4 spec = texture(specular, vUV);

	// Directin towards camera
	vec3 E = normalize( cameraPos - vPosition.xyz);
	vec3 R = reflect( normalize(-lightDirection), localNormal.xyz);
	float s =  max(0,dot(E,R));
	s = pow(s, specPow);

	vec4 finalDiffuse = texColour * d;

	vec4 finalColourLight = vec4(lightColour, 1);

	vec4 finalSpecular = vec4(spec.xyz * s, 1);

	// get the intensity
	//float intensity = clamp(dot(light, vColour), 0,1) + 0.1;

	//fragColor = vec4(lightColour * d + lightColour * s, 1);s

	fragColor = (finalDiffuse * finalColourLight + finalSpecular) * lightIntensity;

	fragColor = vec4(fragColor.xyz,1);
}