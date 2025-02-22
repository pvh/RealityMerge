#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

// third-party
#if defined(_MSC_VER)

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#else

#include <catch2/catch.hpp>
#endif

// regional
#include <cavi/usdj_am/assignment.hpp>
#include <cavi/usdj_am/descriptor.hpp>
#include <cavi/usdj_am/file.hpp>
#include <cavi/usdj_am/utils/document.hpp>
#include <cavi/usdj_am/utils/item.hpp>
#include <cavi/usdj_am/utils/json_writer.hpp>

using std::filesystem::exists;
using std::filesystem::file_size;
using std::filesystem::path;
using std::filesystem::temp_directory_path;

path const ASSETS = "assets";

path const ROOT = "files";

TEST_CASE("Validate `Document` loading and saving", "[Document]") {
    using namespace cavi::usdj_am;

    path const TEMP = temp_directory_path();
    auto STEM =
        GENERATE(as<std::string>{}, "Ball.shadingVariants", "helloWorld", "relativeReference", "usdPhysicsBoxOnBox");
    auto load_path = ROOT / ASSETS / (STEM + ".usdj-am");
    auto document = utils::Document::load(load_path);
    CHECK(document != static_cast<AMdoc*>(nullptr));
    auto save_path = TEMP / (STEM + ".usdj-am");
    document.save(save_path);
    CHECK(file_size(save_path) == file_size(load_path));
    std::ifstream load_ifs(load_path, std::ios::binary | std::ios::in);
    CHECK(load_ifs);
    std::ifstream save_ifs(save_path, std::ios::binary | std::ios::in);
    CHECK(save_ifs);
    auto file_mismatch = std::mismatch(std::istreambuf_iterator<std::ifstream::char_type>(save_ifs),
                                       std::istreambuf_iterator<std::ifstream::char_type>(),
                                       std::istreambuf_iterator<std::ifstream::char_type>(load_ifs));
    CHECK(file_mismatch.first == std::istreambuf_iterator<std::ifstream::char_type>());
}

TEST_CASE("Load a USDJ-AM file", "[File]") {
    using namespace cavi::usdj_am;

    CHECK_THROWS_AS(utils::Document::load(path{}), std::invalid_argument);
    auto document = utils::Document::load(ROOT / ASSETS / "helloWorld.usdj-am");
    CHECK(document != static_cast<AMdoc*>(nullptr));
    CHECK_THROWS_AS(File{nullptr}, std::invalid_argument);
    auto file = File{document};
}

TEST_CASE("Validate `File` with USDA.JSON files", "[File]") {
    using namespace cavi::usdj_am;

    auto STEM =
        GENERATE(as<std::string>{}, "Ball.shadingVariants", "helloWorld", "relativeReference", "usdPhysicsBoxOnBox");
    auto usdj_am_path = ROOT / ASSETS / (STEM + ".usdj-am");
    auto document = utils::Document::load(usdj_am_path);
    CHECK(document != static_cast<AMdoc*>(nullptr));
    auto file = File{document};
    // Match the indenting of the example USDA JSON files.
    utils::JsonWriter json_writer{utils::JsonWriter::Indenter{' ', 2}};
    file.accept(json_writer);
    auto usda_json_path = ROOT / ASSETS / (STEM + ".usda.json");
    std::ifstream ifs(usda_json_path, std::ios::in | std::ios::ate);
    CHECK(ifs);
    std::string usda_json;
    usda_json.reserve(ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    usda_json.assign(std::istreambuf_iterator<std::ifstream::char_type>(ifs),
                     std::istreambuf_iterator<std::ifstream::char_type>());
    CHECK(json_writer.operator std::string() == usda_json);
}

TEST_CASE("Validate nested `File` with USDA.JSON files", "[File]") {
    using namespace cavi::usdj_am;

    auto STEM = GENERATE(as<std::string>{}, "brave-ape-49", "a-cube", "two-cubes", "cube-island", "foolish-ape-51");
    auto usdj_am_path = ROOT / (STEM + ".automerge");
    auto document = utils::Document::load(usdj_am_path);
    CHECK(document != static_cast<AMdoc*>(nullptr));
    auto file = File{document, document.get_item() / "data" / "scene"};
    // Match the indenting of the canonical USDA JSON files.
    utils::JsonWriter json_writer{utils::JsonWriter::Indenter{' ', 2}};
    file.accept(json_writer);
    path const TEMP = temp_directory_path();
    std::ostringstream command;
    // Format the canonical JSON file with jq.
    auto usda_json_path = ROOT / (STEM + ".usda.json");
    auto lhs_jq_json_path = TEMP / (STEM + ".lhs.jq.json");
    command << "jq -S . " << usda_json_path << " > " << lhs_jq_json_path;
    CHECK(system(command.str().c_str()) == 0);
    CHECK(exists(lhs_jq_json_path));
    // Output the USDA JSON file.
    auto usdj_am_json_path = TEMP / (STEM + ".usda.json");
    std::ofstream ofs(usdj_am_json_path, std::ios::out);
    CHECK(ofs);
    ofs << json_writer.operator std::string();
    ofs.close();
    CHECK(exists(usdj_am_json_path));
    // Format the JSON output with jq.
    auto rhs_jq_json_path = TEMP / (STEM + ".rhs.jq.json");
    command.str("");
    command << "jq -S . " << usdj_am_json_path << " > " << rhs_jq_json_path;
    CHECK(system(command.str().c_str()) == 0);
    CHECK(exists(rhs_jq_json_path));
    // Compare the formatted JSON files.
    std::ifstream lhs_fs(lhs_jq_json_path, std::ios::in | std::ios::ate);
    CHECK(lhs_fs);
    std::string lhs_jq_json;
    lhs_jq_json.reserve(lhs_fs.tellg());
    lhs_fs.seekg(0, std::ios::beg);
    lhs_jq_json.assign(std::istreambuf_iterator<std::ifstream::char_type>(lhs_fs),
                       std::istreambuf_iterator<std::ifstream::char_type>());
    std::ifstream rhs_fs(rhs_jq_json_path, std::ios::in | std::ios::ate);
    CHECK(rhs_fs);
    std::string rhs_jq_json;
    rhs_jq_json.reserve(rhs_fs.tellg());
    rhs_fs.seekg(0, std::ios::beg);
    rhs_jq_json.assign(std::istreambuf_iterator<std::ifstream::char_type>(rhs_fs),
                       std::istreambuf_iterator<std::ifstream::char_type>());
    CHECK(lhs_jq_json == rhs_jq_json);
}

TEST_CASE("Validate `Item` path parsing with key leaf", "[utils::Item]") {
    using namespace cavi::usdj_am;

    auto document = utils::Document::load(ROOT / "brave-ape-49.automerge");
    CHECK(document != static_cast<AMdoc*>(nullptr));
    CHECK_THROWS_AS(document.get_item(""), std::invalid_argument);
    CHECK(AMitemObjId(document.get_item("/")) == AM_ROOT);
    CHECK(document.get_item("/") == document.get_item());
    auto unparsed_item = document.get_item() / "data" / "scene" / "descriptor" / "assignments";
    CHECK(unparsed_item.operator AMitem const*());
    auto assignments = Descriptor::Assignments{document, unparsed_item};
    CHECK_THROWS_AS(document.get_item("data/scene/descriptor/assignments"), std::invalid_argument);
    auto parsed_item = document.get_item("/data/scene/descriptor/assignments");
    CHECK(parsed_item == unparsed_item);
    auto unparsed_assignments = Descriptor::Assignments{document, unparsed_item};
    auto parsed_assignments = Descriptor::Assignments{document, parsed_item};
    CHECK(parsed_assignments.get_document() == unparsed_assignments.get_document());
    CHECK(AMobjIdEqual(parsed_assignments.get_object_id(), unparsed_assignments.get_object_id()));
}

TEST_CASE("Validate `Item` path parsing with pos leaf", "[utils::Item]") {
    using namespace cavi::usdj_am;

    auto document = utils::Document::load(ROOT / "brave-ape-49.automerge");
    CHECK(document != static_cast<AMdoc*>(nullptr));
    CHECK_THROWS_AS(document.get_item(""), std::invalid_argument);
    CHECK(AMitemObjId(document.get_item("/")) == AM_ROOT);
    CHECK(document.get_item("/") == document.get_item());
    auto unparsed_item = document.get_item() / "data" / "scene" / "descriptor" / "assignments" / 0;
    CHECK(unparsed_item.operator AMitem const*());
    auto assignments = Descriptor::Assignments{document, unparsed_item};
    CHECK_THROWS_AS(document.get_item("data/scene/descriptor/assignments/0"), std::invalid_argument);
    CHECK_THROWS_AS(document.get_item("data/scene/descriptor/assignments/zero"), std::invalid_argument);
    auto parsed_item = document.get_item("/data/scene/descriptor/assignments/0");
    CHECK(parsed_item == unparsed_item);
    auto unparsed_assignment = Assignment{document, unparsed_item};
    auto parsed_assignment = Assignment{document, parsed_item};
    CHECK(parsed_assignment.get_document() == unparsed_assignment.get_document());
    CHECK(AMobjIdEqual(parsed_assignment.get_object_id(), unparsed_assignment.get_object_id()));
}