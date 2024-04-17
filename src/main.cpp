#include <iostream>
#include <vector>
#include <random>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

enum class ELEMENT
{
	EMPTY,
	WALL,
	DOOR
};

class Level
{
	public:
		Level(size_t width, size_t height) : elements(width, std::vector<ELEMENT>(height, ELEMENT::EMPTY)) {}

		ELEMENT at(size_t x, size_t y) const { return elements[x][y]; }
		ELEMENT &at(size_t x, size_t y) { return elements[x][y]; }

		size_t size() const { return elements.size(); }
		size_t size(size_t index) const { return elements[index].size(); }

	private:
		std::vector<std::vector<ELEMENT>> elements;
};

struct FlagsManager
{
	bool noClip = false;
};

class Player
{
	public:
		explicit Player(const Level &level)
			: level(level), position(1.5f, 1.5f), direction(-1.0f, 0.0f), plane(0.0f, 0.66f) {}

		void handleInput(GLFWwindow* window)
		{
			float moveSpeed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 0.1f : 0.05f, rotSpeed = 0.05f;
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

			if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS && isNoClipTriggered)
			{
				isNoClipTriggered = false;
				flagsManager.noClip = !flagsManager.noClip;
			} else if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_RELEASE) isNoClipTriggered = true;

			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			{
				if (flagsManager.noClip)
				{
					position.x += direction.x * moveSpeed;
					position.y += direction.y * moveSpeed;
				} else
				{
					tryMove(position.x + direction.x * moveSpeed, position.y);
					tryMove(position.x, position.y + direction.y * moveSpeed);
				}
			}
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			{
				if (flagsManager.noClip)
				{
					position.x -= direction.x * moveSpeed;
					position.y -= direction.y * moveSpeed;
				} else
				{
					tryMove(position.x - direction.x * moveSpeed, position.y);
					tryMove(position.x, position.y - direction.y * moveSpeed);
				}
			}
			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) rotate(rotSpeed);
			if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) rotate(-rotSpeed);
		}

		const glm::vec2 &getPosition() const { return position; }
		const glm::vec2 &getDirection() const { return direction; }
		const glm::vec2 &getPlane() const { return plane; }

	private:
		void rotate(float angle)
		{
			float oldDirX = direction.x;
			direction.x = direction.x * std::cos(angle) - direction.y * std::sin(angle);
			direction.y = oldDirX * std::sin(angle) + direction.y * std::cos(angle);

			float oldPlaneX = plane.x;
			plane.x = plane.x * std::cos(angle) - plane.y * std::sin(angle);
			plane.y = oldPlaneX * std::sin(angle) + plane.y * std::cos(angle);
		}

		void tryMove(float newX, float newY)
		{
			if (level.at(static_cast<size_t>(newX), static_cast<size_t>(newY)) == ELEMENT::EMPTY)
			{
				position.x = newX;
				position.y = newY;
			}
		}

		const Level &level;
		glm::vec2 position, direction, plane;
		FlagsManager flagsManager;
		bool isNoClipTriggered = true;
};

class Renderer
{
	public:
		Renderer() : window(nullptr) {}
		~Renderer()
		{
			glfwDestroyWindow(window);
			glfwTerminate();
		}

		void init()
		{
			if (!glfwInit())
			{
				std::cerr << "Failed to initialize GLFW!" << std::endl;
				exit(EXIT_FAILURE);
			}

			window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RaycastingTest", nullptr, nullptr);
			if (!window)
			{
				std::cerr << "Failed to create window!" << std::endl;
				glfwTerminate();
				exit(EXIT_FAILURE);
			}

			glfwMakeContextCurrent(window);
		}

		void renderLevel(const Level &level, const Player &player)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1, -1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glClear(GL_COLOR_BUFFER_BIT);

			for (size_t x = 0; x < SCREEN_WIDTH; ++x)
			{
				float cameraX = 2.0f * static_cast<float>(x) / SCREEN_WIDTH - 1,
					  rayDirX = player.getDirection().x + player.getPlane().x * cameraX,
					  rayDirY = player.getDirection().y + player.getPlane().y * cameraX,
					  deltaDistX = std::abs(1 / rayDirX), deltaDistY = std::abs(1 / rayDirY),
					  sideDistX, sideDistY;

				size_t mapX = static_cast<size_t>(player.getPosition().x), mapY = static_cast<size_t>(player.getPosition().y),
					   hit = 0, stepX, stepY, side;

				if (rayDirX < 0)
				{
					stepX = -1;
					sideDistX = (player.getPosition().x - static_cast<float>(mapX)) * deltaDistX;
				} else
				{
					stepX = 1;
					sideDistX = (static_cast<float>(mapX) + 1.0f - player.getPosition().x) * deltaDistX;
				}

				if (rayDirY < 0)
				{
					stepY = -1;
					sideDistY = (player.getPosition().y - static_cast<float>(mapY)) * deltaDistY;
				} else
				{
					stepY = 1;
					sideDistY = (static_cast<float>(mapY) + 1.0f - player.getPosition().y) * deltaDistY;
				}

				while (hit == 0 && mapX < level.size() && mapY < level.size(0))
				{
					if (sideDistX < sideDistY)
					{
						sideDistX += deltaDistX;
						mapX += stepX;
						side = 0;
					} else
					{
						sideDistY += deltaDistY;
						mapY += stepY;
						side = 1;
					}

					if (mapX < level.size() && mapY < level.size(0) && level.at(mapX, mapY) != ELEMENT::EMPTY) hit = 1;
				}

				if (mapX >= level.size() || mapY >= level.size(0))
				{
					glColor3f(0.0f, 0.0f, 0.0f);
					glBegin(GL_LINES);
					glVertex2i(static_cast<int>(x), 0);
					glVertex2i(static_cast<int>(x), SCREEN_HEIGHT);
					glEnd();

					continue;
				}

				float perpWallDist =
					side == 0 ?
					(static_cast<float>(mapX) - player.getPosition().x + static_cast<float>((1 - stepX) / 2)) / rayDirX :
					(static_cast<float>(mapY) - player.getPosition().y + static_cast<float>((1 - stepY) / 2)) / rayDirY;

				int lineHeight = static_cast<int>(SCREEN_HEIGHT / perpWallDist),
					drawStart = std::max(0, -lineHeight / 2 + SCREEN_HEIGHT / 2),
					drawEnd = std::min(lineHeight / 2 + SCREEN_HEIGHT / 2, static_cast<int>(SCREEN_HEIGHT) - 1);

				glm::vec3 color;
				switch (level.at(mapX, mapY))
				{
					case ELEMENT::WALL:
						color = glm::vec3(1.0f, 0.0f, 0.0f);
						break;
					case ELEMENT::DOOR:
						color = glm::vec3(0.0f, 1.0f, 0.0f);
						break;
					default:
						color = glm::vec3(1.0f, 0.0f, 1.0f);
						break;
				}

				glColor3f(color.r, color.g, color.b);
				glBegin(GL_LINES);
				glVertex2i(static_cast<int>(x), drawStart);
				glVertex2i(static_cast<int>(x), drawEnd);
				glEnd();
			}

			glfwSwapBuffers(window);
		}

		GLFWwindow* getWindow() const { return window; }

	private:
		GLFWwindow* window;
};

Level generateLevel(size_t width, size_t height)
{
	Level level(width, height);

	for (size_t x = 0; x < width; ++x)
	{
		level.at(x, 0) = ELEMENT::WALL;
		level.at(x, height - 1) = ELEMENT::WALL;
	}
	for (size_t y = 0; y < height; ++y)
	{
		level.at(0, y) = ELEMENT::WALL;
		level.at(width - 1, y) = ELEMENT::WALL;
	}

	level.at(width / 2, height / 2) = ELEMENT::DOOR;
	return level;
}

int main()
{
	Level level = generateLevel(20, 50);

	Player player(level);
	Renderer renderer;
	renderer.init();

	while (!glfwWindowShouldClose(renderer.getWindow()))
	{
		player.handleInput(renderer.getWindow());
		renderer.renderLevel(level, player);
		glfwPollEvents();
	}

	return EXIT_SUCCESS;
}
