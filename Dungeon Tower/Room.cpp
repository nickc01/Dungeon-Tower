﻿#include "Room.h"
#include "Renderer.h"
#include "Branch.h"
#include <exception>

using namespace std;

Room& Room::FindAvailableRoom()
{
	vector<Room*> AvailableRooms{};
	AllAvailableRooms(AvailableRooms);

	vector<Room*> ValidRooms{};

	for (Room* room : AvailableRooms)
	{
		for (auto& branch : room->GetBranches())
		{
			if (branch != nullptr && (*branch) == nullptr)
			{
				ValidRooms.push_back(room);
				break;
			}
		}
	}

	int randomIndex = RandomNumber(0, ValidRooms.size());
	return *ValidRooms[randomIndex];
}

void Room::AllAvailableRooms(vector<Room*>& rooms)
{
	if (find(begin(rooms),end(rooms),this) >= end(rooms))
	{
		rooms.push_back(this);
		for (auto& branchPtr : GetBranches())
		{
			auto branch = *branchPtr;
			if (branch != nullptr && branch->GetDestinationRoom() != nullptr)
			{
				branch->GetDestinationRoom()->AllAvailableRooms(rooms);
			}
		}
	}
}

Room::Room(Vector2 center, Vector2 dimensions) :
	Center(center), 
	Dimensions(dimensions),
	backgroundTiles(GetWidth(),GetHeight(),nullptr)
{
	int width = GetWidth();
	int height = GetHeight();

	for (int x = 1; x < width - 1; x++)
	{
		for (int y = 1; y < height - 1; y++)
		{
			backgroundTiles[{x, y}] = make_shared<BackgroundTile>(Vector2(x, y), L' ', Color::Black, Color::Blue);
		}
	}
	for (int y = 1; y < height - 1; y++)
	{
		backgroundTiles[{width - 1, y}] = make_shared<BackgroundTile>(Vector2(width - 1, y), L'║', Color::BrightWhite, Color::Blue,true);
		backgroundTiles[{0, y}] = make_shared<BackgroundTile>(Vector2(0, y), L'║', Color::BrightWhite, Color::Blue,true);
	}
	for (int x = 1; x < width - 1; x++)
	{
		backgroundTiles[{x, height - 1}] = make_shared<BackgroundTile>(Vector2(x, height - 1), L'═', Color::BrightWhite, Color::Blue,true);
		backgroundTiles[{x, 0}] = make_shared<BackgroundTile>(Vector2(x, 0), L'═', Color::BrightWhite, Color::Blue,true);
	}
	backgroundTiles[{0, 0}] = make_shared<BackgroundTile>(Vector2(0, 0), L'╔', Color::BrightWhite, Color::Blue,true);
	backgroundTiles[{width - 1, 0}] = make_shared<BackgroundTile>(Vector2(width - 1, 0), L'╗', Color::BrightWhite, Color::Blue,true);
	backgroundTiles[{0, height - 1}] = make_shared<BackgroundTile>(Vector2(0, height - 1), L'╚', Color::BrightWhite, Color::Blue,true);
	backgroundTiles[{width - 1, height - 1}] = make_shared<BackgroundTile>(Vector2(width - 1, height - 1), L'╝', Color::BrightWhite, Color::Blue,true);
}

Rect Room::GetRect() const
{
	return Rect(Center, Dimensions);
}

Vector2 Room::GetCenter() const
{
	return Center;
}

void Room::SetCenter(Vector2 center)
{
	Center = center;
}

Vector2 Room::GetDimensions() const
{
	return Dimensions;
}

int Room::GetWidth() const
{
	return get<0>(Dimensions);
}

int Room::GetHeight() const
{
	return get<1>(Dimensions);
}

std::shared_ptr<Branch> Room::GetBranch(Direction direction)
{
	switch (direction)
	{
	case Direction::Up:
		return UpBranch;
	case Direction::Right:
		return RightBranch;
	case Direction::Down:
		return DownBranch;
	case Direction::Left:
		return LeftBranch;
	}
	return nullptr;
}

array<shared_ptr<Branch>*, 4> Room::GetBranches()
{
	auto list = std::array<std::shared_ptr<Branch>*, 4>();

	list[0] = &UpBranch;
	list[1] = &DownBranch;
	list[2] = &LeftBranch;
	list[3] = &RightBranch;

	return list;
}

void Room::SetBranch(Direction direction, std::shared_ptr<Branch> branch)
{
	switch (direction)
	{
	case Direction::Up:
		UpBranch = branch;
		break;
	case Direction::Right:
		RightBranch = branch;
		break;
	case Direction::Down:
		DownBranch = branch;
		break;
	case Direction::Left:
		LeftBranch = branch;
		break;
	default:
		break;
	}
}

bool Room::Intersects(const Room& B) const
{
	return GetRect().Intersects(B.GetRect());
}

const shared_ptr<BackgroundTile>& Room::GetTile(Vector2 position) const
{
	return backgroundTiles[position];
}

shared_ptr<BackgroundTile>& Room::GetTile(Vector2 position)
{
	return backgroundTiles[position];
}

const std::shared_ptr<BackgroundTile>& Room::operator[](Vector2 position) const
{
	return GetTile(position);
}

std::shared_ptr<BackgroundTile>& Room::operator[](Vector2 position)
{
	return GetTile(position);
}

void Room::Render() const
{
	auto oldCameraPos = Renderer::CameraPosition;

	Renderer::CameraPosition += (Dimensions / 2);

	Renderer::CameraPosition -= Center;

	for (int x = 0; x < GetWidth(); x++)
	{
		for (int y = 0; y < GetHeight(); y++)
		{
			auto ptr = GetTile({ x,y });
			if (ptr != nullptr)
			{
				ptr->Render();
			}
		}
	}

	Renderer::CameraPosition = oldCameraPos;
}

/*void Room::GetAllConnections(std::vector<const Room*>& OutRooms, std::vector<const Branch*>& OutBranches) const
{
	OutRooms.push_back(this);

	for (auto& branch : GetBranches())
	{
		if (branch != nullptr && find(begin(OutBranches), end(OutBranches), branch) < end(OutBranches))
		{
			OutBranches.push_back(branch.get());
			auto destinationRoom = branch->GetDestinationRoom();
			if (destinationRoom != nullptr && find(begin(OutRooms),end(OutRooms),destinationRoom) < end(OutRooms))
			{
				branch->GetDestinationRoom()->GetAllConnections(OutRooms, OutBranches);
			}
		}
	}
}*/

bool Room::CheckForCollisions()
{
	vector<Room*> Rooms{};

	AllAvailableRooms(Rooms);

	for (auto roomA : Rooms)
	{
		for (auto roomB : Rooms)
		{
			if (roomB == roomA)
			{
				continue;
			}
			else if (roomB->Intersects(*roomA))
			{
				return true;
			}
		}
	}
	return false;
}

void Room::AddRoomToHierarchy(shared_ptr<Room> room)
{
	while (true)
	{
		Room& availableRoom = FindAvailableRoom();

		auto branches = availableRoom.GetBranches();

		int randomBranchIndex = RandomNumber(0, 4);

		auto branchPtr = branches[randomBranchIndex];

		auto& branch = *branchPtr;

		auto rect = room->GetRect();


		if (branchPtr == &availableRoom.UpBranch)
		{
			branch = make_shared<Branch>(Direction::Up);
			branch->SetStartPoint({RandomNumber(rect.Left + 1,rect.Right),rect.Top});
		}
		else if (branchPtr == &availableRoom.DownBranch)
		{
			branch = make_shared<Branch>(Direction::Down);
			branch->SetStartPoint({ RandomNumber(rect.Left + 1,rect.Right),rect.Bottom });
		}
		else if (branchPtr == &availableRoom.LeftBranch)
		{
			branch = make_shared<Branch>(Direction::Left);
			branch->SetStartPoint({ rect.Left,RandomNumber(rect.Bottom + 1,rect.Top) });
		}
		else if (branchPtr == &availableRoom.RightBranch)
		{
			branch = make_shared<Branch>(Direction::Right);
			branch->SetStartPoint({ rect.Right,RandomNumber(rect.Bottom + 1,rect.Top) });
		}

		branch->SetDestinationRoom(room);

		Vector2 EndPoint = branch->GetDestinationPoint();
		Rect destinationRect = room->GetRect();
		Vector2 DestinationCenter = Vector2(0, 0);

		switch (branch->GetDirection())
		{
		case Direction::Up:
			DestinationCenter = Vector2(get<0>(EndPoint), get<1>(EndPoint) + (room->GetHeight() / 2));
			break;
		case Direction::Right:
			DestinationCenter = Vector2(get<0>(EndPoint) + (room->GetWidth() / 2), get<1>(EndPoint));
			break;
		case Direction::Down:
			DestinationCenter = Vector2(get<0>(EndPoint), get<1>(EndPoint) - (room->GetHeight() / 2));
			break;
		case Direction::Left:
			DestinationCenter = Vector2(get<0>(EndPoint) - (room->GetWidth() / 2), get<1>(EndPoint));
			break;
		}


		room->SetCenter(DestinationCenter);

		if (CheckForCollisions())
		{
			branch = nullptr;
			continue;
		}
		else
		{
			return;
		}
	}
}

void Room::AddRoomToHierarchy()
{
	AddRoomToHierarchy(make_shared<Room>(Vector2(0,0),Vector2(RandomNumber(20,48), RandomNumber(10, 24))));
}

const std::vector<Room*> Room::GetRoomHierarchy()
{
	std::vector<Room*> rooms{};
	AllAvailableRooms(rooms);

	return rooms;
}