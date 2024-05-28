#include <psh/arena.h>

namespace psh {
    // -----------------------------------------------------------------------------
    // - Arena implementation -
    // -----------------------------------------------------------------------------

    Arena::Arena(u8* _memory, usize _capacity) noexcept : memory{_memory}, capacity{_capacity} {
        if (psh_likely(capacity != 0)) {
            psh_assert_msg(
                memory != nullptr,
                "Arena created with inconsistent data: non-zero capacity but null memory");
        }
    }

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
