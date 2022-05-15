#ifndef __DENTAL_IMAGE_H__
#define __DENTAL_IMAGE_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <glm/ext.hpp>

namespace Dental {
  class Image {
  public:
    enum class AllocationMode {
      NO_DELETE,
      USE_NEW_DELETE,
      USE_MALLOC_FREE
    };

    Image();
    ~Image();

    Image& operator = (const Image& rhs);
    Image& operator = (Image&& rhs) noexcept;
    Image(const Image& rhs);
    Image(Image&& rhs) noexcept;

    void fileName(const std::string& fileName);
    inline const std::string& fileName() const { return file_name_; }

    void data(unsigned char* data, AllocationMode mode);

    void internalTextureFormat(int internalFormat);
    inline int internalTextureFormat() const {
      return internal_texture_format_;
    }

    void pixelFormat(unsigned int pixelFormat);
    inline unsigned int pixelFormat() const { return pixel_format_; }

    void dataType(unsigned int dataType);
    inline unsigned int dataType() const { return data_type_; }

    inline void packing(unsigned int packing) { packing_ = packing; }
    inline unsigned int packing() const { return packing_; }

    inline void verticallyFliped(bool vertically_fliped) {
      vertically_fliped_ = vertically_fliped;
    }
    inline bool verticallyFliped() const { return vertically_fliped_; }

    void image(unsigned int s, unsigned int t, unsigned int r,
      int internal_texture_format, unsigned int pixel_format, unsigned int type,
      unsigned char *data, AllocationMode mode, unsigned int packing = 1);

    void image(unsigned int s, unsigned int t, unsigned int r,
      int components, unsigned int type,
      unsigned char *data, AllocationMode mode, unsigned int packing = 1);

    /** Width of image. */
    inline unsigned int s() const { return s_; }

    /** Height of image. */
    inline unsigned int t() const { return t_; }

    /** Depth of image. */
    inline unsigned int r() const { return r_; }

    unsigned int textureId();

    void release();

    void readPixels(unsigned int x, unsigned int y,
      unsigned int width, unsigned int height,
      unsigned int format, unsigned int type, unsigned int packing = 1);

    bool isImageTranslucent() const;

    /** Return true if the Image represent a valid and usable imagery.*/
    bool valid() const {
      return s_ != 0 && t_ != 0 && r_ != 0 && data_ != 0 && data_type_ != 0;
    }

    /** Return the number of bits required for each pixel. */
    inline unsigned int pixelSizeInBits() const {
      return computePixelSizeInBits(pixel_format_, data_type_);
    }

    inline unsigned char* data() { return data_; }
    inline const unsigned char* data() const { return data_; }

    inline unsigned int rowSizeInBytes() const {
      return computeRowWidthInBytes(s_, pixel_format_, data_type_, packing_);
    }

    inline unsigned int imageSizeInBytes() const { return rowSizeInBytes() * t_; }

    inline unsigned int rowStepInBytes() const {
      return computeRowWidthInBytes(s_, pixel_format_, data_type_, packing_);
    }

    inline unsigned char *
    data(unsigned int column, unsigned int row = 0, unsigned int image = 0) {
      if (!data_) {
        return nullptr;
      };

      return data_ + (column * pixelSizeInBits()) / 8 + row * rowStepInBytes() +
             image * imageSizeInBytes();
    }

    inline const unsigned char *
    data(unsigned int column, unsigned int row = 0, unsigned int image = 0) const {
      if (!data_) {
        return nullptr;
      }
      return data_ + (column * pixelSizeInBits()) / 8 + row * rowStepInBytes() +
             image * imageSizeInBytes();
    }

    glm::vec4 color(unsigned int s, unsigned t, unsigned r) const;

    glm::vec4 color(const glm::vec3 &texcoord) const;

    static unsigned int computeNumComponents(unsigned int pixelFormat);

    static unsigned int computePixelSizeInBits(unsigned int format, unsigned int type);

    void allocateImage(unsigned int s, unsigned int t, unsigned int r, unsigned int format,
      unsigned int type, unsigned int packing);

    void deallocateData();

private:
    bool isPackedType(unsigned int type);

    unsigned int computePixelFormat(unsigned int format);

    unsigned int computeFormatDataType(unsigned int pixel_format);

    unsigned int
    computeRowWidthInBytes(
      unsigned int width, unsigned int pixelFormat, unsigned int type, unsigned int packing) const;

    unsigned int
    computeImageSizeInBytes(unsigned int width, unsigned int height, unsigned int depth,
      unsigned int pixel_format, unsigned int type,
      unsigned int packing, unsigned int slice_packing, unsigned int image_packing);

    int computeNearestPowerOfTwo(int s, float bias);

private:
    unsigned char* data_;
    std::string file_name_;
    unsigned int s_;
    unsigned int t_;
    unsigned int r_;
    bool vertically_fliped_;

    int internal_texture_format_;
    unsigned int pixel_format_;
    unsigned int data_type_;
    unsigned int packing_;
    AllocationMode allocation_mode_;
    unsigned int texture_id_;
  };

  using ImagePtr = std::shared_ptr<Image>;
}
#endif