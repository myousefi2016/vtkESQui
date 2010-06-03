#include <iostream>
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkTransform.h"
#include "vtkLinearTransform.h"
#include "vtkAppendPolyData.h"
#include "vtkTransformPolyDataFilter.h"

#include "vtkPolyDataReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkTexture.h"
#include "vtkLight.h"
#include "vtkLightCollection.h"

#include "vtkCylinderSource.h"
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"
#include "vtkProperty.h"
#include "vtkDelaunay3D.h"
#include "vtkTextureMapToCylinder.h"
#include "vtkTransformTextureCoords.h"
#include "vtkJPEGReader.h"

#include "vtkFloatArray.h"
#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridGeometryFilter.h"

#include "vtkCommand.h"

#include "vtkMath.h"
#include "vtkHaptic.h"
#include "vtkIHP.h"

namespace IHPExampleNS{
	//All these objects must be defined globally, in order to access them on timer callback
	//Note: Global variables may have its first letter on uppercase
	vtkIHP * Haptic = NULL;

	vtkActor * Tool0Actor = NULL;
	vtkActor * Tool1Actor = NULL;

	vtkTransform * Tool0Transform = NULL;
	vtkTransform * Tool1Transform = NULL;

	float Tool0Depth  = 0;
	float Tool1Depth  = 0;

	void UpdateHaptic(vtkIHP * Haptic);
}

using namespace std;
using namespace IHPExampleNS;

void IHPExampleNS::UpdateHaptic(vtkIHP * Haptic)
{

	float * state;
	float * direction;
	float yaw;
	float pitch;
	float depth;
	float roll;
	float opening;

	vtkActor * tool;
	vtkTransform * toolTx;
	float * toolDepth;

	//Update  state of the IHP device
	Haptic->UpdateDevice();

	for (int i = 0; i<2; i++)
	{
		cout << "### Tool("<<  i <<") ###" << endl;
		//Trocar state
		state = Haptic->GetTrocarState(i);
		//cout << "Direction: (" << state[0] <<", "<< state[1] <<", "<< state[2] <<")"<< endl;

		//Trocar's direction is returned as a unit vector
		direction = Haptic->GetTrocarDirection(i);
		//Trocar's direction angles
		yaw = Haptic->GetTrocarYaw(i);
		pitch = Haptic->GetTrocarPitch(i);

		//Tool-in-the-trocar parameters
		depth = Haptic->GetToolDepth(i);
		roll = Haptic->GetToolRoll(i);
		opening = Haptic->GetToolOpening(i);


		if(i == 0)
		{
			tool = Tool0Actor;
			toolTx = Tool0Transform;
			toolDepth = &Tool0Depth;
		}
		else
		{
			tool = Tool1Actor;
			toolTx = Tool1Transform;
			toolDepth = &Tool1Depth;
		}

		//Set Orientation of the tool
		tool->SetOrientation(pitch,yaw,0);

		std::cout << "Yaw: " << yaw << ", Pitch: "
				<< pitch << ", Roll: "
				<< roll << "\n";

		//Set tool's roll angle.
		tool->RotateZ(roll);

		//Set tool's depth
		double step = -6*depth + *toolDepth;
		toolTx->Translate(0, 0, step);
		*toolDepth=6*depth;

		cout << "Depth: " << *toolDepth  << endl;

		//Set tool's opening. Modify the color of the tool according to the opening parameter
		vtkProperty * prop = tool->GetProperty();
		prop->SetColor(0 ,0 ,opening);
		tool->SetProperty(prop);

		//Display tool buttons/pedal state
		if(Haptic->GetToolButtonState(i)){
			std::cout << "Tool("<<i<< ") Main button is pressed...\n";
		}
		if(Haptic->GetLeftPedalState()){
			std::cout << "Tool("<<i<< ") Left pedal is pressed...\n";
		}
		if(Haptic->GetRightPedalState()){
			std::cout << "Tool("<<i<< ") Right pedal is pressed...\n";
		}

		//Set Tool Feedback Forces

		float force[3];
		force[0] = 0;
		force[1] = 0;
		force[2] = 0;

		if(*toolDepth > 3.5)
		{
			//A Horizontal plane is simulated to check haptic feedback force.
			//Z-Component Elastic Force is applied
			force[2] = 2*(*toolDepth - 3.5);
			std::cout << "Depth: " << *toolDepth << " | Feedback Force: "<< force[2] << "N" << std::endl;
		}
		else force[0] = 0;

		Haptic->SetToolTipForce(i, force);
		Haptic->ApplyForce(i);
	}

	Haptic->FlushForces();

}

class vtkTimerCallback : public vtkCommand
{
public:
	static vtkTimerCallback *New()
	{
		vtkTimerCallback *cb = new vtkTimerCallback;
		cb->FastTimerId = 0;
		cb->FastTimerCount = 0;
		cb->RenderTimerId = 0;
		cb->RenderTimerCount = 0;
		return cb;
	}

	virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
	{
		if (vtkCommand::TimerEvent == eventId)
		{
			int tid = * static_cast<int *>(callData);

			if (tid == this->FastTimerId)
			{
				//Update Haptic Trigger
				UpdateHaptic(Haptic);

				++this->FastTimerCount;
			}
			else if (tid == this->RenderTimerId)
			{
				++this->RenderTimerCount;

				vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
				if (iren && iren->GetRenderWindow() && iren->GetRenderWindow()->GetRenderers())
				{
					iren->Render();
				}
			}
		}
	}

	void SetFastTimerId(int tid)
	{
		this->FastTimerId = tid;
		this->FastTimerCount = 0;
	}

	void SetRenderTimerId(int tid)
	{
		this->RenderTimerId = tid;
		this->RenderTimerCount = 0;
	}

	void Report()
	{
		cout << "vtkTimerCallback::Report" << endl;
		cout << "  FastTimerId: " << this->FastTimerId << endl;
		cout << "  FastTimerCount: " << this->FastTimerCount << endl;
		cout << "  RenderTimerId: " << this->RenderTimerId << endl;
		cout << "  RenderTimerCount: " << this->RenderTimerCount << endl;
	}

private:
	int FastTimerId;
	int FastTimerCount;
	int RenderTimerId;
	int RenderTimerCount;
};

//!This test perform a standard execution of the IHP Haptic Management

int main(int argc, char * argv[])
{
	/**********  Render Window Definitions  ********/
	vtkRenderer * ren1 = vtkRenderer::New();
	ren1->SetBackground(1.0,1.0,1.0);
	
	vtkRenderWindow *renWin = vtkRenderWindow::New();
	renWin->AddRenderer(ren1);
	renWin->SetSize(840,480);

	vtkRenderWindowInteractor * iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(renWin);

	vtkTransform * cameraTransform = vtkTransform::New();
	vtkCamera * camera = ren1->GetActiveCamera();
	camera->SetUserTransform(cameraTransform);
	//Initialize Camera Position
	double * focal = camera->GetFocalPoint();
	std::cout << "Camera Focal Point: " << focal[0] << ", " << focal[1] << ", " << focal[2] << "\n";
	camera->SetFocalPoint(focal[0], focal[1], focal[2] - 12);
	camera->SetPosition(0, 0, 0);
	camera->Yaw(0);
	camera->Elevation(20);
	camera->Pitch(-20);
	camera->Dolly(1);

	/******* Creation & Initialization of figures ********/

	//A cylinder will emulate the tool
	//Instance of vtkCylinderSource and set some of its properties.
	vtkCylinderSource * cylinder = vtkCylinderSource::New();
	cylinder->SetHeight(12.0);
	cylinder->SetRadius(.3);
	cylinder->SetResolution(20);
	cylinder->Update();

	vtkCubeSource * cube = vtkCubeSource::New();
	cube->SetXLength(1);
	cube->SetYLength(1.5);
	cube->SetZLength(1);
	cube->Update();

	//Ball is shifted in depth (note negative sign)
	vtkTransform * transformShift = vtkTransform::New();
	transformShift->Translate(0, -6, 0);

	vtkTransformPolyDataFilter * shiftFilter = vtkTransformPolyDataFilter::New();
	shiftFilter->SetTransform(transformShift);
	shiftFilter->SetInput(cube->GetOutput());

	vtkAppendPolyData * append = vtkAppendPolyData::New();
	append->AddInput(cylinder->GetOutput());
	append->AddInput(shiftFilter->GetOutput());

	//A Transform will be performed to the polyData to adjust it to VTK coordinate system
	// Whole tool is rotated -90 degrees at x axis. Set in a horizontal plane
	vtkTransform * transformRotate = vtkTransform::New();
	transformRotate->RotateX(90);

	vtkTransformPolyDataFilter * transformFilter = vtkTransformPolyDataFilter::New();
	transformFilter->SetTransform(transformRotate);

	transformFilter->SetInput(append->GetOutput());
	transformFilter->Update();

	vtkPolyData * cyl0 = vtkPolyData::New();
	cyl0->DeepCopy(transformFilter->GetOutput());

	//Global Variable
	vtkTransformPolyDataFilter * tool0Filter = vtkTransformPolyDataFilter::New();
	Tool0Transform = vtkTransform::New();

	tool0Filter->SetInput(cyl0);
	tool0Filter->SetTransform(Tool0Transform);

	vtkDataSetMapper *  tool0Mapper = vtkDataSetMapper::New();
	tool0Mapper->SetInput (tool0Filter->GetOutput());

	Tool0Actor = vtkActor::New();
	Tool0Actor->SetMapper(tool0Mapper);

	vtkProperty * cylinderProp = vtkProperty::New();
	cylinderProp->SetColor(0, 0, 0);
	cylinderProp->SetAmbient(100);
	cylinderProp->SetSpecular(200);
	cylinderProp->SetOpacity(0.5);
	Tool0Actor->SetProperty(cylinderProp);
	Tool0Actor->AddPosition(-3, 0.5, 0);
	Tool0Actor->SetOrientation(0, -10, 0);

	//A cylinder will emulate the tool
	//Instance of vtkCylinderSource and set some of its properties.
	vtkPolyData * cyl1 = vtkPolyData::New();
	cyl1->DeepCopy(cyl0);

	//Global Variable
	vtkTransformPolyDataFilter * tool1Filter = vtkTransformPolyDataFilter::New();
	Tool1Transform = vtkTransform::New();

	tool1Filter->SetInput(cyl1);
	tool1Filter->SetTransform(Tool1Transform);

	vtkDataSetMapper *  tool1Mapper = vtkDataSetMapper::New();
	tool1Mapper->SetInput(tool1Filter->GetOutput());

	Tool1Actor = vtkActor::New();
	Tool1Actor->SetMapper(tool1Mapper);

	vtkProperty * cylinderProp1 = vtkProperty::New();
	cylinderProp1->SetColor(0.1, 0.1, 0.1);
	cylinderProp1->SetAmbient(100);
	cylinderProp1->SetSpecular(200);
	cylinderProp1->SetOpacity(0.5);
	Tool1Actor->SetProperty(cylinderProp1);
	Tool1Actor->AddPosition(3, 0.5, 0);
	Tool1Actor->SetOrientation(0, 10, 0);

	//Add Actors to the renderer
	ren1->AddActor(Tool0Actor);
	ren1->AddActor(Tool1Actor);

	iren->Initialize();

	//iren->RemoveAllObservers();

	// Sign up to receive TimerEvent:
	//
	vtkTimerCallback *cb = vtkTimerCallback::New();
	iren->AddObserver(vtkCommand::TimerEvent, cb);
	int tid;

	/******* Haptic Initialization ********/

	//Create an vtkIHP Haptic Object
	Haptic = vtkIHP::New();

	int connected = Haptic->Init();

	if(connected > 0){
		std::cout << "IHP Device is connected..." << std::endl;

		// Create repeating timer for synchronous haptic update
		tid = iren->CreateRepeatingTimer(30);
		cb->SetFastTimerId(tid);
		Haptic->SetInitialPitchAngle(0);
	}
	else
	{
		std::cout << "IHP Device is not connected... \nPlease startup device and restart the application" << std::endl;
		exit(0);
	}

	// Create a slower repeating timer to trigger Render calls.
	// (This fires at the rate of approximately 20 frames per second.)
	//
	tid = iren->CreateRepeatingTimer(50);
	cb->SetRenderTimerId(tid);

	iren->Start();

	Haptic->Release();

	Haptic->Delete();
	ren1->Delete();
	renWin->Delete();
	iren->Delete();
	
	return 0;
}


