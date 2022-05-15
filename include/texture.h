#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <memory>
#include <glad/glad.h>
#include <gl_object.h>
#include <image.h>

namespace Dental {
  class TextureGLObject : public GLObject {
  public:
    TextureGLObject();
    virtual ~TextureGLObject() override;

    TextureGLObject& operator = (TextureGLObject&&) noexcept = delete;
    TextureGLObject& operator = (const TextureGLObject&) = delete;
    TextureGLObject(const TextureGLObject&) = delete;
    TextureGLObject(TextureGLObject&&) noexcept = delete;

    inline unsigned int id() { return id_; }

    virtual void bind(unsigned int target, ImagePtr& image, GLint wrap_s, GLint wrap_t, GLint min_filter, GLint mag_filter);

    virtual void bind() override;

    virtual void unbind() override;

    virtual void release() override;

    virtual void dirty() override;

    virtual bool valid() override;

  private:
    unsigned int id_;
    unsigned int target_;
    bool dirty_;
    bool texture_parameters_dirty_;
    std::weak_ptr<Image> image_;

    GLint wrap_s_, wrap_t_, min_filter_, mag_filter_;
  };

  using TextureGLObjectPtr = std::shared_ptr<TextureGLObject>;

  class Texture;
  using TexturePtr = std::shared_ptr<Texture>;

  class Texture {
  public:
    enum class Wrap {
      WRAP_S = GL_TEXTURE_WRAP_S,
      WRAP_T = GL_TEXTURE_WRAP_T,
    };

    enum class WrapMode {
      CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
      REPEAT = GL_REPEAT,
    };

    enum class Filter {
      MIN_FILTER = GL_TEXTURE_MIN_FILTER,
      MAG_FILTER = GL_TEXTURE_MAG_FILTER,
    };

    enum class FilterMode {
      LINEAR = GL_LINEAR,
      LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
      LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
      NEAREST = GL_NEAREST,
      NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
      NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST
    };

    Texture();
    ~Texture();

    Texture& operator = (const Texture& rhs);
    Texture& operator = (Texture&& rhs) noexcept;
    Texture(const Texture& rhs);
    Texture(Texture&& rhs) noexcept;

    TexturePtr clone();

    void image(const ImagePtr& image);
    const ImagePtr& image() const;

    inline const std::string& imageName() const { return image_name_; }
    inline void imageName(const std::string& name);

    void bind(unsigned int target = 0);

    void filter(Filter which, FilterMode filter);
    FilterMode filter(Filter which) const;

    void wrap(Wrap which, WrapMode wrap);
    WrapMode wrap(Wrap which) const;

    void dirty();

    TextureGLObjectPtr GLObject() { return gl_object_; }

private:
    ImagePtr image_;
    std::string image_name_;

    WrapMode wrap_s_;
    WrapMode wrap_t_;

    FilterMode min_filter_;
    FilterMode mag_filter_;

    TextureGLObjectPtr gl_object_;
  };

  using TexturePtr = std::shared_ptr<Texture>;
}
#endif