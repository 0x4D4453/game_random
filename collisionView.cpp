#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#define GRAVIDADE 20
#define ALTURA_PULO 5

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
      m_window->setVerticalSyncEnabled(true);
    }

    ~GraphicsManager() { delete m_window; }

    bool isOpen() const { return m_window->isOpen(); }

    sf::RenderWindow* getWindow() { return m_window; }

    int getHeight() const { return m_windowHeight; }
    int getWidth() const { return m_windowWidth; }

    void clear(const sf::Color& color) { m_window->clear(color); }

    void display() { m_window->display(); }

    void setView(sf::View& v) { m_window->setView(v); }
};

class Player {
  private:
    sf::RectangleShape m_player;
    float m_speed;
    sf::Vector2f velocity;
    GraphicsManager* m_window;
    bool m_isJumping;

  public:
    Player()
      : m_player(),
        m_speed(300.f),
        velocity(0.f, 0.f),
        m_window(NULL),
        m_isJumping(true)
    {
      m_player.setSize(sf::Vector2f(50.f, 50.f));
      m_player.setFillColor(sf::Color::Magenta);
    }

    ~Player() { }

    void setWindow(GraphicsManager* graphicsManager) { m_window = graphicsManager; }

    void handleInput(float dt) {
      velocity.x = 0.f;
      // velocity.y = 0.f;
   
      // if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
      //   velocity.y -= m_speed * dt;
      // if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
      //   velocity.y += m_speed * dt;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        velocity.x -= m_speed * dt;
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        velocity.x += m_speed * dt;

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !m_isJumping) {
        velocity.y -= sqrt(2.0 * GRAVIDADE * ALTURA_PULO);
        m_isJumping = true;
      }

      if (m_isJumping)
        velocity.y += GRAVIDADE * dt;

      m_player.move(velocity);
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

    void setPosition(float x, float y) { m_player.setPosition(x, y); }

    sf::Vector2f getPosition() { return m_player.getPosition(); }

    sf::FloatRect getGlobalBounds() { return m_player.getGlobalBounds(); }

    void setVelocity(float x, float y) {
      velocity.x = x;
      velocity.y = y;
    }

    sf::Vector2f getVelocity() {
      return velocity;
    }

    void setJumping(bool isJumping) {
      m_isJumping = isJumping;

      if (!isJumping)
        velocity.y = 0;
    }

    void move(float x, float y) { m_player.move(sf::Vector2f(x, y)); }    
};

class Floor {
  private:
    sf::RectangleShape m_floor;
    GraphicsManager* m_window;
  
  public:
    Floor(float height, float width) 
      : m_floor()
    {
      m_floor.setSize(sf::Vector2f(width, height));
      m_floor.setFillColor(sf::Color::Blue);
    }

    ~Floor() {}

    void setWindow(GraphicsManager* graphicsManager) { m_window = graphicsManager; }

    void draw() {
      m_window->getWindow()->draw(m_floor);
    }

    void setPosition(float x, float y) { m_floor.setPosition(x, y); } 

    sf::Vector2f getPosition() { return m_floor.getPosition(); }

    sf::FloatRect getGlobalBounds() { return m_floor.getGlobalBounds(); }
};

class Enemy {
  private:
    sf::RectangleShape m_enemy;
    float m_speed;
    GraphicsManager* m_window;
    sf::Vector2f m_origin;
    int direction;

  public:
    Enemy() {
      m_enemy.setSize(sf::Vector2f(50.f, 50.f));
      m_enemy.setFillColor(sf::Color::Red);
      m_speed = 100.f;
      direction = 0;
    }

    ~Enemy() {}

    void setWindow(GraphicsManager* graphicsManager) { m_window = graphicsManager; }

    void setOrigin(float x, float y) {
      m_origin.x = x;
      m_origin.y = y;
      m_enemy.setPosition(sf::Vector2f(x, y));
    }

    void update(float dt) {
      if (m_enemy.getPosition().x >= (m_origin.x + 200))
        direction = 1;
      else if (m_enemy.getPosition().x <= (m_origin.x - 200))
        direction = 0;

      if (direction)
        m_enemy.move(sf::Vector2f(-m_speed * dt, 0));
      else
        m_enemy.move(sf::Vector2f(m_speed * dt, 0));
    }

    void draw() { m_window->getWindow()->draw(m_enemy); }

    sf::FloatRect getGlobalBounds() { return m_enemy.getGlobalBounds(); }
};

int main() {
  GraphicsManager graphicsManager(1280, 720, 60);
  Player player;
  Floor floor(50.f, graphicsManager.getWidth() * 2);
  Floor wall(250.f, 50.f);
  Enemy enemy;

  sf::View view(sf::Vector2f(0.f, 0.f), sf::Vector2f(1280.f, 720.f));

  //Clock
  float dt;
  sf::Clock dt_clock;

  //Set Window
  player.setWindow(&graphicsManager);
  floor.setWindow(&graphicsManager);
  wall.setWindow(&graphicsManager);
  enemy.setWindow(&graphicsManager);

  //Position
  floor.setPosition(0, graphicsManager.getHeight() - 50.f);
  wall.setPosition(100, graphicsManager.getHeight() - 300.f);
  enemy.setOrigin(graphicsManager.getWidth()/2, graphicsManager.getHeight() -100);

  //Main Loop
  while (graphicsManager.isOpen()) {
    dt = dt_clock.restart().asSeconds();
    
    //Event Queue
    sf::Event event;
    while (graphicsManager.getWindow()->pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        graphicsManager.getWindow()->close();
    }
    
    //Moving
    player.handleInput(dt);
    //player.handleCollision();
    enemy.update(dt);

    //Collision
    if (player.getGlobalBounds().intersects(floor.getGlobalBounds())) {
      sf::FloatRect playerBounds = player.getGlobalBounds();
      sf::FloatRect floorBounds = floor.getGlobalBounds();

      float floorLeft = floorBounds.left;
      float floorRight = floorLeft + floorBounds.width;
      float floorTop = floorBounds.top;
      float floorBottom = floorBounds.top + floorBounds.height;

      float playerLeft = playerBounds.left;
      float playerRight = playerBounds.left + playerBounds.width;
      float playerTop = playerBounds.top;
      float playerBottom = playerBounds.top + playerBounds.width;

      float xOverlap = std::max(0.f, std::min(playerRight, floorRight) - std::max(playerLeft, floorLeft));
      float yOverlap =  std::max(0.f, std::min(playerBottom, floorBottom) - std::max(playerTop, floorTop));
      
      if (yOverlap < xOverlap) {
         if (playerTop <floorTop)
           yOverlap *= -1;
         player.move(0, yOverlap);
      } else if (xOverlap < yOverlap) {
         if (playerLeft < floorLeft)
          xOverlap *= -1;
         player.move(xOverlap, 0);
      }

      player.setJumping(false);
    }

    if (player.getGlobalBounds().intersects(wall.getGlobalBounds())) {
      sf::FloatRect playerBounds = player.getGlobalBounds();
      sf::FloatRect wallBounds = wall.getGlobalBounds();

      float wallLeft = wallBounds.left;
      float wallRight = wallLeft + wallBounds.width;
      float wallTop = wallBounds.top;
      float wallBottom = wallBounds.top + wallBounds.height;

      float playerLeft = playerBounds.left;
      float playerRight = playerBounds.left + playerBounds.width;
      float playerTop = playerBounds.top;
      float playerBottom = playerBounds.top + playerBounds.width;

      float xOverlap = std::max(0.f, std::min(playerRight, wallRight) - std::max(playerLeft, wallLeft));
      float yOverlap =  std::max(0.f, std::min(playerBottom, wallBottom) - std::max(playerTop, wallTop));
            
      if (yOverlap < xOverlap) {
        if (playerTop < wallTop)
          yOverlap *= -1;
        player.move(0, yOverlap);
        player.setJumping(false);
      } else if (xOverlap < yOverlap) {
        if (playerLeft < wallLeft)
          xOverlap *= -1;
        player.move(xOverlap, 0);
      }
    }

    if (player.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
      sf::FloatRect playerBounds = player.getGlobalBounds();
      sf::FloatRect enemyBounds = enemy.getGlobalBounds();

      float enemyLeft = enemyBounds.left;
      float enemyRight = enemyLeft + enemyBounds.width;
      float enemyTop = enemyBounds.top;
      float enemyBottom = enemyBounds.top + enemyBounds.height;

      float playerLeft = playerBounds.left;
      float playerRight = playerBounds.left + playerBounds.width;
      float playerTop = playerBounds.top;
      float playerBottom = playerBounds.top + playerBounds.width;

      float xOverlap = std::max(0.f, std::min(playerRight, enemyRight) - std::max(playerLeft, enemyLeft));
      float yOverlap =  std::max(0.f, std::min(playerBottom, enemyBottom) - std::max(playerTop, enemyTop));
      
      if (yOverlap < xOverlap) {
        if (playerTop < enemyTop)
          yOverlap *= -1;
        player.move(0, yOverlap);
        player.setJumping(false);
      } else if (xOverlap < yOverlap) {
         if (playerLeft < enemyLeft)
          xOverlap *= -1;
         player.move(xOverlap, 0);
      }
    }

    float x = player.getPosition().x;
    float y = player.getPosition().y;

    if ((player.getPosition().x - view.getSize().x/2) < 0)
      x = graphicsManager.getWidth()/2;

    if ((player.getPosition().y) + view.getSize().y/2 > graphicsManager.getHeight())
      y = graphicsManager.getHeight()/2;

    //View / Camera
    view.setCenter(x, y);

    //Clear, Draw, Display
    graphicsManager.clear(sf::Color(236,200,169));
    graphicsManager.setView(view);
    player.draw();
    floor.draw();
    wall.draw();
    enemy.draw();
    graphicsManager.display();
  }

  return 0;
}
