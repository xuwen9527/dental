#include <render_technique.h>
#include <geometry.h>
#include <uuid.h>

namespace Dental {
  RenderTechnique::ProgramPool RenderTechnique::program_pool_;

  RenderTechnique::RenderTechnique(const std::string& name) :
    name_(name),
    uuid_(createUUID()) {
  }

  RenderTechnique::~RenderTechnique() {
    program_.reset();

    auto itr = program_pool_.find(name_);
    if (itr != program_pool_.end()) {
      if (itr->second.use_count() == 1) {
        program_pool_.erase(itr);
      }
    }
  }

  ProgramPtr RenderTechnique::getOrAddProgram(
    const std::string& class_name,
    const std::string& vertex_source,
    const std::string& fragment_source) {

    auto itr = program_pool_.find(class_name);
    if (itr != program_pool_.end()) {
      return itr->second;
    }

    auto program = std::make_shared<Program>(vertex_source, fragment_source);
    program_pool_.insert({ class_name, program });
    return program;
  }

  UniformPtr RenderTechnique::uniform(const std::string& name) const {
    for (auto& itr : uniforms_) {
      if (itr.first->name() == name)
        return itr.first;
    }
    return nullptr;
  }

  void RenderTechnique::addUniform(const UniformPtr& uniform) {
    UniformPtr result = this->uniform(uniform->name());
    if (!result) {
      uniforms_.emplace(uniform, -1);
    }
  }

  void RenderTechnique::removeUniform(const std::string& name) {
    for (auto itr = uniforms_.begin(); itr != uniforms_.end(); ++itr) {
      if (itr->first->name() == name) {
        uniforms_.erase(itr);
        return;
      }
    }
  }

  void RenderTechnique::clearUniform() {
    uniforms_.clear();
  }

  void RenderTechnique::apply(RenderInfo& info) {
    auto& program = this->program();
    program->bind();
    program->bind(info.mv(), info.mvp());

    for (auto& itr : uniforms_) {
      if (itr.second == -1) {
        itr.second = program->uniform_location(itr.first->name().c_str());
      }
      itr.first->bind(itr.second);
    }
  }

  MonoColorRenderTechnique::MonoColorRenderTechnique() : RenderTechnique("MonoColor") {
    addUniform(std::make_shared<UniformVec4>("uColor", glm::vec4(1.0f, 0.f, 0.f, 1.0f)));

    static const char* vertex_source = R"(#version 300 es
layout (location = 0) in vec3 aPosition;
uniform mat4 uMVP;
void main() {
  gl_Position = uMVP * vec4(aPosition, 1.0);
})";

    static const char* fragment_source = R"(#version 300 es
precision mediump float;
uniform vec4 uColor;
out vec4 FragColor;
void main() {
  FragColor = vec4(uColor.x, uColor.y, uColor.z, 1.0);
})";
    program_ = getOrAddProgram(name_, vertex_source, fragment_source);
  }

  const glm::vec4& MonoColorRenderTechnique::color() const {
    return uniform("uColor")->value<glm::vec4>();
  }

  void MonoColorRenderTechnique::color(const glm::vec4& color) {
    uniform("uColor")->value<glm::vec4>(color);
  }

  TextureRenderTechnique::TextureRenderTechnique() : RenderTechnique("Texture") {
    addUniform(std::make_shared<UniformInt>("uTexture", 0));

    static const char* vertex_source = R"(#version 300 es
layout (location = 0) in vec3 aPosition;
layout (location = 3) in vec2 aTexcoord;
precision mediump float;
uniform mat4 uMVP;
out vec2 texcoord;
void main() {
  texcoord = aTexcoord;
  gl_Position = uMVP * vec4(aPosition, 1.0);
})";

    static const char* fragment_source = R"(#version 300 es
precision mediump float;
uniform sampler2D uTexture;
in vec2 texcoord;
out vec4 FragColor;
void main() {
  FragColor = texture(uTexture, texcoord);
})";

    program_ = getOrAddProgram(name_, vertex_source, fragment_source);
  }

  const int& TextureRenderTechnique::texture() const {
    return uniform("uTexture")->value<int>();
  }

  void TextureRenderTechnique::texture(const int& texture) {
    uniform("uTexture")->value<int>(texture);
  }

  TextRenderTechnique::TextRenderTechnique(bool sdf) : RenderTechnique("Text"), sdf_(sdf) {
    addUniform(std::make_shared<UniformInt>("uTexture", 0));
    addUniform(std::make_shared<UniformVec4>("uColor", glm::vec4(1.0f, 1.f, 1.f, 1.0f)));
  
    static const char* vertex_source = R"(#version 300 es
layout (location = 0) in vec3 aPosition;
layout (location = 3) in vec2 aTexcoord;
precision mediump float;
uniform mat4 uMVP;
out vec2 texcoord;
void main() {
  texcoord = aTexcoord;
  gl_Position = uMVP * vec4(aPosition, 1.0);
})";

    if (sdf_) {
      static const char* fragment_source = R"(#version 300 es
// fwidth() is not supported by default on OpenGL ES. Enable it.
#if defined(GL_OES_standard_derivatives)
#extension GL_OES_standard_derivatives : enable
#endif

precision lowp float;
uniform vec4 uColor;
uniform sampler2D uTexture;
in vec2 texcoord;
out vec4 FragColor;

const float glyphEdge = 0.5;
const float gamma = 2.2;

// Supersampling improves rendering quality of very small font sizes (less aliasing), but adds a performance hit
// as it does multiple texture lookups.
//#define SUPERSAMPLE

//#define OUTLINE
const float outlineEdgeWidth = 0.02;
const vec4 outlineColor = vec4(0.0, 0.0, 0.0, 1.0);

float contour(float dist, float edge, float width) {
  return clamp(smoothstep(edge - width, edge + width, dist), 0.0, 1.0);
}

float getSample(vec2 texCoords, float edge, float width) {
  return contour(texture(uTexture, texCoords).a, edge, width);
}

void main() {
  vec4 tex = texture(uTexture, texcoord);
  float dist = tex.a;
  if (dist < 0.49) {
    discard;
  }
  float width = fwidth(dist);
  vec4 textColor = uColor;
  float outerEdge = glyphEdge;

#if defined(SUPERSAMPLE)
  float alpha = contour(dist, outerEdge, width);

  float dscale = 0.354; // half of 1/sqrt2; you can play with this
  vec2 uv = texcoord.xy;
  vec2 duv = dscale * (dFdx(uv) + dFdy(uv));
  vec4 box = vec4(uv - duv, uv + duv);

  float asum = getSample(box.xy, outerEdge, width)
    + getSample(box.zw, outerEdge, width)
    + getSample(box.xw, outerEdge, width)
    + getSample(box.zy, outerEdge, width);

  // weighted average, with 4 extra points having 0.5 weight each,
  // so 1 + 0.5*4 = 3 is the divisor
  alpha = (alpha + 0.5 * asum) / 3.0;
#else
  // No supersampling.
  float alpha = contour(dist, outerEdge, width);
#endif

  alpha = pow(alpha, 1.0 / gamma);
  FragColor = vec4(textColor.rgb, textColor.a * alpha);

  // Premultiplied alpha output.
  FragColor.rgb *= FragColor.a;

#ifdef OUTLINE
  outerEdge = outerEdge - outlineEdgeWidth;
  float outlineOuterAlpha = clamp(smoothstep(outerEdge - width, outerEdge + width, dist), 0.0, 1.0);
  float outlineAlpha = outlineOuterAlpha - alpha;
  FragColor.rgb = mix(outlineColor.rgb, FragColor.rgb, alpha);
  FragColor.a = max(FragColor.a, outlineColor.a * outlineOuterAlpha);
#endif
})";
      program_ = getOrAddProgram(name_, vertex_source, fragment_source);
    } else {
      static const char* fragment_source = R"(#version 300 es
precision mediump float;
uniform vec4 uColor;
uniform sampler2D uTexture;
in vec2 texcoord;
out vec4 FragColor;

#define GUASS_BLUR

void main() {
  vec4 color = texture(uTexture, texcoord);
#ifdef GUASS_BLUR
  ivec2 size = textureSize(uTexture, 0);
  vec2 offset = vec2(1.0/float(size.x), 1.0/float(size.y));
  color *= 0.147761;
  color += texture(uTexture, texcoord + vec2(-offset.x, -offset.y)) * 0.09474;
  color += texture(uTexture, texcoord + vec2( offset.x, -offset.y)) * 0.09474;
  color += texture(uTexture, texcoord + vec2( offset.x,  offset.y)) * 0.09474;
  color += texture(uTexture, texcoord + vec2(-offset.x,  offset.y)) * 0.09474;
  color += texture(uTexture, texcoord + vec2(-offset.x,  0.0)) * 0.118318;
  color += texture(uTexture, texcoord + vec2( offset.x,  0.0)) * 0.118318;
  color += texture(uTexture, texcoord + vec2( 0.0,       offset.y)) * 0.118318;
  color += texture(uTexture, texcoord + vec2( 0.0,      -offset.y)) * 0.118318;
#endif
  color.a = color.r;
  FragColor = color * uColor;
})";
      program_ = getOrAddProgram(name_, vertex_source, fragment_source);
    }
  }

  const glm::vec4& TextRenderTechnique::color() const {
    return uniform("uColor")->value<glm::vec4>();
  }
  
  void TextRenderTechnique::color(const glm::vec4& vec) {
    uniform("uColor")->value<glm::vec4>(vec);
  }

  const int& TextRenderTechnique::texture() const {
    return uniform("uTexture")->value<int>();
  }

  void TextRenderTechnique::texture(const int& value) {
    uniform("uTexture")->value<int>(value);
  }
}