import bpy, pip
from typing import *

pip.main(["install", "pandas"])
pip.main(["install", "tabulate"])

RAD_DEG = 57.29577951308232

class Kerzenlicht_Bridge(bpy.types.Operator):
	bl_idname = "kerzenlicht.parse"
	bl_label = "Render KL"
	bl_options = {'REGISTER'}

	def execute(self, context: bpy.types.Context):
		self.parseKerzenlicht(context, context.blend_data)
		return {"FINISHED"}

	def parseKerzenlicht(self, context: bpy.types.Context, data: bpy.types.BlendData):
		self.KL: List[str] = []

		objects: List[bpy.types.Object] = []
		for object in data.objects:
			if object.get("KL", False):
				if object.type == "MESH":
					objects.append(object)

		self.KL.append("┌Header")
		self.KL.append(" Version 1.0.0")
		self.KL.append("└Header")

		self.KL.append(f"┌Materials( 0 )")
		self.KL.append(f"└Materials")
		
		self.KL.append(f"┌Node-Trees( 0 )")
		self.KL.append(f"└Node-Trees")

		self.KL.append(f"┌Data( {len(objects)} )")
		self.parseData(data)
		self.KL.append(f"└Data")

		self.KL.append(f"┌└Objects( {len(objects)} )")
		build = self.parseObjects(data)
		self.KL.append(f"└└Objects")

		self.KL.append(f"┌Scenes( 1 )")
		self.KL.append(f" ┌Scene [ 0 ]")
		self.KL.append(f"  ┌Objects")
		for i, object in enumerate(objects):
			self.KL.append(f"  {i} {object.name_full}")
		self.KL.append(f"  └Objects")
		self.KL.append(f" └Scene")
		self.KL.append(f"└Scenes")

		self.KL.append(f"┌Build-Steps")
		self.KL.append(f" Active-Scene 0")
		self.KL.append(f" ┌Object-Group")
		self.KL.append(f" └Object-Group")
		self.KL.append(f" ┌Object-Data")
		for object, data in build.items():
			self.KL.append(f"  {object} {data}")
		self.KL.append(f" └Object-Data")
		self.KL.append(f" ┌Object-Node")
		self.KL.append(f" └Object-Node")
		self.KL.append(f"└Build-Steps")

		#print("\n".join(self.KL))
		open("D:/Kerzenlicht Renderer/Runtime/Resources/Ganyu.krz", "w").write("\n".join(self.KL))

	def parseData(self, data: bpy.types.BlendData):
		i = 0
		for object in data.objects:
			if object.get("KL", False):
				if object.type == "MESH":
					self.parseMesh(object.data, i)
					i += 1

	def parseMesh(self, data: bpy.types.Mesh, i: int):
		self.KL.append(f" ┌Data :: Mesh [ {i} ] {data.name_full}")

		self.KL.append(f"  ┌Vertices( {len(data.vertices)} )")

		for j, data_b in enumerate(data.vertices):
			vert: bpy.types.MeshVertex = data_b
			self.KL.append(f"   {j} {-vert.co.x} {vert.co.z} {vert.co.y}")

		self.KL.append("  └Vertices")
		self.KL.append(f"  ┌Normals( {len(data.polygons)} )")

		for j, data_b in enumerate(data.polygons):
			poly: bpy.types.MeshPolygon = data_b
			normals = " ".join([f"{-nor.x} {nor.z} {nor.y}" for nor in [data.vertices[index].normal for index in poly.vertices]])
			self.KL.append(f"   {j} {normals}")

		self.KL.append("  └Normals")
		self.KL.append(f"  ┌UVs( {len(data.polygons)} )")

		uv_layer = data.uv_layers.active.data
		for j, data_b in enumerate(data.polygons):
			poly: bpy.types.MeshPolygon = data_b
			uvs = " ".join([f"{uv_layer[loop_index].uv.x} {uv_layer[loop_index].uv.y}" for loop_index in range(poly.loop_start, poly.loop_start + poly.loop_total)])
			self.KL.append(f"   {j} {uvs}")

		self.KL.append("  └UVs")
		self.KL.append(f"  ┌Faces( {len(data.polygons)} )")

		for j, data_b in enumerate(data.polygons):
			poly: bpy.types.MeshPolygon = data_b
			vertices = " ".join([str(index) for index in poly.vertices])
			self.KL.append(f"   {j} {len(poly.vertices)} {vertices}")

		self.KL.append("  └Faces")

		self.KL.append(f" └Data :: Mesh")

	def parseObjects(self, data: bpy.types.BlendData):
		objects = {}
		for i, object in enumerate(data.objects):
			if object.get("KL", False):
				self.parseObject(object, i)
				objects[i] = i
		return objects

	def parseObject(self, object: bpy.types.Object, i):
		self.KL.append(f" ┌Object [ {i} ] {object.name_full}")
		self.KL.append(f"  Position {-object.location.x} {object.location.z} {object.location.y}")
		self.KL.append(f"  Rotation {90 - object.rotation_euler.x * RAD_DEG} {object.rotation_euler.z * RAD_DEG} {object.rotation_euler.y * RAD_DEG}")
		self.KL.append(f"  Scale {object.scale.x} {object.scale.z} {object.scale.y}")
		self.KL.append(f"  Type Mesh")
		self.KL.append(f" └Object")

class Kerzenlicht_Interface(bpy.types.Panel):
	bl_label = "Kerzenlicht"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "scene"

	def draw(self, context):
		row = self.layout.row()
		row.operator("kerzenlicht.parse", text = "KL")
		row = self.layout.row()
		row.operator("wm.toggle_system_console", icon = 'CONSOLE')
		box = self.layout.box()
		#box.prop(context.scene, "KL_Camera", text = "Render Camera")
		for object in context.visible_objects:
			object: bpy.types.Object = object
			if object.type == "MESH":# or object.type == "LIGHT" or object.type == "CURVE":
				box.prop(object, "KL", text = object.name, toggle = True)

class Kerzenlicht_Console_Toggle(bpy.types.Operator):
	bl_idname = "wm.toggle_system_console"
	bl_label = "Toggle Console"

	def execute(self, context: bpy.types.Context):
		bpy.ops.wm.console_toggle()
		return {"FINISHED"}

Classes = [
	Kerzenlicht_Console_Toggle,
	Kerzenlicht_Interface,
	Kerzenlicht_Bridge
]

def register():
	bpy.types.Object.KL = bpy.props.BoolProperty(name = "KL include", default = False)
	bpy.types.Scene.KL_Camera = bpy.props.PointerProperty(name = "Render Camera", type = bpy.types.Object, description = "Pointer to an object")
	for Class in Classes:
		bpy.utils.register_class(Class)

def unregister():
	del bpy.types.Object.KL
	del bpy.types.Scene.KL_Camera
	for Class in Classes:
		bpy.utils.unregister_class(Class)

if __name__ == "__main__":
	register()