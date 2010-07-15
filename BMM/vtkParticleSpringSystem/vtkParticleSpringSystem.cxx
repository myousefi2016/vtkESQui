#include "vtkParticleSpringSystem.h"

#include "vtkObjectFactory.h"

#include "vtkMotionEquationSolver.h"
#include "vtkVelocityVerletSolver.h"
#include "vtkRK4Solver.h"

vtkCxxRevisionMacro(vtkParticleSpringSystem, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkParticleSpringSystem);

//----------------------------------------------------------------------------
vtkParticleSpringSystem::vtkParticleSpringSystem()
{
	this->SpringCoefficient = 0.0;
	this->DistanceCoefficient = 0;
	this->DampingCoefficient = 0;
	this->DeltaT = 0;
	this->Mass = 0;
	this->RigidityFactor = 0;
	this->Volume = 0;
	this->SystemProperties = NULL;
	this->SolverType = vtkParticleSpringSystem::VelocityVerlet;
	this->ContactIds = NULL;
	this->ContactDisplacements = NULL;
}

//----------------------------------------------------------------------------
vtkParticleSpringSystem::~vtkParticleSpringSystem()
{
	this->Particles->Delete();
	this->Springs->Delete();
}

//----------------------------------------------------------------------------
// VTK specific method: This method is called when the pipeline is calculated.
//----------------------------------------------------------------------------
int vtkParticleSpringSystem::RequestData(
		vtkInformation *vtkNotUsed(request),
		vtkInformationVector **inputVector,
		vtkInformationVector *outputVector)
{

	// Get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Get the input and output
	vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

	//output->DeepCopy(input);

	//Compute Collisions
	this->ComputeContacts();

	// Solve motion equation
	this->Step();

	//Update output points
	vtkPoints * points = input->GetPoints();
	//vtkPoints * points = output->GetPoints();
	for (int i=0; i < input->GetNumberOfPoints(); i++)
	{
		vtkParticle * particle = this->Particles->GetParticle(i);
		points->SetPoint(i, particle->GetPosition());
	}

	output->ShallowCopy(input);

	//Compute System properties
	//TODO: Check why input is not updated on each step (volume computation)
	vtkPolyData * surface = vtkPolyData::New();
	surface->ShallowCopy(output);

	this->SystemProperties->SetInput(surface);
	this->Volume = this->SystemProperties->GetVolume();

	surface->Delete();
	return 1;
}

void vtkParticleSpringSystem::Step()
{
	this->Solver->ComputeNextStep(this->Particles, this->DeltaT);
}

//----------------------------------------------------------------------------
void vtkParticleSpringSystem::Init()
{
	vtkPolyData * input = vtkPolyData::SafeDownCast(this->GetInput());
	vtkPolyData * mesh = input;

	vtkIdType numPoints = mesh->GetNumberOfPoints();
	vtkIdType numCells = mesh->GetNumberOfCells();

	//Initialize Particle System
	this->Particles = vtkParticleCollection::New();
	this->Springs = vtkSpringCollection::New();
	this->Links = vtkCellLinks::New();

	for(int id = 0; id < numPoints; id++)
	{
		//One particle per point
		double * point = mesh->GetPoint(id);

		vtkParticle * p0 = vtkParticle::New();
		p0->SetId(id);
		p0->SetPosition(point);
		p0->SetMass(this->Mass);

		//Check whether the particle already exists
		if(!this->Particles->ContainsParticle(p0))
		{
			this->Particles->InsertNextParticle(p0);
		}
		//Create one link entry per particle
		this->Links->InsertNextPoint(0);
	}
	cout << "Total number of particles: " << this->Particles->GetNumberOfItems() << endl;

	vtkIdList * points = vtkIdList::New();

	for (vtkIdType cellId = 0; cellId < numCells; cellId++) {
		mesh->GetCellPoints(cellId, points);
		int numIds = points->GetNumberOfIds();
		for(vtkIdType jd=0; jd<numIds;jd++)
		{
			vtkIdType p0Id = points->GetId(jd);
			vtkParticle * p0 = this->Particles->GetParticle(p0Id);
			for(vtkIdType kd=0; kd<numIds;kd++)
			{
				vtkIdType p1Id = points->GetId(kd);
				if(p0Id!=p1Id)
				{
					vtkParticle * p1 = this->Particles->GetParticle(p1Id);
					vtkSpring * spring = vtkSpring::New();
					spring->SetId(this->Springs->GetNumberOfItems());
					spring->SetDistanceCoefficient(this->DistanceCoefficient);
					spring->SetDampingCoefficient(this->DampingCoefficient);
					spring->SetSpringCoefficient(this->SpringCoefficient);
					spring->SetDeltaT(this->DeltaT);
					spring->InsertNextParticle(p0);
					spring->InsertNextParticle(p1);
					if(!this->Springs->ContainsSpring(spring))
					{
						spring->Init();
						this->Springs->InsertNextSpring(spring);
						this->Links->ResizeCellList(p0->GetId(),1);
						this->Links->InsertNextCellReference(p0->GetId(),spring->GetId());
						this->Links->ResizeCellList(p1->GetId(),1);
						this->Links->InsertNextCellReference(p1->GetId(),spring->GetId());
						//spring->Print(cout);
					}
				}
			}
		}
	}

	cout << "Total number of springs: " << this->Springs->GetNumberOfItems() << endl;

	switch(this->SolverType)
	{
	case VelocityVerlet:
		this->Solver = vtkVelocityVerletSolver::New();
		cout << "Velocity verlet solver selected\n";
	case RungeKutta4:
		this->Solver = vtkRK4Solver::New();
		cout << "Runge-Kutta45 solver selected\n";
	default:
		break;
	}

	//System Properties
	this->SystemProperties = vtkMassProperties::New();

	//Initialize motion equation solver
	this->Solver->SetDeformationModel(this);
	this->Solver->SetNumberOfParticles(this->Particles->GetNumberOfItems());
	this->Solver->Init();

	//Initialize contact objects
	this->ContactIds = vtkIdList::New();
	this->ContactDisplacements = vtkDoubleArray::New();
	this->ContactDisplacements->SetNumberOfComponents(3);

	//Raise update event
	this->Modified();

	cout << this->GetClassName() << " has been init...\n";
}

//----------------------------------------------------------------------------
void vtkParticleSpringSystem::ComputeForces()
{
	double drag[3];

	for(int i=0; i<this->Particles->GetNumberOfItems(); i++)
	{
		vtkParticle * particle = this->Particles->GetParticle(i);
		//Reset Forces
		particle->SetForce(0, 0, 0);

		//Gravitational Force
		//TODO: Obtain Gravity from simulation
		//FIXME: Reset gravity force
		/*double gravity = -9.0;
		double gf = gravity*particle->GetMass();
		particle->AddForce(0, gf, 0);*/

		//TODO: Add Viscous drag force
		//Viscous Drag
		particle->GetVelocity(drag);
		vtkMath::Normalize(drag);
	}

	double d[3];
	double v[3];
	double F[3];
	double Fs[3];
	double Fd[3];

	//Process springs
	for(int i=0; i<this->Springs->GetNumberOfItems(); i++)
	{
		vtkSpring * spring = this->Springs->GetSpring(i);
		vtkParticle * p0 = spring->GetParticle(0);
		vtkParticle * p1 = spring->GetParticle(1);
		vtkMath::Subtract(p0->GetPosition(), p1->GetPosition(), d); // d = p[0]-p[1]
		vtkMath::Subtract(p0->GetVelocity(), spring->GetParticle(1)->GetVelocity(), v); // v = v[0]-v[1]
		double L = spring->GetRestLength();
		double dNorm = vtkMath::Norm(d);
		double K = spring->GetSpringCoefficient();
		double damping = spring->GetDampingCoefficient();
		double Ad = (dNorm-L);

		// Measure Spring/Damping Force
		for(int j=0; j<3; j++)
		{
			double dD = (d[j]/dNorm);
			//Spring Force
			Fs[j] = -(K*Ad)*dD; //OK
			//Damping Force
			Fd[j] = -(damping*(v[j]*d[j])/dNorm)*dD;//OK

			//Add forces
			F[j] = Fs[j] + Fd[j];
		}
		//TODO: Implement Internal Pressure Force
		//Sum up all forces
		p0->AddForce(F[0], F[1], F[2]);
		p1->AddForce(-F[0], -F[1], -F[2]);

		/*double * F0 = p0->GetForce();
		double * F1 = p1->GetForce();
		cout << "Fs: " << Fs[0] << ","<<Fs[1]<<","<<Fs[2]<<endl;
		cout << "Fd: " << Fd[0] << ","<<Fd[1]<<","<<Fd[2]<<endl;
		cout << "F: " << F[0] << ","<<F[1]<<","<<F[2]<<endl;
		cout << "AccumF0["<< p0->GetId() <<"]: " << F0[0] << ","<<F0[1]<<","<<F0[2]<<endl;
		cout << "AccumF1["<< p1->GetId() <<"]: " << F1[0] << ","<<F1[1]<<","<<F1[2]<<endl;
		*/
	}

	for(int i = 0; i<this->Particles->GetNumberOfItems();i++)
	{
		this->Particles->GetParticle(i)->Update();
	}
}

//----------------------------------------------------------------------------
void vtkParticleSpringSystem::SetContacts(vtkIdList * ids, vtkDoubleArray * directions)
{
	this->ContactIds->DeepCopy(ids);
	this->ContactDisplacements->DeepCopy(directions);
	this->Modified();
}

//----------------------------------------------------------------------------
void vtkParticleSpringSystem::ComputeContacts()
{
	double position[3];
	double distance[3];

	if(this->ContactIds && this->ContactDisplacements)
	{
		for (vtkIdType i = 0; i < this->ContactIds->GetNumberOfIds(); i++)
		{
			//
			vtkIdType id = this->ContactIds->GetId(i);
			double * d = this->ContactDisplacements->GetTuple3(i);

			vtkParticle * p = this->Particles->GetParticle(id);

			//Distance Coefficient constraint
			//Save original position
			p->GetPosition(position);
			p->Print(cout);
			//Add contact displacement
			p->AddPosition(d[0], d[1], d[2]);

			//Retrieve springs containing this particle
			vtkIdType * links = this->Links->GetCells(i);
			for(int j = 0; j < this->Links->GetNcells(i); j++)
			{
				//Retrieve pointer component -> spring Id
				vtkIdType springId = *links;

				//Springs containing particle
				vtkSpring * spring = this->Springs->GetSpring(springId);
				vtkParticle * p0 = spring->GetParticle(0);
				vtkParticle * p1 = spring->GetParticle(1);
				//spring->Print(cout);

				vtkMath::Subtract(p0->GetPosition(), p1->GetPosition(), distance); // distance = p[0]-p[1]
				double dNorm = vtkMath::Norm(distance);
				double L = spring->GetRestLength();
				double ratio = 100*((dNorm-L)/L);
				while(ratio > spring->GetDistanceCoefficient())
				{
					cout << "ratio: "<< ratio <<" > "<< spring->GetDistanceCoefficient() <<" distance coeff...\n";
					p->AddPosition(-d[0]/10, -d[1]/10, -d[2]/10);
					p->Print(cout);
					vtkMath::Subtract(p0->GetPosition(), p1->GetPosition(), distance); // distance = p[0]-p[1]
					dNorm = vtkMath::Norm(distance);
					L = spring->GetRestLength();
					ratio = 100*((dNorm-L)/L);
				}
				//Increment link pointer
				links += 1;
			}
			p->SetContacted(1);
		}

		//Reset contact state
		this->ContactIds->Reset();
		this->ContactDisplacements->Reset();
	}
}

//----------------------------------------------------------------------------
void vtkParticleSpringSystem::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
