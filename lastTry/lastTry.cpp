#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// Screen dimensions
const int screenWidth = 800;
const int screenHeight = 600;

// Player properties
struct Player {
    sf::RectangleShape shape;
    float speed;
    int score;
    bool alive;

    Player() : speed(10.0f), score(0), alive(true) {
        shape.setSize({ 50, 20 });
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(screenWidth / 2, 50);
    }
};

// Bullet properties
struct Bullet {
    sf::RectangleShape shape;
    bool active;

    Bullet(float x, float y) : active(true) {
        shape.setSize({ 5, 10 });
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(x, y);
    }
};

// Enemy properties
struct Enemy {
    sf::RectangleShape shape;
    bool active;

    Enemy(float x, float y) : active(true) {
        shape.setSize({ 40, 20 });
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(x, y);
    }
};

void handleInput(Player& player, std::vector<Bullet>& bullets) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && player.shape.getPosition().x > 0) {
        player.shape.move(-player.speed, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && player.shape.getPosition().x < screenWidth - player.shape.getSize().x) {
        player.shape.move(player.speed, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        bullets.emplace_back(player.shape.getPosition().x + player.shape.getSize().x / 2 - 2.5f, player.shape.getPosition().y + player.shape.getSize().y);
    }
}

void updateBullets(std::vector<Bullet>& bullets) {
    for (auto& bullet : bullets) {
        if (bullet.active) {
            bullet.shape.move(0, 5.0f);
            if (bullet.shape.getPosition().y > screenHeight) {
                bullet.active = false;
            }
        }
    }
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return !b.active; }), bullets.end());
}

void spawnEnemies(std::vector<Enemy>& enemies, float& enemySpawnTimer) {
    if (enemySpawnTimer > 60) { // Adjusted spawn timer for more manageable gameplay
        enemies.emplace_back(rand() % (screenWidth - 40), screenHeight);
        enemySpawnTimer = 0;
    }
    enemySpawnTimer++;
}

void updateEnemies(std::vector<Enemy>& enemies) {
    for (auto& enemy : enemies) {
        if (enemy.active) {
            enemy.shape.move(0, -0.5f); // Reduced speed of enemies
            if (enemy.shape.getPosition().y < 0) {
                enemy.active = false;
            }
        }
    }
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy& e) { return !e.active; }), enemies.end());
}

void checkCollisions(std::vector<Bullet>& bullets, std::vector<Enemy>& enemies, Player& player) {
    // Check bullet-enemy collisions
    for (auto& enemy : enemies) {
        for (auto& bullet : bullets) {
            if (enemy.active && bullet.active && enemy.shape.getGlobalBounds().intersects(bullet.shape.getGlobalBounds())) {
                enemy.active = false;
                bullet.active = false;
                player.score += 10;
            }
        }
    }

    // Check player-enemy collisions
    for (auto& enemy : enemies) {
        if (player.alive && enemy.active && player.shape.getGlobalBounds().intersects(enemy.shape.getGlobalBounds())) {
            player.alive = false; // Game over
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Space Shooter");
    window.setFramerateLimit(60); // Set frame rate limit

    Player player;
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;

    sf::Clock clock;
    float enemySpawnTimer = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (player.alive) {
            handleInput(player, bullets);
            updateBullets(bullets);
            spawnEnemies(enemies, enemySpawnTimer);
            updateEnemies(enemies);
            checkCollisions(bullets, enemies, player);
        }
        else {
            // Restart game when player is not alive
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                player = Player(); // Reset player
                bullets.clear();
                enemies.clear();
                player.alive = true;
            }
        }

        window.clear();

        if (player.alive) {
            window.draw(player.shape);

            for (auto& bullet : bullets) {
                if (bullet.active) {
                    window.draw(bullet.shape);
                }
            }

            for (auto& enemy : enemies) {
                if (enemy.active) {
                    window.draw(enemy.shape);
                }
            }
        }
        else {
            // Optionally display "Game Over" text or other messages
        }

        window.display();
    }

    return 0;
}
