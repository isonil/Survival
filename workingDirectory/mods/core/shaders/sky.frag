uniform sampler2D texture;
uniform vec3 color;

void main()
{
  vec3 fin = texture2D(texture, gl_TexCoord[0].xy) * color;
  gl_FragColor = vec4(fin, 1.0);
}
