#pragma once

#include <optional>
#include <Geode/Result.hpp>
#include <Geode/utils/general.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/GJGameLevel.hpp>

namespace gmd {
    class ImportGmdFile;
    class ExportGmdFile;

    enum class GmdFileType {
        Gmd3,
    };

    constexpr auto DEFAULT_GMD_TYPE = GmdFileType::Gmd3;
    constexpr auto GMD3_VERSION = 1;

    constexpr const char* gmdTypeToString(GmdFileType type) {
        switch (type) {
            case GmdFileType::Gmd3: return "gmd3";
            default:                return nullptr;
        }
    }

    constexpr std::optional<GmdFileType> gmdTypeFromString(const char* type) {
        using geode::utils::hash;
        switch (hash(type)) {
            case hash("gmd3"): return GmdFileType::Gmd3;
            default:           return std::nullopt;
        }
    }

    enum class GmdFileKind {
        None,
        Level,
    };

    GmdFileKind getGmdFileKind(std::filesystem::path const& path);

    struct SequenceTriggerData {
        int group;
        int activations;
    };

    std::vector<SequenceTriggerData> parseSequenceTriggers(std::string const& levelData);

    template<class T>
    class IGmdFile {
    protected:
        std::optional<GmdFileType> m_type;
    
    public:
        T& setType(GmdFileType type) {
            m_type = type;
            return *static_cast<T*>(this);
        }
    };

    class ImportGmdFile : public IGmdFile<ImportGmdFile> {
    protected:
        std::filesystem::path m_path;
        bool m_importSong = false;

        ImportGmdFile(std::filesystem::path const& path);

        geode::Result<std::string> getLevelData() const;

    public:
        static ImportGmdFile from(std::filesystem::path const& path);
        bool tryInferType();
        ImportGmdFile& inferType();
        ImportGmdFile& setImportSong(bool song);
        geode::Result<GJGameLevel*> intoLevel() const;
    };

    class ExportGmdFile : public IGmdFile<ExportGmdFile> {
    protected:
        GJGameLevel* m_level;
        bool m_includeSong = false;

        ExportGmdFile(GJGameLevel* level);

        geode::Result<std::string> getLevelData() const;

    public:
        static ExportGmdFile from(GJGameLevel* level);
        ExportGmdFile& setIncludeSong(bool song);
        geode::Result<geode::ByteVector> intoBytes() const;
        geode::Result<> intoFile(std::filesystem::path const& path) const;
    };

    geode::Result<> exportLevelAsGmd(
        GJGameLevel* level,
        std::filesystem::path const& to,
        GmdFileType type = DEFAULT_GMD_TYPE
    );

    geode::Result<GJGameLevel*> importGmdAsLevel(
        std::filesystem::path const& from
    );
}