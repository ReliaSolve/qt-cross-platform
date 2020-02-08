#include "GeneralOpenGLWidget.h"
#include <QMessageBox>

GeneralOpenGLWidget::GeneralOpenGLWidget(QWidget *parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)
{
  // Make our OpenGL context current before calling any OpenGL functions.
  makeCurrent();

  // Done with our context.
  doneCurrent();
}

GeneralOpenGLWidget::~GeneralOpenGLWidget()
{
  // Make our OpenGL context current before calling any OpenGL functions.
  makeCurrent();

  /// @todo Replace with whatever the program should do.
  teardownMyStuff();

  // Delete the GLSL program and shaders.
  teardownGL();

  // Done with our context.
  doneCurrent();
}

void GeneralOpenGLWidget::initializeGL()
{
  // Our OpenGL context is automatically current in this method.
  // Initialize OpenGL Backend
  initializeOpenGLFunctions();

  /// @todo Replace with whatever the program should do.
  setupMyStuff();
}

void GeneralOpenGLWidget::resizeGL(int width, int height)
{
  // Our OpenGL context is automatically current in this method.
  // Currently we are not handling width/height changes.
  // The glViewport will already have been handled for us.
  /// @todo Note that this will distort the image as the aspect ratio
  /// of the window changes.
  (void)width;
  (void)height;
}

void GeneralOpenGLWidget::paintGL()
{
  // Our OpenGL context is automatically current in this method.
  // Clear the background color and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /// @todo Replace with whatever the program should do.
  drawMyStuff();
}

void GeneralOpenGLWidget::teardownGL()
{
  // Common teardown code.
  if (m_vertexShaderId) {
      glDeleteShader(m_vertexShaderId);
  }
  if (m_fragmentShaderId) {
      glDeleteShader(m_fragmentShaderId);
  }
  glUseProgram(0);
  if (m_programId) {
      glDeleteProgram(m_programId);
  }
}

static const GLchar* defaultVertexShaderBody =
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 1) in vec4 vertexColor;\n"
    "out vec4 fragmentColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(position,1);\n"
    "   fragmentColor = vertexColor;\n"
    "}\n";

static const GLchar* defaultFragmentShaderBody =
    "in vec4 fragmentColor;\n"
    "layout(location = 0) out vec4 color;\n"
    "void main()\n"
    "{\n"
    "    color = fragmentColor;\n"
    "}\n";

QString GeneralOpenGLWidget::makeVertexShader(QString body)
{
  QString ret;

  // Add the header
  if (QOpenGLContext::currentContext()->isOpenGLES()) {
    /// @todo Do we need this casting here and elsewhere?
    ret.append("#version 300 es\n");
  } else {
    ret.append("#version 330\n");
  }

  // Add the body
  if (body.size() > 0) {
    ret.append(body);
  } else {
    ret.append(defaultVertexShaderBody);
  }
  return ret;
}

QString GeneralOpenGLWidget::makeFragmentShader(QString body)
{
  QString ret;

  // Add the header
  if (QOpenGLContext::currentContext()->isOpenGLES()) {
    ret.append("#version 300 es\n");
    // Add default precision specifiers for GLES fragment shaders.
    // These can be overridden in the shader itself, but there must
    // be a default for each type.
    ret.append("precision mediump float;\n");
    ret.append("precision mediump int;\n");
  } else {
    ret.append("#version 330\n");
  }

  // Add the body
  if (body.size() > 0) {
    ret.append(body);
  } else {
    ret.append(defaultFragmentShaderBody);
  }
  return ret;
}

QString GeneralOpenGLWidget::checkShaderError(GLuint shaderId)
{
  QString ret;
  GLint result = GL_FALSE;
  int infoLength = 0;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
  glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLength);
  if (result == GL_FALSE) {
      std::vector<GLchar> errorMessage(static_cast<unsigned>(infoLength) + 1);
      glGetShaderInfoLog(shaderId, infoLength, nullptr, &errorMessage[0]);
      ret = &errorMessage[0];
  }
  return ret;
}

QString GeneralOpenGLWidget::checkProgramError(GLuint programId) {
  QString ret;
  GLint result = GL_FALSE;
  int infoLength = 0;
  glGetProgramiv(programId, GL_LINK_STATUS, &result);
  glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLength);
  if (result == GL_FALSE) {
      std::vector<GLchar> errorMessage(static_cast<unsigned>(infoLength) + 1);
      glGetProgramInfoLog(programId, infoLength, nullptr, &errorMessage[0]);
      ret = &errorMessage[0];
  }
  return ret;
}

QString GeneralOpenGLWidget::buildProgram(QString vertexShader, QString fragmentShader)
{
  QString ret;

  // If we already have the program and shaders defined, tear them down.
  if (m_vertexShaderId) {
      glDeleteShader(m_vertexShaderId);
      m_vertexShaderId = 0;
  }
  if (m_fragmentShaderId) {
      glDeleteShader(m_fragmentShaderId);
      m_fragmentShaderId = 0;
  }
  if (m_programId) {
      glDeleteProgram(m_programId);
      m_programId = 0;
  }

  // Compile the vertex shader and fragment shader, then link them together
  // into the program.
  m_vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
  std::string shaderString = vertexShader.toStdString();
  const char *vertexPrograms[] = { shaderString.c_str() };
  glShaderSource(m_vertexShaderId, 1, vertexPrograms, nullptr);
  glCompileShader(m_vertexShaderId);
  ret = checkShaderError(m_vertexShaderId);
  if (ret.size() > 0) {
    glDeleteShader(m_vertexShaderId);
    m_vertexShaderId = 0;
    return QString("Error compiling vertex shader: ") + ret;
  }

  m_fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  shaderString = fragmentShader.toStdString();
  const char *fragmentPrograms[] = { shaderString.c_str() };
  glShaderSource(m_fragmentShaderId, 1, fragmentPrograms, nullptr);
  glCompileShader(m_fragmentShaderId);
  ret = checkShaderError(m_fragmentShaderId);
  if (ret.size() > 0) {
    glDeleteShader(m_vertexShaderId);
    m_vertexShaderId = 0;
    glDeleteShader(m_fragmentShaderId);
    m_fragmentShaderId = 0;
    return QString("Error compiling fragment shader: ") + ret;
  }

  m_programId = glCreateProgram();
  if (m_programId == 0) {
      glDeleteShader(m_vertexShaderId);
      m_vertexShaderId = 0;
      glDeleteShader(m_fragmentShaderId);
      m_fragmentShaderId = 0;
      return QString("Error creating program.");
  }
  glAttachShader(m_programId, m_vertexShaderId);
  glAttachShader(m_programId, m_fragmentShaderId);
  glLinkProgram(m_programId);
  ret = checkProgramError(m_programId);
  if (ret.size() > 0) {
    glDeleteShader(m_vertexShaderId);
    m_vertexShaderId = 0;
    glDeleteShader(m_fragmentShaderId);
    m_fragmentShaderId = 0;
    glDeleteProgram(m_programId);
    m_programId = 0;
    return QString("Error Linking GLSL program: ") + ret;
  }

  return ret;
}

/// @todo Replace all of the following with whatever is needed to make the
/// class behave as desired.

class myStuff_vertex {
public:
  GLfloat pos[3];
  GLfloat col[4];
};

void GeneralOpenGLWidget::setupMyStuff()
{
  // Set global information
  glClearColor(0.7f, 0.9f, 1.0f, 1.0f);

  // Make the default vertex and fragment shaders.
  QString ret = buildProgram(makeVertexShader(""), makeFragmentShader(""));
  if (ret.size() > 0) {
    QMessageBox::critical(this, "Error", "GeneralOpenGLWidget::setupMyStuff(): Failed to build program: " + ret);
    return;
  }

  // Generate the vertex buffer we're going to use to send data to the GPU
  glGenBuffers(1, &m_vertexBuffer);

  // Set up the vertex array buffer that describes the bindings and mappings.
  glGenVertexArrays(1, &m_vertexArray);
  glBindVertexArray(m_vertexArray);

  // Configure the vertex-buffer objects within our vertex-array object.
  // On MacOSX, we had to create a vertex array object and use it before we
  // could enable vertex attribute arrays and set the attribute pointers.
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
  {
    size_t const stride = sizeof(myStuff_vertex);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
      reinterpret_cast<GLvoid*>(offsetof(myStuff_vertex, pos)));

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride,
      reinterpret_cast<GLvoid*>(offsetof(myStuff_vertex, col)));
  }
  glBindVertexArray(0);
}

void GeneralOpenGLWidget::teardownMyStuff()
{
  glBindVertexArray(0);
  if (m_vertexBuffer != 0) {
    glDeleteBuffers(1, &m_vertexBuffer);
    m_vertexBuffer = 0;
  }
  if (m_vertexArray != 0) {
    glDeleteVertexArrays(1, &m_vertexArray);
    m_vertexArray = 0;
  }
}

void GeneralOpenGLWidget::drawMyStuff()
{
  if (m_programId == 0) { return; }

  // Make a triangle.  Rebuilding this every frame is not efficient but it is simpler
  // for this example than keeping track of state in the initialization and slot.
  QVector<myStuff_vertex> vertexBufferData;
  myStuff_vertex v;
  v.col[0] = 1; v.col[1] = 1; v.col[2] = 1; v.col[3] = 1;
  if (m_red) {
    v.col[0] = 1;
    v.col[1] = 0;
    v.col[2] = 0;
  }

  v.pos[0] = -1; v.pos[1] = -1; v.pos[2] = 0;
  vertexBufferData.push_back(v);
  v.pos[0] = 1; v.pos[1] = -1; v.pos[2] = 0;
  vertexBufferData.push_back(v);
  v.pos[0] = 0; v.pos[1] = 1; v.pos[2] = 0;
  vertexBufferData.push_back(v);

  // Use the GLSL program and vertex array
  glUseProgram(m_programId);
  glBindVertexArray(m_vertexArray);

  // Push new vertex data to the buffer (buffer was bound with glBindVertexArray).
  glBufferData(GL_ARRAY_BUFFER,
    sizeof(vertexBufferData[0]) * static_cast<unsigned>(vertexBufferData.size()),
    &vertexBufferData[0], GL_STATIC_DRAW);

  // Draw the triangle.
  GLsizei numElements = static_cast<GLsizei>(vertexBufferData.size());
  glDrawArrays(GL_TRIANGLES, 0, numElements);
}

void GeneralOpenGLWidget::Red(bool red)
{
  /// @todo Replace this with whatever is appropriate, including removing the
  /// red button from the GUI if needed.
  m_red = red;

  // Force the widget to repaint so that we see the change.
  update();
}
