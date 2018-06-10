uniform sampler2D texture1;
uniform sampler2D causticsTexture;
uniform vec3 precomputedLighting; // since flat terrain is just one quad, directional lighting is precomputed
uniform vec3 fogColor;
uniform float time;

varying vec3 worldPos;

const float waterHeight = 15.0;

void main()
{
    vec2 texture1Pos = gl_TexCoord[0].xy * 3.0;

    // get texture color
	
	vec3 texture = texture2D(texture1, texture1Pos).rgb;
	
	// calculate fog and final color
	
	vec3 fin = texture * precomputedLighting;
	
  if(worldPos.y < waterHeight)
    fin += texture2D(causticsTexture, vec2(gl_TexCoord[0].x, gl_TexCoord[0].y + time / 800000.0 * 30.0)).rgb;
    
  float fogFactor = clamp((gl_FragCoord.z / gl_FragCoord.w - gl_Fog.start) * gl_Fog.scale, 0.0, 1.0);
	
	gl_FragColor = vec4(mix(fin, fogColor, fogFactor), 1.0);
}
