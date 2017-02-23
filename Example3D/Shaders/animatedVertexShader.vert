#version 410
layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 uv;
layout(location=3) in vec4 tangent;
layout(location=4) in vec4 weights;
layout(location=5) in vec4 indices;

out vec4 vNormal;
out vec4 vPosition;
out vec4 vWorldNormal;
out vec2 vUV;


uniform mat4 projectionViewWorldMatrix;
uniform mat4 m;

const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

void main()
{
	ivec4 index = ivec4(indices);

	vec4 P = bones[index.x] * position * weights.x;
	P += bones[ index.y ] * position * weights.y;
	P += bones[ index.z ] * position * weights.z;
	P += bones[ index.w ] * position * weights.w;

	vNormal = normal;
	vPosition = m * P;
	vWorldNormal = normalize(m * vec4(normal.xyz, 0));
	vUV = uv;

	gl_Position = projectionViewWorldMatrix * P;
}