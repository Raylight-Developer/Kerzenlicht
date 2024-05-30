from typing import List, Tuple, Dict
# Module for Easy Typing and Auto Completion

# QT Items
class Application: pass
class Button: pass
class Dock: pass
class Drop_Down: pass
class File_Browser: pass
class Grid_Layout: pass
class Graphics_View: pass
class Label: pass
class Linear_Layout: pass
class Linear_Contents: pass
class Main_Window: pass
class Menu: pass
class Option: pass
class Scroll_Area: pass
class Slider: pass
class Splitter: pass
class Spreadsheet: pass
class Spreadsheet_Item: pass
class Tree: pass
class Tree_Item: pass
class Text_Input: pass
class Text_Stream: pass
class Toggle: pass
class ToolBar: pass
class Value_Input: pass
class Widget: pass

# Workspaces
class Main_Window(Main_Window):
	app: Application
	file: 'File'
	log: 'Lace'
	workspaces: Dict[str, 'Workspace_Manager']
	def __init__(self, app: Application): pass

class Workspace_Manager(Linear_Contents):
	pass

# Worker classes

# Core classes
class File():
	version: str
	def __init__(): pass
	def f_loadFile(file_path: str): pass
	def f_saveFile(file_path: str): pass
	def f_printFile(): pass

class Lace():
	def __init__(): pass
	def str() -> str: pass

# Core structures
class Vec3():
	def __init__(): pass
class Vec2():
	def __init__(): pass