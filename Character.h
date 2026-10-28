#ifndef MARTA_CHARACTER_H
#define MARTA_CHARACTER_H

#include <SFML/Graphics.hpp>

using namespace sf;

class Character {
public:
    float dx, dy;
    FloatRect rect;
    Sprite sprite;
    float currentFrame;
    bool onGround;
    bool life;

    Character();
    virtual ~Character() = default;

    virtual void Update(float time) = 0;
    virtual void Collision(int num) = 0;
};

#endif //MARTA_CHARACTER_H
