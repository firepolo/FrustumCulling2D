#include <iostream>
#include <sfml\Graphics.hpp>
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>

struct Renderer : sf::Sprite
{
	sf::Uint32 width, height;

	sf::Texture target;

	union
	{
		sf::Uint32 *rgba;
		sf::Uint8 *bytes;
	} pixels;

	Renderer(sf::Uint32 width, sf::Uint32 height, sf::Uint32 scale) : sf::Sprite()
	{
		this->width = width / scale;
		this->height = height / scale;
		target.create(this->width, this->height);
		setTexture(target, true);
		setScale(scale, scale);
		pixels.rgba = new sf::Uint32[width * height];
	}

	~Renderer()
	{
		delete[] pixels.rgba;
		sf::Sprite::~Sprite();
	}

	inline void clear(int rgba)
	{
		for (int i = 0, size = width * height; i < size; ++i) pixels.rgba[i] = rgba;
	}

	inline void setPixel(int x, int y, int rgba)
	{
		if (x >= 0 && x < width && y >= 0 && y < height) pixels.rgba[y * width + x] = rgba;
	}

	inline void fillRect(int x, int y, int w, int h, sf::Uint32 rgba)
	{
		int b = y + h;
		int r = x + w;

		for (; y < b; ++y)
		{
			if (y < 0 || y >= height) continue;
			int row = y * width;

			for (int c = x; c < r; ++c)
			{
				if (c < 0 || c >= width) continue;
				pixels.rgba[row + c] = rgba;
			}
		}
	}

	inline void swapBuffer()
	{
		target.update(pixels.bytes);
	}
};

struct Map
{
	sf::Uint32 size;
	sf::Uint32 *data;

	Map() : size(0), data(0) {}

	void create(sf::Uint32 size)
	{
		this->size = size;
		data = new sf::Uint32[size * size];

		srand(time(0));
		for (int i = 0, s = size * size; i < s; ++i) data[i] = sf::Uint32(rand() / (float)RAND_MAX * 0xffffff00) | 0xff;
	}

	~Map()
	{
		if (data) delete[] data;
	}
};

struct Plane
{
	glm::vec2 origin;
	glm::vec2 normal;

	void set(const glm::vec2 &origin, const glm::vec2 &vector)
	{
		this->origin = origin;
		normal.x = vector.y;
		normal.y = -vector.x;
	}
};

struct Rect
{
	float x, y, w, h;

	Rect(float x, float y, float w, float h)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}
};

struct Frustum
{
	static const int NPlanes = 3;

	const float LinSpeed = 2.0f;
	const float YawSpeed = 0.2f;
	const float Far = 50.0f;
	const float Fov = glm::pi<float>() / 180.0f * 70.0f;
	const float Fov2 = Fov / 2.0f;
	const float Side = glm::tan(Fov2) * Far;

	glm::vec2 position;
	glm::vec2 look;
	glm::vec2 target;
	float yaw;

	Plane planes[NPlanes];
	Map map;

	Frustum(const glm::vec2 &position)
	{
		this->position = position;
		yaw = 0;
		turn(0);

		map.create(2048);
	}

	inline bool inner(float x, float y)
	{
		for (int i = 0; i < NPlanes; ++i)
		{
			Plane &plane = planes[i];
			if (plane.normal.x * (x - plane.origin.x) + plane.normal.y * (y - plane.origin.y) > 0) return false;
		}
		return true;
	}

	inline Rect &getBound()
	{
		Rect bound(INT_MAX, INT_MAX, INT_MIN, INT_MIN);
		for (int i = 0; i < NPlanes; ++i)
		{
			Plane &plane = planes[i];
			if (plane.origin.x < bound.x) bound.x = plane.origin.x;
			if (plane.origin.x > bound.w) bound.w = plane.origin.x;
			if (plane.origin.y < bound.y) bound.y = plane.origin.y;
			if (plane.origin.y > bound.h) bound.h = plane.origin.y;
		}
		bound.w = bound.x + glm::abs(bound.w - bound.x);
		bound.h = bound.y + glm::abs(bound.h - bound.y);
		return bound;
	}

	inline void turn(float r)
	{
		yaw += r;
		look.x = glm::cos(yaw);
		look.y = glm::sin(yaw);
	}

	inline void move(const glm::vec2 &dir)
	{
		position += dir;
	}

	void update()
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) move(look * LinSpeed);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) move(look * -LinSpeed);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) move(glm::vec2(look.y * LinSpeed, -look.x * LinSpeed));
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) move(glm::vec2(-look.y * LinSpeed, look.x * LinSpeed));

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) turn(-YawSpeed);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) turn(YawSpeed);

		target = position + look * Far;

		glm::vec2 l = target + glm::vec2(look.y, -look.x) * Side;
		glm::vec2 r = target + glm::vec2(-look.y, look.x) * Side;
		planes[0].set(position, l - position);
		planes[1].set(l, r - l);
		planes[2].set(r, position - r);
	}

	void draw(Renderer &renderer)
	{
		Rect b = getBound();
		for (int y = b.y; y < b.h; ++y)
		{
			if (y < 0 || y >= map.size) continue;
			for (int x = b.x; x < b.w; ++x)
			{
				if (x < 0 || x >= map.size) continue;
				if (inner(x, y)) renderer.setPixel(x, y, map.data[y * map.size + x]);
			}
		}

		renderer.fillRect(position.x, position.y, 1, 1, 0xff0000ff);
		renderer.fillRect(target.x, target.y, 1, 1, 0xffffffff);
	}
};

int main(int argc, char *argv[])
{
	const int Width = 1024;
	const int Height = 768;
	const sf::Time SleepTime = sf::milliseconds(33);

	sf::RenderWindow window(sf::VideoMode(Width, Height), "Frusum culling");
	sf::Event event;
	
	Renderer renderer(Width, Height, 4);
	Frustum frustum(glm::vec2(renderer.width / 2, renderer.height / 2));

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape) || event.type == sf::Event::Closed) window.close();
		}

		frustum.update();

		renderer.clear(0xff);
		frustum.draw(renderer);
		renderer.swapBuffer();

		window.clear();
		window.draw(renderer);
		window.display();
		sf::sleep(SleepTime);
	}

	return 0;
}