#!/bin/sh
# Scenario.tcl \
exec tclsh "$0" ${1+"$@"}

#proc UpdateSimulation {} {
#	global Simulation
#	global Scenario
#	
#	Simulation Update 
#}

#proc RunSimulation {} {
#		UpdateSimulation
#    after idle {after 0 RunSimulation};  # "Immediately" after idle
#    after 10;  # Sleep for 10 ms to simulate a heavily loaded machine
#}

proc CreateSimulation {} {
	
	global Scenario
	global Simulation
	global SimulationManager
	global renWin
	
	puts "Creating simulation..."
	vtkRenderer ren1
	vtkRenderWindow renWin
	renWin AddRenderer ren1
	
	vtkRenderWindowInteractor iren
	iren SetRenderWindow renWin

	set vtkw [vtkTkRenderWidget .reader.window.c.ren  -width 840 -height 490 -rw renWin]
	
	#
	# Setup Tk bindings and VTK observers for that widget.
	#
	::vtk::bind_tk_render_widget $vtkw
	#pack the window into the frame

	pack $vtkw

	renWin Render

	vtkSimulation Simulation
	vtkSimulationManager SimulationManager
	vtkScenario Scenario
	Scenario SetRenderWindow renWin
	
	SimulationManager SetCollisionDetectionLibrary "vtkbioeng"
	SimulationManager SetScenario Scenario
	SimulationManager Init

	Simulation SetSimulationManager SimulationManager
	
	puts "Simulation has been created."
}

proc ImportSimulation {path name} {
	
	CreateSimulation
	
	puts "Importing simulation from SRML file..."
	vtkSRMLImporter SRMLImporter
	SRMLImporter SetDataPath $path
	SRMLImporter SetFileName $name
	SRMLImporter SetSimulation Simulation
	SRMLImporter Read
	
	puts "Simulation has been correctly imported..."
	
	AddEvents
	
	Simulation Run
	
}