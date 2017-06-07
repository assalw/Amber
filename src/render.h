#pragma once

#define MAX_CHANNELS 1024

int render_init(int width, int height, const char *vertex_file, const char *fragment_file);
int render_close();

int render(int width, int height, double transition);

int render_handle_command();

int render_add_channel(float x, float y, float width, float height, float rotation);
int render_load_texture(const char *file);
