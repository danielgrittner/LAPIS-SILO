#include "silo/common/input_stream_wrapper.h"

#include <utility>

#include <spdlog/spdlog.h>
#include <boost/iostreams/detail/error.hpp>
#include <boost/iostreams/filter/lzma.hpp>
#include <boost/iostreams/filter/zstd.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/read.hpp>

#include "silo/preprocessing/preprocessing_exception.h"

namespace {

std::filesystem::path appendXZ(const std::filesystem::path& filename) {
   return {filename.string() + ".xz"};
}

std::filesystem::path appendZST(const std::filesystem::path& filename) {
   return {filename.string() + ".zst"};
}

}  // namespace

namespace silo {
InputStreamWrapper::InputStreamWrapper(const std::filesystem::path& filename)
    : input_stream(std::make_unique<boost::iostreams::filtering_istream>()) {
   if (std::filesystem::exists(filename)) {
      SPDLOG_INFO("Detected file without specialized ending, processing raw: " + filename.string());
      file = std::ifstream(filename, std::ios::binary);
   } else if (std::filesystem::exists(appendXZ(filename))) {
      SPDLOG_INFO("Detected file-ending .xz for input file " + filename.string());
      file = std::ifstream(appendXZ(filename), std::ios::binary);
      input_stream->push(boost::iostreams::lzma_decompressor());
   } else if (std::filesystem::exists(appendZST(filename))) {
      SPDLOG_INFO("Detected file-ending .zst for input file " + filename.string());
      file = std::ifstream(appendZST(filename), std::ios::binary);
      input_stream->push(boost::iostreams::zstd_decompressor());
   } else {
      throw silo::PreprocessingException(
         "Cannot find file with name or associated endings (.xz, .zst): " + filename.string()
      );
   }
   input_stream->push(file);
}

std::istream& silo::InputStreamWrapper::getInputStream() const {
   return *input_stream;
}

}  // namespace silo
