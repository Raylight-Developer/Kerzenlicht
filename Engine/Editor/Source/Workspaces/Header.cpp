#include "Workspaces/Header.hpp"

#include "Main_Window.hpp"

GUI::WORKSPACE::Workspace_Header::Workspace_Header(Main_Window* parent) :
	GUI::ToolBar(parent),
	parent(parent)
{
}