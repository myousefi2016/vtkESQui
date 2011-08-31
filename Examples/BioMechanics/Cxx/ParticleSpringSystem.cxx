#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataWriter.h"
#include "vtkDataSetMapper.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "vtkActor.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"

#include "vtkPolyData.h"
#include "vtkPolyDataWriter.h"
#include "vtkXMLPolyDataReader.h"

#include "vtkTimerLog.h"
#include "vtkPointLocator.h"

#include "vtkSmartPointer.h"
#include "vtkCommand.h"

#include "vtkParticleSpringSystemInterface.h"

class vtkPSSTimerCallback : public vtkCommand
{
public:
	static vtkPSSTimerCallback *New()
	{
		vtkPSSTimerCallback *cb = new vtkPSSTimerCallback;
		cb->FastTimerId = 0;
		cb->FasterTimerId = 0;
		cb->RenderTimerId = 0;
		return cb;
	}

	virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
	{
		if (vtkCommand::TimerEvent == eventId)
		{
			int tid = * static_cast<int *>(callData);

			if (tid == this->FastTimerId)
			{
				cout << "new collision\n";

				vtkPolyData * mesh = vtkPolyData::SafeDownCast(this->DeformationModel->GetInput());

				//Locate collision points
				vtkPointLocator * locator = vtkPointLocator::New();
				double bounds[6];
				mesh->GetBounds(bounds);

				double p[3] = {bounds[0], 0, 0};

				vtkIdList * list = vtkIdList::New();
				locator->SetDataSet(mesh);
				locator->FindClosestNPoints(5, p, list);

				//Set Collisions
				double force[3];
				force[0] = 0.15;//-0.1;
				force[1] = 0.05;
				force[2] = 0;//0.05;

				for(vtkIdType i = 0; i< list->GetNumberOfIds(); i++)
				{
					int id = list->GetId(i);
					this->DeformationModel->AddDisplacement(id, force);
				}
			}
			else if (tid == this->FasterTimerId)
			{
				vtkTimerLog * timer = vtkTimerLog::New();
				timer->StartTimer();
				this->DeformationModel->Modified();
				this->DeformationModel->Update();
				timer->StopTimer();

				std::cout << "[Test] Execution Rate: " << 1/(timer->GetElapsedTime()) << "\n";

			}
			else if (tid == this->RenderTimerId)
			{
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
	}

	void SetFasterTimerId(int tid)
	{
		this->FasterTimerId = tid;
	}

	void SetRenderTimerId(int tid)
	{
		this->RenderTimerId = tid;
	}

	void SetDeformationModel(vtkParticleSpringSystemInterface * DeformationModel)
	{
		this->DeformationModel = DeformationModel;
	}

	void SetCollisionIds(vtkIdList * list)
	{
		this->List = list;
	}
private:
	int FastTimerId;
	int RenderTimerId;
	int FasterTimerId;

	vtkIdList * List;

	vtkParticleSpringSystemInterface * DeformationModel;
};

int main(int argc, char * argv[])
{
	const char * filename = "/home/jballesteros/Workspace/data/vtkESQuiData/Scenario/Organs/ball_def_c10.vtp";

	if (argc > 1)
	{
		filename = argv[1];
	}

	vtkSmartPointer<vtkXMLPolyDataReader> reader =
			vtkSmartPointer<vtkXMLPolyDataReader>::New();
	reader->SetFileName(filename);
	reader->Update();

	vtkPolyData * mesh = reader->GetOutput();

	vtkSmartPointer<vtkParticleSpringSystemInterface> pss = vtkSmartPointer<vtkParticleSpringSystemInterface>::New();
	pss->SetName("ball_def_c10");
	pss->SetFileName(filename);
	pss->SetColor(1,0,0);
	pss->SetOpacity(1.0);
	pss->SetSolverType(vtkMotionEquationSolver::VelocityVerlet);
	pss->SetSpringCoefficient(150);
	pss->SetDistanceCoefficient(10);
	pss->SetDampingCoefficient(5);//Friction
	pss->SetMass(.5);
	pss->SetDeltaT(0.001);//1ms
	pss->Init();

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

	vtkSmartPointer<vtkRenderWindow> renWin =
			vtkSmartPointer<vtkRenderWindow>::New();
	renWin->SetSize(500,500);
	renWin->AddRenderer(renderer);

	vtkSmartPointer<vtkRenderWindowInteractor> iren =
			vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInput(mesh);

	vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(0,1,0);
	actor->GetProperty()->SetOpacity(0.5);

	renderer->AddActor(actor);
	renderer->AddActor(pss->GetActor());
	renderer->SetBackground(1,1,1);

	renderer->ResetCamera();
	iren->Initialize();

	renWin->Render();

	// Sign up to receive TimerEvent:
	//
	vtkPSSTimerCallback * cb = vtkPSSTimerCallback::New();
	iren->AddObserver(vtkCommand::TimerEvent, cb);
	int tid;

	cb->SetDeformationModel(pss);

	//Create a faster timer for DeformationModel update
	tid = iren->CreateRepeatingTimer(1);
	cb->SetFasterTimerId(tid);

	//Create a collision every 5 seconds
	tid = iren->CreateRepeatingTimer(5000);
	cb->SetFastTimerId(tid);

	// Create a slower repeating timer to trigger Render calls.
	// (This fires at the rate of approximately 25 frames per second.)
	//
	tid = iren->CreateRepeatingTimer(40);
	cb->SetRenderTimerId(tid);

	iren->Start();

	return 0;
}

