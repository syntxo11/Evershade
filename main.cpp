#include <SFML/Graphics.hpp>
#include <vector>
#include <sstream>
#include <filesystem>
#include <iostream>
#include "Character.h"


using namespace sf;

float offsetX = 0, offsetY = 0;
View view;
int totalpoints = 0;
int lives = 3;
bool isGameOver = false;

const int H = 28;
const int W = 150;

String TileMap[H] = {
        "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "0                                                                                                                                                     0",
        "0                                                                                                                                                     0",
        "0                                                                                                                                                     0",
        "0                                                                                                                                                     0",
        "0                                                                                                                                                     0",
        "0                                                                                                                                                     0",
        "0                                                                                                                                                     0",
        "0                                                                                                                                                     0",
        "0                                                                                                                                                     0",
        "0                                                                                                                                                    0",
        "0                                                                                                                                                    0",
        "0                                                                                                                                                    0",
        "0                                                                                    w                                                               0",
        "0                   w                                  w                   w                                                                         0",
        "0                                      w                                                                                                             0",
        "0                                                                                                                                                    0",
        "0                      c                                                       k                                                                     0",
        "0                      I                                                r     k    k   k                T                                            0",
        "0                                                                      rr     k   kkk kkk                                                            0",
        "0                               kckck                                 rrr     k    k   k                                                             0",
        "0               c    kckck       I I                                 rrrr      k                                                                     0",
        "0               I     I I              t0                           rrrrr                                                                            0",
        "0G                                     00              t0          rrrrrr            G                                                               0",
        "0   123     d    g       d             00              00         rrrrrrr                                  i                                         0",
        "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
        "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
        "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
};

class PLAYER : public Character {
public:
    bool invincible;  // Додано стан невразливості
    Clock invincibleClock;  // Доданий таймер невразливості

    PLAYER(Texture &image) {
        sprite.setTexture(image);
        sprite.setTextureRect(IntRect(108, 144, 17, 16));
        rect = FloatRect(100, 180, 16, 16);
        dx = dy = 0.1;
        currentFrame = 0;
        invincible = false;  // Початковий стан невразливості - false
    }

    void Update(float time) override {
        rect.left += dx * time;
        Collision(0);

        if (!onGround) dy = dy + 0.0005 * time;
        rect.top += dy * time;
        onGround = false;
        Collision(1);

        currentFrame += time * 0.005;
        if (currentFrame > 3) currentFrame -= 3;

        if (dx > 0) sprite.setTextureRect(IntRect(108 + 31 * int(currentFrame), 144, 17, 16));
        if (dx < 0) sprite.setTextureRect(IntRect(109 + 31 * int(currentFrame) + 16, 144, -17, 16));

        sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
        dx = 0;

        if (invincible && invincibleClock.getElapsedTime().asSeconds() > 5) {
            invincible = false;
        }
    }

    void Collision(int num) override {
        for (int i = rect.top / 16; i < (rect.top + rect.height) / 16; i++)
            for (int j = rect.left / 16; j < (rect.left + rect.width) / 16; j++) {
                if (i < 0 || i >= H || j < 0 || j >= static_cast<int>(TileMap[i].getSize())) continue;
                if ((TileMap[i][j] == 'P') || (TileMap[i][j] == 'k') || (TileMap[i][j] == '0') || (TileMap[i][j] == 'r') || (TileMap[i][j] == 't'||TileMap[i][j] == 'c')) {
                    if (dy > 0 && num == 1) { rect.top = i * 16 - rect.height; dy = 0; onGround = true; }
                    if (dy < 0 && num == 1) { rect.top = i * 16 + 16; dy = 0; }
                    if (dx > 0 && num == 0) { rect.left = j * 16 - rect.width; }
                    if (dx < 0 && num == 0) { rect.left = j * 16 + 16; }
                }
                if (TileMap[i][j] == 'I') {
                    if (dy < 0 && num == 1) {
                        TileMap[i][j] = ' ';
                        int randItem = rand() % 7 + 1;
                        if (randItem == 1) {
                            TileMap[i - 2][j] = '1';
                        } else if(randItem == 2) {
                            TileMap[i - 2][j] = '2';
                        }else {
                            TileMap[i - 2][j] = '3';
                        }
                    }
                }
                if (TileMap[i][j] == '1') {
                    TileMap[i][j] = ' ';
                    invincible = true;
                    invincibleClock.restart();
                }
                if (TileMap[i][j] == '2') {
                    TileMap[i][j] = ' ';
                    if (lives < 3) lives++;
                }
                if (TileMap[i][j] == '3') {
                    TileMap[i][j] = ' ';
                    totalpoints += 100;
                }
                if (TileMap[i][j] == 'i') {
                    isGameOver = true;
                }
            }
    }
};


class ENEMY : public Character {
public:
    ENEMY(Texture &image, int x, int y) {
        sprite.setTexture(image);
        rect = FloatRect(x, y, 16, 16);
        dx = 0.05;
        currentFrame = 0;
        life = true;
    }

    void Update(float time) override {
        rect.left += dx * time;
        Collision(0);
        if (!onGround) dy = dy + 0.0005 * time;

        rect.top += dy * time;
        onGround = false;
        Collision(1);

        currentFrame += time * 0.005;
        if (currentFrame > 2) currentFrame -= 2;

        sprite.setTextureRect(IntRect(20 * int(currentFrame), 18, 20, 16));
        if (!life) sprite.setTextureRect(IntRect(46, 18, 19, 16));

        sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
    }

    void Collision(int num) override {
        for (int i = rect.top / 16; i < (rect.top + rect.height) / 16; i++)
            for (int j = rect.left / 16; j < (rect.left + rect.width) / 16; j++) {
                if (i < 0 || i >= H || j < 0 || j >= static_cast<int>(TileMap[i].getSize())) continue;
                if ((TileMap[i][j] == 'P') || (TileMap[i][j] == 'k') || (TileMap[i][j] == '0') || (TileMap[i][j] == 'r') || (TileMap[i][j] == 't')) {
                    if (dy > 0 && num == 1) { rect.top = i * 16 - rect.height; dy = 0; onGround = true; }
                    if (dy < 0 && num == 1) { rect.top = i * 16 + 16; dy = 0; }
                    if (dx > 0 && num == 0) { rect.left = j * 16 - rect.width; dx *= -1; }
                    else if (dx < 0 && num == 0) { rect.left = j * 16 + 16; dx *= -1; }
                }
            }
    }
};

class FLYING_ENEMY : public Character {
public:
    bool hasGravity;

    FLYING_ENEMY(Texture &image, int x, int y) {
        sprite.setTexture(image);
        rect = FloatRect(x, y, 16, 16);
        dx = 0.05;
        currentFrame = 0;
        life = true;
        hasGravity = false;
    }

    void Update(float time) override {
        rect.left += dx * time;
        Collision(0);

        if (hasGravity && !onGround) dy = dy + 0.0005 * time;
        if (hasGravity) {
            rect.top += dy * time;
        }
        onGround = false;
        Collision(1);

        currentFrame += time * 0.005;
        if (currentFrame > 2) currentFrame -= 2;


        if (dx>0) sprite.setTextureRect(IntRect(22 * int(currentFrame), 0, 22, 14));
        if (dx<0) sprite.setTextureRect(IntRect(22 * int(currentFrame)+22, 0, -22, 14));


        if (!life)   sprite.setTextureRect(IntRect(45, 6, 20, 8));

        sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
    }

    void Collision(int num) override {
        for (int i = rect.top / 16; i < (rect.top + rect.height) / 16; i++)
            for (int j = rect.left / 16; j < (rect.left + rect.width) / 16; j++) {
                if (i < 0 || i >= H || j < 0 || j >= static_cast<int>(TileMap[i].getSize())) continue;
                if ((TileMap[i][j] == 'P') || (TileMap[i][j] == 'k') || (TileMap[i][j] == '0') || (TileMap[i][j] == 'r') || (TileMap[i][j] == 't' )|| (TileMap[i][j] == 'c')) {
                    if (dy > 0 && num == 1) { rect.top = i * 16 - rect.height; dy = 0; onGround = true; }
                    if (dy < 0 && num == 1) { rect.top = i * 16 + 16; dy = 0; }
                    if (dx > 0 && num == 0) { rect.left = j * 16 - rect.width; dx *= -1; }
                    else if (dx < 0 && num == 0) { rect.left = j * 16 + 16; dx *= -1; }
                }
            }
    }
};


class ScoreText {
public:
    Text text;
    Clock lifetimeClock;
    bool active;

    ScoreText(Font &font, int points, float x, float y) {
        text.setFont(font);
        text.setCharacterSize(5);
        text.setFillColor(Color::Yellow);
        text.setString("+" + std::to_string(points));
        text.setPosition(x, y);
        lifetimeClock.restart();
        active = true;
    }

    void update() {
        if (lifetimeClock.getElapsedTime().asSeconds() > 1) {
            active = false;
        }
    }
};


void showGameOverScreen(RenderWindow &window, Font &font, int totalpoints, int gameTime, bool levelComplete) {
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(levelComplete ? Color::Green : Color::Red);
    gameOverText.setString(levelComplete ? "Level Complete!" : "Game Over");
    gameOverText.setPosition(window.getSize().x / 2 - gameOverText.getLocalBounds().width / 2, 150);

    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    std::stringstream ss;
    ss << "Score: " << totalpoints << "\nTime: " << gameTime << "s";
    scoreText.setString(ss.str());
    scoreText.setPosition(window.getSize().x / 2 - scoreText.getLocalBounds().width / 2, 250);

    Text restartText;
    restartText.setFont(font);
    restartText.setCharacterSize(24);
    restartText.setFillColor(Color::White);
    restartText.setString("Press R to Restart");
    restartText.setPosition(window.getSize().x / 2 - restartText.getLocalBounds().width / 2, 350);

    Text menuText;
    menuText.setFont(font);
    menuText.setCharacterSize(24);
    menuText.setFillColor(Color::White);
    menuText.setString("Press Q return to Menu");
    menuText.setPosition(window.getSize().x / 2 - menuText.getLocalBounds().width / 2, 450);

    window.clear();
    window.draw(gameOverText);
    window.draw(scoreText);
    window.draw(restartText);
    window.draw(menuText);
    window.display();
}

void showMenu(RenderWindow &window, Font &font) {
    Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(48);
    titleText.setFillColor(Color::White);
    titleText.setString("MARTA Game");
    titleText.setPosition(window.getSize().x / 2 - titleText.getLocalBounds().width / 2, 100);

    Text startText;
    startText.setFont(font);
    startText.setCharacterSize(24);
    startText.setFillColor(Color::White);
    startText.setString("Press Enter to Start");
    startText.setPosition(window.getSize().x / 2 - startText.getLocalBounds().width / 2, 200);

    Text exitText;
    exitText.setFont(font);
    exitText.setCharacterSize(24);
    exitText.setFillColor(Color::White);
    exitText.setString("Press Esc to Exit");
    exitText.setPosition(window.getSize().x / 2 - exitText.getLocalBounds().width / 2, 300);

    window.clear();
    window.draw(titleText);
    window.draw(startText);
    window.draw(exitText);
    window.display();
}

int main(int, char* argv[]) {
    RenderWindow window(VideoMode::getDesktopMode(), "MARTA!");
    view.reset(FloatRect(0, 0, 640, 480));
    const auto gameFolder = std::filesystem::absolute(argv[0]).parent_path();

    Texture tileSet;
    if (!tileSet.loadFromFile((gameFolder / "textures/K.png").string())) {
        std::cerr << "Could not load textures/K.png\n";
        return 1;
    }

    PLAYER Mario(tileSet);

    std::vector<Character*> enemies;

    Sprite tile(tileSet);

    Clock clock;
    Clock gameClock; // Доданий годинник для відстеження часу гри
    Font font;
    if (!font.loadFromFile((gameFolder / "font/8B.ttf").string())) {
        std::cerr << "Could not load font/8B.ttf\n";
        return 1;
    }

    Text gameTimeText;
    gameTimeText.setFont(font);
    gameTimeText.setCharacterSize(24);
    gameTimeText.setFillColor(Color::White);
    gameTimeText.setPosition(300, 20);

    Text invincibleText;
    invincibleText.setFont(font);
    invincibleText.setCharacterSize(10);
    invincibleText.setFillColor(Color::Yellow);
    invincibleText.setPosition(300, 50);

    Texture heartTexture;
    if (!heartTexture.loadFromFile((gameFolder / "textures/heart.png").string())) {
        std::cerr << "Could not load textures/heart.png\n";
        return 1;
    }
    Sprite heartSprite(heartTexture);

    std::vector<ScoreText> scoreTexts;
    const std::vector<String> originalMap(TileMap, TileMap + H);
    FloatRect castleGoal;
    for (int row = 0; row < H; ++row) {
        for (std::size_t column = 0; column < TileMap[row].getSize(); ++column) {
            if (TileMap[row][column] == 'T') {
                castleGoal = FloatRect(column * 16.0f, row * 16.0f + 16.0f, 108.0f, 96.0f);
            }
        }
    }

    bool levelComplete = false;
    int finalGameTime = 0;
    bool showMenuScreen = true;

    auto startGame = [&]() {
        for (auto* enemy : enemies) delete enemy;
        enemies.clear();
        for (int row = 0; row < H; ++row) TileMap[row] = originalMap[row];
        scoreTexts.clear();
        lives = 3;
        totalpoints = 0;
        isGameOver = false;
        levelComplete = false;
        finalGameTime = 0;
        Mario.rect = FloatRect(100, 120, 16, 16);
        Mario.dx = Mario.dy = 0;
        Mario.onGround = false;
        Mario.invincible = false;
        Mario.currentFrame = 0;
        Mario.sprite.setTextureRect(IntRect(108, 144, 17, 16));
        view.reset(FloatRect(0, 0, 640, 480));
        offsetX = offsetY = 0;
        gameClock.restart();

        enemies.push_back(new ENEMY(tileSet, 10 * 16, 13 * 16));
        enemies.push_back(new ENEMY(tileSet, 20 * 16, 13 * 16));
        enemies.push_back(new ENEMY(tileSet, 30 * 16, 13 * 16));
        enemies.push_back(new FLYING_ENEMY(tileSet, 9 * 16, 17 * 16));
    };

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();

        time = time / 500;

        if (time > 20) time = 20;

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }

        if (showMenuScreen) {
            if (Keyboard::isKeyPressed(Keyboard::Enter)) {
                showMenuScreen = false;
                startGame();
            }
            if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                window.close();
                break;
            }
            window.setView(window.getDefaultView());
            showMenu(window, font);
            continue;
        }

        if (isGameOver) {
            if (Keyboard::isKeyPressed(Keyboard::R)) {
                startGame();
                continue;
            }
            if (Keyboard::isKeyPressed(Keyboard::Q)) {
                isGameOver = false;
                showMenuScreen = true;
                continue;
            }
            window.setView(window.getDefaultView());
            showGameOverScreen(window, font, totalpoints, finalGameTime, levelComplete);
            continue;
        }

        if (Keyboard::isKeyPressed(Keyboard::Left)) Mario.dx = -0.1;
        if (Keyboard::isKeyPressed(Keyboard::Right)) Mario.dx = 0.1;
        if (Keyboard::isKeyPressed(Keyboard::Up)) if (Mario.onGround) { Mario.dy = -0.27; Mario.onGround = false; }

        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            isGameOver = true;
            finalGameTime = gameClock.getElapsedTime().asSeconds();
            continue;
        }

        Mario.Update(time);
        if (isGameOver || Mario.rect.intersects(castleGoal)) {
            isGameOver = true;
            levelComplete = true;
            finalGameTime = gameClock.getElapsedTime().asSeconds();
            continue;
        }

        for (auto& enemy : enemies) {
            enemy->Update(time);
            if (Mario.rect.intersects(enemy->rect)) {
                if (enemy->life) {
                    if (Mario.dy > 0) {
                        enemy->dx = 0;
                        Mario.dy = -0.2;
                        enemy->life = false;
                        totalpoints += 100; // Додаємо очки за вбитого ворога
                        scoreTexts.push_back(ScoreText(font, 100, Mario.rect.left - offsetX, Mario.rect.top - offsetY));
                        if (auto fe = dynamic_cast<FLYING_ENEMY*>(enemy)) {
                            fe->hasGravity = true;
                        }
                    } else if (!Mario.invincible) {
                        lives--;
                        if (lives < 1) {
                            isGameOver = true;
                            finalGameTime = gameClock.getElapsedTime().asSeconds();
                            break;
                        } else {
                            Mario.rect.left = 100; // Відправляємо гравця на початкову позицію
                            Mario.rect.top = 180;
                            Mario.dx = Mario.dy = 0;
                            Mario.onGround = false;
                            view.reset(FloatRect(0, 0, 640, 480)); // Скидаємо камеру
                            offsetX = 0;
                            offsetY = 0;
                        }
                    }
                }
            }
        }

        if (isGameOver) continue;

        for (auto it = scoreTexts.begin(); it != scoreTexts.end();) {
            it->update();
            if (!it->active) {
                it = scoreTexts.erase(it);
            } else {
                ++it;
            }
        }


        if (Mario.rect.left > 200) offsetX = Mario.rect.left - 200;

        window.setView(view);
        window.clear(Color(107, 140, 255));



        for (int i = 0; i < H; i++)
            for (int j = 0; j < W && j < static_cast<int>(TileMap[i].getSize()); j++) {
                if (TileMap[i][j] == 'P')  tile.setTextureRect(IntRect(144 - 16 * 3, 112, 16, 16));
                if (TileMap[i][j] == 'k')  tile.setTextureRect(IntRect(144, 112, 16, 16));
                if (TileMap[i][j] == 'c')  tile.setTextureRect(IntRect(144 - 16, 112, 16, 16));
                if (TileMap[i][j] == 't')  tile.setTextureRect(IntRect(0, 47, 37, 95 - 47));
                if (TileMap[i][j] == 'g')  tile.setTextureRect(IntRect(0, 16 * 9 - 5, 3 * 16, 16 * 2 + 5));
                if (TileMap[i][j] == 'G')  tile.setTextureRect(IntRect(145, 215, 222 - 145, 255 - 222));
                if (TileMap[i][j] == 'd')  tile.setTextureRect(IntRect(0, 105, 74, 128 - 106));
                if (TileMap[i][j] == 'w')  tile.setTextureRect(IntRect(99, 224, 140 - 99, 255 - 224));
                if (TileMap[i][j] == 'r')  tile.setTextureRect(IntRect(144 - 32, 112, 16, 16));

                if (TileMap[i][j] == 'T')  tile.setTextureRect(IntRect(95, 16, 108, 96));

                if (TileMap[i][j] == '1')  tile.setTextureRect(IntRect(176, 176, 16, 16));
                if (TileMap[i][j] == '2')  tile.setTextureRect(IntRect(192, 176, 16, 16));
                if (TileMap[i][j] == '3')  tile.setTextureRect(IntRect(208, 176, 16, 16));

                if (TileMap[i][j] == 'I')  tile.setTextureRect(IntRect(224, 176, 16, 16));

                if (TileMap[i][j] == 'i')  tile.setTextureRect(IntRect(240, 240, 16, 16));

                if ((TileMap[i][j] == ' ') || (TileMap[i][j] == '0')) continue;

                tile.setPosition(j * 16 - offsetX, i * 16 - offsetY + (TileMap[i][j] == 'T' ? 16 : 0));
                window.draw(tile);
            }

        window.draw(Mario.sprite);

        for (auto& enemy : enemies) {
            window.draw(enemy->sprite);
        }

        for (auto& scoreText : scoreTexts) {
            window.draw(scoreText.text);
        }

        // Відображення часу гри
        int gameTime = gameClock.getElapsedTime().asSeconds();
        std::stringstream ss;
        ss << "Time: " << gameTime;
        gameTimeText.setString(ss.str());
        window.draw(gameTimeText);

        // Відображення невразливості
        if (Mario.invincible) {
            std::stringstream invincibleSS;
            invincibleSS << "Invincible: " << 5 - (int)Mario.invincibleClock.getElapsedTime().asSeconds() << "s";
            invincibleText.setString(invincibleSS.str());
            window.draw(invincibleText);
        }

        // Відображення кількості життів
        for (int i = 0; i < lives; i++) {
            heartSprite.setPosition(10 + i * 35, 10);
            window.draw(heartSprite);
        }

        window.display();

    }

    // Очистити пам'ять виділену для ворогів
    for (auto& enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

    return 0;
}


