uniform sampler2D texture;

void main()
{
	float alpha = texture2D(texture, gl_TexCoord[0].xy).a;
	gl_FragColor = vec4(1.0, 1.0, 1.0, alpha);
}
