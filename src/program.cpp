#include <glad/glad.h>
#include <program.h>

namespace {
  unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    if (!shader) {
      //todo::error  info
      return 0;
    }

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint compileStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (!compileStatus) {
      GLchar info[512];
      GLsizei length = 0;
      glGetShaderInfoLog(shader, 512, &length, info);
      glDeleteShader(shader);
      //todo::error  info
      return 0;
    }

    return shader;
  }
}

namespace Dental {
  Program::Program(
    const std::string& vertex_source, 
    const std::string& fragment_source) :
    program_(0),
    dirty_(false),
    vertex_source_(vertex_source),
    fragment_source_(fragment_source) {
  }

  Program::~Program() {
    release();
  }

  int Program::attrib_location(const std::string& name) {
    return glGetAttribLocation(program_, name.c_str());
  }

  int Program::uniform_location(const std::string& name) {
    return glGetUniformLocation(program_, name.c_str());
  }

  void Program::create() {
    GLuint program = glCreateProgram();
    if (!program) {
      //todo:: error
      return;
    }

    unsigned int vertex_shader = compileShader(GL_VERTEX_SHADER, vertex_source_.c_str());
    unsigned int fragment_shader = compileShader(GL_FRAGMENT_SHADER, fragment_source_.c_str());

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
      glDeleteShader(vertex_shader);
      glDeleteShader(fragment_shader);
      glDeleteProgram(program);
      //todo:: error
      return;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    program_ = program;
  }

  void Program::bind() {
    if (dirty_) {
      if (program_) {
        release();
      }
      dirty_ = false;
    }

    if (!program_) {
      create();
      if (!program_) {
        return;
      }
    }

    glUseProgram(program_);
  }

  void Program::bind(const glm::mat4& mv, const glm::mat4& mvp) {
    int mvp_location = glGetUniformLocation(program_, "uMVP");
    int mv_location = glGetUniformLocation(program_, "uMV");

    if (mv_location != -1) {
      glUniformMatrix4fv(mv_location, 1, GL_FALSE, (float*)glm::value_ptr(mv));
    }

    if (mvp_location != -1) {
      glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (float*)glm::value_ptr(mvp));
    }
  }

  void Program::unbind() {
    glUseProgram(0);
  }

  void Program::release() {
    glDeleteProgram(program_);
    program_ = 0;
  }

  void Program::dirty() {
    dirty_ = true;
  }

  bool Program::valid() {
    return program_;
  }
}