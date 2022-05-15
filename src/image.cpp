#include <algorithm>
#include <image.h>
#include <glad/glad.h>

namespace {
  template<typename T>
  bool findLowerAlphaValueInRow(unsigned int num, T *data, T value, unsigned int delta) {
    for (unsigned int i = 0; i < num; ++i) {
      if (*data < value) {
        return true;
      }
      data += delta;
    }
    return false;
  }

  template<typename T>
  bool maskedFindLowerAlphaValueInRow(unsigned int num, T *data, T value, T mask,
                                       unsigned int delta) {
    for (unsigned int i = 0; i < num; ++i) {
      if ((*data & mask) < value) return true;
      data += delta;
    }
    return false;
  }

  template<typename T>
  glm::vec4 readColor(GLenum pixel_format, T *data, float scale) {
    switch (pixel_format) {
      case (GL_DEPTH_COMPONENT):   //intentionally fall through and execute the code for GL_LUMINANCE
      case (GL_LUMINANCE): {
        float l = float(*data++) * scale;
        return glm::vec4(l, l, l, 1.0f);
      }
      case (GL_ALPHA): {
        float a = float(*data++) * scale;
        return glm::vec4(1.0f, 1.0f, 1.0f, a);
      }
      case (GL_LUMINANCE_ALPHA): {
        float l = float(*data++) * scale;
        float a = float(*data++) * scale;
        return glm::vec4(l, l, l, a);
      }
      case (GL_RGB): {
        float r = float(*data++) * scale;
        float g = float(*data++) * scale;
        float b = float(*data++) * scale;
        return glm::vec4(r, g, b, 1.0f);
      }
      case (GL_RGBA): {
        float r = float(*data++) * scale;
        float g = float(*data++) * scale;
        float b = float(*data++) * scale;
        float a = float(*data++) * scale;
        return glm::vec4(r, g, b, a);
      }
    }
    return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  }
}

namespace Dental {
  Image::Image() : s_ (0), t_(0), r_(0), vertically_fliped_(false),
    internal_texture_format_(0), pixel_format_(0),
    data_type_(GL_UNSIGNED_BYTE), packing_(4),
    allocation_mode_(AllocationMode::USE_NEW_DELETE), 
    data_(nullptr),
    texture_id_(0) {
  }

  Image::~Image() {
    release();
    deallocateData();
  }

  Image::Image(const Image& rhs) {
    *this = rhs;
  }
  
  Image::Image(Image&& rhs) noexcept {
    *this = std::move(rhs);
  }

  Image& Image::operator = (const Image& rhs) {
    if (this != &rhs) {
      data(rhs.data_, rhs.allocation_mode_);
      file_name_ = rhs.file_name_;
      s_ = rhs.s_;
      t_ = rhs.t_;
      r_ = rhs.r_;
      vertically_fliped_ = rhs.vertically_fliped_;
      internal_texture_format_ = rhs.internal_texture_format_;
      pixel_format_ = rhs.pixel_format_;
      data_type_ = rhs.data_type_;
      packing_ = rhs.packing_;
      texture_id_ = rhs.texture_id_;
    }
    return *this;
  }
  
  Image& Image::operator = (Image&& rhs) noexcept {
    if (this != &rhs) {
      data_ = rhs.data_;
      file_name_ = std::move(file_name_);
      s_ = std::move(rhs.s_);
      t_ = std::move(rhs.t_);
      r_ = std::move(rhs.r_);
      vertically_fliped_ = std::move(rhs.vertically_fliped_);
      internal_texture_format_ = std::move(rhs.internal_texture_format_);
      pixel_format_ = std::move(rhs.pixel_format_);
      data_type_ = std::move(rhs.data_type_);
      packing_ = std::move(rhs.packing_);
      allocation_mode_ = std::move(rhs.allocation_mode_);
      texture_id_ = std::move(rhs.texture_id_);
    }
    return *this;
  }

  void Image::deallocateData() {
    if (data_) {
      if (allocation_mode_ == AllocationMode::USE_NEW_DELETE) {
        delete[] data_;
      }
      else if (allocation_mode_ == AllocationMode::USE_MALLOC_FREE) {
        ::free(data_);
      }
      data_ = nullptr;
    }
  }

  void Image::release() {
    if (texture_id_) {
      glDeleteTextures(1, &texture_id_);
      texture_id_ = 0;
    }
  }

  void Image::fileName(const std::string &fileName) {
    file_name_ = fileName;
  }

  void Image::data(unsigned char *data, AllocationMode mode) {
    deallocateData();
    data_ = data;
    allocation_mode_ = mode;
  }

  unsigned int Image::textureId() {
    if (!texture_id_) {
      glGenTextures(1, &texture_id_);
      if (texture_id_) {
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        GLint alignment;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, internal_texture_format_,
          s_, t_, 0,
          pixel_format_, data_type_, data_);

        glGenerateMipmap(GL_TEXTURE_2D);

        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        glBindTexture(GL_TEXTURE_2D, 0);

        deallocateData();
      }
    }
    return texture_id_;
  }

  bool Image::isPackedType(GLenum type) {
    switch (type) {
      case (GL_UNSIGNED_SHORT_5_6_5):
      case (GL_UNSIGNED_SHORT_4_4_4_4):
      case (GL_UNSIGNED_SHORT_5_5_5_1):
        return true;
      default:
        return false;
    }
  }

  GLenum Image::computePixelFormat(GLenum format) {
    switch (format) {
      case (GL_DEPTH_COMPONENT16):
        return GL_DEPTH_COMPONENT;
      default:
        return format;
    }
  }

  GLenum Image::computeFormatDataType(GLenum pixel_format) {
    switch (pixel_format) {
      case GL_RGBA:
      case GL_RGB:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_ALPHA:
        return GL_UNSIGNED_BYTE;
      default: {
        //todo::
        //LOG_ERROR("error computeFormatType = %o", pixel_format);
        return 0;
      }
    }
  }

  unsigned int Image::computeNumComponents(GLenum pixel_format) {
    switch (pixel_format) {
      // case (GL_STENCIL_INDEX):
      //     return 1;
      case (GL_DEPTH_COMPONENT):
        return 1;
      case (GL_DEPTH_COMPONENT16):
        return 1;
      case (GL_ALPHA):
        return 1;
      case (GL_RGB):
        return 3;
      case (GL_RGBA):
        return 4;
      case (GL_LUMINANCE):
        return 1;
      case (GL_LUMINANCE_ALPHA):
        return 2;
      default: {
        //todo::
        //LOG_ERROR("error pixelFormat = %o", pixelFormat);
        return 0;
      }
    }
  }

  unsigned int Image::computePixelSizeInBits(GLenum format, GLenum type) {
    switch (type) {
      case (GL_BYTE):
      case (GL_UNSIGNED_BYTE):
        return 8 * computeNumComponents(format);
      case (GL_SHORT):
      case (GL_UNSIGNED_SHORT):
        return 16 * computeNumComponents(format);
      case (GL_INT):
      case (GL_UNSIGNED_INT):
      case (GL_FLOAT):
        return 32 * computeNumComponents(format);
      case (GL_UNSIGNED_SHORT_5_6_5):
      case (GL_UNSIGNED_SHORT_4_4_4_4):
      case (GL_UNSIGNED_SHORT_5_5_5_1):
        return 16;
      default: {
        //LOG_ERROR("error type = %d", type);
        //todo::
        return 0;
      }
    }
  }

  unsigned int Image::computeRowWidthInBytes(unsigned int width, GLenum pixel_format,
    GLenum type, unsigned int packing) const {
    unsigned int pixel_size = computePixelSizeInBits(pixel_format, type);
    unsigned int width_in_bits = width * pixel_size;
    unsigned int packing_in_bits = packing != 0 ? packing * 8 : 8;
    //INFO << "width="<<width<<" pixelSize="<<pixelSize<<"  width in bit="<<widthInBits<<" packingInBits="<<packingInBits<<" widthInBits%packingInBits="<<widthInBits%packingInBits<<std::endl;
    return (width_in_bits / packing_in_bits + ((width_in_bits % packing_in_bits) ? 1 : 0)) * packing;
  }

  unsigned int
  Image::computeImageSizeInBytes(unsigned int width, unsigned int height, unsigned int depth,
    GLenum pixel_format, GLenum type, unsigned int packing,
    unsigned int slice_packing, unsigned int image_packing) {
    if (width == 0 || height == 0 || depth == 0) {
      return 0;
    }
    // compute size of one row
    unsigned int size = computeRowWidthInBytes(width, pixel_format, type, packing);

    // now compute size of slice
    size *= height;
    size += slice_packing - 1;
    size -= size % slice_packing;

    // compute size of whole image
    size *= depth;
    size += image_packing - 1;
    size -= size % image_packing;

    return std::max<unsigned int>(size, packing);
  }

  int Image::computeNearestPowerOfTwo(int s, float bias) {
    if ((s & (s - 1)) != 0) {
      // it isn't so lets find the closest power of two.
      // yes, logf and powf are slow, but this code should
      // only be called during scene graph initilization,
      // if at all, so not critical in the greater scheme.
      float p2 = logf((float) s) / logf(2.0f);
      float rounded_p2 = floorf(p2 + bias);
      s = (int) (powf(2.0f, rounded_p2));
    }
    return s;
  }

  void Image::internalTextureFormat(GLint internalFormat) {
    internal_texture_format_ = internalFormat;
  }

  void Image::pixelFormat(GLenum pixelFormat) {
    pixel_format_ = pixelFormat;
  }

  void Image::dataType(GLenum dataType) {
    if (data_type_ == dataType) {
      return; // do nothing if the same.
    }

    if (data_type_ == 0) {
      // setting the datatype for the first time
      data_type_ = dataType;
    } else {
      //todo::
      //LOG_WARN("Image::setDataType(..) - warning, attempt to reset the data type not permitted.");
    }
  }

  void Image::allocateImage(unsigned int s, unsigned int t, unsigned int r,
    GLenum format, GLenum type, unsigned int packing) {
    unsigned int previousTotalSize = 0;

    if (data_) {
      previousTotalSize = computeRowWidthInBytes(s_, pixel_format_, data_type_, packing_) * t_ * r_;
    }

    unsigned int newTotalSize = computeRowWidthInBytes(s, format, type, packing) * t * r;

    if (newTotalSize != previousTotalSize) {
      if (newTotalSize) {
        data(new unsigned char[newTotalSize], AllocationMode::USE_NEW_DELETE);
      } else {
        deallocateData(); // and sets it to nullptr.
      }
    }

    if (data_) {
      s_ = s;
      t_ = t;
      r_ = r;
      pixel_format_ = format;
      data_type_ = type;
      packing_ = packing;

      // preserve internalTextureFormat if already set, otherwise
      // use the pixelFormat as the source for the format.
      if (internal_texture_format_ == 0) {
        internal_texture_format_ = format;
      }
    } else {
      // failed to allocate memory, for now, will simply set values to 0.
      s_ = 0;
      t_ = 0;
      r_ = 0;
      pixel_format_ = 0;
      data_type_ = 0;
      packing_ = 0;

      // commenting out reset of internal_texture_format_ as we are changing
      // policy so that allocateImage honours previous settings of internal_texture_format_.
      //internal_texture_format_ = 0;
    }
  }

  void Image::image(unsigned int s, unsigned int t, unsigned int r,
    GLint internalTextureFormat, GLenum pixel_format, GLenum type,
    unsigned char *data_ptr, AllocationMode mode, unsigned int packing) {
    s_ = s;
    t_ = t;
    r_ = r;

    internal_texture_format_ = internalTextureFormat;
    pixel_format_ = pixel_format;
    data_type_ = type;

    data(data_ptr, mode);

    packing_ = packing;
  }

  void Image::image(unsigned int s, unsigned int t, unsigned int r,
    int components, GLenum type,
    unsigned char *data, AllocationMode mode, unsigned int packing) {
    int pixel_format;
    switch (components) {
    case 1:
      pixel_format = GL_LUMINANCE;
      break;
    case 2:
      pixel_format = GL_LUMINANCE_ALPHA;
      break;
    case 3:
      pixel_format = GL_RGB;
      break;
    case 4:
      pixel_format = GL_RGBA;
      break;
    default:
      //todo:: warning
      break;
    }

    int internal_texture_format = pixel_format;
    image(s, t, r, internal_texture_format, pixel_format, type, data, mode, packing);
  }

  void Image::readPixels(unsigned int x, unsigned int y, unsigned int width,
    unsigned int height, GLenum format, GLenum type, unsigned int packing) {
    allocateImage(width, height, 1, format, type, packing);

    glPixelStorei(GL_PACK_ALIGNMENT, packing_);

    glReadPixels(x, y, width, height, format, type, data_);
  }

  bool Image::isImageTranslucent() const {
    unsigned int offset = 0;
    unsigned int delta = 1;
    switch (pixel_format_) {
      case (GL_ALPHA):
        offset = 0;
        delta = 1;
        break;
      case (GL_LUMINANCE_ALPHA):
        offset = 1;
        delta = 2;
        break;
      case (GL_RGBA):
        offset = 3;
        delta = 4;
        break;
      case (GL_RGB):
        return false;
      default:
        return false;
    }

    for (unsigned int ir = 0; ir < r(); ++ir) {
      for (unsigned int it = 0; it < t(); ++it) {
        const unsigned char *d = data(0, it, ir);
        switch (data_type_) {
          case (GL_BYTE):
            if (findLowerAlphaValueInRow(s(), (char *) d + offset, (char) 127, delta)) {
              return true;
            }
            break;
          case (GL_UNSIGNED_BYTE):
            if (findLowerAlphaValueInRow(s(), (unsigned char *) d + offset, (unsigned char) 255, delta)) {
              return true;
            }
            break;
          case (GL_SHORT):
            if (findLowerAlphaValueInRow(s(), (short *) d + offset, (short) 32767, delta)) {
              return true;
            }
            break;
          case (GL_UNSIGNED_SHORT):
            if (findLowerAlphaValueInRow(s(), (unsigned short *) d + offset, (unsigned short) 65535, delta)) {
              return true;
            }
            break;
          case (GL_INT):
            if (findLowerAlphaValueInRow(s(), (int *) d + offset, (int) 2147483647, delta)) {
              return true;
            }
            break;
          case (GL_UNSIGNED_INT):
            if (findLowerAlphaValueInRow(s(), (unsigned int *) d + offset, 4294967295u, delta)) {
              return true;
            }
            break;
          case (GL_FLOAT):
            if (findLowerAlphaValueInRow(s(), (float *) d + offset, 1.0f, delta)) {
              return true;
            }
            break;
          case (GL_UNSIGNED_SHORT_5_5_5_1):
            if (maskedFindLowerAlphaValueInRow(s(), (unsigned short *) d, (unsigned short) 0x0001, (unsigned short) 0x0001, 1)) {
              return true;
            }
            break;
          case (GL_UNSIGNED_SHORT_4_4_4_4):
            if (maskedFindLowerAlphaValueInRow(s(), (unsigned short *) d, (unsigned short) 0x000f, (unsigned short) 0x000f, 1)) {
              return true;
            }
            break;
        }
      }
    }

    return false;
  }

  glm::vec4 Image::color(unsigned int s, unsigned t, unsigned r) const {
    const unsigned char *ptr = data(s, t, r);

    switch (data_type_) {
      case (GL_BYTE):
        return readColor(pixel_format_, (char *) ptr, 1.0f / 128.0f);
      case (GL_UNSIGNED_BYTE):
        return readColor(pixel_format_, (unsigned char *) ptr, 1.0f / 255.0f);
      case (GL_SHORT):
        return readColor(pixel_format_, (short *) ptr, 1.0f / 32768.0f);
      case (GL_UNSIGNED_SHORT):
        return readColor(pixel_format_, (unsigned short *) ptr, 1.0f / 65535.0f);
      case (GL_INT):
        return readColor(pixel_format_, (int *) ptr, 1.0f / 2147483648.0f);
      case (GL_UNSIGNED_INT):
        return readColor(pixel_format_, (unsigned int *) ptr, 1.0f / 4294967295.0f);
      case (GL_FLOAT):
        return readColor(pixel_format_, (float *) ptr, 1.0f);
    }
    return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  }

  glm::vec4 Image::color(const glm::vec3& texcoord) const {
    glm::vec3 coord = texcoord;
    if (coord.x < 0.f) coord.x = 0.f;
    if (coord.y < 0.f) coord.y = 0.f;
    if (coord.z < 0.f) coord.z = 0.f;

    unsigned int s = (unsigned int)(coord.x * float(s_ - 1)) % s_;
    unsigned int t = (unsigned int)(coord.y * float(t_ - 1)) % t_;
    unsigned int r = (unsigned int)(coord.z * float(r_ - 1)) % r_;
    return color(s, t, r);
  }
}