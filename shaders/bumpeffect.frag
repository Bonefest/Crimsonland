varying vec2 texturePosition;

uniform sampler2D text;
uniform float time;
uniform vec2 screenSize;

#define PI 3.1415926
#define HALF_PI 1.5707963

float random (vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*43758.5453123);
}

// NOTE(mizofix): this function returns normalized value that should occur
// every n seconds (e.g shaking every 15 secs with duration 2 secs)
float every(float nsec, float duration) {
  float result = mod(time, nsec);
  if(result > duration) {
    result = 0.0;
  }

  result /= duration;

  return result;
}

void main() {

  float rnd = random(vec2(sin(time * 1.0), sin(time * 1.0)));

  vec2 normalizedScreenPos = vec2(gl_FragCoord.x / screenSize.x,
                                  gl_FragCoord.y / screenSize.y);


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

  float t1 = mod(time, 15.0);
  if(t1 > 2.0) {
    t1 = 0.0;
  }

  // NOTE(mizofix): White-line effect

  float wlDuration = 3.0;
  float wlPeriod = 15.0;
  float wlCurrentTimeVal = every(wlPeriod, wlDuration);

  float lineVal = sin(HALF_PI * wlCurrentTimeVal) * 1.4 - 0.2f;

  float k = 0.2;
  float lagLineDist = abs(texturePosition.y - lineVal);
  float maxDist = 0.02 + random(vec2(time * 0.3, time * 0.2)) * 0.025f;
  if(lagLineDist < maxDist) {
    k = 0.5;
  } else if(lagLineDist < maxDist + 0.01) {
    k = 0.5 - 0.25 * (maxDist + 0.01 - lagLineDist) / 0.01;
  }

  vec4 gray = vec4(vec3(random(vec2(noiseX + rnd, noiseY + rnd))), 1.0) * k;

  // NOTE(mizofix): Distortion effect

  float dstDuration = 3.0;
  float dstPeriod = 15.0;
  float dstCurrentTimeVal = every(dstPeriod, dstDuration);

  float distortionKX = 0.0025;
  float distortionKY = 0.0;

  // NOTE(mizofix): Whenever pixel is close to white-line - distortion strength
  // is increasing
  if(lagLineDist < maxDist) {
    distortionKX = 0.2;
    distortionKY = 0.05;
  }

  float s = sin(HALF_PI * dstCurrentTimeVal) * (4.0 + rnd * 2.0);
  distortionKX *= s;
  distortionKY *= s * s;

  vec4 color = texture2D(text, vec2(x + rnd * distortionKX, y + rnd * distortionKY)) + gray;
  gl_FragColor = color;
  //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
