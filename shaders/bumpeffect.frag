varying vec2 texturePosition;

uniform sampler2D text;
uniform float time;
uniform vec2 screenSize;

float random (vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*43758.5453123);
}

void main() {

  float rnd = random(vec2(sin(time * 1.0), sin(time * 1.0)));

  vec2 normalizedScreenPos = vec2(gl_FragCoord.x / screenSize.x,
                                  gl_FragCoord.y / screenSize.y);

  float value = length(normalizedScreenPos - vec2(0.5, 0.5)) * 0.1;
  float valuex = (normalizedScreenPos.x - 0.5) * 0.1 + sin(time) * 0.5 + 0.5;
  float valuey = (normalizedScreenPos.y - 0.5) * 0.1 + sin(time) * 0.5 + 0.5;

  float xdist = texturePosition.x - 0.5;
  float ydist = texturePosition.y - 0.5;

  float valx = pow(xdist, 2.0);
  float valy = pow(ydist, 2.0);
  if(xdist < 0.0) valx = -valx;
  if(ydist < 0.0) valy = -valy;

  float x = (texturePosition.x + valx * 0.5 + 0.5) * 0.5;
  float y = (texturePosition.y + valy + 0.5) * 0.5;

  float noiseX = floor(x * 350.0) / 350.0;
  float noiseY = floor(y * 350.0) / 350.0;

  float lineVal = sin(time * 0.5f) * 2.0 + 1.0f;
  float k = 0.2;
  float lagLineDist = abs(texturePosition.y - lineVal);
  float maxDist = 0.02 + random(vec2(time * 0.3, time * 0.2)) * 0.025f;
  if(lagLineDist < maxDist) {
    k = 0.5;
  } else if(lagLineDist < maxDist + 0.01) {
    k = 0.5 - 0.25 * (maxDist + 0.01 - lagLineDist) / 0.01;
  }

  vec4 gray = vec4(vec3(random(vec2(noiseX + rnd, noiseY + rnd))), 1.0) * k;

  //vec4 gray = vec4(vec3(random(vec2(y + rnd, y + rnd))), 1.0) * 0.25;

  float distortionK = 0.0025;
  if(lagLineDist < maxDist) {
    distortionK = 0.1;
  }

  vec4 color = texture2D(text, vec2(x + rnd * distortionK, y + rnd * 0.00)) + gray;
  gl_FragColor = color;
  //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
