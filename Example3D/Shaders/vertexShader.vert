#version 410
layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 uv;

out vec4 vNormal;
out vec4 vPosition;
out vec4 vWorldNormal;
out vec2 vUV;


uniform mat4 MVP;
uniform mat4 m;

void main()
{
	vNormal = normal;
	vPosition = position;
	vWorldNormal = m * vec4(normal.xyz, 0);
	vUV = uv;
	gl_Position = MVP * position;
}