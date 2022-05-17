#ifndef __NODE_H__
#define __NODE_H__

#include <memory>
#include <list>
#include <string>
#include <glm/mat4x4.hpp>
#include <visitor.h>
#include <geometry.h>
#include <bounding_sphere.h>

namespace Dental {
  class Node;
  using NodePtr = std::shared_ptr<Node>;
  using NodeWeakPtr = std::weak_ptr<Node>;

#define Mate_Node(library, class) \
  virtual const std::string& className() const override { static std::string name = #class; return name; } \
  virtual const std::string& libraryName() const override { static std::string name = #library; return name; }

  class Node : public std::enable_shared_from_this<Node> {
  public:
    Node();
    virtual ~Node();

    virtual const std::string& className() const { static std::string name = "Node"; return name; } \
    virtual const std::string& libraryName() const { static std::string name = "Dental"; return name; }

    Node& operator = (const Node& rhs);
    Node& operator = (Node&& rhs) noexcept;
    Node(const Node& rhs);
    Node(Node&& rhs) noexcept;

    inline void name(const std::string& name) { name_ = name; }
    inline const std::string& name() const { return name_; }
    
    inline void mv(const glm::mat4& mv) { mv_ = mv; }
    inline const glm::mat4& mv() const { return mv_; }

    inline void uuid(const std::string& id) { uuid_ = id; }
    inline const std::string& uuid() const { return uuid_; }

    inline glm::vec3 origin() const;
    void origin(const glm::vec3&);

    void get_scale_rotate_translate(glm::vec3& scale, glm::quat& rotate, glm::vec3& translate) const;
    void set_scale_rotate_translate(glm::vec3 scale, glm::quat rotate, glm::vec3 translate);

    void scale(const glm::vec3& scale);
    void translate(const glm::vec3& trans);
    void rotate(float angle, const glm::vec3& normal);

    void addGeometry(const GeometryPtr& geometry);
    GeometryPtr geometry(const std::string& name) const;

    int geometryIndex(const GeometryPtr& geometry);
    void insertGeometry(unsigned int index, const GeometryPtr& geometry);

    unsigned int numGeometry() const;
    GeometryPtr geometry(unsigned int index) const;

    bool removeGeometry(unsigned int index);
    bool removeGeometry(const GeometryPtr& geometry);
    void clearGeometry();

    void dirtyBounding();
    BoundingSphere& boundingSphere();

    inline virtual void accept(Visitor& visitor);

    virtual void traverse(Visitor& visitor);

    inline NodePtr ptr() { return shared_from_this(); }

  protected:
    std::string name_;
    glm::mat4 mv_;
  	std::string uuid_;
    std::vector<GeometryPtr> geometrys_;

    BoundingSphere bounding_sphere_;
    bool dirty_bounding_;
  };
}
#endif