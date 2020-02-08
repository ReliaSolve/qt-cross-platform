#ifndef GENERALOPENGLWINDOW_H
#define GENERALOPENGLWINDOW_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

class GeneralOpenGLWidget : public QOpenGLWidget
    , protected QOpenGLExtraFunctions
{
 Q_OBJECT

public:
  GeneralOpenGLWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  ~GeneralOpenGLWidget() override;

  // OpenGL-related functions
  /// @todo Modify the behavior of these to match application needs.
  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;
  void teardownGL();

  // OpenGL/GLES compatible shader creation functions.

  /// @brief Prepend the appropriate header to a vertex shader.
  /// @param [in] body The shader description except for the header line,
  ///         which will be filled in based on the particular OpenGL library being
  ///         used.  The shader should be compatible with GLES 3.0 (no uniform
  ///         initialization, etc.).  If there is an empty body, a default shader
  ///         that uses vertex position in location 0 and color in location 1
  ///         without lighting will be constructed.
  /// @return Complete shader, with appropriate header prepended.
  QString makeVertexShader(QString body);

  /// @brief Prepend the appropriate header to a fragment shader.
  /// @param [in] body The shader description except for the header line,
  ///         which will be filled in based on the particular OpenGL library being
  ///         used.  The shader should be compatible with GLES 3.0 (no uniform
  ///         initialization, etc.).  Default precisions of mediump for both
  ///         float and int will be provided; these can be overridden in the shader
  ///         itself on a per-parameter basis.  If there is an empty body, a default shader
  ///         that matches the default vertex shader will be constructed.
  /// @return Complete shader, with appropriate header prepended.
  QString makeFragmentShader(QString body);

  /// @brief Construct and link a GLSL program given a vertex and fragment shader.
  /// @param [in] vertexShader Source for the vertex shader.  The
  ///         makeVertexShader() function should be used to construct it.
  /// @param [in] fragmentShader Source for the vertex shader.  The
  ///         makeFragmentShader() function should be used to construct it.
  /// @return Empty string on success, string with a description of the
  ///         error in case of failure.
  QString buildProgram(QString vertexShader, QString fragmentShader);

public slots:
  /// @todo Modify these to match the application needs.
  void Red(bool red);

protected:
  /// Variables used to instantiate the GLSL program.
  GLuint m_vertexShaderId = 0;
  GLuint m_fragmentShaderId = 0;
  GLuint m_programId = 0;

  /// @brief See if the specified program compiled okay.
  /// @param shaderId to determine the status of
  /// @return Empty string if the program is okay, descriptive message if not.
  QString checkShaderError(GLuint shaderId);

  /// @brief See if the specified program linked okay.
  /// @param programId to determine the status of
  /// @return Empty string if the program is okay, descriptive message if not.
  QString checkProgramError(GLuint shaderId);

  /// @todo Replace all of the below with whatever is needed to make the class
  /// behave as desired.
  void setupMyStuff();
  void teardownMyStuff();
  void drawMyStuff();
  bool m_red = false;
  GLuint m_vertexBuffer = 0;
  GLuint m_vertexArray = 0;
};

#endif // GENERALOPENGLWINDOW_H
