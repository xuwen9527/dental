#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include <string>
#include <unordered_map>
#include <glm/ext.hpp>
#include <uniform.h>
#include <gl_object.h>

namespace Dental {
  class Program : public GLObject {
  public:
    Program(
      const std::string& vertex_source,
      const std::string& fragment_source);

    ~Program();

    Program& operator = (Program&&) noexcept = delete;
    Program& operator = (const Program&) = delete;
    Program(const Program&) = delete;
    Program(Program&&) noexcept = delete;

    int attrib_location(const std::string& name);
    int uniform_location(const std::string& name);

    void bind(const glm::mat4& mv, const glm::mat4& mvp);

    virtual void bind() override;

    virtual void unbind() override;

    virtual void release() override;

    virtual void dirty() override;

    virtual bool valid() override;

  protected:
    void create();

    unsigned int program_;

    bool dirty_;
    std::string vertex_source_;
    std::string fragment_source_;
  };

  using ProgramPtr = std::shared_ptr<Program>;
}
#endif