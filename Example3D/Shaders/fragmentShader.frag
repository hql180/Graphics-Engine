#version 410
in vec4 vNormal;
in vec4 vPosition;
in vec4 vWorldNormal;
in vec2 vUV;
out vec4 fragColor;

uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform float lightIntensity;
uniform vec3 cameraPos;
uniform float specPow;
uniform sampler2D tex;


void main()
{
	float d = max(0,dot(normalize(vWorldNormal.xyz), lightDirection)) + 0.1;
	vec3 E = normalize( cameraPos - vPosition.xyz);
	vec3 R = reflect( -lightDirection, vNormal.xyz);
	float s = max(0,dot(E,R));
	s = pow(s, specPow);

	//vec2 uv = vWorldNormal.xy;
	vec4 texColour = texture(tex, vUV);

	// get the intensity
	//float intensity = clamp(dot(light, vColour), 0,1) + 0.1;

	//fragColor = vec4(lightColour * d + lightColour * s, 1);

	fragColor = texColour * d + vec4(lightColour * (d * lightIntensity) + lightColour * s, 1);
}