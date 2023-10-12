#include <iostream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

class GraphicsManager {
  private:
    sf::RenderWindow* m_window;
    int m_windowHeight;
    int m_windowWidth;
    int m_frameRate;

  public:
    GraphicsManager(int windowWidth, int windowHeight, int frameRate)
      : m_windowHeight(windowHeight),
        m_windowWidth(windowWidth),
        m_frameRate(frameRate)
    {
      m_window = new sf::RenderWindow(sf::VideoMode(m_windowWidth, m_windowHeight), "Test");
      m_window->setFramerateLimit(m_frameRate);
    }

    ~GraphicsManager() { delete m_window; }

    bool isOpen() const { return m_window->isOpen(); }

    sf::RenderWindow* getWindow() { return m_window; }

    int getHeight() const { return m_windowHeight; }
    int getWidth() const { return m_windowWidth; }

    void clear() { m_window->clear(); }

    void display() { m_window->display(); }
};

class Player {
  private:
    sf::RectangleShape m_player;
    float m_speed;
    sf::Vector2f m_velocity;
    GraphicsManager* m_window;

  public:
    Player()
      : m_player(),
        m_speed(300.f),
        m_velocity(0.f, 0.f),
        m_window(NULL)
    {
      m_player.setSize(sf::Vector2f(30.f, 30.f));
      m_player.setFillColor(sf::Color::Magenta);
    }

    ~Player() { }

    void setWindow(GraphicsManager* graphicsManager) { m_window = graphicsManager; }

    void handleInput(float dt) {
      m_velocity.x = 0.f;
      m_velocity.y = 0.f;

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        m_velocity.y -= m_speed * dt;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        m_velocity.y += m_speed * dt;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        m_velocity.x -= m_speed * dt;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        m_velocity.x += m_speed * dt;

      m_player.move(m_velocity);
    }

    void handleCollision() {
      if (m_player.getPosition().x < 0)
        m_player.setPosition(0, m_player.getPosition().y);
      if (m_player.getPosition().y < 0)
        m_player.setPosition(m_player.getPosition().x, 0);
      if (m_player.getPosition().x > (m_window->getWidth() - m_player.getSize().x))
        m_player.setPosition((m_window->getWidth() - m_player.getSize().x), m_player.getPosition().y);
      if (m_player.getPosition().y > (m_window->getHeight() - m_player.getSize().y))
        m_player.setPosition(m_player.getPosition().x, (m_window->getHeight() - m_player.getSize().y));
    }

    void draw() { m_window->getWindow()->draw(m_player); }

};

int main() {
  GraphicsManager graphicsManager(400, 400, 60);
  Player player;

  float dt;
  sf::Clock dtClock;

  player.setWindow(&graphicsManager);

  while (graphicsManager.isOpen()) {
    dt = dtClock.restart().asSeconds();
    sf::Event event;

    while (graphicsManager.getWindow()->pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        graphicsManager.getWindow()->close();
    }

    player.handleInput(dt);
    player.handleCollision();
    
    graphicsManager.clear();
    player.draw();
    graphicsManager.display();
  }

  return 0;
}
