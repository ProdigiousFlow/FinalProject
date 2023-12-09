// FinalProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

const float M_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

void MoveMonkey(PhysicsSprite& monkey, int elapsedMS) {
    if (Keyboard::isKeyPressed(Keyboard::Right)) {
        Vector2f newPos(monkey.getCenter());
        newPos.x = newPos.x + (M_SPEED * elapsedMS);
        monkey.setCenter(newPos);
    }
    if (Keyboard::isKeyPressed(Keyboard::Left)) {
        Vector2f newPos(monkey.getCenter());
        newPos.x = newPos.x - (M_SPEED * elapsedMS);
        monkey.setCenter(newPos);
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}


int main()
{
    RenderWindow window(VideoMode(800, 600), "Banana Catcher");
    World world(Vector2f(0, 0));
    int score(0);
    int lives(20);

    Music music;
    if (!music.openFromFile("jungle.ogg")) {
        cout << "Failed to load jungle.ogg ";
        exit(6);
    }
    music.play();

    PhysicsSprite& monkey = *new PhysicsSprite();
    Texture monkeyTex;
    LoadTex(monkeyTex, "images/monkey.png");
    monkey.setTexture(monkeyTex);
    Vector2f sz = monkey.getSize();
    monkey.setCenter(Vector2f(400,
        600 - (sz.y / 2)));

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle bottom;
    bottom.setSize(Vector2f(800, 10));
    bottom.setCenter(Vector2f(400, 595));
    bottom.setStatic(true);
    world.AddPhysicsBody(bottom);


    PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    Texture bananaTex;
    LoadTex(bananaTex, "images/banana.png");
    PhysicsShapeList<PhysicsSprite> bananas;
    for (int i(0); i < 2; i++) {
        PhysicsSprite& banana = bananas.Create();
        banana.setTexture(bananaTex);

        banana.setCenter(Vector2f(1 + rand() % 800, 50));
        banana.setVelocity(Vector2f(0, 0.25));
        world.AddPhysicsBody(banana);
        banana.onCollision =
            [&monkey, &world, &bottom, &banana, &bananas, &score, &lives]
            (PhysicsBodyCollisionResult result) {
            if (result.object2 == monkey) {
                world.RemovePhysicsBody(banana);
                bananas.QueueRemove(banana);
                score += 10;
            }
            if (result.object2 == bottom) {
                world.RemovePhysicsBody(banana);
                lives -= 1;
            }
            };
    }

    Font fnt;
    if (!fnt.loadFromFile("arial.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    while (lives > 0) {

        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            MoveMonkey(monkey, deltaMS);

            bananas.DoRemovals();
            for (PhysicsShape& banana : bananas) {
                window.draw((PhysicsSprite&)banana);
            }

            window.draw(monkey);
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(fnt);
            window.draw(scoreText);

            window.display();
        }

        window.display();


    }
}


