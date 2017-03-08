#version 410
in vec2 vTexCoord;
out vec4 FragColour;

uniform sampler2D target;

vec4 simple()
{
	return texture(target, vTexCoord);
}

vec4 boxBlur()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;

	// 9-tap box kernel
	vec4 colour = texture(target, vTexCoord);
	colour += texture(target, vTexCoord + vec2(-texel.x, texel.y));
	colour += texture(target, vTexCoord + vec2(-texel.x, 0));
	colour += texture(target, vTexCoord + vec2(-texel.x, -texel.y));
	colour += texture(target, vTexCoord + vec2(0, texel.y));
	colour += texture(target, vTexCoord + vec2(0, -texel.y));
	colour += texture(target, vTexCoord + vec2(texel.x, texel.y));
	colour += texture(target, vTexCoord + vec2(texel.x, 0));
	colour += texture(target, vTexCoord + vec2(texel.x, -texel.y)); 

	return colour / 9;
}

vec4 distort()
{
	vec2 mid = vec2(0.5f);

	float distanceFromCentre = distance(vTexCoord, mid);

	vec2 normalizedCoord = normalize(vTexCoord - mid);

	float bias = distanceFromCentre +

	sin(distanceFromCentre * 15) * 0.02f;

	vec2 newCoord = mid + bias * normalizedCoord;

	return texture(target, newCoord);
}

void main()
{
	FragColour =  mix(boxBlur(), distort(), boxBlur().a);
	//FragColour = distort();
}