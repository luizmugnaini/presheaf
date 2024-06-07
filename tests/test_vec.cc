#include <psh/assert.h>
#include <psh/vec.h>
#include "utils.h"

using namespace psh;

void mat4_indexing() {
    Mat4 mat = Mat4::id();
    for (u32 row = 0; row < 4; ++row) {
        for (u32 column = 0; column < 4; ++column) {
            psh_debug_fmt("(%d, %d) = %f", row, column, mat.at(row, column));
            if (row != column) {
                psh_assert(float_equal(mat.at(row, column), 0.0f));
            } else {
                psh_assert(float_equal(mat.at(row, column), 1.0f));
            }
        }
    }

    test_passed();
}

int main() {
    mat4_indexing();
    return 0;
}
