varying vec2 texturePosition;

void main()
{

  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  texturePosition = vec2(gl_MultiTexCoord0);

}
