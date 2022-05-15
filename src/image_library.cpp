#include <image_library.h>
#include <reader_writer.h>
#include <iostream>
#include <config.h>
#include <glad/glad.h>

namespace Dental {
  ImageLibrary::ImageLibrary() {
    if (!add("texture", IMAGES_DIR + "texture.jpg")) {
      ImagePtr image = std::make_shared<Image>();
      image->allocateImage(64, 64, 1, GL_RGB, GL_UNSIGNED_BYTE, 1);
      add("texture", image);
    }
  }

  ImageLibrary::~ImageLibrary() {

  }

  void ImageLibrary::add(const std::string& name, ImagePtr image) {
    images_.insert(std::make_pair(name, image));
  }

  ImagePtr ImageLibrary::add(const std::string& name, const std::string& file_name) {
    auto image_result = ReaderWriter::readImage(file_name);
    auto image = std::get<0>(image_result);
    if (!image) {
      std::cout << "image read error," << std::get<2>(image_result) << std::endl;
      return nullptr;
    }
    add(name, image);
    return image;
  }

  bool ImageLibrary::remove(const std::string& name) {
    auto itr = images_.find(name);
    if (itr == images_.end()) {
      return false;
    }
    images_.erase(name);
    return true;
  }

  ImagePtr ImageLibrary::get(const std::string& name) {
    auto itr = images_.find(name);
    if (itr == images_.end()) {
      return nullptr;
    }
    return itr->second;
  }

  ImagePtr ImageLibrary::getOrAdd(const std::string& name, const std::string& file_name) {
    auto image = get(name);
    if (!image) {
      image = add(name, file_name);
    }
    return image;
  }

  bool ImageLibrary::replaceName(const std::string& old_name, const std::string& new_name) {
    auto image = get(old_name);
    auto itr = images_.find(new_name);
    if (image && itr == images_.end()) {
      add(new_name, image);
      if (remove(old_name)) {
        return true;
      }
    }
    return false;
  }

  bool ImageLibrary::replaceImage(const std::string& name, ImagePtr& image) {
    auto itr = images_.find(name);
    if (itr == images_.end()) {
      return false;
    }
    itr->second = image;
    return true;
  }

  ImagePtr ImageLibrary::replaceImage(const std::string& name, const std::string& file_name) {
    auto itr = images_.find(name);
    if (itr != images_.end()) {
      auto image_result = ReaderWriter::readImage(file_name);
      auto image = std::get<0>(image_result);
      if (image) {
        itr->second = image;
        return image;
      } else {
        std::cout << "image read error," << std::get<2>(image_result) << std::endl;
      }
    }
    return nullptr;
  }

  void ImageLibrary::clean() {
    for (auto& itr : images_) {
      auto& image = itr.second;
      if (image.use_count() == 1) {
        image->release();
      }
    }
  }

  ImageLibrary& ImageLibrary::instance() {
    static ImageLibrary font_library;
    return font_library;
  }
}