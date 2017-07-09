#version 450 core

layout (binding = 0) uniform sampler2D colorTexture; 

uniform vec2 u_texelStep = vec2(1.0 / 1280.0, 1.0 / 720.0);
uniform int u_showEdges = 0;
uniform int u_fxaaOn = 1;

uniform float u_lumaThreshold = 0.5;
uniform float u_mulReduce = 1.0 / 8.0;
uniform float u_minReduce = 1.0 / 128.0;
uniform float u_maxSpan = 8.0;

const float gamma = 1.8;

in vec2 passUV;

layout (location = 0) out vec4 fragColor;

// see FXAA
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
// http://iryoku.com/aacourse/downloads/09-FXAA-3.11-in-15-Slides.pdf
// http://horde3d.org/wiki/index.php5?title=Shading_Technique_-_FXAA

vec3 fxaa() {
	vec3 rgbM = texture(colorTexture, passUV).rgb;

	// Possibility to toggle FXAA on and off.
	if (u_fxaaOn == 0) {
		return rgbM;
	}

	// Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates. 
	vec3 rgbNW = textureOffset(colorTexture, passUV, ivec2(-1, 1)).rgb;
    vec3 rgbNE = textureOffset(colorTexture, passUV, ivec2(1, 1)).rgb;
    vec3 rgbSW = textureOffset(colorTexture, passUV, ivec2(-1, -1)).rgb;
    vec3 rgbSE = textureOffset(colorTexture, passUV, ivec2(1, -1)).rgb;

	// see http://en.wikipedia.org/wiki/Grayscale
	const vec3 toLuma = vec3(0.299, 0.587, 0.114);
	
	// Convert from RGB to luma.
	float lumaNW = dot(rgbNW, toLuma);
	float lumaNE = dot(rgbNE, toLuma);
	float lumaSW = dot(rgbSW, toLuma);
	float lumaSE = dot(rgbSE, toLuma);
	float lumaM = dot(rgbM, toLuma);

	// Gather minimum and maximum luma.
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	
	// If contrast is lower than a maximum threshold ...
	if (lumaMax - lumaMin < lumaMax * u_lumaThreshold) {
		// ... do no AA and return.
		return rgbM;
	}  
	
	// Sampling is done along the gradient.
	vec2 samplingDirection;	
	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.  
    float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * u_mulReduce, u_minReduce);

	// Factor for norming the sampling direction plus adding the brightness influence. 
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
    
    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
    samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-u_maxSpan, -u_maxSpan), vec2(u_maxSpan, u_maxSpan)) * u_texelStep;
	
	// Inner samples on the tab.
	vec3 rgbSampleNeg = texture(colorTexture, passUV + samplingDirection * (1.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(colorTexture, passUV + samplingDirection * (2.0/3.0 - 0.5)).rgb;

	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;  

	// Outer samples on the tab.
	vec3 rgbSampleNegOuter = texture(colorTexture, passUV + samplingDirection * (0.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(colorTexture, passUV + samplingDirection * (3.0/3.0 - 0.5)).rgb;
	
	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;   
	
	// Calculate luma for checking against the minimum and maximum value.
	float lumaFourTab = dot(rgbFourTab, toLuma);

	vec3 result;

	// Are outer samples of the tab beyond the edge ... 
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax) {
		// ... yes, so use only two samples.
		result = rgbTwoTab;
	} else {
		// ... no, so use four samples. 
		result =  rgbFourTab;
	}

	// Show edges for debug purposes.	
	if (u_showEdges != 0) {
		result.r = 1.0;
	}

	return result;
} 

void main(void) {
	vec3 cl = fxaa();

	fragColor = vec4(pow(cl, vec3(1.0) / gamma), 1.0);

}