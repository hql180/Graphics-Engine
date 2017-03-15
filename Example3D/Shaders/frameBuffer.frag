#version 410
in vec2 vTexCoord;
out vec4 FragColour;

uniform sampler2D target;
uniform bool distortU;
uniform bool sharpenU;
uniform bool blurU;
uniform bool sobelU;
uniform bool radialBlurU;
uniform float iGlobalTime;
uniform vec3 lightDirection;
uniform mat4 MVP;

vec2 tempCoord;
vec4 tempColour;

//mat3 sx = mat3 ( 1, 2, 1,
//				   0, 0, 0,
//				  -1, -2, -1 );

//mat3 sy = mat3 (1, 0, -1,
//				  2, 0, -2,
//				  1, 0, -1 );

const float samples = 100;

// 2x1 hash, used to jitter samples
float hash( vec2 p )
{ 
	return fract(sin(dot(p, vec2(41, 289)))*45758.5453); 
}

vec3 lOff()
{    
    
    vec2 u = sin(vec2(1.57, 0) - iGlobalTime/2);
	u = normalize((inverse(MVP) * vec4(lightDirection,1))).xy;
	//vec3 light = normalize(lightDirection);
	//u = light.xz;
    mat2 a = mat2(u, -u.y, u.x);
    //a = mat2 (light, light.z);

    vec3 l = normalize(vec3(1.5, 1., -0.5));
	normalize((inverse(MVP) * vec4(lightDirection,1))).xyz;
    l.xz = a * l.xz;
    l.xy = a * l.xy;
    
    return l;    
}


vec4 simple()
{
	return texture(target, tempCoord);
}

vec4 sharpen()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;

	tempColour = tempColour * (11/3);
	tempColour += texture(target, tempCoord + vec2(0, texel.y)) * (-2/3);
	tempColour += texture(target, tempCoord - vec2(0, texel.y)) * (-2/3);
	tempColour += texture(target, tempCoord + vec2(texel.x, 0)) * (-2/3);
	tempColour += texture(target, tempCoord - vec2(texel.x, 0)) * (-2/3);
	tempColour.a = 1.0f;

	tempColour = tempColour / 3;

	return tempColour;
}

vec4 boxBlur()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;

	// 9-tap box kernel
	//tempColour = texture(target, tempCoord);
	tempColour += texture(target, tempCoord + vec2(-texel.x, texel.y));
	tempColour += texture(target, tempCoord + vec2(-texel.x, 0));
	tempColour += texture(target, tempCoord + vec2(-texel.x, -texel.y));
	tempColour += texture(target, tempCoord + vec2(0, texel.y));
	tempColour += texture(target, tempCoord + vec2(0, -texel.y));
	tempColour += texture(target, tempCoord + vec2(texel.x, texel.y));
	tempColour += texture(target, tempCoord + vec2(texel.x, 0));
	tempColour += texture(target, tempCoord + vec2(texel.x, -texel.y)); 

	tempColour = tempColour / 9;

	return tempColour;
}

vec2 distort()
{
	vec2 mid = vec2(0.5f);

	float distanceFromCentre = distance(vTexCoord, mid);

	vec2 normalizedCoord = normalize(vTexCoord - mid);

	float bias = distanceFromCentre +

	sin(distanceFromCentre * 15) * 0.02f;

	vec2 newCoord = mid + bias * normalizedCoord;

	//tempCoord = newCoord;

	//return texture(target, newCoord);

	return newCoord;
}

vec4 sobel()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;

	//mat3 I;
	//for (int i=0; i<3; i++) 
	//{
 //       for (int j=0; j<3; j++) 
	//	{
 //           vec3 test  = texelFetch(target, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
 //           I[i][j] = length(test); 
	//	}
	//}

	//float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]); 
	//float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);

	//float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));
	//tempColour = vec4(tempColour.xyz - vec3(g), 1.0);
	vec4 X = texture(target, tempCoord) * 0;
	X += texture(target, tempCoord + vec2(-texel.x, texel.y));
	X += texture(target, tempCoord + vec2(-texel.x, 0)) * 2;
	X += texture(target, tempCoord + vec2(-texel.x, -texel.y));
	X += texture(target, tempCoord + vec2(texel.x,  texel.y)) * -1;
	X += texture(target, tempCoord + vec2(texel.x,  0)) * -2;
	X += texture(target, tempCoord + vec2(texel.x,  -texel.y)) * -1;

	vec4 Y = texture(target, tempCoord) * 0;
	Y += texture(target, tempCoord + vec2(-texel.x, texel.y));
	Y += texture(target, tempCoord + vec2(0,		texel.y)) * 2;
	Y += texture(target, tempCoord + vec2(texel.x,  texel.y));
	Y += texture(target, tempCoord + vec2(-texel.x, -texel.y)) * -1;
	Y += texture(target, tempCoord + vec2(0,		-texel.y)) * -2;
	Y += texture(target, tempCoord + vec2(texel.x,  -texel.y)) * -1;

	vec4 temp = sqrt (pow(X, vec4(2)) + pow(Y,vec4(2)));

	//for greyscale outline
	//float intensity = (temp.x + temp.y + temp.z) /3;
	//vec4 edge = vec4(1-intensity, 1-intensity, 1-intensity, 1);
	vec4 edge = vec4(temp.x, temp.y, temp.z, 1);

	tempColour = tempColour + edge;

	//tempColour = temp;

	return tempColour;

}

vec4 radialBlur()
{
	vec2 uv = tempCoord;

	// Radial blur factors.
    //
    // Falloff, as we radiate outwards.
    float decay = 0.97; 
    // Controls the sample density, which in turn, controls the sample spread.
    float density = 0.3; 
    // Sample weight. Decays as we radiate outwards.
    float weight = 0.05; 

	// Light offset. Kind of fake. See above.
    vec3 l = normalize(lOff());

	l = (MVP * vec4(lightDirection, 0)).xyz;  

    // Offset texture position (uv - .5), offset again by the fake light movement.
    // It's used to set the blur direction (a direction vector of sorts), and is used 
    // later to center the spotlight.
    //
    // The range is centered on zero, which allows the accumulation to spread out in
    // all directions. Ie; It's radial.
    vec2 tuv =  uv - .5 - l.xy*.45;
    
    // Dividing the direction vector above by the sample number and a density factor
    // which controls how far the blur spreads out. Higher density means a greater 
    // blur radius.
    vec2 dTuv = tuv * density/samples;
    
    // Grabbing a portion of the initial texture sample. Higher numbers will make the
    // scene a little clearer, but I'm going for a bit of abstraction.
    vec4 col = texture(target, uv.xy) * .25;
    
	col = tempColour *.35;

    // Jittering, to get rid of banding. Vitally important when accumulating discontinuous 
    // samples, especially when only a few layers are being used.
    uv += dTuv*(hash(uv.xy + fract(iGlobalTime))* 2 - 1);
    
    // The radial blur loop. Take a texture sample, move a little in the direction of
    // the radial direction vector (dTuv) then take another, slightly less weighted,
    // sample, add it to the total, then repeat the process until done.
    for(float i=0.; i < samples; i++){
    
        uv -= dTuv;
        col += texture(target, uv) * weight;
        weight *= decay;
        
    }
    
    // Multiplying the final color with a spotlight centered on the focal point of the radial
    // blur. It's a nice finishing touch... that Passion came up with. If it's a good idea,
    // it didn't come from me. :)
    col *= (1. - dot(tuv, tuv)*.70);
    
    // Smoothstepping the final color, just to bring it out a bit, then applying some 
    // loose gamma correction.
    vec4 fragColor = sqrt(smoothstep(0, 1, col));
    
    // Bypassing the radial blur to show the raymarched scene on its own.
    //fragColor = sqrt(texture(iChannel0, fragCoord.xy / iResolution.xy));
	return fragColor *.9f;

}

void main()
{
	tempCoord = distortU ? distort() : vTexCoord;
	tempColour = simple();	
	tempColour = sobelU ? sobel() : tempColour;
	tempColour = blurU ? boxBlur() : tempColour;	
	tempColour = sharpenU ? sharpen() : tempColour;
	tempColour = radialBlurU ? radialBlur() : tempColour;
	
	FragColour = tempColour;

	

	//FragColour = radialBlur();

	FragColour.a = 1.0f;
}