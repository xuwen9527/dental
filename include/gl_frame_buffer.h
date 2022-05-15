#ifndef __GLFRAMEBUFFER_H__
#define __GLFRAMEBUFFER_H__

#include <unordered_map>
#include <gl_object.h>

namespace Dental {
  class GLFrameBuffer : public GLObject {
  public:
    GLFrameBuffer();
    ~GLFrameBuffer() override { release(); }

    GLFrameBuffer& operator = (GLFrameBuffer&&) noexcept = delete;
    GLFrameBuffer& operator = (const GLFrameBuffer&) = delete;
    GLFrameBuffer(const GLFrameBuffer&) = delete;
    GLFrameBuffer(GLFrameBuffer&&) noexcept = delete;

    virtual void bind() override;

    virtual void unbind() override;

    virtual void release() override;

    virtual void dirty() override;

    virtual bool valid() override;

    void resize(unsigned int width, unsigned int height);

    inline unsigned int width() const { return width_; }
    inline unsigned int height() const { return height_; }

    void attachColor(unsigned int color_attachment = 0);

    inline unsigned int depth() const { return depth_; }
    unsigned int color(unsigned int color_attachment = 0) const;

    void blit(
      int x, int y,
      unsigned int width, unsigned int height,
      unsigned int color_attachment = 0) const;

  protected:
    bool dirty_;
    unsigned int width_;
    unsigned int height_;

    unsigned int fbo_;
    unsigned int depth_;
    std::unordered_map<unsigned int, unsigned int> colors_;
  };

  using GLFrameBufferPtr = std::shared_ptr<GLFrameBuffer>;

  class GLFrameRenderBuffer : public GLFrameBuffer {
  public:
    GLFrameRenderBuffer();
    ~GLFrameRenderBuffer() override { release(); }

    GLFrameRenderBuffer& operator = (GLFrameBuffer&&) noexcept = delete;
    GLFrameRenderBuffer& operator = (const GLFrameBuffer&) = delete;
    GLFrameRenderBuffer(const GLFrameBuffer&) = delete;
    GLFrameRenderBuffer(GLFrameBuffer&&) noexcept = delete;

    void bind() override;
    void unbind() override;

    void release() override;

    void multiSample(unsigned int);
    inline unsigned int multiSample() { return multi_sample_; }

  protected:
    unsigned int multi_sample_;
  };

  using GLFrameRenderBufferPtr = std::shared_ptr<GLFrameRenderBuffer>;

  class GLFrameTextureBuffer : public GLFrameBuffer {
  public:
    GLFrameTextureBuffer();
    ~GLFrameTextureBuffer() override { release(); }

    GLFrameTextureBuffer& operator = (GLFrameTextureBuffer&&) noexcept = delete;
    GLFrameTextureBuffer& operator = (const GLFrameTextureBuffer&) = delete;
    GLFrameTextureBuffer(const GLFrameTextureBuffer&) = delete;
    GLFrameTextureBuffer(GLFrameTextureBuffer&&) noexcept = delete;

    void bind() override;

    void release() override;
  };

  using GLFrameTextureBufferPtr = std::shared_ptr<GLFrameTextureBuffer>;
}
#endif