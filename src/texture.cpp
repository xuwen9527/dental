#include <texture.h>
#include <glad/glad.h>
#include <image_library.h>

namespace {
  bool check_mipmaped(GLint filter) {
    return
      filter == GL_LINEAR_MIPMAP_LINEAR || 
      filter == GL_LINEAR_MIPMAP_NEAREST ||
      filter == GL_NEAREST_MIPMAP_LINEAR || 
      filter == GL_NEAREST_MIPMAP_NEAREST;
  }
}

namespace Dental {
  TextureGLObject::TextureGLObject() : GLObject(), id_(0),
    target_(0), dirty_(true), texture_parameters_dirty_(true),
    wrap_s_(GL_CLAMP_TO_EDGE), wrap_t_(GL_CLAMP_TO_EDGE),
    min_filter_(GL_NEAREST), mag_filter_(GL_LINEAR) {
  }

  TextureGLObject::~TextureGLObject() {
    release();
  }

  void TextureGLObject::bind(unsigned int target, ImagePtr& image,
    GLint wrap_s, GLint wrap_t, GLint min_filter, GLint mag_filter) {
    target_ = target;

    if (image_.lock() != image) {
      image_ = image;
      dirty_ = true;
      id_ = 0;
    }

    if (wrap_s_ != wrap_s || wrap_t_ != wrap_t ||
      min_filter_ != min_filter || mag_filter_ != mag_filter) {
      wrap_s_ = wrap_s;
      wrap_t_ = wrap_t;
      min_filter_ = min_filter;
      mag_filter_ = mag_filter;
      texture_parameters_dirty_ = true;
    }
  }

  void TextureGLObject::bind() {
    if (!id_) {
      if (!image_.expired()) {
        auto image = image_.lock();
        id_ = image->textureId();
      }
      if (!id_) {
        //todo: error
        return;
      }
    }

    glActiveTexture(GL_TEXTURE0 + target_);
    glBindTexture(GL_TEXTURE_2D, id_);

    if (!dirty_) {
      return;
    }

    dirty_ = false;

    if (texture_parameters_dirty_) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter_);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter_);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s_);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t_);

      if (check_mipmaped(min_filter_) || check_mipmaped(mag_filter_)) {
        float mMaxAnisotropy;
        //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &mMaxAnisotropy);
        //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, mMaxAnisotropy);
      }

      texture_parameters_dirty_ = false;
    }
  }

  void TextureGLObject::unbind() {
    glActiveTexture(GL_TEXTURE0 + target_);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void TextureGLObject::release() {
    if (id_) {
      id_ = 0;
    }
  }

  void TextureGLObject::dirty() {
    dirty_ = true;
    texture_parameters_dirty_ = true;
  }

  bool TextureGLObject::valid() {
    return id_ != 0;
  }

  Texture::Texture() : 
    gl_object_(std::make_shared<TextureGLObject>()),
    image_name_("texture"),
    wrap_s_(WrapMode::CLAMP_TO_EDGE),
    wrap_t_(WrapMode::CLAMP_TO_EDGE),
    min_filter_(FilterMode::LINEAR_MIPMAP_NEAREST),
    mag_filter_(FilterMode::LINEAR_MIPMAP_LINEAR) {
    image_ = ImageLibrary::instance().get(image_name_);
  }

  Texture::~Texture() {
  }

  Texture& Texture::operator = (const Texture& rhs) {
    if (this != &rhs) {
      image_ = rhs.image_;
      image_name_ = rhs.image_name_;
      wrap_s_ = rhs.wrap_s_;
      wrap_t_ = rhs.wrap_t_;
      min_filter_ = rhs.min_filter_;
      mag_filter_ = rhs.mag_filter_;
      dirty();
    }
    return *this;
  }

  Texture& Texture::operator = (Texture&& rhs) noexcept {
    if (this != &rhs) {
      image_ = std::move(rhs.image_);
      image_name_ = std::move(rhs.image_name_);
      wrap_s_ = std::move(rhs.wrap_s_);
      wrap_t_ = std::move(rhs.wrap_t_);
      min_filter_ = std::move(rhs.min_filter_);
      mag_filter_ = std::move(rhs.mag_filter_);
      gl_object_ = std::move(rhs.gl_object_);
    }
    return *this;
  }

  Texture::Texture(const Texture& rhs) {
    *this = rhs;
  }

  Texture::Texture(Texture&& rhs) noexcept {
    *this = std::move(rhs);
  }

  void Texture::image(const ImagePtr& image) {
    image_ = image;
  }

  void Texture::imageName(const std::string& name) {
    image_name_ = name;
    image_ = ImageLibrary::instance().get(name);
  }

  TexturePtr Texture::clone() {
    TexturePtr texture = std::make_shared<Texture>(*this);
    return texture;
  }

  const ImagePtr& Texture::image() const {
    return image_;
  }

  void Texture::filter(Filter which, FilterMode filter) {
    switch (which) {
      case Filter::MIN_FILTER :
        min_filter_ = filter;
        break;
      case Filter::MAG_FILTER :
        mag_filter_ = filter;
        break;
      default :
        //todo: warn
        break;
    }
  }

  Texture::FilterMode Texture::filter(Filter which) const {
    switch (which) {
      case Filter::MIN_FILTER :
        return min_filter_;
      case Filter::MAG_FILTER :
        return mag_filter_;
      default :
        //todo: warn
        return min_filter_;
    }
  }

  void Texture::wrap(Wrap which, WrapMode wrap) {
    switch (which) {
      case Wrap::WRAP_S :
        wrap_s_ = wrap;
        break;
      case Wrap::WRAP_T :
        wrap_t_ = wrap;
        break;
      default :
        break;
    }
  }

  Texture::WrapMode Texture::wrap(Wrap which) const {
    switch (which) {
      case Wrap::WRAP_S :
        return wrap_s_;
      case Wrap::WRAP_T :
        return wrap_t_;
      default :
        return wrap_s_;
    }
  }

  void Texture::dirty() {
    gl_object_->dirty();
  }

  void Texture::bind(unsigned int target) {
    gl_object_->bind(target, image_,
      static_cast<std::underlying_type<Wrap>::type>(wrap_s_), 
      static_cast<std::underlying_type<Wrap>::type>(wrap_t_),
      static_cast<std::underlying_type<Filter>::type>(min_filter_), 
      static_cast<std::underlying_type<Filter>::type>(mag_filter_));
  }
}