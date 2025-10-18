#include "Ball.h"
#include "GameManager.h" // avoid cicular dependencies
#include <iostream>

Ball::Ball(sf::RenderWindow* window, float velocity, GameManager* gameManager)
    : _window(window), _velocity(velocity), _gameManager(gameManager),
    _timeWithPowerupEffect(0.f), _isFireBall(false), _isAlive(true), _direction({1,1})
{
    _sprite.setRadius(RADIUS);
    _sprite.setFillColor(sf::Color::Cyan);
    _sprite.setPosition(0, 300);
    _sprite.setOrigin(_sprite.getRadius(), _sprite.getRadius()); // Centre the sprite origin so ball trail draws nicely, regardless of rotation 

    // Load the audio file and set the sound 
    paddle_hit_buffer.loadFromFile("Audio/paddle_hit.wav");
    paddle_hit_sound.setBuffer(paddle_hit_buffer); 

    brick_break_buffer.loadFromFile("Audio/brick_break.wav");
    brick_break_sound.setBuffer(brick_break_buffer);
}

Ball::~Ball()
{
}

void Ball::update(float dt)
{
    // check for powerup, tick down or correct
    if (_timeWithPowerupEffect > 0.f)
    {
        _timeWithPowerupEffect -= dt;
    }
    else
    {
        if (_velocity != VELOCITY)
            _velocity = VELOCITY;   // reset speed.
        else
        {
            setFireBall(0);    // disable fireball
            _sprite.setFillColor(sf::Color::Cyan);  // back to normal colour.
        }        
    }

    // Fireball effect
    if (_isFireBall)
    {
        // Flickering effect
        int flicker = rand() % 50 + 205; // Random value between 205 and 255
        _sprite.setFillColor(sf::Color(flicker, flicker / 2, 0)); // Orange flickering color
    }

    // Update position with a subtle floating-point error
    _sprite.move(_direction * _velocity * dt);

    // check bounds and bounce
    sf::Vector2f position = _sprite.getPosition();
    sf::Vector2u windowDimensions = _window->getSize();

    // Time step controls distance between each trail point
    const float trail_time_step = 0.04f;
    static float trail_time = 0.f;

    trail_time += dt;
    if (trail_time >= trail_time_step)
    {
        // Reset trail time for the next point 
        trail_time = 0.f;

        // Create the point and add it to the trail vector
        BallTrail point;
        point.shape.setRadius(3.f);
        point.shape.setOrigin(1.5f, 1.5f);
        point.shape.setPosition(_sprite.getPosition());
        point.shape.setFillColor(sf::Color(255, 255, 255, 200));
        point.lifetime = 0.5f;
        trail.push_back(point);

    }

    // For each element in the trail vector
    for (auto it = trail.begin(); it != trail.end();) {
        // Decrease the lifetime 
        it->lifetime -= dt;

        // If lifetime is now 0 (or below), erase the point 
        if (it->lifetime <= 0.f) {
            it = trail.erase(it);
        }
        else
        {
            // Lower the alpha value as the lifetime decreases
            sf::Color fade = it->shape.getFillColor();
            fade.a = 255 * it->lifetime;
            it->shape.setFillColor(fade);

            ++it;
        }
    }

    // bounce on walls
    if ((position.x >= windowDimensions.x - 2 * RADIUS && _direction.x > 0) || (position.x <= 0 && _direction.x < 0))
    {
        _direction.x *= -1;
    }

    // bounce on ceiling
    if (position.y <= 0 && _direction.y < 0)
    {
        _direction.y *= -1;
    }

    // lose life bounce
    if (position.y > windowDimensions.y)
    {
        _sprite.setPosition(0, 300);
        _direction = { 1, 1 };
        _gameManager->loseLife();
    }

    // collision with paddle
    if (_sprite.getGlobalBounds().intersects(_gameManager->getPaddle()->getBounds()))
    {
        _direction.y *= -1; // Bounce vertically
        paddle_hit_sound.play(); // Play the paddle hit audio

        float paddlePositionProportion = (_sprite.getPosition().x - _gameManager->getPaddle()->getBounds().left) / _gameManager->getPaddle()->getBounds().width;
        _direction.x = paddlePositionProportion * 2.0f - 1.0f;

        // Adjust position to avoid getting stuck inside the paddle
        _sprite.setPosition(_sprite.getPosition().x, _gameManager->getPaddle()->getBounds().top - 2 * RADIUS);
    }

    // collision with bricks
    int collisionResponse = _gameManager->getBrickManager()->checkCollision(_sprite, _direction);
    if (_isFireBall) return; // no collisisons when in fireBall mode.
    if (collisionResponse == 1)
    {
        _direction.x *= -1; // Bounce horizontally
        brick_break_sound.play();
    }
    else if (collisionResponse == 2)
    {
        _direction.y *= -1; // Bounce vertically
        brick_break_sound.play();
    }
}

void Ball::render()
{
    // Draw each point in the trail 
    for (const BallTrail& point : trail)
        _window->draw(point.shape);

    _window->draw(_sprite);
}

void Ball::setVelocity(float coeff, float duration)
{
    _velocity = coeff * VELOCITY;
    _timeWithPowerupEffect = duration;
}

void Ball::setFireBall(float duration)
{
    if (duration) 
    {
        _isFireBall = true;
        _timeWithPowerupEffect = duration;        
        return;
    }
    _isFireBall = false;
    _timeWithPowerupEffect = 0.f;    
}
