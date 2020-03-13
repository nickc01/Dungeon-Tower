#pragma once
#include <SFML/Graphics.hpp>
#include "Array2D.h"
#include "BackgroundTile.h"
#include <array>

class Branch;
enum class Direction;

class Room
{
	sf::Vector2<int> Center;
	sf::Vector2<int> Dimensions;

	Array2D<std::shared_ptr<BackgroundTile>> backgroundTiles;

	std::shared_ptr<Branch> UpBranch = nullptr;
	std::shared_ptr<Branch> DownBranch = nullptr;
	std::shared_ptr<Branch> LeftBranch = nullptr;
	std::shared_ptr<Branch> RightBranch = nullptr;

	Room& FindAvailableRoom();
	void AllAvailableRooms(std::vector<Room*>& rooms);
	void AllAvailableBranches(std::vector<Branch*>& branches);


public:
	Room(sf::Vector2<int> center, sf::Vector2<int> dimensions);

	sf::Rect<int> GetRect() const;
	sf::Vector2<int> GetCenter() const;
	void SetCenter(sf::Vector2<int> center);

	sf::Vector2<int> GetDimensions() const;

	int GetWidth() const;
	int GetHeight() const;

	std::shared_ptr<Branch> GetBranch(Direction direction);
	std::array<std::shared_ptr<Branch>*,4> GetBranches();
	std::vector<std::shared_ptr<Branch>*> GetEmptyBranches();
	void SetBranch(Direction direction, std::shared_ptr<Branch> branch);


	bool Intersects(const Room& B) const;

	const std::shared_ptr<BackgroundTile>& GetTile(sf::Vector2<int> position) const;
	std::shared_ptr<BackgroundTile>& GetTile(sf::Vector2<int> position);


	const std::shared_ptr<BackgroundTile>& operator[](sf::Vector2<int> position) const;
	std::shared_ptr<BackgroundTile>& operator[](sf::Vector2<int> position);


	bool CheckForCollisions();

	void AddRoomToHierarchy(std::shared_ptr<Room> room);
	void AddRoomToHierarchy();

	const std::vector<Room*> GetAllConnectedRooms();
	const std::vector<Branch*> GetAllConnectedBranches();
};

