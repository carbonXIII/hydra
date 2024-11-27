#include <miral/runner.h>
#include <miral/x11_support.h>

using namespace miral;

int main(int argc, char const* argv[]) {
  MirRunner runner{argc, argv};

  return runner.run_with({
      X11Support{},
    });
}
