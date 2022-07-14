#include <render_technique.h>
#include <geometry.h>
#include <uuid.h>
#include <camera.h>

namespace Dental {
  static ProgramPtr createWhiteProgram() {
    static const char* vertex_source = R"(#version 300 es
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
out vec3 pos;
out vec3 normal;
uniform mat4 uMVP;
uniform mat4 uMV;
void main() {
  gl_Position = uMVP * vec4(aPosition, 1.0);
  vec4 ecPos = uMV * vec4(aPosition, 1.0);
  mat3 normal_matrix = mat3(uMV);
  normal = normalize(normal_matrix * aNormal);
  pos = (ecPos / ecPos.w).xyz;
})";

    static const char* fragment_source = R"(#version 300 es
precision mediump float;
in vec3 pos;
in vec3 normal;
out vec4 FragColor;
const vec3 eyePos        = vec3(0.0, 0.0, 0.0);
const vec3 lightPos      = vec3(0.0, 0.0, 100.0);
const vec4 cessnaColor   = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 lightAmbient  = vec4(0.2, 0.2, 0.2, 1.0);
const vec4 lightDiffuse  = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 lightSpecular = vec4(0.2, 0.2, 0.2, 1.0);
void DirectionalLight(in vec3 normal, in vec3 ecPos,
  inout vec4 ambient, inout vec4 diffuse, inout vec4 specular) {
  vec3 lightDir = normalize(lightPos - ecPos);
  vec3 viewDir = normalize(-ecPos);
  bool blin = true;
  if (blin) {
    vec3 halfwayDir = normalize(lightDir + viewDir);
    specular = pow(max(dot(normal, halfwayDir), 0.0), 16.0) * lightSpecular;
  } else {
    vec3 reflectDir = reflect(-lightDir, normal);
    specular = pow(max(dot(viewDir, reflectDir), 0.0), 8.0) * lightSpecular;
  }
  ambient = lightAmbient;
  float nDotVP = max(0.0, dot(normal, lightDir));
  diffuse = lightDiffuse * nDotVP;
}
void main() {
    vec4 ambiCol = vec4(0.0);
    vec4 diffCol = vec4(0.0);
    vec4 specCol = vec4(0.0);
    DirectionalLight(normal, pos, ambiCol, diffCol, specCol);
    FragColor = cessnaColor * (ambiCol + diffCol + specCol);
})";
    return std::make_shared<Program>(vertex_source, fragment_source);
  }

  static ProgramPtr createColorProgram() {
      static const char* vertex_source = R"(#version 300 es
layout (location = 0) in vec3 aPosition;
layout (location = 2) in vec4 aColor;
uniform mat4 uMVP;
out vec3 pos;
out vec4 color;
void main() {
  gl_Position = uMVP * vec4(aPosition, 1.0);
  color = aColor;
})";

    static const char* fragment_source = R"(#version 300 es
precision mediump float;
in vec3 color;
out vec4 FragColor;
void main() {
    FragColor = color;
})";
    return std::make_shared<Program>(vertex_source, fragment_source);
  }

  static ProgramPtr createTextureProgram() {
    static const char* vertex_source = R"(#version 300 es
layout (location = 0) in vec3 aPosition;
layout (location = 3) in vec2 aTexCoord;
out vec3 pos;
out vec2 texcoord;
uniform mat4 uMVP;
void main() {
  gl_Position = uMVP * vec4(aPosition, 1.0);
  texcoord = aTexCoord.xy;
})";

    static const char* fragment_source = R"(#version 300 es
precision mediump float;
uniform sampler2D texture0;
out vec4 FragColor;
void main() {
    FragColor = texture(texture0, texcoord);
})";
    return std::make_shared<Program>(vertex_source, fragment_source);
  }

  ProgramPtr createBlackProgram() {
    static const char* vertex_source = R"(#version 300 es
layout (location = 0) in vec3 aPosition;
uniform mat4 uMVP;
out vec4 pos;
void main() {
  pos = uMVP * vec4(aPosition, 1.0);
  gl_Position = pos;
})";

    static const char* fragment_source = R"(#version 300 es
precision mediump float;
out vec4 FragColor;
in vec4 pos;
void main() {
  FragColor = vec4((pos.xyz + 1.0) * 0.5, 1.f);
})";

    return std::make_shared<Program>(vertex_source, fragment_source);
  }

  ProgramPtr createShadowProgram() {
    static const char* vertex_source = R"(#version 300 es
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
uniform mat4 uMVP;
uniform mat4 uMV;
uniform mat4 uDepthMVP;
out vec3 pos;
out vec3 normal;
out vec3 frag;
void main() {
  vec4 depth_pos = uDepthMVP * vec4(aPosition, 1.0);
  frag = depth_pos.xyz;
  gl_Position = uMVP * vec4(aPosition, 1.0);
  vec4 ecPos = uMV * vec4(aPosition, 1.0);
  mat3 normal_matrix = mat3(uMV);
  normal = normalize(normal_matrix * aNormal);
  pos = (ecPos / ecPos.w).xyz;
})";

    static const char* fragment_source = R"(#version 300 es
precision mediump float;
uniform sampler2D texture0;
in vec3 pos;
in vec3 normal;
in vec3 frag;
out vec4 FragColor;
const vec3 lightPos      = vec3(0.0, 0.0, 100.0);
const vec4 cessnaColor   = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 lightAmbient  = vec4(0.4, 0.4, 0.4, 1.0);
const vec4 lightDiffuse  = vec4(0.3, 0.3, 0.3, 1.0);
const vec4 lightSpecular = vec4(0.1, 0.1, 0.1, 1.0);
void DirectionalLight(in vec3 normal, in vec3 ecPos,
  inout vec4 ambient, inout vec4 diffuse, inout vec4 specular) {
  vec3 lightDir = normalize(lightPos - ecPos);
  vec3 viewDir = normalize(-ecPos);
  bool blin = true;
  if (blin) {
    vec3 halfwayDir = normalize(lightDir + viewDir);
    specular = pow(max(dot(normal, halfwayDir), 0.0), 8.0) * lightSpecular;
  } else {
    vec3 reflectDir = reflect(-lightDir, normal);
    specular = pow(max(dot(viewDir, reflectDir), 0.0), 8.0) * lightSpecular;
  }
  ambient = lightAmbient;
  float nDotVP = max(0.0, dot(normal, lightDir));
  diffuse = lightDiffuse * nDotVP;
}
void main() {
  vec2 texcoord = (frag.xy + 1.0) * 0.5;

  float depth = texture(texture0, texcoord).r;
  ivec2 size = textureSize(texture0, 0);
  vec2 offset = vec2(1.0/float(size.x), 1.0/float(size.y));

  float d[4];
  d[0] = texture(texture0, texcoord + vec2(0.0,  offset.y)).r;
  d[1] = texture(texture0, texcoord + vec2(0.0, -offset.y)).r;
  d[2] = texture(texture0, texcoord + vec2( offset.x, 0.0)).r;
  d[3] = texture(texture0, texcoord + vec2(-offset.x, 0.0)).r;

  float dx = d[2] - d[3];
  float dy = d[0] - d[1];

  offset = offset * vec2(sign(dx), sign(dy));
  float near = texture(texture0, texcoord + offset).r;
  if (near > 0.99) {
    depth = texture(texture0, texcoord - offset).r;
  }

  depth = depth * 2.0 - 1.0;
  float shadow = clamp(1.0 - (frag.z - depth), 0.0, 1.0);

  vec4 ambiCol = vec4(0.0);
  vec4 diffCol = vec4(0.0);
  vec4 specCol = vec4(0.0);
  DirectionalLight(normal, pos, ambiCol, diffCol, specCol);
  vec4 color = cessnaColor * (ambiCol + diffCol + specCol);
  FragColor = vec4(pow(color.xyz, vec3(1.0/2.2)), 1.0);
  FragColor.gb *= shadow;
  // FragColor = vec4(color.xyz * shadow, 1.0);
  // FragColor = vec4(shadow, shadow, shadow, 1.0);
})";

    return std::make_shared<Program>(vertex_source, fragment_source);
  }

  ProgramPool& ProgramPool::instance() {
    static ProgramPool pool;
    return pool;
  }

  ProgramPtr ProgramPool::getOrAddProgram(
    const std::string& class_name,
    const std::string& vertex_source,
    const std::string& fragment_source) {

    auto itr = find(class_name);
    if (itr != end()) {
      return itr->second;
    }

    auto program = std::make_shared<Program>(vertex_source, fragment_source);
    insert({ class_name, program });
    return program;
  }

  ProgramPool::ProgramPool() {
    emplace("white", createWhiteProgram());
    emplace("color", createColorProgram());
    emplace("texture", createTextureProgram());
    emplace("black", createBlackProgram());
    emplace("shadow", createShadowProgram());
  }

  RenderTechnique::RenderTechnique(const std::string& name) :
    name_(name),
    uuid_(createUUID()) {
  }

  RenderTechnique::~RenderTechnique() {
    program_.reset();
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

  void RenderTechnique::apply(RenderInfo& info, Geometry& geometry) {
    auto& program = this->program();
    program->bind();
    program->bind(info.mv(), info.mvp());

    for (auto& itr : uniforms_) {
      if (itr.second == -1) {
        itr.second = program->uniform_location(itr.first->name().c_str());
      }
      itr.first->bind(itr.second);
    }

    geometry.render();
  }

  DefaultRenderTechnique::DefaultRenderTechnique() : RenderTechnique("Default") {
  }

  void DefaultRenderTechnique::apply(RenderInfo& info, Geometry& geometry) {
    if (geometry.texcoordArray()->size() && geometry.texture()) {
      program_ = ProgramPool::instance()["texture"];
      if (uniform("texture0")) {
        addUniform(std::make_shared<UniformInt>("texture0", 0));
      }
    } else if (geometry.colorArray()->size()) {
      program_ = ProgramPool::instance()["color"];
    } else {
      program_ = ProgramPool::instance()["white"];
    }
    RenderTechnique::apply(info, geometry);
  }

  ShadowRenderTechnique::ShadowRenderTechnique() :
    RenderTechnique("Shadow"), 
    size_(1024),
    mv_(glm::identity<glm::mat4>()) {
    uniform_tex_ = std::make_shared<UniformInt>("texture0", 0);
    uniform_mvp_ = std::make_shared<UniformMat4>("uDepthMVP", glm::identity<glm::mat4>());
    frambuffer.resize(size_, size_);
    frambuffer.attachColor();
  }

  void ShadowRenderTechnique::renderDepth(RenderInfo& info, Geometry& geometry) {
    BoundingSphere sphere = geometry.boundingSphere();
    auto center = sphere.center();
    auto radius = sphere.radius();

    auto half_size = size_ / 2.f;

    auto mv = glm::lookAt(glm::vec3(center.x, center.y, center.z + sphere.radius()), center, glm::vec3(0.f, 1.f, 0.f));
    mv *= glm::scale(mv, glm::vec3(half_size / radius, half_size / radius, half_size / radius));

    mv *= mv_;

    RenderInfo depth_render_info;
    depth_render_info.viewport(Viewport(0.f, 0.f, (float)frambuffer.width(), (float)frambuffer.height()));
    depth_render_info.mvp(
      mv,
      glm::ortho(-half_size, half_size, -half_size, half_size, -half_size, half_size));

    uniform_mvp_->value<glm::mat4>(depth_render_info.mvp());

    program_ = ProgramPool::instance()["black"];

    frambuffer.bind();

    depth_render_info.viewport().apply();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program_->bind();
    program_->bind(depth_render_info.mv(), depth_render_info.mvp());
    geometry.render();

    frambuffer.unbind();

    info.viewport().apply();
    // frambuffer.blit(0, 0, 400, 400);
  }

  void ShadowRenderTechnique::renderShadow(RenderInfo& info, Geometry& geometry) {
    program_ = ProgramPool::instance()["shadow"];
    program_->bind();
    program_->bind(info.mv(), info.mvp());

    uniform_mvp_->bind(program_->uniform_location("uDepthMVP"));
    uniform_tex_->bind(program_->uniform_location("texture0"));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frambuffer.depth());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    geometry.render();

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void ShadowRenderTechnique::apply(RenderInfo& info, Geometry& geometry) {
    renderDepth(info, geometry);
    renderShadow(info, geometry);
  }
}