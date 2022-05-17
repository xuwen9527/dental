#ifndef __RENDER_TECHNIQUE_H__
#define __RENDER_TECHNIQUE_H__

#include <memory>
#include <program.h>
#include <render_info.h>

namespace Dental {
  class RenderInfo;
  class Geometry;

#define Mate_RenderTechnique(class)                      \
  virtual const std::string& className() const override  \
  { static std::string name = #class; return name; }     \
  class& operator = (class&&) noexcept = delete;         \
  class& operator = (const class&) = delete;             \
  class(const class&) = delete;                          \
  class(class&&) noexcept = delete;

  class RenderTechnique {
  public:
    using UniformMap = std::unordered_map<UniformPtr, int>;
    using ProgramPool = std::unordered_map<std::string, ProgramPtr>;

    RenderTechnique(const std::string& name);
    virtual ~RenderTechnique();

    virtual const std::string& className() const { static std::string name = "RenderTechnique"; return name; }

    RenderTechnique& operator = (RenderTechnique&&) noexcept = delete;
    RenderTechnique& operator = (const RenderTechnique&) = delete;
    RenderTechnique(const RenderTechnique&) = delete;
    RenderTechnique(RenderTechnique&&) noexcept = delete;

    inline const std::string& name() const { return name_; }

    inline void uuid(const std::string& id) { uuid_ = id; }
    inline const std::string& uuid() const { return uuid_; }

    inline ProgramPtr& program() { return program_; }

    UniformPtr uniform(const std::string& name) const;

    const UniformMap& uniforms() const { return uniforms_; }

    virtual void apply(RenderInfo& info, const Geometry& geometry);

  protected:
    void addUniform(const UniformPtr& uniform);
    void removeUniform(const std::string& name);
    void clearUniform();

    static ProgramPtr getOrAddProgram(
      const std::string& class_name,
      const std::string& vertex_source,
      const std::string& fragment_source);

  protected:
    std::string name_;
    std::string uuid_;
    UniformMap uniforms_;

    ProgramPtr program_;
    static ProgramPool program_pool_;
  };

  using RenderTechniquePtr = std::shared_ptr<RenderTechnique>;

  class DefaultRenderTechnique : public RenderTechnique {
  public:
    DefaultRenderTechnique();

    Mate_RenderTechnique(DefaultRenderTechnique)

    void apply(RenderInfo& info, const Geometry& geometry) override;
  };

  using DefaultRenderTechniquePtr = std::shared_ptr<DefaultRenderTechnique>;
}
#endif