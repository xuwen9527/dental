#ifndef __BOUNDING_BOX_H__
#define __BOUNDING_BOX_H__

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Dental {
  template<typename VT>
  class BoundingSphereImpl;

  template<typename VT>
  class BoundingBoxImpl {
  private:
    using vec_type = VT;
    using value_type = typename VT::value_type;

    vec_type min_;
    vec_type max_;

  public:
    inline BoundingBoxImpl() {
      init();
    }

    template<typename BT>
    inline BoundingBoxImpl(const BoundingBoxImpl<BT>& bb) :
      min_(bb.min_),
      max_(bb.max_) {}

    inline BoundingBoxImpl(const vec_type& min, const vec_type& max) :
      min_(min),
      max_(max) {}

    inline void init() {
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        min_[i] = std::numeric_limits<float>::max();
        max_[i] = -std::numeric_limits<float>::max();
      }
    }

    inline BoundingBoxImpl& operator = (BoundingBoxImpl&& rhs) noexcept {
      if (this != &rhs) {
        min_ = std::move(rhs.min_);
        max_ = std::move(rhs.max_);
      }
      return *this;
    }

    inline BoundingBoxImpl& operator = (const BoundingBoxImpl& rhs) {
      if (this != &rhs) {
        min_ = rhs.min_;
        max_ = rhs.max_;
      }
      return *this;
    }

    inline BoundingBoxImpl(const BoundingBoxImpl& rhs) {
      *this = rhs;
    }

    inline BoundingBoxImpl(BoundingBoxImpl&& rhs) noexcept {
      *this = std::move(rhs);
    }

    inline bool operator == (const BoundingBoxImpl& rhs) const {
      return min_ == rhs.min_ && max_ == rhs.max_;
    }
    inline bool operator != (const BoundingBoxImpl& rhs) const {
      return min_ != rhs.min_ || max_ != rhs.max_;
    }

    inline bool valid() const {
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        if (max_[i] < min_[i]) return false;
      }
      return true;
    }

    inline void set(const vec_type& min, const vec_type& max) {
      min_ = min;
      max_ = max;
    }

    inline const vec_type& min() const { return min_; }
    inline const vec_type& max() const { return max_; }

    inline const value_type& min(unsigned int index) const { return min_[index]; }
    inline const value_type& max(unsigned int index) const { return max_[index]; }
    inline value_type range(unsigned int index) const { return max_[index] - min_[index]; }

    inline const vec_type center() const {
      return (min_ + max_) * (value_type)(0.5);
    }

    inline const vec_type corner(unsigned int pos) const {
      vec_type v;
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i, pos = pos >> 1) {
        v[i] = (pos & 1) ? max_[i] : min_[i];
      }
      return v;
    }

    inline value_type radius() const {
      return std::sqrt(radius2());
    }

    inline value_type radius2() const {
      vec_type e;
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        e[i] = (max_[i] - min_[i]) / 2.0;
      }

      value_type length{ 0.0 };
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        length += e[i] * e[i];
      }
      return length;
    }

    inline void expandBy(const vec_type& v) {
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        if (v[i] < min_[i]) min_[i] = v[i];
        if (v[i] > max_[i]) max_[i] = v[i];
      }
    }

    void expandBy(const BoundingBoxImpl& bb) {
      if (!bb.valid()) return;
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        if (bb.min_[i] < min_[i]) min_[i] = bb.min_[i];
        if (bb.max_[i] > max_[i]) max_[i] = bb.max_[i];
      }
    }

    template<typename VA>
    void expandBy(const VA& va) {
      for (auto& v : *va) {
        expandBy(v);
      }
    }

    template<typename BST>
    void expandBy(const BoundingSphereImpl<BST>& sh) {
      if (!sh.valid()) return;
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        if (sh.center_[i] - sh.radius_ < min_[i]) min_[i] = sh.center_[i] - sh.radius_;
        if (sh.center_[i] + sh.radius_ > max_[i]) max_[i] = sh.center_[i] + sh.radius_;
      }
    }

    BoundingBoxImpl intersect(const BoundingBoxImpl& bb) const {
      vec_type min, max;
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        min[i] = std::max(min_[i](), bb.min_[i]);
        max[i] = std::min(max_[i](), bb.max_[i]);
      }
      return BoundingBoxImpl(min, max);
    }

    bool intersects(const BoundingBoxImpl& bb) const {
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        if (std::max(min_[i], bb.min_[i]) > std::min(max_[i], bb.max_[i]))
          return false;
      }
      return true;
    }

    inline bool contains(const vec_type& v) const {
      if (!valid()) return false;
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        if (v[i] < min_[i] || v[i] > max_[i]) return false;
      }
      return true;
    }

    inline bool contains(const vec_type& v, value_type epsilon) const {
      if (!valid()) return false;
      for (typename vec_type::length_type i = 0; i < vec_type::length(); ++i) {
        if ((v[i] + epsilon) < min_.x() || (v[i] - epsilon) > max_[i]) return false;
      }
      return true;
    }
  };

  using BoundingBoxf = BoundingBoxImpl<glm::vec3>;
  using BoundingBoxd = BoundingBoxImpl<glm::dvec3>;

  using BoundingBox = BoundingBoxf;
}

#endif