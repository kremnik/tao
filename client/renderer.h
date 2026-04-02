#pragma once

#include "types/game_types.h"
#include <SFML/Graphics.hpp>
#include <string>

namespace tao {

class Renderer {
public:
    bool init(const std::string& title);
    void render(const GameSnapshot& state, bool connected,
                const std::string& statusMsg, uint8_t myId);
    bool isOpen() const;
    sf::RenderWindow& window() { return window_; }

private:
    void drawPaddle(float cx, float cy, sf::Color color);
    void drawBall(float cx, float cy);
    void drawBlock(float x, float y, bool alive, int row);
    void drawText(const std::string& str, float x, float y,
                  unsigned size, sf::Color color);

    sf::RenderWindow window_;
    sf::Font         font_;
    bool             fontLoaded_ = false;
};

} // namespace tao
