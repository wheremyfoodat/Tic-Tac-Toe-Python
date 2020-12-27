#include "include/gpu.h"
#include "include/helpers.h"

void GPU::quad_monochrome () {
    auto color = commandParameters[0] & 0xFF'FFFF; // the 24 bit RGB color of the quad

    auto vertex1 = commandParameters[1];
    auto vertex2 = commandParameters[2];
    auto vertex3 = commandParameters[3];
    auto vertex4 = commandParameters[4];

    printf ("Vertex 1\tX: %d Y: %d\n", vertex1 & 0xFFFF, vertex1 >> 16);
    printf ("Vertex 2\tX: %d Y: %d\n", vertex2 & 0xFFFF, vertex2 >> 16);
    printf ("Vertex 3\tX: %d Y: %d\n", vertex3 & 0xFFFF, vertex3 >> 16);
    printf ("Vertex 4\tX: %d Y: %d\n", vertex4 & 0xFFFF, vertex4 >> 16);

    renderer.push_quad(vertex1, vertex2, vertex3, vertex4, color);
}

void GPU::quad_shaded () {
    auto color1 = commandParameters[0] & 0xFF'FFFF;
    auto vertex1 = commandParameters[1];

    auto color2 = commandParameters[2] & 0xFF'FFFF;
    auto vertex2 = commandParameters[3];

    auto color3 = commandParameters[4] & 0xFF'FFFF;
    auto vertex3 = commandParameters[5];

    auto color4 = commandParameters[6] & 0xFF'FFFF;
    auto vertex4 = commandParameters[7];

    renderer.push_quad (vertex1, color1, vertex2, color2, vertex3, color3, vertex4, color4);
}

void GPU::tri_shaded () {
    auto color1 = commandParameters[0] & 0xFF'FFFF;
    auto vertex1 = commandParameters[1];

    auto color2 = commandParameters[2] & 0xFF'FFFF;
    auto vertex2 = commandParameters[3];

    auto color3 = commandParameters[4] & 0xFF'FFFF;
    auto vertex3 = commandParameters[5];

    renderer.push_tri (vertex1, color1, vertex2, color2, vertex3, color3);
}
