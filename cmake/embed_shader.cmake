file(READ "${INPUT}" shader_source)

file(WRITE "${OUTPUT}"
        "#pragma once

namespace shader {
inline constexpr auto ${NAME} = R\"GLSL(
${shader_source}
)GLSL\";
}
")
