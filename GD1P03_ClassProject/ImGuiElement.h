#pragma once
#include "imgui.h"
#include "imgui_stdlib.h"
#include <vector>
#include <string>
//#include "Global.h"

class ImGuiElement
{
public:

	ImGuiElement();
	~ImGuiElement();

	bool open = true;
	std::string label = "Element";

	void CreateLabelID(std::string &label);
	
	static inline int elements;
	virtual void OnImGuiRender();
};

