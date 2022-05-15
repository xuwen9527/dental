#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <memory>
#include <string>
#include <unordered_map>
#include <array.h>
#include <primitive_set.h>
#include <texture.h>
#include <render_info.h>
#include <render_technique.h>
#include <bounding_box.h>
#include <visitor.h>

namespace Dental {
  class Geometry;
  using GeometryPtr = std::shared_ptr<Geometry>;

  class Node;
  using NodePtr = std::shared_ptr<Node>;
  using NodeWeakPtr = std::weak_ptr<Node>;

#define Mate_Geometry(library, class) \
  virtual const std::string& className() const { static std::string name = #class; return name; } \
  virtual const std::string& libraryName() const { static std::string name = #library; return name; }

  class Geometry : public std::enable_shared_from_this<Geometry> {
  public:
    using TextureMap = std::unordered_map<unsigned int, TexturePtr>;

    enum class Attrib {
      POSITION = 0,
      NORMAL = 1,
      COLOR = 2,
      TEXCOORD = 3
    };

    Geometry();
    virtual ~Geometry();

    Mate_Geometry(Dental, Geometry)

    Geometry& operator = (const Geometry& rhs);
    Geometry& operator = (Geometry&& rhs) noexcept;
    Geometry(const Geometry& rhs);
    Geometry(Geometry&& rhs) noexcept;

    inline GeometryPtr ptr() { return shared_from_this(); }

    inline void parent(const NodePtr& node) { parent_ = node; }
    inline NodePtr parent() const;

    virtual void accept(Visitor& visitor);

    inline void name(const std::string& name) { name_ = name; }
    inline const std::string& name() const { return name_; }

    inline void mv(const glm::mat4& mv) { mv_ = mv; }
    inline const glm::mat4& mv() const { return mv_; }

    inline void uuid(const std::string& id) { uuid_ = id; }
    inline const std::string& uuid() const { return uuid_; }							   
															
    glm::vec3 origin() const;
    void origin(glm::vec3);

    void get_scale_rotate_translate(glm::vec3& scale, glm::quat& rotate, glm::vec3& translate) const;
    void set_scale_rotate_translate(glm::vec3 scale, glm::quat rotate, glm::vec3 translate);

    void set_scale(const glm::vec3 scale);
    void scale(const glm::vec3 scale);
    glm::vec3 scale() const;

    void translate(const glm::vec3& trans);

    void rotate(float angle, const glm::vec3& normal);
    void angle(const glm::vec3);
    glm::vec3 angle() const;

    inline const Vec3ArrayPtr& vertexArray() const { return vertex_array_; }
    inline const Vec3ArrayPtr& normalArray() const { return normal_array_; }
    inline const Vec4ArrayPtr& colorArray() const { return color_array_; }
    inline const Vec2ArrayPtr& texcoordArray() const { return texcoord_array_; }

    inline Vec3ArrayPtr vertexArray() { return vertex_array_; }
    inline Vec3ArrayPtr normalArray() { return normal_array_; }
    inline Vec4ArrayPtr colorArray() { return color_array_; }
    inline Vec2ArrayPtr texcoordArray() { return texcoord_array_; }

    void addPrimitiveSet(const PrimitiveSetPtr& primitive_set);
    void setPrimitiveSet(const PrimitiveSetPtr& primitive_set);
    PrimitiveSetPtr primitiveSet(unsigned int index = 0) const;

    void removePrimitiveSet(unsigned int index);
    void clearPrimitiveSets();
    inline unsigned int numPrimitiveSets() const { return primitive_sets_.size(); }

    void texture(const TexturePtr& texture, unsigned int target = 0);
    TexturePtr texture(unsigned int target = 0) const;

    inline const TextureMap& textures() const { return textures_; }
    void removeTexture(unsigned int target);
    void clearTexture();

    void dirty();

    void renderTechnique(const RenderTechniquePtr& technique);
    RenderTechniquePtr& renderTechnique();
    const RenderTechniquePtr& renderTechnique() const;

    virtual BoundingBox boundingBox() const;

    virtual void render(RenderInfo& info);

  private:
    void dirtyGLObjects();
    void setupGLObjects();

  protected:
    std::string name_;
    NodeWeakPtr parent_;
    glm::mat4 mv_;
    bool dirty_;
    std::string uuid_;

    Vec3ArrayPtr vertex_array_;
    Vec3ArrayPtr normal_array_;
    Vec4ArrayPtr color_array_;
    Vec2ArrayPtr texcoord_array_;

    std::vector<PrimitiveSetPtr> primitive_sets_;
    TextureMap textures_;

    RenderTechniquePtr render_technique_;

    GLObjectPtrs gl_objects_;
  };
}
#endif