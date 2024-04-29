#include <psh/file_system.h>

namespace psh {
    String read_file(Arena* arena, StrPtr path) noexcept {
        if (path == nullptr) {
            return String{};
        }

        std::FILE* const file = std::fopen(path, "rb");
        if (file == nullptr) {
            std::perror("Couldn't open file.\n");
            return String{};
        }
        if (arena == nullptr) {
            log(LogLevel::Error, "read_file called with a null arena.");
            return String{};
        }

        // Find the file size.
        if (std::fseek(file, 0, SEEK_END) == -1) {
            std::perror("Couldn't seek end of file.");
            return String{};
        }
        isize const file_size = std::ftell(file);
        if (file_size == -1) {
            std::perror("Couldn't tell the size of the file.\n");
            return String{};
        }
        if (std::fseek(file, 0, SEEK_SET) == -1) {
            std::perror("Couldn't seek start of file.\n");
            return String{};
        }

        // Acquire memory for the buffer.
        auto const  buf_size = static_cast<usize>(file_size);
        char* const buf      = arena->alloc<char>(buf_size + 1);
        psh_assert_msg(
            buf != nullptr,
            "read_file unable to acquire enough memory to store the file content into a "
            "buffer.");

        // Read the whole file into the buffer.
        usize const read_count = std::fread(buf, 1, buf_size, file);
        if (std::ferror(file) != 0) {
            std::perror("Couldn't read file.\n");
        } else {
            buf[read_count] = 0;  // TODO: We don't even need a zero terminated string here since we
                                  //       are returning a String. Should we still do it anyway?
        }

        psh_discard(std::fclose(file));

        return String{arena, buf_size, buf};
    }
}  // namespace psh
