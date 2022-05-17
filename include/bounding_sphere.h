#ifndef __BOUNDINGSPHERE_H__
#define __BOUNDINGSPHERE_H__

#include <glm/ext.hpp>
#include <glm/gtx/norm.hpp>
#include <float.h>
#include <type_traits>

namespace Dental {
  template<typename VT>
  class BoundingBoxImpl;

  template<typename VT>
  class BoundingSphereImpl {
    private:
    using vec_type = VT;
    using value_type = typename VT::value_type;

    vec_type center_;
    value_type radius_;

  public:
    /** Construct a default bounding sphere with radius to -1.0f, representing an invalid/unset bounding sphere.*/
    BoundingSphereImpl() : center_(0.0, 0.0, 0.0), radius_(-1.0) {}

    /** Creates a bounding sphere initialized to the given extents. */
    BoundingSphereImpl(const vec_type &center, value_type radius) : center_(center),
                                                                    radius_(radius) {}

    /** Creates a bounding sphere initialized to the given extents. */
    BoundingSphereImpl(const BoundingSphereImpl &bs) : center_(bs.center_),
                                                       radius_(bs.radius_) {}

    /** Creates a bounding sphere initialized to the given extents. */
    BoundingSphereImpl(const BoundingBoxImpl<VT> &bb) : center_(0.0, 0.0, 0.0),
                                                        radius_(-1.0) { expandBy(bb); }

    /** Clear the bounding sphere. Reset to default values. */
    inline void init() {
      center_.x = 0.f;
      center_.y = 0.f;
      center_.z = 0.f;
      radius_ = -1.0;
    }

    /* Returns true of the bounding sphere extents are valid, false
     * otherwise.
     */
    inline bool valid() const { return radius_ >= 0.0; }

    inline bool operator==(const BoundingSphereImpl &rhs) const {
      return center_ == rhs.center_ && radius_ == rhs._radius;
    }

    inline bool operator!=(const BoundingSphereImpl &rhs) const {
      return center_ != rhs.center_ || radius_ == rhs._radius;
    }

    /** Set the bounding sphere to the given center/radius using floats. */
    inline void set(const vec_type &center, value_type radius) {
      center_ = center;
      radius_ = radius;
    }

    /** Returns the center of the bounding sphere. */
    inline vec_type &center() { return center_; }

    /** Returns the const center of the bounding sphere. */
    inline const vec_type &center() const { return center_; }

    /** Returns the radius of the bounding sphere. */
    inline value_type &radius() { return radius_; }

    /** Returns the const radius of the bounding sphere. */
    inline value_type radius() const { return radius_; }

    /** Returns the squared length of the radius. Note, For performance
     * reasons, the calling method is responsible for checking to make
     * sure the sphere is valid. */
    inline value_type radius2() const { return radius_ * radius_; }

    template<typename VA>
    void expandBy(const VA &va) {
      for (auto &v : va) {
        expandBy(v);
      }
    }

    /** Expands the sphere to encompass the given point. Repositions the
     * sphere center to minimize the radius increase. If the sphere is
     * uninitialized, set its center to v and radius to zero.
     */
    void expandBy(const vec_type &v) {
      if (valid()) {
        vec_type dv = v - center_;
        value_type r{0.0};
        for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
          r += dv[i] * dv[i];
        }
        r = std::sqrt(r);
        if (r > radius_) {
          value_type dr = (r - radius_) * 0.5;
          center_ += dv / r * dr;
          radius_ += dr;
        } // else do nothing as vertex is within sphere.
      } else {
        center_ = v;
        radius_ = 0.000001f;
      }
    };

    /** Expands the sphere to encompass the given sphere. Repositions the
     * sphere center to minimize the radius increase. If the sphere is
     * uninitialized, set its center and radius to match sh. */
    void expandBy(const BoundingSphereImpl &sh);

    /** Expands the sphere to encompass the given box. Repositions the
     * sphere center to minimize the radius increase. */
    template<typename BBT>
    void expandBy(const BoundingBoxImpl<BBT> &bb);

    /** Expands the sphere to encompass the given point. Does not
     * reposition the sphere center. If the sphere is
     * uninitialized, set its center to v and radius to zero. */
    template<typename vector_type>
    void expandRadiusBy(const vector_type &v);

    /** Expands the sphere to encompass the given sphere. Does not
     * repositions the sphere center. If the sphere is
     * uninitialized, set its center and radius to match sh. */
    void expandRadiusBy(const BoundingSphereImpl &sh);

    /** Expands the sphere to encompass the given box. Does not
     * repositions the sphere center. */
    template<typename BBT>
    void expandRadiusBy(const BoundingBoxImpl<BBT> &bb);

    /** Returns true if v is within the sphere. */
    inline bool contains(const vec_type &v) const {
      return valid() && (glm::distance2(v, center_) <= radius2());
    }

    /** Returns true if there is a non-empty intersection with the given
     * bounding sphere. */
    inline bool intersects(const BoundingSphereImpl &bs) const {
      vec_type d = center_ - bs._center;
      value_type length2 = d * d;
      return valid() && bs.valid() &&
             (length2 <= (radius_ + bs._radius) * (radius_ + bs._radius));
    }
  };

  template<typename VT>
  template<typename vector_type>
  void BoundingSphereImpl<VT>::expandRadiusBy(const vector_type &v) {
    if (valid()) {
      vec_type d = (vec_type(v) - center_);
      value_type r = sqrt(d.x * d.x + d.y + d.y + d.z * d.z);
      if (r > radius_){
        radius_ = r;
      }
    // else do nothing as vertex is within sphere.
    } else {
      center_ = v;
      radius_ = 0.0;
    }
  }

  template<typename VT>
  void BoundingSphereImpl<VT>::expandBy(const BoundingSphereImpl &sh) {
    // ignore operation if incomming BoundingSphere is invalid.
    if (!sh.valid()) {
      return;
    }
      // This sphere is not set so use the inbound sphere
    if (!valid()) {
      center_ = sh.center_;
      radius_ = sh.radius_;
      return;
    }

    // Calculate d == The distance between the sphere centers
    vec_type d_vec = center_ - sh.center();
    value_type d{0.0};
    for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
      d += d_vec[i] * d_vec[i];
    }
    d = std::sqrt(d);

    // New sphere is already inside this one
    if (d + sh.radius() <= radius_) {
      return;
    }

    //  New sphere completely contains this one
    if (d + radius_ <= sh.radius()) {
      center_ = sh.center_;
      radius_ = sh.radius_;
      return;
    }

    // Build a new sphere that completely contains the other two:
    //
    // The center point lies halfway along the line between the furthest
    // points on the edges of the two spheres.
    //
    // Computing those two points is ugly - so we'll use similar triangles
    double new_radius = (radius_ + d + sh.radius()) * 0.5;
    double ratio = (new_radius - radius_) / d;

    center_[0] += (sh.center()[0] - center_[0]) * ratio;
    center_[1] += (sh.center()[1] - center_[1]) * ratio;
    center_[2] += (sh.center()[2] - center_[2]) * ratio;

    radius_ = new_radius;
  }

  template<typename VT>
  void BoundingSphereImpl<VT>::expandRadiusBy(const BoundingSphereImpl &sh) {
    if (sh.valid()) {
      if (valid()) {
        vec_type d_vec = sh._center - center_;
        value_type d = sqrt(d_vec.x * d_vec.x + d_vec.y * d_vec.y);
        value_type r = d + sh.radius_;
        if (r > radius_) {
          radius_ = r;
        }
        // else do nothing as vertex is within sphere.
      } else {
        center_ = sh.center_;
        radius_ = sh.radius_;
      }
    }
  }

  template<typename VT>
  template<typename BBT>
  void BoundingSphereImpl<VT>::expandBy(const BoundingBoxImpl<BBT> &bb) {
    if (bb.valid()) {
      if (valid()) {
        BoundingBoxImpl<vec_type> newbb(bb);
        for (unsigned int i = 0; i < 8; ++i) {
          vec_type v = bb.corner(i) - center_; // get the direction vector from corner
          v.normalize(); // normalise it.
          v *= -radius_; // move the vector in the opposite direction distance radius.
          v += center_; // move to absolute position.
          newbb.expandBy(v); // add it into the new bounding box.
        }
        center_ = newbb.center();
        radius_ = newbb.radius();
      } else {
        center_ = bb.center();
        radius_ = bb.radius();
      }
    }
  }

  template<typename VT>
  template<typename BBT>
  void BoundingSphereImpl<VT>::expandRadiusBy(const BoundingBoxImpl<BBT> &bb) {
    if (bb.valid()) {
      if (valid()) {
        for (unsigned int c = 0; c < 8; ++c) {
          expandRadiusBy(bb.corner(c));
        }
      } else {
        center_ = bb.center();
        radius_ = bb.radius();
      }
    }
  }

  using BoundingSpheref = BoundingSphereImpl<glm::vec3>;
  using BoundingSphered = BoundingSphereImpl<glm::dvec3>;

  using BoundingSphere = BoundingSpheref;
}
#endif
