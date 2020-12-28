#pragma once
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "helpers.h"

using Vertex = sf::Vertex;
using Vector2D = sf::Vector2f;
using Color = sf::Color;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

/*
 * PSX vertex format: each vertex is 1 32-bit number which is formatted as
 * YyyyXxxx (top 16 bits are y, low 16 are x)
*/

class VRAM {
public:
    std::vector <u32> pixels;

    void setPixel (int x, int y, u32 color);
    u32 getPixel24bit(int x, int y);

    VRAM();
};

class BeegRenderer {
    sf::ContextSettings context_settings;
    sf::RenderWindow window;
    sf::VertexArray vertex_buffer; // TODO: Ditch this, have a pool of sf::VertexArrays
    sf::Texture texture_atlas;
    int vertex_buffer_index;

public:    
    VRAM vram = VRAM();

    BeegRenderer (int width, int height, std::string title) :   context_settings(0, 0, 0, 1, 1, sf::ContextSettings::Attribute::Default, true),
                                                                window (sf::VideoMode(width, height), title.c_str(), sf::Style::Default, context_settings),
                                                                vertex_buffer (sf::Triangles, 0) {
        window.clear(); // init color to 0xDEADBEFF;
        poll_events();
        window.display();
        texture_atlas.create(1024, 512);

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
                case sf::Event::Resized: window.display(); break;
                // if you want to handle other events, add the code here
            }
        }
    }

    template <const bool semi_transparent>
    constexpr auto BGRToRGBA (u32 value) -> u32 {
        auto r = value & 0xFF;
        auto g = (value >> 8) & 0xFF;
        auto b = (value >> 16) & 0xFF;
        u32 alpha = 255;

        if constexpr (semi_transparent)
            alpha = 128;

        return (r << 24) | (g << 16) | (b << 8) | alpha;
    }

    void push_vertex (const Vector2D& position) {
        vertex_buffer[vertex_buffer_index++] = Vertex (position);
    }

    void push_vertex (const Vector2D& position, const Color color) {
        vertex_buffer[vertex_buffer_index++] = Vertex (position, color);
        vertex_buffer_index++;
    }

    void push_vertex (const Vector2D& position, const u32 color) {
        //vertex_buffer[vertex_buffer_index++] = Vertex (position, Color (color));
        vertex_buffer.append (Vertex(position, Color(color)));
        vertex_buffer_index++;
    }

    void push_vertex (const Vector2D& position, const u8 red, const u8 green, const u8 blue) {
        vertex_buffer[vertex_buffer_index++] = Vertex (position, Color (red, green, blue));
    }

    void push_vertex (const Vector2D& position, const u8 red, const u8 green, const u8 blue, const u8 alpha) {
        vertex_buffer[vertex_buffer_index++] = Vertex (position, Color (red, green, blue, alpha));
    }

    // PSX-specific
    template <const bool semi_transparent>
    void push_tri (u32 vertex1, u32 vertex2, u32 vertex3, u32 colorBGR) { // monochrome tri
        const auto v1 = Vector2D((float) (vertex1 & 0xFFFF), (float)(vertex1 >> 16));
        const auto v2 = Vector2D((float) (vertex2 & 0xFFFF), (float)(vertex2 >> 16));
        const auto v3 = Vector2D((float) (vertex3 & 0xFFFF), (float)(vertex3 >> 16));

        const auto colorRGB = BGRToRGBA <semi_transparent> (colorBGR);

        push_vertex(v1, colorRGB);
        push_vertex(v2, colorRGB);
        push_vertex(v3, colorRGB);
    }

    template <const bool semi_transparent>
    void push_tri (u32 vertex1, u32 color1, u32 vertex2, u32 color2, u32 vertex3, u32 color3) { // shaded tri
        const auto p1 = Vector2D((float) (vertex1 & 0xFFFF), (float)(vertex1 >> 16));
        const auto p2 = Vector2D((float) (vertex2 & 0xFFFF), (float)(vertex2 >> 16));
        const auto p3 = Vector2D((float) (vertex3 & 0xFFFF), (float)(vertex3 >> 16));

        color1 = BGRToRGBA <semi_transparent> (color1);
        color2 = BGRToRGBA <semi_transparent> (color2);
        color3 = BGRToRGBA <semi_transparent> (color3);

        push_vertex(p1, color1);
        push_vertex(p2, color2);
        push_vertex(p3, color3);
    }

    // PSX-specific
    template <const bool semi_transparent>
    void push_quad (u32 vertex1, u32 vertex2, u32 vertex3, u32 vertex4, u32 colorBGR) { // monochrome quad
        const auto p1 = Vector2D((float) (vertex1 & 0xFFFF), (float)(vertex1 >> 16));
        const auto p2 = Vector2D((float) (vertex2 & 0xFFFF), (float)(vertex2 >> 16));
        const auto p3 = Vector2D((float) (vertex3 & 0xFFFF), (float)(vertex3 >> 16));
        const auto p4 = Vector2D((float) (vertex4 & 0xFFFF), (float)(vertex4 >> 16));

        auto colorRGB = BGRToRGBA <semi_transparent> (colorBGR);

        // break quad into 2 triangles, 6 vertices
        push_vertex(p1, colorRGB);
        push_vertex(p2, colorRGB);
        push_vertex(p3, colorRGB);
        push_vertex(p2, colorRGB);
        push_vertex(p3, colorRGB);
        push_vertex(p4, colorRGB);
    }

    template <bool semi_transparent>
    void push_quad (u32 vertex1, u32 color1, u32 vertex2, u32 color2, u32 vertex3, u32 color3, u32 vertex4, u32 color4) { // shaded quad
        const auto p1 = Vector2D((float) (vertex1 & 0xFFFF), (float)(vertex1 >> 16));
        const auto p2 = Vector2D((float) (vertex2 & 0xFFFF), (float)(vertex2 >> 16));
        const auto p3 = Vector2D((float) (vertex3 & 0xFFFF), (float)(vertex3 >> 16));
        const auto p4 = Vector2D((float) (vertex4 & 0xFFFF), (float)(vertex4 >> 16));

        color1 = BGRToRGBA <semi_transparent> (color1);
        color2 = BGRToRGBA <semi_transparent> (color2);
        color3 = BGRToRGBA <semi_transparent> (color3);
        color4 = BGRToRGBA <semi_transparent> (color4);

        push_vertex(p1, color1);
        push_vertex(p2, color2);
        push_vertex(p3, color3);
        push_vertex(p2, color2);
        push_vertex(p3, color3);
        push_vertex(p4, color4);
    }

    void draw () {
        poll_events();
        if (vertex_buffer_index == 0)
            return;

        window.draw(vertex_buffer);

        sf::Texture texture; // dump RGBA values of VRAM for debugging
        texture.create(1024, 512);
        texture.update((u8*) vram.pixels.data());
        sf::Sprite sprite(texture);
        window.draw(sprite);

        window.display();

        vertex_buffer.clear();
        vertex_buffer_index = 0;
    }
};
