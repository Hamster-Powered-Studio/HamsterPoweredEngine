#include "TileMapCollisionScript.h"
#include "Engine/Renderer.h"
#include "Components/Components.h"

struct Tile
{
    Tile(int x, int y, int width=32, int height=32) : X(x), Y(y), Width(width), Height(height) {}
    int X;
    int Y;
    int Width;
    int Height;

    //operator int() { return X + (Width * Y); }
};

std::vector<Tile> GetSurroundingTiles(int TileX, int TileY, int width)
{
    std::vector<Tile> tiles;
    tiles.push_back(Tile(TileX + 1, TileY, width, width));
    tiles.push_back(Tile(TileX + 1, TileY + 1, width, width));
    tiles.push_back(Tile(TileX + 1, TileY - 1, width, width));
    tiles.push_back(Tile(TileX - 1, TileY, width, width));
    tiles.push_back(Tile(TileX - 1, TileY + 1, width, width));
    tiles.push_back(Tile(TileX - 1, TileY - 1, width, width));
    tiles.push_back(Tile(TileX, TileY + 1, width, width));
    tiles.push_back(Tile(TileX, TileY - 1, width, width));
    return tiles;
}

sf::FloatRect GetCollisionBox(const Tile& tile, sf::Vector2f tmCoords)
{
    
    float left = tmCoords.x + ((tile.X * tile.Width));
    float top = tmCoords.y + ((tile.Y * tile.Height));
    return sf::FloatRect(left, top, tile.Width, tile.Height);

}

void TilemapCollisionScript::OnUpdate(float deltaTime)
{
    ScriptableActor::OnUpdate(deltaTime);
    std::vector<sf::RectangleShape*> rects;
    for (int i = 0; i < rects.size(); i++)
    {
        delete rects[i];
        rects[i] = nullptr;
    }
    rects.clear();
    
    auto view = m_Actor.GetScene()->Reg().view<TileMapComponent, TransformComponent>();
    
    auto& playerBox = GetComponent<BoxColliderComponent>();
    
    
    for (auto& map : view)
    {
        auto& TileComp = view.get<TileMapComponent>(map);
        auto& transform = view.get<TransformComponent>(map);
        //if (TileComp.Colliders.size() <= 0) continue;
        

        if (!TileComp.Bounds.intersects(playerBox.Collider)) continue;

        //translate player pos to tiles
        sf::Vector2f pos = sf::Vector2f(GetComponent<TransformComponent>().Transform.Pos.x - TileComp.Bounds.left, GetComponent<TransformComponent>().Transform.Pos.y - TileComp.Bounds.top);
        int TileX = (pos.x / (TileComp.tileWidth));
        int TileY = (pos.y / (TileComp.tileHeight));

        Tile PlayerTile(TileX, TileY, TileComp.tileWidth, TileComp.tileHeight);

        //get tiles around player
        std::vector<Tile> surroundingTiles = GetSurroundingTiles(PlayerTile.X, PlayerTile.Y, PlayerTile.Width);
        for (auto& tile : surroundingTiles)
        {
            sf::FloatRect coll = GetCollisionBox(tile, sf::Vector2f(TileComp.Bounds.left, TileComp.Bounds.top));
            sf::RectangleShape* previewRect = new sf::RectangleShape();
            rects.push_back(previewRect);
            previewRect->setSize(sf::Vector2f(coll.width, coll.height));
            previewRect->setPosition(coll.left, coll.top);
            //previewRect->setOrigin(tile.Width/2, tile.Height/2);
            
            
                for (unsigned int i = 0; i < TileComp.Colliders.size(); i++)
                {
                    int TestTile = TileComp.Layout[tile.X + (TileComp.width * tile.Y)];
                    if (TestTile == TileComp.Colliders[i])
                    {
                        Renderer::RenderQueue.push(RenderItem(previewRect, 999999));
                    }

                }
                

            
        }
        int tileIndex = TileX + (TileComp.width * TileY);
        
    }
}
