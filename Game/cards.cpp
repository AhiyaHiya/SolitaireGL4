#include "cards.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>
#include <fstream>
#include <ranges>

auto compile_card_shader(std::string_view shader_relative_path, GLenum shader_type)
    -> std::expected<GLuint, error_message_t> // shader id
{
    auto current_working_path = std::filesystem::current_path();
    auto shader_path          = current_working_path / shader_relative_path;

    auto content_result = read_file_content(shader_path);
    if (!content_result.has_value())
    {
        return std::unexpected("Failed to read shader file: " + std::string(shader_relative_path));
    }
    auto shader_source = content_result.value();

    auto           shader_id      = glCreateShader(shader_type);
    constexpr auto shader_count   = 1;
    const GLchar*  shader_src_ptr = shader_source.c_str();
    glShaderSource(shader_id, shader_count, &shader_src_ptr, nullptr);
    glCompileShader(shader_id);

    return shader_id;
}

auto create_card_renderer() -> std::expected<std::shared_ptr<card_renderer>, error_message_t>
{
    // Compile shaders
    auto vertex_compiled_result = compile_card_shader("Shaders/card.vert", GL_VERTEX_SHADER);
    if (!vertex_compiled_result)
    {
        return std::unexpected("Failed to compile vertex shader: " +
                               vertex_compiled_result.error());
    }
    auto vertex_shader_id = vertex_compiled_result.value();

    auto fragment_compiled_result = compile_card_shader("Shaders/card.frag", GL_FRAGMENT_SHADER);
    if (!fragment_compiled_result)
    {
        return std::unexpected("Failed to compile fragment shader: " +
                               fragment_compiled_result.error());
    }
    auto fragment_shader_id = fragment_compiled_result.value();

    // Link shader program
    auto program_id_result = link_shader_program(vertex_shader_id, fragment_shader_id);
    if (!program_id_result)
    {
        return std::unexpected("Failed to link shader program: " + program_id_result.error());
    }
    auto program_id = program_id_result.value();

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
    glUseProgram(program_id);

    auto [vao_id, vbo_id] = create_vao_vbo();

    // Load up the PNG
    auto load_card_textures_result = load_card_textures();
    if (!load_card_textures_result.has_value())
    {
        return std::unexpected("Failed to load card textures: " +
                               load_card_textures_result.error());
    }

    auto card_renderer_ptr            = std::make_shared<card_renderer>();
    card_renderer_ptr->shader_program = program_id;
    card_renderer_ptr->texture_array  = load_card_textures_result.value();
    card_renderer_ptr->vao            = vao_id;
    card_renderer_ptr->vbo            = vbo_id;
    card_renderer_ptr->uProjection    = glGetUniformLocation(program_id, "uProjection");
    card_renderer_ptr->uPosition      = glGetUniformLocation(program_id, "uPosition");
    card_renderer_ptr->uSize          = glGetUniformLocation(program_id, "uSize");
    card_renderer_ptr->uCardIndex     = glGetUniformLocation(program_id, "uCardIndex");
    card_renderer_ptr->uFaceUp        = glGetUniformLocation(program_id, "uFaceUp");

    return card_renderer_ptr;
}

// VAO, VBO, Vertex Array Object, Vertex Buffer Object
[[nodiscard]]
auto create_vao_vbo() -> std::pair<GLuint, GLuint>
{
    // For both VAO and VBO, there is only 1 item being created
    constexpr auto item_count = GLsizei(1);

    auto vao_id = GLuint();
    glGenVertexArrays(item_count, &vao_id);
    glBindVertexArray(vao_id);

    auto vbo_id = GLuint();
    glGenBuffers(item_count, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

    auto vertices = std::array<GLfloat, 30>{
        // pos x   y    z    u    v
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // TL
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // TR
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // BR

        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // TL (repeat)
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // BR (repeat)
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f  // BL
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    constexpr auto position_attribute_index = GLuint(0);
    constexpr auto position_axis_count      = 3;
    constexpr auto column_count             = 5;
    glVertexAttribPointer(position_attribute_index,
                          GLint(position_axis_count),              // position_size,
                          GLenum(GL_FLOAT),                        // position_type,
                          GLboolean(GL_FALSE),                     // position_normalized,
                          GLsizei(column_count * sizeof(GLfloat)), // position_stride,
                          nullptr);                                // position_offset
    glEnableVertexAttribArray(position_attribute_index);

    // TexCoord attribute
    constexpr auto texcoord_attribute_index = GLuint(1);
    constexpr auto texcoord_axis_count      = 2;
    const auto     texcoord_offset          = GLintptr(position_axis_count * sizeof(GLfloat));
    glVertexAttribPointer(texcoord_attribute_index,
                          GLint(texcoord_axis_count),              // texcoord_size,
                          GLenum(GL_FLOAT),                        // texcoord_type,
                          GLboolean(GL_FALSE),                     // texcoord_normalized,
                          GLsizei(column_count * sizeof(GLfloat)), // texcoord_stride,
                          reinterpret_cast<GLvoid*>(texcoord_offset));
    glEnableVertexAttribArray(texcoord_attribute_index);

    glBindVertexArray(0);

    return {vao_id, vbo_id};
}

void draw_cards(const std::shared_ptr<card_renderer>& cr)
{
    static const auto demo_cards = std::vector<card>{
        {200.0f, 700.0f, 0, true},  // Face up, near bottom-left
        {400.0f, 700.0f, 0, false}, // Face down (shows back)
        {600.0f, 700.0f, 13, true}  // e.g., Ace of Diamonds (index 13, adjust per JSON)
    };

    constexpr float card_width_px  = 120.0f;
    constexpr float card_height_px = 168.0f;

    // Bind shared resources (do this once per frame, outside per-card loop ideally)
    glBindVertexArray(cr->vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, cr->texture_array);
    glUniform2f(cr->uSize, card_width_px, card_height_px);

    // Draw each card
    for (const auto& card : demo_cards)
    {
        glUniform2f(cr->uPosition, card.x, card.y);
        glUniform1i(cr->uCardIndex, card.index);
        glUniform1i(cr->uFaceUp, card.face_up ? 1 : 0);

        glDrawArrays(GL_TRIANGLES, 0, 6); // Draw the quad (2 triangles, 6 verts)
    }

    // Unbind (optional, good practice)
    glBindVertexArray(0);
}

auto link_shader_program(GLuint vertex_shader_id, GLuint fragment_shader_id)
    -> std::expected<GLuint, error_message_t>
{
    auto program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    auto program_linked = GLint();
    glGetProgramiv(program_id, GL_LINK_STATUS, &program_linked);
    if (program_linked != GL_TRUE)
    {
        auto       log_length  = GLsizei(0);
        const auto size        = 1024;
        auto       err_message = std::string(size, '\0');
        glGetProgramInfoLog(program_id, size, &log_length, err_message.data());
        return std::unexpected(err_message);
    }

    return program_id;
}

auto load_card_textures() -> std::expected<GLuint, error_message_t>
{
    // --------------------------- Load the card texture atlas ---------------------------
    // Actual width, height, and channels will be set by stbi_load
    auto asset_image_result = load_png_data();
    if (!asset_image_result.has_value())
    {
        return std::unexpected("Failed to load card textures: " + asset_image_result.error());
    }
    auto asset_image = asset_image_result.value();

    // --------------------------- Load JSON data ---------------------------
    // Load up JSON data and try to create textures
    auto json_data_result = load_json_data();
    if (!json_data_result.has_value())
    {
        return std::unexpected("Failed to load JSON data: " + json_data_result.error());
    }
    auto json_data = json_data_result.value();

    // --------------------------- Create OpenGL texture array ---------------------------
    auto           card_texture_array = GLuint();
    constexpr auto texture_count      = GLsizei(1);
    glGenTextures(texture_count, &card_texture_array);
    glBindTexture(GL_TEXTURE_2D_ARRAY, card_texture_array);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Go through each card entry in the JSON and extract the texture coordinates

    const auto  frames      = json_data["frames"];
    const auto  num_layers  = static_cast<GLsizei>(frames.size());
    auto        frames_itr  = frames.begin();
    const auto& first_frame = frames_itr.value();
    const auto  width       = first_frame["w"].get<int>();
    const auto  height      = first_frame["h"].get<int>();
    glTexImage3D(GL_TEXTURE_2D_ARRAY, // target
                 0,                   // level
                 GL_RGBA8,            // internal format
                 width,
                 height,
                 num_layers, // depth (number of layers)
                 0,          // border
                 GL_RGBA,
                 GL_UNSIGNED_BYTE, // type
                 nullptr);         // data

    for (const auto& [index, entry] : std::views::enumerate(frames.items()))
    {
        const auto& [card_name, frame_data] = entry;

        const auto x = frame_data["x"].get<int>();
        const auto y = frame_data["y"].get<int>();

        // Load texture
        auto pixel_data = asset_image->stride_of_data_at(x, y);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, // target
                        0,                   // level
                        0,                   // xoffset
                        0,                   // yoffset
                        index,               // layer
                        width,
                        height,
                        1, // depth
                        GL_RGBA,
                        GL_UNSIGNED_BYTE, // type
                        pixel_data);
    }

    return card_texture_array;
}

auto load_json_data() -> std::expected<nlohmann::json, error_message_t>
{
    // Load JSON data from file
    auto current_working_path = std::filesystem::current_path();
    auto cards_path           = current_working_path / "Assets/cards.json";

    auto json_file_stream = std::ifstream(cards_path);
    try
    {
        return nlohmann::json::parse(json_file_stream); // This could throw
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::string("Failed to parse JSON data: ") + e.what());
    }
}

auto load_png_data() -> std::expected<std::shared_ptr<asset_image>, error_message_t>
{
    // Set up the asset path
    auto current_working_path = std::filesystem::current_path();
    auto cards_path           = current_working_path / "Assets/cards.png";
    if (!std::filesystem::exists(cards_path))
    {
        return std::unexpected("Cards PNG file not found at path: " + cards_path.string());
    }

    // Load the png data; width, height, and channels will be set by stbi_load
    auto  width = 0, height = 0, channels = 0;
    auto* raw_data = stbi_load(cards_path.string().c_str(), &width, &height, &channels, 0);
    if (raw_data == nullptr || width == 0 || height == 0 || channels == 0)
    {
        return std::unexpected("Failed to load PNG data from file: " + cards_path.string());
    }

    // Wrap the data and the image info in the asset_image object
    auto data_ptr = std::shared_ptr<stbi_uc>(raw_data, stbi_image_free);
    return std::make_shared<asset_image>(data_ptr, width, height, channels);
}

// TODO: Clean up implementation
auto read_file_content(const std::filesystem::path& path)
    -> std::expected<std::string, error_message_t>
{
    // Make sure the file exists
    if (!std::filesystem::exists(path))
    {
        return std::unexpected("File does not exist: " + path.string());
    }

    // Attempt to open the file
    auto file_stream = std::ifstream(path, std::ios::in);
    if (!file_stream.is_open())
    {
        return std::unexpected("Failed to open file: " + path.string());
    }
    // Seek to the end to get the size
    file_stream.seekg(0, std::ios::end);

    // Set up a new string object to hold the content
    const auto size    = file_stream.tellg();
    auto       content = std::string(size, '\0');

    // Bring the reader back to the beginning of the file and read the content
    file_stream.seekg(0);
    file_stream.read(content.data(), size);
    return content;
}
