#include <glad/glad.h>
#include <vector>
#include <gl_frame_buffer.h>

namespace Dental {
  GLFrameBuffer::GLFrameBuffer() :
    dirty_(true),
    width_(400), height_(300),
    fbo_(0), depth_(0) {
  }

  void GLFrameBuffer::bind() {
    if (fbo_ == 0) {
      glGenFramebuffers(1, &fbo_);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  }

  void GLFrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void GLFrameBuffer::release() {
    if (fbo_) {
      glDeleteFramebuffers(1, &fbo_);
      fbo_ = 0;
    }
  }

  void GLFrameBuffer::dirty() {
    dirty_ = true;
  }

  bool GLFrameBuffer::valid() {
    return !dirty_ && fbo_ != 0 && depth_ != 0 && colors_.size();
  }

  void GLFrameBuffer::resize(unsigned int width, unsigned int height) {
    if (width_ != width || height_ != height) {
      width_ = width;
      height_ = height;
      dirty();
    }
  }

  void GLFrameBuffer::attachColor(unsigned int color_attachment) {
    colors_[color_attachment] = 0;
  }

  unsigned int GLFrameBuffer::color(unsigned int color_attachment) const {
    auto itr = colors_.find(color_attachment);
    if (itr != colors_.end()) {
      return itr->second;
    }
    return 0;
  }

  void GLFrameBuffer::blit(
    int x, int y,
    unsigned int width, unsigned int height, unsigned int color_attachment) const {
    auto itr = colors_.find(color_attachment);
    if (itr == colors_.end()) {
      return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, itr->second);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(0, 0, width_, height_, x, y, x + width, y + height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBlitFramebuffer(0, 0, width_, height_, x, y, x + width, y + height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  GLFrameRenderBuffer::GLFrameRenderBuffer() : GLFrameBuffer(),
    multi_sample_(4) {
  }

  void GLFrameRenderBuffer::bind() {
    if (dirty_) {
      release();
      dirty_ = false;
    }

    if (!fbo_) {
      if (depth_ == 0) {
        glGenRenderbuffers(1, &depth_);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_);
        if (multi_sample_ == 0) {
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
        }
        else {
          glRenderbufferStorageMultisample(GL_RENDERBUFFER, multi_sample_, GL_DEPTH24_STENCIL8, width_, height_);
        }
      }

      for (auto& itr : colors_) {
        if (itr.second == 0) {
          glGenRenderbuffers(1, &(itr.second));
          glBindRenderbuffer(GL_RENDERBUFFER, itr.second);
          if (multi_sample_ == 0) {
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width_, height_);
          }
          else {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, multi_sample_, GL_RGBA8, width_, height_);
          }
        }
      }

      GLFrameBuffer::bind();

      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_);
      for (auto& itr : colors_) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, (GL_COLOR_ATTACHMENT0 + itr.first), GL_RENDERBUFFER, itr.second);
      }

      std::vector<GLenum> drawbuffers;
      drawbuffers.reserve(colors_.size());
      for (auto& itr : colors_) {
        drawbuffers.emplace_back(GL_COLOR_ATTACHMENT0 + itr.first);
      }
      glDrawBuffers(drawbuffers.size(), drawbuffers.data());

      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE) {
        //todo: error
      }
    }

    GLFrameBuffer::bind();
  }

  void GLFrameRenderBuffer::release() {
    GLFrameBuffer::release();

    if (depth_) {
      glDeleteRenderbuffers(1, &depth_);
      depth_ = 0;
    }

    for (auto& itr : colors_) {
      if (itr.second) {
        glDeleteRenderbuffers(1, &itr.second);
        itr.second = 0;
      }
    }
  }

  void GLFrameRenderBuffer::multiSample(unsigned int multi_sample) {
    multi_sample_ = multi_sample;
    dirty();
  }

  void GLFrameRenderBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  }

  GLFrameTextureBuffer::GLFrameTextureBuffer() : GLFrameBuffer() {
  }

  void GLFrameTextureBuffer::bind() {
    if (dirty_) {
      release();
      dirty_ = false;
    }

    if (!fbo_) {
      if (depth_ == 0) {
        glGenTextures(1, &depth_);
        glBindTexture(GL_TEXTURE_2D, depth_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width_, height_, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      }

      for (auto& itr : colors_) {
        if (itr.second == 0) {
          glGenTextures(1, &(itr.second));
          glBindTexture(GL_TEXTURE_2D, itr.second);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
      }

      glBindTexture(GL_TEXTURE_2D, 0);

      GLFrameBuffer::bind();

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_, 0);
      for (auto& itr : colors_) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, (GL_COLOR_ATTACHMENT0 + itr.first), GL_TEXTURE_2D, itr.second, 0);
      }

      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if (status != GL_FRAMEBUFFER_COMPLETE) {
        //todo: error
      }
    }

    GLFrameBuffer::bind();
  }

  void GLFrameTextureBuffer::release() {
    GLFrameBuffer::release();

    if (depth_) {
      glDeleteTextures(1, &depth_);
      depth_ = 0;
    }

    for (auto& itr : colors_) {
      if (itr.second) {
        glDeleteTextures(1, &itr.second);
        itr.second = 0;
      }
    }
  }
}