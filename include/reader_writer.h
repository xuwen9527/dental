#ifndef __READERWRITER_H__
#define __READERWRITER_H__

#include <string>
#include <tuple>
#include <unordered_map>
#include "geometry.h"

namespace Dental::ReaderWriter {
  enum class Status {
    FILE_LOADED,
    FILE_SAVED,
    FILE_NOT_FIND,
    FILE_NOT_HANDLED,
    ERROR_IN_WRITING_FILE,
    ERROR_IN_READING_FILE
  };

	using Options = std::unordered_map<std::string, std::string>;
  class WriteOptions : public Options {
  public:
    //是否保存二进制
    void binary(bool flag);

    //stl文件是否是彩色模式保存
    void colorMode(bool flag);

    void option(std::string parma, std::string value);

    std::string option(std::string parma) const;
  };

  std::tuple<ImagePtr, Status, std::string> readImage(unsigned char* data, std::size_t size);

  std::tuple<ImagePtr, Status, std::string> readImage(const std::string& file_name);
  
  std::tuple<Status, std::string> writeImage(const std::string &file_name, const Image &image);

  std::tuple<GeometryPtr, Status, std::string> read(const std::string &file_name);

  std::tuple<Status, std::string> write(const std::string &file_name, const Geometry& geometry, const WriteOptions& options);

  bool acceptsExtension(const std::string &extension);
}

#endif
