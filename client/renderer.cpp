#include "renderer.h"
#include "types/constants.h"
#include <array>

namespace tao {

static const sf::Color BG_COLOR(26, 26, 46);
static const sf::Color PADDLE1_COLOR(15, 52, 96);
static const sf::Color PADDLE2_COLOR(233, 69, 96);
static const sf::Color BALL_COLOR = sf::Color::White;

static const std::array<sf::Color, 4> ROW_COLORS = {{
    sf::Color(243, 156, 18),
    sf::Color(46, 204, 113),
    sf::Color(52, 152, 219),
    sf::Color(155, 89, 182),
}};

bool Renderer::init(const std::string& title) {
    window_.create(
        sf::VideoMode(
            static_cast<unsigned>(constants::FIELD_WIDTH),
            static_cast<unsigned>(constants::FIELD_HEIGHT)),
        title,
        sf::Style::Titlebar | sf::Style::Close);
    window_.setVerticalSyncEnabled(true);

#ifdef _WIN32
    fontLoaded_ = font_.loadFromFile("C:\\Windows\\Fonts\\consola.ttf")
               || font_.loadFromFile("C:\\Windows\\Fonts\\arial.ttf");
#else
    fontLoaded_ = font_.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")
               || font_.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf");
#endif
    return true;
}

bool Renderer::isOpen() const { return window_.isOpen(); }

void Renderer::render(const GameSnapshot& state, bool connected,
                      const std::string& statusMsg, uint8_t myId) {
    window_.clear(BG_COLOR);

    // Blocks
    int cols = constants::BLOCK_COLUMNS;
    for (int i = 0; i < static_cast<int>(state.blocksAlive.size()); ++i) {
        int row = i / cols;
        int col = i % cols;
        float totalW = cols * constants::BLOCK_WIDTH
                     + (cols - 1) * constants::BLOCK_GAP;
        float startX = (constants::FIELD_WIDTH - totalW) / 2.0f;
        float bx = startX + col * (constants::BLOCK_WIDTH + constants::BLOCK_GAP);
        float by = constants::BLOCK_AREA_TOP
                 + row * (constants::BLOCK_HEIGHT + constants::BLOCK_GAP);
        drawBlock(bx, by, state.blocksAlive[i], row);
    }

    // Paddles
    drawPaddle(state.paddle1X, constants::PADDLE1_Y, PADDLE1_COLOR);
    drawPaddle(state.paddle2X, constants::PADDLE2_Y, PADDLE2_COLOR);

    // Ball
    if (state.ballActive)
        drawBall(state.ballX, state.ballY);

    // Score
    std::string scoreStr = "P1: " + std::to_string(state.score1)
                         + "   P2: " + std::to_string(state.score2);
    drawText(scoreStr, constants::FIELD_WIDTH / 2.0f - 80.0f, 2.0f, 18,
             sf::Color::White);

    // Which player am I
    if (myId > 0) {
        std::string me = "You are Player " + std::to_string(myId);
        drawText(me, constants::FIELD_WIDTH - 200.0f, 2.0f, 14,
                 sf::Color(200, 200, 200));
    }

    // Connection dot
    {
        sf::CircleShape dot(5.0f);
        dot.setFillColor(connected ? sf::Color::Green : sf::Color::Red);
        dot.setPosition(5.0f, 5.0f);
        window_.draw(dot);
    }

    // Status bar
    drawText(statusMsg, 8.0f, constants::FIELD_HEIGHT - 22.0f, 14,
             sf::Color(220, 220, 220));

    window_.display();
}

void Renderer::drawPaddle(float cx, float cy, sf::Color color) {
    sf::RectangleShape rect(
        sf::Vector2f(constants::PADDLE_WIDTH, constants::PADDLE_HEIGHT));
    rect.setFillColor(color);
    rect.setPosition(cx - constants::PADDLE_WIDTH / 2.0f, cy);
    window_.draw(rect);
}

void Renderer::drawBall(float cx, float cy) {
    sf::CircleShape circle(constants::BALL_RADIUS);
    circle.setFillColor(BALL_COLOR);
    circle.setPosition(cx - constants::BALL_RADIUS, cy - constants::BALL_RADIUS);
    window_.draw(circle);
}

void Renderer::drawBlock(float x, float y, bool alive, int row) {
    if (!alive) return;
    sf::RectangleShape rect(
        sf::Vector2f(constants::BLOCK_WIDTH, constants::BLOCK_HEIGHT));
    int colorIdx = row % static_cast<int>(ROW_COLORS.size());
    rect.setFillColor(ROW_COLORS[colorIdx]);
    rect.setOutlineColor(sf::Color(255, 255, 255, 40));
    rect.setOutlineThickness(1.0f);
    rect.setPosition(x, y);
    window_.draw(rect);
}

void Renderer::drawText(const std::string& str, float x, float y,
                        unsigned size, sf::Color color) {
    if (!fontLoaded_) return;
    sf::Text text;
    text.setFont(font_);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    window_.draw(text);
}

} // namespace tao
