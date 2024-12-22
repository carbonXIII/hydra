#include <backend/font.h>

#include <fontconfig/fontconfig.h>
#include <memory>

namespace hydra {
  struct PatternDeleter {
    void operator()(FcPattern* p) {
      FcPatternDestroy(p);
    }
  };

  using Pattern = std::unique_ptr<FcPattern, PatternDeleter>;

  FontConfig::FontConfig() {
    FcInit();
  }

  FontConfig::~FontConfig() {
    FcFini();
  }

  std::optional<std::string> FontConfig::match_font(std::string const& pattern_str) {
    Pattern pattern { FcNameParse(reinterpret_cast<const FcChar8*>(pattern_str.c_str())) };
    if(!pattern) return std::nullopt;

    FcConfigSubstitute(NULL, pattern.get(), FcMatchPattern);
    FcDefaultSubstitute(pattern.get());

    FcResult result;
    Pattern match { FcFontMatch(NULL, pattern.get(), &result) };
    if(!match) return std::nullopt;

    int size;
    FcChar8 *path;
    FcPatternGetString(match.get(), FC_FILE, 0, &path);
    FcPatternGetInteger(match.get(), FC_SIZE, 0, &size);

    return std::string(reinterpret_cast<const char*>(path));
  }
}
