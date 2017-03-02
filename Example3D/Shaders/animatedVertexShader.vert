#version 410
layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec2 uv;
layout(location=3) in vec4 tangent;
layout(location=4) in vec4 weights;
layout(location=5) in vec4 indices;

out vec4 vPosition;
out vec4 vWorldNormal;
out vec2 vUV;
out vec3 vTangent; // worldspace
out vec3 vBiTangent; // worldspace


uniform mat4 MVP;
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

	vec4 norm = vec4(normal.xyz, 0);
	vec4 N = bones[ index.x ] * norm * weights.x;
	N += bones[ index.y ] * norm * weights.y;
	N += bones[ index.z ] * norm * weights.z;
	N += bones[ index.w ] * norm * weights.w;

	vec4 tang = vec4(tangent.xyz, 0);
	vec4 T = bones[ index.x ] * tang * weights.x;
	T += bones[ index.y ] * tang * weights.y;
	T += bones[ index.z ] * tang * weights.z;
	T += bones[ index.w ] * tang * weights.w;

	vPosition = m * P;
	vWorldNormal = normalize(m * N);
	vUV = uv;

	vTangent = normalize(m * T).xyz;
	vBiTangent = cross(vWorldNormal.xyz, vTangent.xyz);

	gl_Position = MVP * P;
}