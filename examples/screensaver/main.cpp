#include "window.hpp"

int main(int argc, char **argv) {
  try {
    abcg::Application app(argc, argv);

    Window window;
    window.setOpenGLSettings(
        {.samples = 2, .vSync = 1, .doubleBuffering = false});
    window.setWindowSettings({
        .width = 600,
        .height = 600,
        .title = "Screen Saver",
    });

    app.run(window);
  } catch (std::exception const &exception) {
    fmt::print(stderr, "{}\n", exception.what());
    return -1;
  }
  return 0;
}
