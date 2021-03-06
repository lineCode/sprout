#include "raw_reader.hpp"
#include <istream>
#include <string>
#include "base/math/vector4.inl"
#include "image/typed_image.inl"

namespace image::encoding::raw {

std::shared_ptr<Image> Reader::read(std::istream& stream) const {
    int3 dimensions(512, 512, 32);

    Image::Description description(Image::Type::Byte1, dimensions);

    auto volume = std::make_shared<Byte1>(description);

    int32_t const num_bytes = dimensions[0] * dimensions[1] * dimensions[2];
    stream.read(reinterpret_cast<char*>(volume->data()), num_bytes);

    return volume;

    //            const int3 dimensions(2, 2, 2);

    //            Image::Description description(Image::Type::Float1, dimensions);

    //            auto volume = std::make_shared<Float1>(description);

    //        volume->at(0, 0, 0) = 0.1f; volume->at(1, 0, 0) = 0.1f;
    //        volume->at(0, 1, 0) = 0.9f; volume->at(1, 1, 0) = 0.9f;

    //        volume->at(0, 0, 1) = 0.1f; volume->at(1, 0, 1) = 0.1f;
    //        volume->at(0, 1, 1) = 0.9f; volume->at(1, 1, 1) = 0.9f;

    //            volume->at(0, 0, 0) = 0.1f;   volume->at(1, 0, 0) = 0.2f;  volume->at(2, 0, 0) =
    //            0.3f; volume->at(0, 1, 0) = 0.25f;  volume->at(1, 1, 0) = 0.f;   volume->at(2, 1,
    //            0) = 0.75f; volume->at(0, 2, 0) = 0.25f;  volume->at(1, 2, 0) = 0.5f;
    //            volume->at(2, 2, 0) = 0.75f;

    //            volume->at(0, 0, 1) = 0.4f;  volume->at(1, 0, 1) = 0.5f;   volume->at(2, 0, 1) =
    //            0.6f; volume->at(0, 1, 1) = 0.5f;  volume->at(1, 1, 1) = 0.25f;  volume->at(2, 1,
    //            1) = 0.f; volume->at(0, 2, 1) = 0.f;   volume->at(1, 2, 1) = 0.25f;  volume->at(2,
    //            2, 1) = 0.5f;

    //            volume->at(0, 0, 2) = 0.7f;   volume->at(1, 0, 2) = 0.8f;  volume->at(2, 0, 2) =
    //            0.9f; volume->at(0, 1, 2) = 0.75f;  volume->at(1, 1, 2) = 0.5f;  volume->at(2, 1,
    //            2) = 0.25f; volume->at(0, 2, 2) = 0.75f;  volume->at(1, 2, 2) = 0.f; volume->at(2,
    //            2, 2) = 0.25f;

    // return volume;
}

}  // namespace image::encoding::raw
