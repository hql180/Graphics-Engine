#version 410

in vec4 vPosition;
in vec4 vWorldNormal;
in vec2 vUV;
in vec3 vTangent;
in vec3 vBiTangent;
out vec4 fragColor;

uniform vec3 lightDirection;

uniform vec3 cameraPos;
uniform float specPow;
uniform sampler2D tex;
uniform sampler2D normal;
uniform vec4 baseCol;
uniform vec4 brightCol;

uniform sampler2D scribble1;
uniform sampler2D scribble2;
uniform sampler2D scribble3;
uniform sampler2D scribble4;

void main()
{
	mat3 TBN = mat3(normalize(vTangent),
					normalize(vBiTangent),
					normalize(vWorldNormal));

	// normal in local tangent-normal space
	vec3 N = texture(normal, vUV).rgb * 2 - 1;
	
	// normal in worldspace after being perturbed by the normal maop
	vec3 localNormal = normalize(TBN * N);

	float d = max(0,dot(localNormal, normalize(lightDirection)));
	//float d = max(0,dot(normalize(vWorldNormal.xyz), lightDirection)) + 0.1;
	// getting vector to camera

	float uvscale = 50;
	vec4 col = texture(scribble1, vUV*uvscale);
	if (d > 0.2)
	{
		col = texture(scribble2, vUV*uvscale);
	}
	if (d > 0.6)
	{
		col = texture(scribble3, vUV*uvscale);
	}
	if (d > 0.95)
	{
		col = texture(scribble4, vUV*uvscale);
	}

	col = baseCol + col * (brightCol-baseCol);
	col.w = 1;
	//if (d > 0.2)
	//{
	//	col = texture(scribble2, vUV*3.5);
	//}
	//if (d > 0.6)
	//{
	//	col = texture(scribble3, vUV*2.7);
	//}
	//if (d > 0.95)
	//{
	//	col = texture(scribble4, vUV*1.3);
	//}

	fragColor = col;
}