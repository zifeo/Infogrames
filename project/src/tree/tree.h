#pragma once

#include "icg_helper.h"
#include <glm/gtc/type_ptr.hpp>

class Tree {

  private:
    GLuint vertex_array_id_;               // vertex array object
    GLuint vertex_buffer_object_position_; // memory buffer for positions
    GLuint vertex_buffer_object_index_;    // memory buffer for indices
    GLuint program_id_;                    // GLSL shader program ID
    GLuint perlin_texture_id_;               // perlin texture ID
    GLuint tree_texture_id_;               // water texture ID
    GLuint num_indices_;                   // number of vertices to render
    GLuint MVP_id_;                        // model, view, proj matrix ID
    GLuint x_chunk_id_;                           //x value of the chunk
    GLuint y_chunk_id_;                           //y value of the chunk

  public:
    void Init() {
        // compile the shaders.
        program_id_ = icg_helper::LoadShaders("tree_vshader.glsl", "tree_fshader.glsl");
        if (!program_id_) {
            exit(EXIT_FAILURE);
        }

        glUseProgram(program_id_);

        // vertex one vertex array
        glGenVertexArrays(1, &vertex_array_id_);
        glBindVertexArray(vertex_array_id_);

        // vertex coordinates and indices
        {
            std::vector<GLfloat> vertices;
            std::vector<GLuint> indices;

            float scaleFactor = 16;

            vertices.push_back(-1/scaleFactor);
            vertices.push_back(-1/scaleFactor);

            vertices.push_back(-1/scaleFactor);
            vertices.push_back(1/scaleFactor);

            vertices.push_back(1/scaleFactor);
            vertices.push_back(-1/scaleFactor);

            vertices.push_back(1/scaleFactor);
            vertices.push_back(1/scaleFactor);

            indices.push_back(0);
            indices.push_back(1);
            indices.push_back(2);
            indices.push_back(3);

            num_indices_ = indices.size();

            // position buffer
            glGenBuffers(1, &vertex_buffer_object_position_);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_position_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

            // vertex indices
            glGenBuffers(1, &vertex_buffer_object_index_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_buffer_object_index_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

            // position shader attribute
            GLuint loc_position = glGetAttribLocation(program_id_, "position");
            glEnableVertexAttribArray(loc_position);
            glVertexAttribPointer(loc_position, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
        }

        {
            initTexture("tree_texture.tga", &tree_texture_id_, "tree_tex", GL_TEXTURE0 + 1);
        }

        // other uniforms
        MVP_id_ = glGetUniformLocation(program_id_, "MVP");

        x_chunk_id_ = glGetUniformLocation(program_id_, "x_chunk");
        y_chunk_id_ = glGetUniformLocation(program_id_, "y_chunk");

        // to avoid the current object being polluted
        glBindVertexArray(0);
        glUseProgram(0);
    }

    void Cleanup() {
        glBindVertexArray(0);
        glUseProgram(0);
        glDeleteBuffers(1, &vertex_buffer_object_position_);
        glDeleteBuffers(1, &vertex_buffer_object_index_);
        glDeleteVertexArrays(1, &vertex_array_id_);
        glDeleteProgram(program_id_);
        glDeleteTextures(1, &tree_texture_id_);
    }

    void Draw(float angle, float x_in_chunk, float y_in_chunk, GLint perlin_texture_id, float time, int x, int y, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        glUseProgram(program_id_);
        glBindVertexArray(vertex_array_id_);

        // bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, perlin_texture_id);

        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, tree_texture_id_);

        // setup MVP
        glm::mat4 MVP = projection * view * model;
        glUniformMatrix4fv(MVP_id_, ONE, DONT_TRANSPOSE, glm::value_ptr(MVP));

        // pass the current time stamp to the shader.
        glUniform1f(glGetUniformLocation(program_id_, "time"), time);

        glUniform1i(x_chunk_id_, x);
        glUniform1i(y_chunk_id_, y);

        glUniform1f(glGetUniformLocation(program_id_, "angle"), angle);

        glUniform1f(glGetUniformLocation(program_id_, "x_in_chunk"), x_in_chunk);
        glUniform1f(glGetUniformLocation(program_id_, "y_in_chunk"), y_in_chunk);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        glDrawElements(GL_TRIANGLE_STRIP, num_indices_, GL_UNSIGNED_INT, 0);

        glDisable(GL_BLEND);

        glBindVertexArray(0);
        glUseProgram(0);
    }

  private:
    void initTexture(string filename, GLuint *texture_id, string texture_name, int val) {

        int width;
        int height;
        int nb_component;
        // set stb_image to have the same coordinates as OpenGL
        stbi_set_flip_vertically_on_load(1);
        unsigned char *image = stbi_load(filename.c_str(), &width, &height, &nb_component, 0);

        if (image == nullptr) {
            throw(string("Failed to load texture"));
        }

        glGenTextures(1, texture_id);
        glBindTexture(GL_TEXTURE_2D, *texture_id);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        if (nb_component == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        } else if (nb_component == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        }

        GLuint tex_id = glGetUniformLocation(program_id_, texture_name.c_str());
        glUseProgram(program_id_);
        glUniform1i(tex_id, val - GL_TEXTURE0);

        // cleanup
        glBindTexture(GL_TEXTURE_2D, val);
        stbi_image_free(image);
    }
};
