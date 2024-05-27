import bpy
from typing import *

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

		self.materials = 0
		self.lights = 0
		self.meshes = 0
		self.curves = 0

		self.KL.append("ƒ⠀Header")
		self.KL.append(" render_step 7")
		self.KL.append(" render_step_stop 2")
		self.KL.append("~⠀Header")

		lights = []
		meshes = []
		curves = []
		desgraph = context.evaluated_depsgraph_get()
		for object_instance in desgraph.object_instances:
			object: bpy.types.Object = object_instance.object
			if object.get("KL", False):
				if object.type == "LIGHT":
					lights.append(object)
				if object.type == "MESH":
					meshes.append(object)
				if object.type == "CURVE":
					curves.append(object)

		self.parseCamera()

		self.KL.append(f"ƒ⠀Materials <{len(data.materials)}>")
		for material in data.materials:
			self.parseMaterial(material)
		self.KL.append("~⠀Materials")

		self.KL.append(f"ƒ⠀Lights <{len(lights)}>")
		for light in lights:
			self.parseLight(light)
		self.KL.append("~⠀Lights")

		self.KL.append(f"ƒ⠀Curves <{len(curves)}>")
		for curve in curves:
			self.parseCurve(curve)
		self.KL.append("~⠀Curves")
		
		self.KL.append(f"ƒ⠀Meshes <{len(meshes)}>")
		for mesh in meshes:
			self.parseMesh(mesh)
		self.KL.append("~⠀Meshes")

		print("\n".join(self.KL))
		open("D:/Kerzenlicht Renderer/Resources/Assets/Test.krz", "w").write("\n".join(self.KL))

	def parseCamera(self):
		if 'KL_Camera' in bpy.context.scene and bpy.context.scene.KL_Camera is not None:
			object: bpy.types.Object = bpy.context.scene.KL_Camera
			camera: bpy.types.Camera = object.data
			self.KL.append("ƒ⠀Camera")
			self.KL.append(f" Position: ( {-object.location.x} {object.location.z} {object.location.y} )")
			self.KL.append(f" Rotation: ( {90 - object.rotation_euler.x * RAD_DEG} {object.rotation_euler.z * RAD_DEG} {object.rotation_euler.y * RAD_DEG} )")
			self.KL.append(f" Lens: {camera.lens_unit}")
			if camera.lens_unit == "MILLIMETERS":
				self.KL.append(f" Focal-Length: {camera.lens/1000}")
				if camera.sensor_fit == "HORIZONTAL":
					self.KL.append(f" Sensor-Size: {camera.sensor_width/1000}")
				else:
					self.KL.append(f" Sensor-Size: {camera.sensor_height/1000}")
			else:
				self.KL.append(f" FOV: {camera.angle}")
			self.KL.append("~⠀Camera")

	def parseMaterial(self, material: bpy.types.Material):
		self.KL.append(f" ƒ⠀Material [{self.materials}] {material.name_full}")
		#for node in material.node_tree:
		#	node: bpy.types.ShaderNode = node
		#	node.bl_static_type
		self.KL.append(f" ~⠀Material [{self.materials}] {material.name_full}")
		self.materials += 1

	def parseLight(self, object: bpy.types.Object):
		light: bpy.types.Light = object.data

		self.KL.append(f" ƒ⠀Light::{light.type} [{self.lights}] {object.name_full}")

		if light.type == "SUN":
			light: bpy.types.SunLight = light
			self.KL.append(f"  Power: {light.energy}")
		elif light.type == "POINT":
			light: bpy.types.PointLight = light
			self.KL.append(f"  Pos: ( {object.location.x} {object.location.z} {object.location.y} )")
			self.KL.append(f"  Col: ( {light.color.r} {light.color.g} {light.color.b} )")
			self.KL.append(f"  Power: {light.energy}")
		elif light.type == "AREA":
			light: bpy.types.AreaLight = light
			self.KL.append(f"  {light.energy}")

		self.KL.append(f" ~⠀Light::{light.type} [{self.lights}] {object.name_full}")
		self.lights += 1

	def parseCurve(self, object: bpy.types.Object):
		curve: bpy.types.Curve = object.data
		curve_points = object.to_mesh()
		curve_pos  = curve_points.attributes.get('position')
		curve_rad  = curve_points.attributes.get('point_radius')
		curve_size = curve_points.attributes.get('spline_state')
		matrix = object.matrix_world.copy()

		spline_sizes = []
		spline_point_count = 0

		for size in curve_size.data:
			if size.value:
				spline_sizes.append(spline_point_count + 1)
				spline_point_count = 0
			else:
				spline_point_count += 1

		print(curve_rad.data)

		self.KL.append(f" ƒ⠀Curve [{self.curves}] {object.name_full}")
		self.KL.append(f"  ƒ⠀Splines <{len(curve.splines)}>")

		last_handle = 0
		for j, data_b in enumerate(spline_sizes):
			self.KL.append(f"   ƒ⠀Spline [{j}]")
			self.KL.append(f"    ƒ⠀Handles <{data_b}>")
			for k in range(last_handle, last_handle + data_b):
				pos = matrix @ curve_pos.data[k].vector
				self.KL.append(f"     {k - last_handle} "+ "{" +f" ( {-pos.x} {pos.z} {pos.y} ) | {curve_rad.data[k].value}" + " }")

			self.KL.append(f"    ~⠀Handles")
			self.KL.append(f"   ~⠀Spline [{j}]")
			last_handle += data_b

		self.KL.append(f"  ~⠀Splines")

		self.KL.append(f" ~⠀Curve [{self.curves}] {object.name_full}")
		self.curves += 1
		object.to_mesh_clear()

	def parseMesh(self, object: bpy.types.Object):
		mesh: bpy.types.Mesh = object.data
		matrix = object.matrix_world.copy()

		self.KL.append(f" ƒ⠀Mesh [{self.meshes}] {object.name_full}")

		self.KL.append(f"  ƒ⠀Materials <{len(object.material_slots)}>")

		for i, material in enumerate(object.material_slots):
			material: bpy.types.MaterialSlot = material
			self.KL.append(f"   {i} {material.material.name_full}")

		self.KL.append("  ~⠀Materials")

		self.KL.append(f"  ƒ⠀Vertices <{len(mesh.vertices)}>")

		for j, data_b in enumerate(mesh.vertices):
			vert: bpy.types.MeshVertex = data_b
			vert_pos = matrix @ vert.co
			self.KL.append(f"   {j} " + "{" + f" ( {-vert_pos.x} {vert_pos.z} {vert_pos.y} ) | [ " + " ".join([f"( {group} : {weight} )" for group, weight in vert.groups.items()]) + " ] }")

		self.KL.append("  ~⠀Vertices")
		self.KL.append(f"  ƒ⠀Faces <{len(mesh.polygons)}>")

		uv_layer = mesh.uv_layers.active.data

		for j, data_b in enumerate(mesh.polygons):
			poly: bpy.types.MeshPolygon = data_b
			self.KL.append(f"   {j} " + "{ [ " + " ".join([str(index) for index in poly.vertices]) + " ] | [ " + " ".join([f"( {-nor.x} {nor.z} {nor.y} )" for nor in [matrix @ mesh.vertices[index].normal for index in poly.vertices]]) + " ] | [ " + " ".join([f"( {uv_layer[loop_index].uv.x} {uv_layer[loop_index].uv.y} )" for loop_index in range(poly.loop_start, poly.loop_start + poly.loop_total)]) + " ] | " +  f"[{poly.material_index}] " +" }")

		self.KL.append("  ~⠀Faces")

		self.KL.append(f" ~⠀Mesh [{self.meshes}] {object.name_full}")
		self.meshes += 1

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
		box.prop(context.scene, "KL_Camera", text = "Render Camera")
		for object in context.visible_objects:
			object: bpy.types.Object = object
			if object.type == "MESH" or object.type == "LIGHT" or object.type == "CURVE":
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