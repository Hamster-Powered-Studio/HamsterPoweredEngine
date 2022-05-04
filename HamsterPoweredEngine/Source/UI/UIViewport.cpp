#include "UI/UIViewport.h"
#include "Engine/Global.h"
#include "Engine/Renderer.h"
#include <iostream>

#include "UIHierarchy.h"
#include "Components/Components.h"

UIViewport::UIViewport()
{
	CreateLabelID(label);
	
	
}

void UIViewport::OnImGuiRender()
{

	ImGuiElement::OnImGuiRender();

	ImGui::Begin(label.c_str(), &open);

	if (ImGui::GetWindowSize().x != size.x || ImGui::GetWindowSize().y != size.y)
	{
		global::Game->ResizeGameView();
	}

	size = ImGui::GetWindowSize();

	ImVec2 image_pos = ImGui::GetCursorScreenPos();
	ImGui::Image(*Renderer::GetView());
	
	
	float MPosX = (ImGui::GetMousePos().x -image_pos.x);
	float MPosY = (ImGui::GetMousePos().y  -image_pos.y);
	
	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		
		sf::Vector2i clickCoords = {(int)MPosX, (int)MPosY};
		sf::Vector2f worldCoords = Renderer::GetView()->mapPixelToCoords(clickCoords, Renderer::GetView()->getView());

		entt::registry& Registry = global::Game->currentScene->Reg();
		auto maps = Registry.view<TileMapComponent, TransformComponent>();
		{
			for (auto actor : maps)
			{
				if (global::Game->hier->m_SelectionContext == actor)
				{
					
					const auto& [transform, tm] = maps.get<TransformComponent, TileMapComponent>(actor);

					//Create a bounding box based on the dimensions of the tilemap
					sf::Vector2f boundsSize((float)tm.width * (float)tm.tileWidth, (float)tm.height * (float)tm.tileHeight);
					sf::Vector2f boundsPos(transform.Transform.Pos.x - (boundsSize.x / 2), transform.Transform.Pos.y - (boundsSize.y / 2));
					sf::FloatRect bounds(boundsPos, boundsSize);

					
					if(bounds.contains(worldCoords) && tm.Visible)
					{
						std::cout << "inside";
						//Create a new view focusing fully on the tilemap, and use it to convert the coordinates to its own space.
						sf::View MapView(bounds);
						//sf::Vector2i pos = Renderer::GetView()->mapCoordsToPixel(worldCoords, MapView);
						sf::Vector2f pos = worldCoords - boundsPos;
						//pos.y *=2;
						std::cout << pos.x << " " << pos.y << std::endl;

						int TileX = (pos.x / (tm.tileWidth));
						int TileY = (pos.y / (tm.tileHeight));
						std::cout << TileX << " " << TileY << std::endl;

						int tileToChange;
						tileToChange = TileX + (tm.width * TileY);
						tm.Layout[tileToChange] = tm.SelectedTile;
						tm.UpdateTexture(tileToChange);
						//tm.Load();
					}
					
				}
			}
		}

	}


	
	ImGui::End();
	if (!open) global::Game->Destroy(this);

}