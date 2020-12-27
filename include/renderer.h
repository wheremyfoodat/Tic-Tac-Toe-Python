#pragma once
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "helpers.h"

using Vertex = sf::Vertex;
using Vector2D = sf::Vector2f;
using Color = sf::Color;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

constexpr const int VERTEX_BUFFER_LEN = 64 * 1024;

/*
 * PSX vertex format: each vertex is 1 32-bit number which is formatted as
 * YyyyXxxx (top 16 bits are y, low 16 are x)
*/


class BeegRenderer {
    sf::RenderWindow window;
    sf::VertexArray vertex_buffer;
    int vertex_buffer_index;

public:
    BeegRenderer (int width, int height, std::string title) :   window (sf::VideoMode(width, height), title.c_str()),
                                                                vertex_buffer (sf::Triangles, VERTEX_BUFFER_LEN) {
        window.clear (sf::Color(0xDE, 0xAD, 0xBE)); // init color to 0xDEADBEFF;
        poll_events();
        window.display();

        vertex_buffer_index = 0;
    }

    auto isOpen() -> bool {
        return window.isOpen();
    }

    void close() {
        window.close();
    }

    void set_title (std::string title) {
        window.setTitle(title.c_str());
    }

    void poll_events () {
        sf::Event event;

        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed: close(); break;
                // if you want to handle other events, add the code here
            }
        }
    }

    void push_vertex (const Vector2D& position) {
        vertex_buffer[vertex_buffer_index++] = Vertex (position);
    }

    void push_vertex (const Vector2D& position, const Color color) {
        vertex_buffer[vertex_buffer_index++] = Vertex (position, color);
    }

    void push_vertex (const Vector2D& position, const u32 color) {
        vertex_buffer[vertex_buffer_index++] = Vertex (position, Color (color));
    }

    void push_vertex (const Vector2D& position, const u8 red, const u8 green, const u8 blue) {
        vertex_buffer[vertex_buffer_index++] = Vertex (position, Color (red, green, blue));
    }

    void push_vertex (const Vector2D& position, const u8 red, const u8 green, const u8 blue, const u8 alpha) {
        vertex_buffer[vertex_buffer_index++] = Vertex (position, Color (red, green, blue, alpha));
    }

    // PSX-specific
    void push_tri (u32 vertex1, u32 vertex2, u32 vertex3, u32 colorBGR) { // monochrome tri
        const auto v1 = Vector2D((float) (vertex1 & 0xFFFF), (float)(vertex1 >> 16));
        const auto v2 = Vector2D((float) (vertex2 & 0xFFFF), (float)(vertex2 >> 16));
        const auto v3 = Vector2D((float) (vertex3 & 0xFFFF), (float)(vertex3 >> 16));

        const auto colorRGB = Helpers::BGRToRGBA(colorBGR);
        if (vertex_buffer_index + 3 > VERTEX_BUFFER_LEN)
            Helpers::panic ("Vertex buffer overflow");

        push_vertex(v1, colorRGB);
        push_vertex(v2, colorRGB);
        push_vertex(v3, colorRGB);
    }

    void push_tri (u32 vertex1, u32 color1, u32 vertex2, u32 color2, u32 vertex3, u32 color3) { // shaded tri
        const auto p1 = Vector2D((float) (vertex1 & 0xFFFF), (float)(vertex1 >> 16));
        const auto p2 = Vector2D((float) (vertex2 & 0xFFFF), (float)(vertex2 >> 16));
        const auto p3 = Vector2D((float) (vertex3 & 0xFFFF), (float)(vertex3 >> 16));

        color1 = Helpers::BGRToRGBA(color1);
        color2 = Helpers::BGRToRGBA(color2);
        color3 = Helpers::BGRToRGBA(color3);

        if (vertex_buffer_index + 3 > VERTEX_BUFFER_LEN)
            Helpers::panic ("Vertex buffer overflow");

        push_vertex(p1, color1);
        push_vertex(p2, color2);
        push_vertex(p3, color3);
    }

    // PSX-specific
    void push_quad (u32 vertex1, u32 vertex2, u32 vertex3, u32 vertex4, u32 colorBGR) { // monochrome quad
        const auto p1 = Vector2D((float) (vertex1 & 0xFFFF), (float)(vertex1 >> 16));
        const auto p2 = Vector2D((float) (vertex2 & 0xFFFF), (float)(vertex2 >> 16));
        const auto p3 = Vector2D((float) (vertex3 & 0xFFFF), (float)(vertex3 >> 16));
        const auto p4 = Vector2D((float) (vertex4 & 0xFFFF), (float)(vertex4 >> 16));

        const auto colorRGB = Helpers::BGRToRGBA(colorBGR);

        if (vertex_buffer_index + 6 > VERTEX_BUFFER_LEN)
            Helpers::panic ("Vertex buffer overflow");

        // break quad into 2 triangles, 6 vertices
        push_vertex(p1, colorRGB);
        push_vertex(p2, colorRGB);
        push_vertex(p3, colorRGB);
        push_vertex(p2, colorRGB);
        push_vertex(p3, colorRGB);
        push_vertex(p4, colorRGB);
    }

    void push_quad (u32 vertex1, u32 color1, u32 vertex2, u32 color2, u32 vertex3, u32 color3, u32 vertex4, u32 color4) { // shaded quad
        const auto p1 = Vector2D((float) (vertex1 & 0xFFFF), (float)(vertex1 >> 16));
        const auto p2 = Vector2D((float) (vertex2 & 0xFFFF), (float)(vertex2 >> 16));
        const auto p3 = Vector2D((float) (vertex3 & 0xFFFF), (float)(vertex3 >> 16));
        const auto p4 = Vector2D((float) (vertex4 & 0xFFFF), (float)(vertex4 >> 16));

        color1 = Helpers::BGRToRGBA(color1);
        color2 = Helpers::BGRToRGBA(color2);
        color3 = Helpers::BGRToRGBA(color3);
        color4 = Helpers::BGRToRGBA(color4);

        if (vertex_buffer_index + 6 > VERTEX_BUFFER_LEN)
            Helpers::panic ("Vertex buffer overflow");

        push_vertex(p1, color1);
        push_vertex(p2, color2);
        push_vertex(p3, color3);
        push_vertex(p2, color2);
        push_vertex(p3, color3);
        push_vertex(p4, color4);
    }

    void draw () {
        poll_events();

        // TODO: Why won't this work if I call the draw function on the VertexArray?
        sf::VertexArray dummyTriangle (sf::Triangles, 3);
        for (int i = 0; i < vertex_buffer_index / 3; i++) {
            dummyTriangle[0] = vertex_buffer[i * 3];
            dummyTriangle[1] = vertex_buffer[i * 3 + 1];
            dummyTriangle[2] = vertex_buffer[i * 3 + 2];

            window.draw (dummyTriangle);
        }

        window.display();

        vertex_buffer.clear();
        vertex_buffer_index = 0;
    }
};
