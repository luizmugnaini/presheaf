#include <psh/arena.h>

namespace psh {
    // -----------------------------------------------------------------------------
    // - Arena implementation -
    // -----------------------------------------------------------------------------

    void Arena::clear() noexcept {
        offset = 0;
    }

    ScratchArena Arena::make_scratch() noexcept {
        return ScratchArena{this};
    }

    // -----------------------------------------------------------------------------
    // - Scratch arena implementation -
    // -----------------------------------------------------------------------------

    ScratchArena::ScratchArena(Arena* parent) noexcept
        : arena{parent}, saved_offset{parent->offset} {}

    ScratchArena::~ScratchArena() noexcept {
        arena->offset = saved_offset;
    }

    ScratchArena ScratchArena::decouple() const noexcept {
        return ScratchArena{arena};
    }
}  // namespace psh
