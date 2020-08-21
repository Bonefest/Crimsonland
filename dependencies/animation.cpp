
class Animation {
public:

  Animation(): texture(nullptr),
               frozen(false), repeat(false), scale(1.0f),
               m_startX(0), m_startY(0),
               m_currentX(0), m_currentY(0),
               m_endX(0), m_endY(0),
               m_frameWidth(0), m_frameHeight(0), m_rowWidth(0),
               m_finished(false), m_frameDuration(0.0f), m_elapsedTime(0.0f) { }

  Animation(int startX, int startY, int endX, int endY,
            int frameWidth, int frameHeight, int rowWidth,
            float frameDuration): texture(nullptr), frozen(false), repeat(false), scale(1.0f),
                                  m_startX(startX), m_startY(startY),
                                  m_currentX(startX), m_currentY(startY),
                                  m_endX(endX), m_endY(endY),
                                  m_frameWidth(frameWidth), m_frameHeight(frameHeight),
                                  m_rowWidth(rowWidth),
                                  m_finished(false),
                                  m_frameDuration(frameDuration), m_elapsedTime(0.0f) { }

  // NOTE(mizofix): moves animation frame and returns a
  // rect which corresponds to a frame on spritesheet
  void update(float delta) {
    if(!frozen && !m_finished) {
      m_elapsedTime += delta;
      if(m_elapsedTime >= m_frameDuration) {
        m_currentX += m_frameWidth;

        if(m_currentX >= m_startX + m_rowWidth - m_frameWidth) {

          if(m_currentY < m_endY) {
            m_currentX = m_startX;
            m_currentY += m_frameHeight;
          } else {
            if(repeat) {
              reset();
            } else {
              m_finished = true;
            }
          }
        }

        m_elapsedTime = 0.0f;
      }
    }

  }

  void setFrameDuration(float duration) {
    m_frameDuration = duration;
  }

  SDL_Rect getSourceRect() const {
    SDL_Rect source;
    source.w = m_frameWidth;
    source.h = m_frameHeight;
    source.x = m_currentX;
    source.y = m_currentY;

    return source;
  }

  void reset() {
    m_currentX = m_startX;
    m_currentY = m_startY;

    m_finished = false;
    m_elapsedTime = 0.0f;
  }

  bool isFinished() const {
    return m_finished;
  }

  SDL_Texture* texture;
  bool frozen;
  bool repeat;

  float scale;

private:
  int m_startX, m_startY;
  int m_currentX, m_currentY;
  int m_endX, m_endY;

  int m_frameWidth;
  int m_frameHeight;

  int m_rowWidth;

  bool m_finished;

  float m_frameDuration;
  float m_elapsedTime;
};
