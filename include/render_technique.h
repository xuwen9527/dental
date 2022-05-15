#ifndef __RENDER_TECHNIQUE_H__
#define __RENDER_TECHNIQUE_H__

#include <memory>
#include <program.h>
#include <render_info.h>

namespace Dental {
  class RenderInfo;
  class Geometry;

  using GeometryPtr = std::shared_ptr<Geometry>;

#define Mate_RenderTechnique(class) \
  virtual const std::string& className() const { static std::string name = #class; return name; }

#define Mate_RenderTechniqueEX(class) \
  virtual const std::string& className() const override { static std::string name = #class; return name; }

  class RenderTechnique {
  public:
    using UniformMap = std::unordered_map<UniformPtr, int>;
    using ProgramPool = std::unordered_map<std::string, ProgramPtr>;

    RenderTechnique(const std::string& name);
    virtual ~RenderTechnique();

    Mate_RenderTechnique(RenderTechnique)

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

    virtual void apply(RenderInfo& info);

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

  class MonoColorRenderTechnique : public RenderTechnique {
  public:
    MonoColorRenderTechnique();

    Mate_RenderTechniqueEX(MonoColorRenderTechnique)

    MonoColorRenderTechnique& operator = (MonoColorRenderTechnique&&) noexcept = delete;
    MonoColorRenderTechnique& operator = (const MonoColorRenderTechnique&) = delete;
    MonoColorRenderTechnique(const MonoColorRenderTechnique&) = delete;
    MonoColorRenderTechnique(MonoColorRenderTechnique&&) noexcept = delete;

    const glm::vec4& color() const;
    void color(const glm::vec4& vec);
  };

  using MonoColorRenderTechniquePtr = std::shared_ptr<MonoColorRenderTechnique>;

  class TextureRenderTechnique : public RenderTechnique {
  public:
    TextureRenderTechnique();

    Mate_RenderTechniqueEX(TextureRenderTechnique)

    TextureRenderTechnique& operator = (TextureRenderTechnique&&) noexcept = delete;
    TextureRenderTechnique& operator = (const TextureRenderTechnique&) = delete;
    TextureRenderTechnique(const TextureRenderTechnique&) = delete;
    TextureRenderTechnique(TextureRenderTechnique&&) noexcept = delete;

    const int& texture() const;
    void texture(const int& value);
  };

  using TextureRenderTechniquePtr = std::shared_ptr<TextureRenderTechnique>;

  class TextRenderTechnique : public RenderTechnique {
  public:
    TextRenderTechnique(bool sdf = true);

    Mate_RenderTechniqueEX(TextRenderTechnique)

    TextRenderTechnique& operator = (TextRenderTechnique&&) noexcept = delete;
    TextRenderTechnique& operator = (const TextRenderTechnique&) = delete;
    TextRenderTechnique(const TextRenderTechnique&) = delete;
    TextRenderTechnique(TextRenderTechnique&&) noexcept = delete;

    const glm::vec4& color() const;
    void color(const glm::vec4& vec);

    const int& texture() const;
    void texture(const int& value);

  private:
    bool sdf_;
  };

  using TextRenderTechniquePtr = std::shared_ptr<TextRenderTechnique>;
}
#endif