uniform sampler2D texture;
uniform vec2 sampleStep;
uniform vec3 color;

void main()
{
    vec4 c = texture2D(texture, gl_TexCoord[0].xy);
    vec4 n = texture2D(texture, gl_TexCoord[0].xy + vec2(-sampleStep.x, -sampleStep.y));
    vec4 s = texture2D(texture, gl_TexCoord[0].xy + vec2(-sampleStep.x, sampleStep.y));
    vec4 w = texture2D(texture, gl_TexCoord[0].xy + vec2(sampleStep.x, -sampleStep.y));
    vec4 e = texture2D(texture, gl_TexCoord[0].xy + vec2(sampleStep.x, sampleStep.y));
    
    vec4 sampled = -4.0 * c + n + s + w + e;
    
	  gl_FragColor = sampled * vec4(color, 0.6);
	
    // colors
    // 0.3 0.3 0.8 0.6 - blue
    // 1.0 0.3 0.0 0.6 - orange
    // 0.3 0.3 0.3 0.5 - gray
}