uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D slopeTexture;
uniform sampler2D splatMap;
uniform sampler2D normalMap;
uniform sampler2D causticsTexture;
uniform float ambientLight;
uniform vec3 directionalLightDir;
uniform vec3 directionalLightColor;
uniform vec3 fogColor;
uniform float time;

varying vec3 worldPos;

const float waterHeight = 15.0;

void main()
{
    // pos for each texture

    vec2 texture1Pos = gl_TexCoord[1].xy * 1.0 * 3.0;
    //vec2 texture2Pos = gl_TexCoord[1].xy * 5.0 * 1.0; // 3.0
    //vec2 texture2Pos_2 = gl_TexCoord[1].xy * 1.37;
    vec2 texture2Pos = gl_TexCoord[1].xy * 5.0 * 1.0;
    vec2 texture3Pos = gl_TexCoord[1].xy * 8.0 * 2.0;
    //vec2 slopeTexturePos = gl_TexCoord[1].xy * 16.0212842; // 16.0
    vec2 slopeTexturePos = gl_TexCoord[1].xy * 8.0212842;
    //vec2 slopeTexturePos_2 = gl_TexCoord[1].xy * 2.17395205; // 2.0
    vec2 slopeTexturePos_2 = gl_TexCoord[1].xy * 1.342985;

    // splat's r, g, b --> texture1, texture2, texture3

    vec4 splat = texture2D(splatMap, gl_TexCoord[0].xy);

    // get textures colors and combine them

    vec3 texture1Col = texture2D(texture1, texture1Pos).rgb;
    //vec3 texture2Col = texture2D(texture2, texture2Pos).rgb * 0.5 + texture2D(texture2, texture2Pos_2).rgb * 0.5; // TODO: mix (maybe?)
    vec3 texture2Col = texture2D(texture2, texture2Pos).rgb;
    vec3 texture3Col = texture2D(texture3, texture3Pos).rgb;
    vec3 slopeCol = (texture2D(slopeTexture, slopeTexturePos).rgb + texture2D(slopeTexture, slopeTexturePos_2).rgb) * 0.5; // TODO: mix (maybe?)

    vec3 combinedThreeTextures = texture1Col * splat.r + texture2Col * splat.g + texture3Col * splat.b;

    // get normal (y and z are swapped, normally up-vector is 0,1,0)
	// note that we don't care about model transformation, assuming rotation = 0 (FIXME)

    vec3 normal = texture2D(normalMap, gl_TexCoord[0].xy).xzy * 2.0 - 1.0;
	normal.x = -normal.x;
    normal = normalize(normal);

    // calculate slope value

    float slope = (clamp(normal.y, 0.8, 0.9) - 0.8) * 10.0; // TODO
	
	// calculate directional lighting
	
	vec3 directionalLighting = ambientLight + (1.0 - ambientLight) * max(dot(normal, -directionalLightDir), 0.0) * directionalLightColor;
	
	// calculate normal shading
	
	//float normalShading = normal.y * normal.y;
  float normalShading = max(0.6, normal.y);
	
	// calculate fog and final color
	
	vec3 fin = mix(slopeCol, combinedThreeTextures, slope) * normalShading * directionalLighting;
	
  if(worldPos.y < waterHeight)
    fin += texture2D(causticsTexture, vec2(gl_TexCoord[1].x, gl_TexCoord[1].y - time / 800000.0 * 30.0)).rgb;
    
  float fogFactor = clamp((gl_FragCoord.z / gl_FragCoord.w - gl_Fog.start) * gl_Fog.scale, 0.0, 1.0);
	
	gl_FragColor = vec4(mix(fin, fogColor, fogFactor), 1.0);
}
