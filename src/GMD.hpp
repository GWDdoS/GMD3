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
        Lvl,
        Gmd,
        Gmd2,
        Gmd3,
    };

    enum class GmdListFileType {
        Gmdl,
    };

    constexpr auto DEFAULT_GMD_TYPE = GmdFileType::Gmd;
    constexpr auto DEFAULT_GMD_LIST_TYPE = GmdListFileType::Gmdl;
    constexpr auto GMD2_VERSION = 1;

    constexpr const char* gmdTypeToString(GmdFileType type) {
        switch (type) {
            case GmdFileType::Lvl:  return "lvl";
            case GmdFileType::Gmd:  return "gmd";
            case GmdFileType::Gmd2: return "gmd2";
            case GmdFileType::Gmd3: return "gmd3";
            default:                return nullptr;
        }
    }

    constexpr std::optional<GmdFileType> gmdTypeFromString(const char* type) {
        using geode::utils::hash;
        switch (hash(type)) {
            case hash("lvl"):  return GmdFileType::Lvl;
            case hash("gmd"):  return GmdFileType::Gmd;
            case hash("gmd2"): return GmdFileType::Gmd2;
            case hash("gmd3"): return GmdFileType::Gmd3;
            default:           return std::nullopt;
        }
    }

    constexpr const char* gmdListTypeToString(GmdListFileType type) {
        switch (type) {
            case GmdListFileType::Gmdl: return "gmdl";
            default:                    return nullptr;
        }
    }

    constexpr std::optional<GmdListFileType> gmdListTypeFromString(const char* type) {
        using geode::utils::hash;
        switch (hash(type)) {
            case hash("gmdl"): return GmdListFileType::Gmdl;
            default:           return std::nullopt;
        }
    }

    enum class GmdFileKind {
        None,
        Level,
        List,
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

    class ImportGmdList final {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

        ImportGmdList(std::filesystem::path const& path);

    public:
        static ImportGmdList from(std::filesystem::path const& path);
        ~ImportGmdList();

        ImportGmdList& setType(GmdListFileType type);

        geode::Result<geode::Ref<GJLevelList>> intoList();
    };

    class ExportGmdList final {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

        ExportGmdList(GJLevelList* list);

    public:
        static ExportGmdList from(GJLevelList* list);
        ~ExportGmdList();

        ExportGmdList& setType(GmdListFileType type);

        geode::Result<geode::ByteVector> intoBytes() const;
        geode::Result<> intoFile(std::filesystem::path const& path) const;
    };

    geode::Result<> exportListAsGmd(
        GJLevelList* list,
        std::filesystem::path const& to,
        GmdListFileType type = DEFAULT_GMD_LIST_TYPE
    );

    geode::Result<geode::Ref<GJLevelList>> importGmdAsList(
        std::filesystem::path const& from
    );
}