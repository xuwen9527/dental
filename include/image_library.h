#ifndef __IMAGE_LIBRARY_H__
#define __IMAGE_LIBRARY_H__

#include <image.h>

namespace Dental {
  class ImageLibrary {
  public:
    using Images = std::unordered_map<std::string, ImagePtr>;

    static ImageLibrary& instance();

    void add(const std::string& name, ImagePtr);
    ImagePtr add(const std::string& name, const std::string& file_name);
    ImagePtr get(const std::string& name);
    bool remove(const std::string& name);

    bool replaceName(const std::string& old_name, const std::string& new_name);
    bool replaceImage(const std::string& name, ImagePtr& image);
    ImagePtr replaceImage(const std::string& name, const std::string& file_name);

    ImagePtr getOrAdd(const std::string& name, const std::string& file_name);

    inline const Images& images() const { return images_; }

    void clean();

    inline void clear() { images_.clear(); }

  protected:
    ImageLibrary();
    ~ImageLibrary();

    Images images_;
  };
}
#endif