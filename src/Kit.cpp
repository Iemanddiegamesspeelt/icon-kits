#include "Kit.hpp"
#include "Utils.hpp"

void Kit::captureFromGame() {
    auto* gm = GameManager::get();
    if (!gm) return;

    cube = gm->getPlayerFrame();
    ship = gm->getPlayerShip();
    ball = gm->getPlayerBall();
    ufo = gm->getPlayerBird();
    wave = gm->getPlayerDart();
    robot = gm->getPlayerRobot();
    spider = gm->getPlayerSpider();
    swing = gm->getPlayerSwing();
    jetpack = gm->getPlayerJetpack();
    primaryColor = gm->getPlayerColor();
    secondaryColor = gm->getPlayerColor2();
    glowColor = gm->m_playerGlowColor;
    glow = gm->getPlayerGlow();
    deathEffect = gm->getPlayerDeathEffect();
    streak = gm->m_playerStreak;
    shipFire = gm->m_playerShipFire;
    createdAt = KitsUtils::currentTimestamp();
}

void Kit::applyToGame() const {
    auto* gm = GameManager::get();
    if (!gm) return;

    gm->setPlayerFrame(cube);
    gm->setPlayerShip(ship);
    gm->setPlayerBall(ball);
    gm->setPlayerBird(ufo);
    gm->setPlayerDart(wave);
    gm->setPlayerRobot(robot);
    gm->setPlayerSpider(spider);
    gm->setPlayerSwing(swing);
    gm->setPlayerJetpack(jetpack);
    gm->setPlayerColor(primaryColor);
    gm->setPlayerColor2(secondaryColor);
    gm->m_playerGlowColor = glowColor;
    gm->setPlayerGlow(glow);
    gm->setPlayerDeathEffect(deathEffect);
    gm->m_playerStreak = streak;
    gm->m_playerShipFire = shipFire;
}

matjson::Value Kit::toJson() const {
    auto obj = matjson::makeObject({
        {"name", name},
        {"cube", cube},
        {"ship", ship},
        {"ball", ball},
        {"ufo", ufo},
        {"wave", wave},
        {"robot", robot},
        {"spider", spider},
        {"swing", swing},
        {"jetpack", jetpack},
        {"primaryColor", primaryColor},
        {"secondaryColor", secondaryColor},
        {"glowColor", glowColor},
        {"glow", glow},
        {"deathEffect", deathEffect},
        {"streak", streak},
        {"shipFire", shipFire},
        {"favorite", favorite},
        {"createdAt", static_cast<double>(createdAt)}
    });
    return obj;
}

Kit Kit::fromJson(matjson::Value const& json) {
    Kit kit;
    try {
        if (json.contains("name") && json["name"].isString())
            kit.name = json["name"].asString().unwrap();

        auto readInt = [&](const char* key, int defaultVal) -> int {
            if (json.contains(key) && json[key].isNumber())
                return static_cast<int>(json[key].asInt().unwrap());
            return defaultVal;
        };

        auto readBool = [&](const char* key, bool defaultVal) -> bool {
            if (json.contains(key) && json[key].isBool())
                return json[key].asBool().unwrap();
            return defaultVal;
        };

        kit.cube = readInt("cube", 1);
        kit.ship = readInt("ship", 1);
        kit.ball = readInt("ball", 1);
        kit.ufo = readInt("ufo", 1);
        kit.wave = readInt("wave", 1);
        kit.robot = readInt("robot", 1);
        kit.spider = readInt("spider", 1);
        kit.swing = readInt("swing", 1);
        kit.jetpack = readInt("jetpack", 1);
        kit.primaryColor = readInt("primaryColor", 0);
        kit.secondaryColor = readInt("secondaryColor", 3);
        kit.glowColor = readInt("glowColor", 0);
        kit.glow = readBool("glow", false);
        kit.deathEffect = readInt("deathEffect", 1);
        kit.streak = readInt("streak", 1);
        kit.shipFire = readInt("shipFire", 1);
        kit.favorite = readBool("favorite", false);

        if (json.contains("createdAt") && json["createdAt"].isNumber())
            kit.createdAt = static_cast<int64_t>(json["createdAt"].asDouble().unwrap());
        else
            kit.createdAt = KitsUtils::currentTimestamp();
    }
    catch (...) {
        log::warn("Failed to parse kit from JSON, using defaults");
    }
    return kit;
}
