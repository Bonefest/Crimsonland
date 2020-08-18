varying vec2 texturePosition;

uniform sampler2D text;
uniform float time;
uniform vec2 screenSize;

void main() {
  vec2 normalizedScreenPos = vec2(gl_FragCoord.x / screenSize.x,
                                  gl_FragCoord.y / screenSize.y);

  float value = length(normalizedScreenPos - vec2(0.5, 0.5)) * 0.1;
  float valuex = (normalizedScreenPos.x - 0.5) * 0.1 + sin(time) * 0.5 + 0.5;
  float valuey = (normalizedScreenPos.y - 0.5) * 0.1 + sin(time) * 0.5 + 0.5;

  float xdist = texturePosition.x - 0.5;
  float ydist = texturePosition.y - 0.5;

  float valx = pow(xdist, 4.0);
  float valy = pow(ydist, 4.0);
  if(xdist < 0.0) valx = -valx;
  if(ydist < 0.0) valy = -valy;

  float x = (texturePosition.x + valx * 0.5 + 0.5) * 0.5;
  float y = (texturePosition.y + valy + 0.5) * 0.5;

  vec4 color = texture2D(text, vec2(x, y));
  gl_FragColor = color;
  //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
