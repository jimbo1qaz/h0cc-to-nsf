#include "BinaryFileStream.h"
#include "ChannelMap.h"
#include "Compiler.h"
#include "FamiTrackerDocIOJson.h"
#include "FamiTrackerEnv.h"
#include "FamiTrackerModule.h"
#include "Kraid.h"
#include "SoundChipService.h"

#include "DocumentFile.h"
#include "FamiTrackerDocIO.h"
#include "FamiTrackerDocOldIO.h"
#include <Assertion.h>

#include "CLI11/CLI11.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <utility>

using std::string;

class CStdoutLog : public CCompilerLog {
public:
    void WriteLog(std::string_view text) override { std::cout << text; };

    void Clear() override {}
};

const auto ERROR_LEVEL = module_error_level_t::MODULE_ERROR_NONE;

std::unique_ptr<CFamiTrackerModule> loadModule(string in) {
    // CFamiTrackerDoc::OpenDocument(LPCWSTR lpszPathName)
    CDocumentFile OpenFile;

    OpenFile.Open(in);
    OpenFile.ValidateFile();  // Read header ID and version
    bool useCompat = OpenFile.GetFileVersion() < 0x0200u;

    if (auto newModule =
            useCompat ? compat::OpenDocumentOld(OpenFile.GetBinaryReader())
                      : CFamiTrackerDocReader{OpenFile, ERROR_LEVEL}.Load()) {
        return newModule;
    } else {
        throw CModuleException();
    }
}


static const char *const NSF_EXT = ".nsf";

int main(int argc, char **argv) try {
    CLI::App app{"0cc-to-nsf: Cross-platform 0CC to NSF converter"};

    string in_name;
    string out_name;

    {
        std::optional<string> out_name_maybe;

        app.add_option("in_name", in_name, ".0cc file")->required();
        app.add_option("out_name", out_name_maybe, ".nsf file (optional)");
        app.failure_message(CLI::FailureMessage::help);
        CLI11_PARSE(app, argc, argv);

        if (out_name_maybe) {
            out_name = *out_name_maybe;
        } else {
            fs::path out_path = in_name;
            out_path.replace_extension(NSF_EXT);
            out_name = out_path.string();
        }
    }

    // open module
    auto modptr = loadModule(in_name);
    auto &modfile = *modptr;

    // render to {module name}.wav

    CBinaryFileStream nsffile{out_name, std::ios::out | std::ios::binary};
    CCompiler compiler{modfile, std::make_unique<CStdoutLog>()};
    compiler.ExportNSF(nsffile, 0);
    nsffile.Close();

    // CBinaryFileStream outfile{"kraid.0cc", std::ios::out | std::ios::binary};
    // CFamiTrackerDocWriter io{outfile,
    //                          module_error_level_t::MODULE_ERROR_DEFAULT};
    // io.Save(modfile);
    // outfile.Close();

    std::cout << "Success\n";
    return 0;
} catch (std::exception &e) {
    std::cerr << "C++ exception: " << e.what() << '\n';
    throw;
} catch (...) {
    std::cerr << "Unknown exception\n";
    throw;
}
