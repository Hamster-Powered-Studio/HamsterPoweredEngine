#include "Level.h"
#include "Camera.h"
#include "Character.h"
#include "Global.h"

/*
template<class T>
std::enable_if_t<std::is_base_of_v<Actor, T>, Actor*> Level::SpawnActor(sf::Vector2f location, int index)
{
    Actor* newActor = new T();
    newActor->setPosition(location);
    if (!newActor) return nullptr;

    Actors.insert(Actors.begin() + index, newActor);
    return newActor;

}

Level::Level(std::string path)
{
    LoadLevel(path);
    Begin();
}

Level::~Level()
{
    for (Actor* i : Actors)
    {
        delete i;
    }
}

bool Level::SaveLevel(std::string path)
{
    return false;
}

bool Level::LoadLevel(std::string path)
{
    global::World = this;
    return false;
}

void Level::Update()
{
    for (Actor* i : Actors)
    {
        i->Update();
    }
}

void Level::Begin()
{
    Character* character = static_cast<Character*>(SpawnActor<Character>({ 50, 50 }));
    character->Possess();
    character->LoadTexture("resources/beethoicon.png");

    Actor* cam = SpawnActor<Camera>();
    cam->displayName = "Camera";
    cam->Attach(character->PlayerCam);

    Actor* cam2 = SpawnActor<Camera>();
    cam2->displayName = "Camera2";
    cam2->Attach(cam);

    SpawnActor<Actor>({ 50, 50 })->LoadTexture("resources/ScoreBox.png");
    Actor* test = SpawnActor<Actor>({ 0, 0 });
    test->Attach(character);
    test->displayName = "Another Component";
}
*/