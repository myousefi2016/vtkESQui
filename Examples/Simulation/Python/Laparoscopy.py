
import vtk
import vtkesqui
import time

def main():

	fn0 = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Tools/Grasper/stick.vtp";
	fn0c = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Tools/Grasper/stick_col.vtp";
	fn1 = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Tools/Grasper/lever_l.vtp";
	fn1c = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Tools/Grasper/lever_l_col.vtp";
	fn2 = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Tools/Grasper/lever_r.vtp";
	fn2c = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Tools/Grasper/lever_r_col.vtp";
	fn0t = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Textures/metal.jpg";
	fn3 = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Tools/Probe/tip.vtp";
	fn3c = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Tools/Probe/tip_col.vtp";
	fn4 = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Organs/ball.vtp";
	fn4c = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Organs/ball_col.vtp";
	fn4d = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Organs/ball_def_c10.vtp";
	fn4t = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Textures/liver.jpg";
	fn5 = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Organs/cavity.vtp";
	fn5c = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Organs/cavity_col.vtp"
	fn5d = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Organs/cavity_def_c10.vtp"
	fn5t = "/Users/davidcastillobolado/vtkESQuiData/Scenario/Textures/intestine2.jpg";

	# Left Tool (Probe)
	# Visualization model
	vis_stick_l = vtkesqui.vtkVisualizationModel()
	vis_stick_l.SetName("vis_stick_l")
	vis_stick_l.SetFileName(fn0)
	vis_stick_l.SetTextureFileName(fn0t)
	vis_stick_l.SetOpacity(1.0)
	vis_stick_l.SetColor(1.0, 1.0, 1.0)
	
	# Collision model
	col_stick_l = vtkesqui.vtkCollisionModel()
	col_stick_l.SetName("col_stick_l_vtkbioeng")
	col_stick_l.SetFileName(fn0c)
	col_stick_l.SetOpacity(0.0)
	col_stick_l.SetColor(1.0, 1.0, 1.0)
	
	# First tool element (stick_l)
	stick_l = vtkesqui.vtkScenarioElement()
	stick_l.SetName("stick_l")
	stick_l.SetPosition(-4.0, 0.0, 0.0)
	stick_l.SetOrientation(0, 0, 20)
	stick_l.SetOrigin(0,0,6)
	stick_l.SetVisualizationModel(vis_stick_l)
	stick_l.SetCollisionModel(col_stick_l)
	
	# Visualization model
	vis_tip_l = vtkesqui.vtkVisualizationModel()
	vis_tip_l.SetName("vis_tip_l")
	vis_tip_l.SetFileName(fn3)
	vis_tip_l.SetTextureFileName(fn0t)
	vis_tip_l.SetOpacity(1.0)
	vis_tip_l.SetColor(1.0, 0.0, 0.0)
	
	# Collision model
	col_tip_l = vtkesqui.vtkCollisionModel()
	col_tip_l.SetName("col_tip_l_vtkbioeng")
	col_tip_l.SetFileName(fn3c)
	col_tip_l.SetOpacity(0.0)
	col_tip_l.SetColor(1.0, 1.0, 1.0)
	
	# Second tool element (tip_l)
	tip_l = vtkesqui.vtkScenarioElement()
	tip_l.SetName("tip_l")
	tip_l.SetPosition(-4.0, 0.0, 0.0)
	tip_l.SetOrientation(0, 0, 20)
	tip_l.SetOrigin(0,0,6)
	tip_l.SetVisualizationModel(vis_tip_l)
	tip_l.SetCollisionModel(col_tip_l)
	
	probe_l = vtkesqui.vtkToolProbe()
	probe_l.AddElement(stick_l)
	probe_l.AddElement(tip_l)
	
	# Right tool (Grasper)
	# Visualization model
	vis_stick_r = vtkesqui.vtkVisualizationModel()
	vis_stick_r.SetName("vis_stick_r")
	vis_stick_r.SetFileName(fn0)
	vis_stick_r.SetTextureFileName(fn0t)
	vis_stick_r.SetOpacity(1.0)
	vis_stick_r.SetColor(1.0, 1.0, 1.0)
	
	# Collision model
	col_stick_r = vtkesqui.vtkCollisionModel()
	col_stick_r.SetName("col_stick_r_vtkbioeng")
	col_stick_r.SetFileName(fn0c)
	col_stick_r.SetOpacity(0.0)
	col_stick_r.SetColor(1.0, 1.0, 1.0)
	
	# First tool element (stick_r)
	stick_r = vtkesqui.vtkScenarioElement()
	stick_r.SetName("stick_r")
	stick_r.SetPosition(4.0, 0.0, 0.0)
	stick_r.SetOrientation(0, 0, -20)
	stick_r.SetOrigin(0,0,6)
	stick_r.SetVisualizationModel(vis_stick_r)
	#stick_r.SetCollisionModel(col_stick_r)
	
	# Visualization model
	vis_l_lever_r = vtkesqui.vtkVisualizationModel()
	vis_l_lever_r.SetName("vis_l_lever_r")
	vis_l_lever_r.SetFileName(fn1)
	vis_l_lever_r.SetTextureFileName(fn0t)
	vis_l_lever_r.SetOpacity(1.0)
	vis_l_lever_r.SetColor(0.0, 1.0, 0.0)
	
	# Collision model
	col_l_lever_r = vtkesqui.vtkCollisionModel()
	col_l_lever_r.SetName("col_l_lever_r_vtkbioeng")
	col_l_lever_r.SetFileName(fn1c)
	col_l_lever_r.SetOpacity(0.0)
	col_l_lever_r.SetColor(1.0, 1.0, 1.0)
	
	# Second tool element (l_lever_r)
	l_lever_r = vtkesqui.vtkScenarioElement()
	l_lever_r.SetName("l_lever_r")
	l_lever_r.SetPosition(4.0, 0.0, 0.0)
	l_lever_r.SetOrientation(0, 0, -20)
	l_lever_r.SetOrigin(0,0,6)
	l_lever_r.SetVisualizationModel(vis_l_lever_r)
	l_lever_r.SetCollisionModel(col_l_lever_r)
	
	# Visualization model
	vis_r_lever_r = vtkesqui.vtkVisualizationModel()
	vis_r_lever_r.SetName("vis_r_lever_r")
	vis_r_lever_r.SetFileName(fn2)
	vis_r_lever_r.SetTextureFileName(fn0t)
	vis_r_lever_r.SetOpacity(1.0)
	vis_r_lever_r.SetColor(0.0, 1.0, 0.0)
	
	# Collision model
	col_r_lever_r = vtkesqui.vtkCollisionModel()
	col_r_lever_r.SetName("col_r_lever_r_vtkbioeng")
	col_r_lever_r.SetFileName(fn2c)
	col_r_lever_r.SetOpacity(0.0)
	col_r_lever_r.SetColor(1.0, 1.0, 1.0)
	
	# Second tool element (r_lever_r)
	r_lever_r = vtkesqui.vtkScenarioElement()
	r_lever_r.SetName("r_lever_r")
	r_lever_r.SetPosition(4.0, 0.0, 0.0)
	r_lever_r.SetOrientation(0, 0, -20)
	r_lever_r.SetOrigin(0,0,6)
	r_lever_r.SetVisualizationModel(vis_r_lever_r)
	r_lever_r.SetCollisionModel(col_r_lever_r)
	
	grasper_r = vtkesqui.vtkToolGrasper()
	grasper_r.SetStick(stick_r)
	grasper_r.SetLeftLever(l_lever_r)
	grasper_r.SetRightLever(r_lever_r)
	
	# Add an Organ
	# Visualization model
	vis_ball = vtkesqui.vtkVisualizationModel()
	vis_ball.SetName("vis_ball")
	vis_ball.SetFileName(fn4)
	vis_ball.SetTextureFileName(fn4t)
	vis_ball.SetOpacity(1.0)
	
	# Collision model
	col_ball = vtkesqui.vtkCollisionModel()
	col_ball.SetName("col_ball_vtkbioeng")
	col_ball.SetFileName(fn4c)
	col_ball.SetOpacity(0.0)
	
	# Deformation model
	def_ball = vtkesqui.vtkParticleSpringSystemInterface()
	def_ball.SetName("def_ball_particle")
	def_ball.SetFileName(fn4d)
	def_ball.SetOpacity(0.0)
	def_ball.SetSpring(150)
	def_ball.SetDistance(1.0)
	def_ball.SetDamping(3)
	def_ball.SetMass(.1)
	def_ball.SetTimeStep(0.001)
	
	# Organ element (ball)
	e_ball = vtkesqui.vtkScenarioElement()
	e_ball.SetName("ball")
	e_ball.SetPosition(0.0, 0.0, -2.0)
	e_ball.SetOrientation(0, 0, 0)
	e_ball.SetVisualizationModel(vis_ball)
	e_ball.SetCollisionModel(col_ball)
	e_ball.SetDeformationModel(def_ball)
	
	ball = vtkesqui.vtkOrgan()
	ball.AddElement(e_ball)
	
	# Add an Organ (Cavity)
	# Visualization model
	vis_cavity = vtkesqui.vtkVisualizationModel()
	vis_cavity.SetName("vis_cavity")
	vis_cavity.SetFileName(fn5)
	vis_cavity.SetTextureFileName(fn5t)
	vis_cavity.SetOpacity(1.0)

	# Collision model
	col_cavity = vtkesqui.vtkCollisionModel()
	col_cavity.SetName("col_cavity_vtkbioeng")
	col_cavity.SetFileName(fn5c)
	col_cavity.SetOpacity(0.0)

	# Deformation model
	def_cavity = vtkesqui.vtkParticleSpringSystemInterface()
	def_cavity.SetName("def_cavity_particle")
	def_cavity.SetFileName(fn5d)
	def_cavity.SetOpacity(0.0)
	def_cavity.SetSpring(150)
	def_cavity.SetDistance(1.0)
	def_cavity.SetDamping(3)
	def_cavity.SetMass(.1)
	def_cavity.SetTimeStep(0.001)
	
	# Organ element (cavity)
	e_cavity = vtkesqui.vtkScenarioElement()
	e_cavity.SetName("cavity")
	e_cavity.SetPosition(0.0, 0.0, -2.0)
	e_cavity.SetOrientation(-30, 0, 0)
	e_cavity.SetVisualizationModel(vis_cavity)
	e_cavity.SetCollisionModel(col_cavity)
	e_cavity.SetDeformationModel(def_cavity)
	
	cavity = vtkesqui.vtkOrgan()
	cavity.AddElement(e_cavity)
	
	scenario = vtkesqui.vtkScenario()
	scenario.SetName("vtkESQui - Laparoscopy")
	
	scenario.AddObject(probe_l)
	scenario.AddObject(grasper_r)
	scenario.AddObject(ball)
	scenario.AddObject(cavity)
	
	# Scenario camera
	camera = vtk.vtkCamera()
	camera.SetPosition(0, 0, 6)
	camera.SetFocalPoint(0, 0, -6)
	camera.Yaw(0)
	camera.Elevation(20)
	camera.Pitch(-15)
	camera.ParallelProjectionOff()
	camera.SetViewAngle(70)
	camera.Dolly(1.2)
	scenario.SetCamera(camera)
	
	#/********** Lights	**********/
	headLight = vtk.vtkLight()
	headLight.SetLightTypeToHeadlight()
	headLight.PositionalOn()
	headLight.SetIntensity(0.8)
	headLight.SetConeAngle(60)
	scenario.AddLight(headLight)
	
	ambientLight = vtk.vtkLight()
	ambientLight.SetLightTypeToSceneLight()
	ambientLight.PositionalOff()
	ambientLight.SetPosition(0,1,1)
	ambientLight.SetIntensity(0.5)
	scenario.AddLight(ambientLight)

	style = vtkesqui.vtkDefaultInteractorStyle()
	style.SetScenario(scenario)
	
	simulation = vtkesqui.vtkSimulation()
	simulation.SetScenario(scenario)
	simulation.SetInteractorStyle(style)
	simulation.SetRenderTimerRate(50)
	simulation.SetSimulationTimerRate(50)
	simulation.SetInteractionTimerRate(50)
	simulation.InteractionOn()
	simulation.CollisionOn()
	simulation.Initialize()
	
	simulation.Run()
	
if __name__ == '__main__':
	main()
