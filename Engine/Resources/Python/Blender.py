import bpy, pip
import mathutils as mu
from math import *
from typing import *

pip.main(["install", "pandas"])
pip.main(["install", "tabulate"])

RAD_DEG = 57.29577951308232
MATRIX = mu.Euler((radians(-90), radians(0), radians(0))).to_matrix().to_4x4()
matrix_3x3 = MATRIX.to_3x3()
inverse_3x3 = matrix_3x3.inverted()
NORMAL_MATRIX = inverse_3x3.transposed()

def to_ptr(data: str) -> int:
	hash_value = hash(data)
	positive_hash_value = abs(hash_value)
	max_13_digit_number = 10**13 - 1
	scaled_hash_value = positive_hash_value % max_13_digit_number
	return scaled_hash_value

class Kerzenlicht_Bridge(bpy.types.Operator):
	bl_idname = "kerzenlicht.parse"
	bl_label = "KL File"
	bl_options = {'REGISTER'}

	filepath: bpy.props.StringProperty(subtype="FILE_PATH")
	pointer_map = {}

	def execute(self, context: bpy.types.Context):
		self.parseKerzenlicht(context, context.blend_data)
		return {"FINISHED"}

	def invoke(self, context, event):
		context.window_manager.fileselect_add(self)
		return {'RUNNING_MODAL'}
	
	def parseKerzenlicht(self, context: bpy.types.Context, data: bpy.types.BlendData):
		self.KL: List[str] = []

		depsgraph = bpy.context.evaluated_depsgraph_get()
		objects: List[bpy.types.Object] = []
		for object in data.objects:
			object: bpy.types.Object
			if object.get("KL", False):
				if object.type == "MESH":
					objects.append(object.evaluated_get(depsgraph))

		self.KL.append("┌Header")
		self.KL.append(" Version 1.0.0")
		self.KL.append("└Header")

		self.KL.append(f"┌Node-Trees( 0 )")
		self.KL.append(f"└Node-Trees")

		self.KL.append(f"┌Shaders( 0 )")
		self.KL.append(f"└Shaders")

		self.KL.append(f"┌Data( {len(objects)} )")
		self.parseData(objects)
		self.KL.append(f"└Data")

		self.KL.append(f"┌Objects( {len(objects)} )")
		build = self.parseObjects(objects)
		self.KL.append(f"└Objects")

		self.KL.append(f"┌Scenes( 1 )")
		self.KL.append(f" ┌Scene [ 0 ]")
		self.KL.append(f"  * {to_ptr('scene'+bpy.context.scene.name_full)}")
		self.KL.append(f"  ┌Objects")
		for object in objects:
			self.KL.append(f"  * {to_ptr('object'+object.name_full)}")
		self.KL.append(f"  └Objects")
		self.KL.append(f" └Scene")
		self.KL.append(f"└Scenes")

		self.KL.append(f"┌Build-Steps")
		self.KL.append(f" Active-Scene * {to_ptr('scene'+bpy.context.scene.name_full)}")
		self.KL.append(f" ┌Object-Group")
		self.KL.append(f" └Object-Group")
		self.KL.append(f" ┌Object-Data")
		for object, data in build.items():
			self.KL.append(f"  * {object} * {data}")
		self.KL.append(f" └Object-Data")
		self.KL.append(f" ┌Object-Node")
		self.KL.append(f" └Object-Node")
		self.KL.append(f" ┌Node-Pointer")
		self.KL.append(f" └Node-Pointer")
		self.KL.append(f"└Build-Steps")

		#print("\n".join(self.KL))
		open(self.filepath, "w").write("\n".join(self.KL))

	def parseData(self, objects: List[bpy.types.Object]):
		i = 0
		for object in objects:
			if object.type == "MESH":
				ptr = to_ptr("data"+object.data.name_full)
				self.parseMesh(object, object.data, i, ptr)
				i += 1

	def parseMesh(self, object: bpy.types.Object, data: bpy.types.Mesh, i: int, ptr: str):
		self.KL.append(f" ┌Data [ {i} ] {data.name_full}")

		self.KL.append(f"  * {ptr}")
		self.KL.append("  Type MESH")
		self.KL.append("  ┌Mesh")
		self.KL.append(f"   ┌Vertices( {len(data.vertices)} )")
		for j, vert in enumerate(data.vertices):
			vert: bpy.types.MeshVertex
			vert_pos: mu.Vector = MATRIX @ vert.co
			self.KL.append(f"    {j} ( {vert_pos.x} {vert_pos.y} {vert_pos.z} )")

		self.KL.append("   └Vertices")
		self.KL.append(f"   ┌Vertex-Groups( {len(object.vertex_groups)} )")

		for j, vertex_group in enumerate(object.vertex_groups):
			vertices = []
			vertex_group: bpy.types.VertexGroup = vertex_group
			self.KL.append(f"    ┌Vertex-Group [ {j} ] {vertex_group.name}")
			for k, vert in enumerate(data.vertices):
				vert: bpy.types.MeshVertex
				for group in vert.groups:
					group: bpy.types.VertexGroupElement = group
					if group.group == vertex_group.index:
						vertices.append(str(k))
			self.KL.append(f"     [ {' '.join(vertices)} ]")
			self.KL.append("    └Vertex-Group")

		self.KL.append("   └Vertex-Groups")
		self.KL.append(f"   ┌Faces( {len(data.polygons)} )")

		for j, data_b in enumerate(data.polygons):
			poly: bpy.types.MeshPolygon = data_b
			vertices = " ".join([str(index) for index in poly.vertices])
			self.KL.append(f"    {j} {len(poly.vertices)} [ {vertices} ]")

		self.KL.append("   └Faces")
		self.KL.append(f"   ┌Normals( {len(data.polygons)} )")

		for j, poly in enumerate(data.polygons):
			poly: bpy.types.MeshPolygon = poly
			normals = " ".join([f"( {nor.x} {nor.y} {nor.z} )" for nor in [NORMAL_MATRIX @ normal for normal in [data.vertices[index].normal for index in poly.vertices]]])
			self.KL.append(f"    {j} {len(poly.vertices)} {normals}")

		self.KL.append("   └Normals")
		self.KL.append(f"   ┌UVs( {len(data.polygons)} )")

		uv_layer = data.uv_layers.active.data
		for j, data_b in enumerate(data.polygons):
			poly: bpy.types.MeshPolygon = data_b
			uvs = " ".join([f"( {uv_layer[loop_index].uv.x} {uv_layer[loop_index].uv.y} )" for loop_index in range(poly.loop_start, poly.loop_start + poly.loop_total)])
			self.KL.append(f"    {j} {len(poly.vertices)} {uvs}")

		self.KL.append("   └UVs")
		self.KL.append("  └Mesh")
		self.KL.append(f" └Data")

	def parseObjects(self, objects: List[bpy.types.Object]):
		object_map = {}
		for i, object in enumerate(objects):
			ptr = to_ptr("object"+object.name_full)
			self.parseObject(object, i, ptr)
			object_map[ptr] = to_ptr("data"+object.data.name_full)
		return object_map

	def parseObject(self, object: bpy.types.Object, i: int, ptr: str):
		self.KL.append(f" ┌Object [ {i} ] {object.name_full}")
		self.KL.append(f"  * {ptr}")
		pos: mu.Vector = object.location
		rot: mu.Euler = object.rotation_euler
		scale: mu.Vector = object.scale
		self.KL.append(f"  Position ( {pos.x} {pos.z} {pos.y} )")
		self.KL.append(f"  Rotation ( {rot.x * RAD_DEG} {rot.z * RAD_DEG} {rot.y * RAD_DEG} )")
		self.KL.append(f"  Scale    ( {scale.x} {scale.y} {scale.z} )")
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