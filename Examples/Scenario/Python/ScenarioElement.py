#!/usr/bin/env python

import sys
import vtk
import vtkesqui

fn ="/home/jballesteros/Workspace/data/vtkESQuiData/Scenario/Tools/Probe/stick.vtp";
cfn ="/home/jballesteros/Workspace/data/vtkESQuiData/Scenario/Tools/Probe/stick_col.vtp";
tfn ="/home/jballesteros/Workspace/data/vtkESQuiData/Scenario/Textures/steel.jpg";

fnb ="/home/jballesteros/Workspace/data/vtkESQuiData/Scenario/Tools/Probe/tip.vtp";
cfnb ="/home/jballesteros/Workspace/data/vtkESQuiData/Scenario/Tools/Probe/tip_col.vtp";
tfnb ="/home/jballesteros/Workspace/data/vtkESQuiData/Scenario/Textures/metal.jpg";

# Generate scenario element

sphere = vtk.vtkSphereSource()
sphere.Update()

input = sphere.GetOutput()

decimate = vtk.vtkDecimatePro()
decimate.SetInput(input)
decimate.SetTargetReduction(.50)
decimate.Update()

input_col = decimate.GetOutput()

vis = vtkesqui.vtkVisualizationModel();
vis.SetName("stick_vis");
vis.SetInput(input);
vis.SetTextureFileName(tfn);
vis.SetOpacity(1.0);
vis.SetColor(1.0, 1.0, 1.0);

col = vtkesqui.vtkCollisionModel();
col.SetName("vtkbioeng");
col.SetInput(input_col);
col.SetOpacity(0.5);
col.SetColor(0.0, 0.0, 1.0);

stick = vtkesqui.vtkScenarioElement();
stick.SetId(0);
stick.SetName("stick");
stick.SetOrigin(0.0, 0.0, 0.0);
stick.SetPosition(3.0, 0.0, 0.0);
stick.SetOrientation(0, 0, -20);
stick.SetVisualizationModel(vis);
stick.SetCollisionModel(col);

stick.Update()

ren = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)

ren.AddActor(vis.GetActor())
ren.AddActor(col.GetActor())

ren.SetBackground(1.0, 1.0, 1.0)
renWin.SetSize(500, 500)

ren.ResetCamera()
ren.GetActiveCamera().Azimuth(45)
iren.Initialize()

renWin.Render()

iren.Start()
