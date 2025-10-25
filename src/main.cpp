#include <Geode/Loader.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/ui/Popup.hpp>
#include "gmd3.hpp"
#include "shared.hpp"

using namespace geode::prelude;
using namespace gmd;

static auto FILE_PICK_OPTIONS = file::FilePickOptions {
    std::nullopt,
    {
        {
            "GMD3 Level Files",
            { "*.gmd3" }
        }
    }
};

struct $modify(GMD3ImportLayer, LevelBrowserLayer) {
    struct Fields {
        EventListener<Task<Result<std::vector<std::filesystem::path>>>> pickListener;
    };

    void onOpenFile(CCObject*) {
        m_fields->pickListener.setFilter(file::pickMany(FILE_PICK_OPTIONS));
        m_fields->pickListener.bind([this](auto event) {
            if (auto result = event->getValue()) {
                if (result->isOk()) {
                    auto paths = **result;
                    for (auto const& path : paths) {
                        auto importRes = ImportGmdFile::from(path)
                            .inferType()
                            .setImportSong(true)
                            .intoLevel();
                        
                        if (importRes) {
                            auto level = importRes.unwrap();
                            level->retain();
                            LocalLevelManager::get()->m_localLevels->insertObject(level, 0);
                            
                            createQuickPopup(
                                "Success",
                                fmt::format("Imported: {}", level->m_levelName),
                                "OK",
                                nullptr,
                                [](auto, bool) {
                                    auto scene = CCScene::create();
                                    auto layer = LevelBrowserLayer::create(
                                        GJSearchObject::create(SearchType::MyLevels)
                                    );
                                    scene->addChild(layer);
                                    CCDirector::sharedDirector()->replaceScene(
                                        CCTransitionFade::create(.5f, scene)
                                    );
                                }
                            );
                        }
                        else {
                            FLAlertLayer::create(
                                "Error Importing",
                                importRes.unwrapErr(),
                                "OK"
                            )->show();
                        }
                    }
                }
                else {
                    FLAlertLayer::create(
                        "Error Opening File",
                        result->unwrapErr(),
                        "OK"
                    )->show();
                }
            }
        });
    }

    $override
    bool init(GJSearchObject* search) {
        if (!LevelBrowserLayer::init(search))
            return false;

        if (search->m_searchType == SearchType::MyLevels) {
            auto menu = this->getChildByID("new-level-menu");
            if (menu) {
                auto btn = CCMenuItemSpriteExtra::create(
                    CircleButtonSprite::createWithSpriteFrameName(
                        "file.png"_spr, 1.0f,
                        CircleBaseColor::Blue,
                        CircleBaseSize::Big
                    ),
                    this,
                    menu_selector(GMD3ImportLayer::onOpenFile)
                );
                btn->setID("gmd3-import-button"_spr);
                menu->addChild(btn);
                menu->updateLayout();
            }
        }

        return true;
    }
};