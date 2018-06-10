varying vec3 worldPos;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_MultiTexCoord1;
    
    worldPos = gl_Vertex.xyz;
	
    gl_Position = ftransform();
}
