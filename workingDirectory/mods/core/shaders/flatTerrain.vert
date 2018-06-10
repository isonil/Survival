varying vec3 worldPos;

const float textureScale = 30.0; // TODO: remove hardcoded texture scale

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0 * textureScale;
    
    worldPos = gl_Vertex.xyz;
  
    gl_Position = ftransform();
}
