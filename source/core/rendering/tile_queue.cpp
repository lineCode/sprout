#include "tile_queue.hpp"
#include "base/math/vector.inl"

namespace rendering {

Tile_queue::Tile_queue(math::uint2 resolution, math::uint2 tile_dimensions) :
	tiles_(static_cast<size_t>(std::ceil(static_cast<float>(resolution.x) / static_cast<float>(tile_dimensions.x)))
		 * static_cast<size_t>(std::ceil(static_cast<float>(resolution.y) / static_cast<float>(tile_dimensions.y)))),
	current_produce_(0), current_consume_(0) {
	math::uint2 current_pixel(0, 0);
	for (;;) {
		push(Rectui{current_pixel, math::min(current_pixel + tile_dimensions, resolution)});

		current_pixel.x += tile_dimensions.x;

		if (current_pixel.x >= resolution.x) {
			current_pixel.x = 0;
			current_pixel.y += tile_dimensions.y;
		}

		if (current_pixel.y >= resolution.y) {
			break;
		}
	}
}

size_t Tile_queue::size() const {
	return tiles_.size();
}

void Tile_queue::restart() {
	current_produce_ = tiles_.size();
	current_consume_ = 0;
}

bool Tile_queue::pop(Rectui& tile) {
	std::lock_guard<std::mutex> lock(mutex_);

	if (current_consume_ < tiles_.size()) {
		tile = tiles_[current_consume_++];
		return true;
	}

	return false;
}

void Tile_queue::push(const Rectui& tile) {
	tiles_[current_produce_++] = tile;
}

}
