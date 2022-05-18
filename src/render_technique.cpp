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

  void RenderTechnique::apply(RenderInfo& info, const Geometry& geometry) {
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

  DefaultRenderTechnique::DefaultRenderTechnique() : RenderTechnique("Default") {
    static const char* vertex_source = R"(#version 300 es
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoord;
out vec3 pos;
out vec3 normal;
out vec4 color;
out vec2 texcoord;
flat out int style;
uniform mat4 uMVP;
uniform mat4 uMV;
uniform bool showTexture;
uniform bool showColor;
void main() {
  gl_Position = uMVP * vec4(aPosition, 1.0);
  texcoord = aTexCoord.xy;
  vec4 ecPos = uMV * vec4(aPosition, 1.0);
  mat3 normal_matrix = mat3(uMV);
  normal = normalize(normal_matrix * aNormal);
  pos = (ecPos / ecPos.w).xyz;
  color = showColor ? aColor : vec4(0.0);
  style = showColor ? 1 : (showTexture ? 2 : 3);
  gl_PointSize = 4.0;
})";

    static const char* fragment_source = R"(#version 300 es
precision mediump float;
uniform sampler2D texture0;
in vec3 pos;
in vec3 normal;
in vec4 color;
in vec2 texcoord;
flat in int style;
out vec4 FragColor;
const vec3 eyePos        = vec3(0.0, 0.0, 0.0);
const vec3 lightPos      = vec3(0.0, 0.0, 100.0);
const vec4 cessnaColor   = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 lightAmbient  = vec4(0.2, 0.2, 0.2, 1.0);
const vec4 lightDiffuse  = vec4(0.4, 0.4, 0.4, 1.0);
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
  if (style == 1) {
    FragColor = color;
  }
  if(style == 2) {
    FragColor = texture(texture0, texcoord);
  } else {
    vec4 ambiCol = vec4(0.0);
    vec4 diffCol = vec4(0.0);
    vec4 specCol = vec4(0.0);
    DirectionalLight(normal, pos, ambiCol, diffCol, specCol);
    FragColor = cessnaColor * (ambiCol + diffCol + specCol);
  }
})";

    addUniform(std::make_shared<UniformBool>("showTexture", false));
    addUniform(std::make_shared<UniformBool>("showColor", false));
    addUniform(std::make_shared<UniformInt>("texture0", 0));

    program_ = getOrAddProgram(name_, vertex_source, fragment_source);
  }

  void DefaultRenderTechnique::apply(RenderInfo& info, const Geometry& geometry) {
    uniform("showTexture")->value<bool>(geometry.texcoordArray()->size() && geometry.texture());
    uniform("showColor")->value<bool>(geometry.colorArray()->size() > 0);
    RenderTechnique::apply(info, geometry);
  }
}