#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


class GameManager;  // forward declaration

struct BallTrail {
    sf::CircleShape shape;
    float lifetime;
};

class Ball {
public:
    Ball(sf::RenderWindow* window, float velocity, GameManager* gameManager);
    ~Ball();
    void update(float dt);
    void render();
    void setVelocity(float coeff, float duration);
    void setFireBall(float duration);

private:
    sf::CircleShape _sprite;
    sf::Vector2f _direction;
    sf::RenderWindow* _window;
    float _velocity;
    bool _isAlive;
    bool _isFireBall;
    float _timeWithPowerupEffect;

    std::vector<BallTrail> trail; // Store all the points for the ball trail

    // Audio 
    sf::SoundBuffer paddle_hit_buffer;
    sf::Sound paddle_hit_sound;

    GameManager* _gameManager;  // Reference to the GameManager


    static constexpr float RADIUS = 10.0f;      
    static constexpr float VELOCITY = 350.0f;   // for reference.
};

