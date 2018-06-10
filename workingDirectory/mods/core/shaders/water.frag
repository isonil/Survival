uniform sampler2D waterTexture;
uniform sampler2D terrainHeightMap;
uniform sampler2D foamTexture;
uniform float time;
uniform vec3 fogColor;
uniform vec3 precomputedLighting;

// maxHeight is needed to transform height map texture pixel color range to world-space height
// it's actual terrain size divided by height map texture size multiplied by 255 (255 is terrain size used by Irrlicht for terrains with scale 1.0)

const float terrainScale = 500.0 / 1024.0;
const float maxHeight = terrainScale * 255.0;

// water height in world-space

const float waterHeight = 15.0;

void main()
{
	// calculate height value, it's terrain's world-space height at current fragment position
	// (it's 1.0 - fragment position, because of Irrlicht terrain generation implementation)
	
	float height = texture2D(terrainHeightMap, 1.0 - gl_TexCoord[0].xy).r * maxHeight;
	
	// base texture coord, used by both water texture and foam texture
	
	vec2 texCoord = vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + time / 800000.0);
	
	// water color
	
	vec3 waterSample1 = texture2D(waterTexture, texCoord * 400.0).rgb;
	vec3 waterSample2 = texture2D(waterTexture, texCoord * 25.0).rgb;
	
	// foam color
	
	vec3 foamSample1 = texture2D(foamTexture, texCoord * 400.0).rgb; // 350.0, 400.0
	vec3 foamSample2 = texture2D(foamTexture, texCoord * 187.0).rgb; // 50.0, 187.0
	
	// calculate coast factor, 0.0 - 1.0
	
	//float coastFactor = clamp((height - waterHeight + 7.56) * 0.2, 0.0, 1.0); //OLD VERSION (?)
	float coastFactor = clamp((height - waterHeight + 2.0) * 0.6, 0.0, 1.0);
	coastFactor *= coastFactor;
	
	// calculate final color
	
	vec3 finalColor = (waterSample1 + waterSample2) * 0.5 +
	                  (foamSample1 + foamSample2) * coastFactor;
	finalColor = min(finalColor, 1.0);
	
	// add white borders near coasts
	
	float whiteFactor = clamp((height - waterHeight + 1.44) * 0.5, 0.0, 1.0); //OLD VERSION (?)
	//float whiteFactor = clamp((height - waterHeight + 1.44) * 0.0, 0.0, 1.0);
	finalColor = mix(finalColor, vec3(1.0, 1.0, 1.0), whiteFactor);
	
  float fogFactor = clamp((gl_FragCoord.z / gl_FragCoord.w - gl_Fog.start) * gl_Fog.scale, 0.0, 1.0);
  
  vec4 fogColor4 = vec4(fogColor, 1.0);
  vec4 finalColor4 = vec4(finalColor * precomputedLighting, 0.55);
  
	gl_FragColor = mix(finalColor4, fogColor4, fogFactor);
}
