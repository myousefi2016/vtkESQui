/*==========================================================================
--------------------------------------------------
Program: ESQUI
Info and Bugs: {marf,jballesteros}@itccanarias.org
url: http://motivando.me 
--------------------------------------------------

Copyright (c) 2006-2007, Center for Technology in Medicine (CTM), 
University of Las Palmas de Gran Canaria (ULPGC), Canary Islands, Spain.
Copyright (c) 2007-2010, Institute of Technology at CanaryIslands (ITC),
Canary Islands, Spain.

This software is free software; you can redistribute it and/or modify it 
under the terms of the GNU Lesser General Public License (LGPL) as published
by the Free Software Foundation, either version 3 of the License, or (at 
your option) any later version.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1) Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2) Redistributions in binary form must reproduce the above copyright 
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

You should have received a copy of the GNU Lesser General Public License 
along with this program.  If not, see <http://www.gnu.org/licenses/>.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
==========================================================================*/
#include "vtkCUDAParticleSystemInterface.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkIdList.h"

#include "vtkBoundaryCondition.h"
#include "vtkBoundaryConditionCollection.h"

vtkCxxRevisionMacro(vtkCUDAParticleSystemInterface, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkCUDAParticleSystemInterface);

//FIXME: Unify vtkCUDAParticleSystem and vtkParticleSystem

//----------------------------------------------------------------------------
vtkCUDAParticleSystemInterface::vtkCUDAParticleSystemInterface()
{
  this->System = vtkCUDAParticleSystem::New();
  this->Spring = 0.0;
  this->Distance = 0;
  this->Damping = 0;
  this->TimeStep = 0;
  this->Mass = 0;
  this->Gravity[0] = this->Gravity[1] = this->Gravity[2] = 0;
  this->TimeStep = 0;
  this->SolverType = vtkCUDAMotionEquationSolver::VelocityVerlet;

  //Force to 1 input only
  this->SetNumberOfInputPorts(1);
}

//----------------------------------------------------------------------------
vtkCUDAParticleSystemInterface::~vtkCUDAParticleSystemInterface()
{
  this->System->Delete();
}

//--------------------------------------------------------------------------
void vtkCUDAParticleSystemInterface::Initialize()
{
  this->Superclass::Initialize();

  //Set input data
  this->System->SetInput(this->GetInput());
  //Set common parameters
  this->System->SetGravity(this->Gravity);
  this->System->SetTimeStep(this->TimeStep);
  //Set Mass-Spring System parameters
  this->System->SetDistance(this->Distance);
  this->System->SetSpring(this->Spring);//Friction
  this->System->SetDamping(this->Damping);//Friction
  this->System->SetMass(this->Mass);
  this->System->SetSolverType(this->SolverType);
  //Initialize system
  this->System->Init();

  //Once the system has been initialized the boundary conditions are set
  this->BoundaryConditions->InitTraversal();
  while(vtkBoundaryCondition * c = this->BoundaryConditions->GetNextBoundaryCondition())
  {
    if(c->GetType() == vtkBoundaryCondition::Neumann){
      // Set condition value
      /* 0 -> Fixed
       * 1 -> Free
       */
      //TODO: Enable particle status in vtkCUDAparticleSystem
      //this->System->SetParticleStatus(c->GetPointId(), c->GetValue());
    }
  }

  if (this->Debug) this->Print(cout);
}

// VTK specific method: This method is called when the pipeline is calculated.
//----------------------------------------------------------------------------
int vtkCUDAParticleSystemInterface::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector) {

  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  //Get the input and output
  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  //Output
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  //cout << this->GetClassName() << "::RequestData(" << this->GetName() << ")\n";

  if(!this->Initialized) this->Initialize();

  if(this->Status)
  {
    //Force recalculation of the output in every step
    this->System->Modified();
    this->System->Update();

    vtkPolyData * out = this->System->GetOutput();

    //Set visualization parameters
    this->Actor->SetVisibility(this->Visibility);
    if(this->IsVisible())
    {
      this->Actor->GetProperty()->SetColor(this->Color);
      this->Actor->GetProperty()->SetOpacity(this->Opacity);

      this->Mapper->SetInput(out);
    }

    //Update filter output
    output->ShallowCopy(out);
  }
  else output->ShallowCopy(input);

  return 1;
}

//--------------------------------------------------------------------------
void vtkCUDAParticleSystemInterface::InsertDisplacement(vtkIdType id, double * vector)
{
  this->InsertDisplacement(id, vector[0], vector[1], vector[2]);
}

//--------------------------------------------------------------------------
void vtkCUDAParticleSystemInterface::InsertDisplacement(vtkIdType id, double x, double y, double z)
{
  this->System->InsertDisplacement(id, x, y, z);
}

//--------------------------------------------------------------------------
void vtkCUDAParticleSystemInterface::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "SpringCoefficient: " << this->Spring << endl;
  os << indent << "DampingCoefficient: " << this->Damping << endl;
  os << indent << "DistanceCoefficient: " << this->Distance << endl;
  os << indent << "Mass: " << this->Mass << endl;
  os << indent << "DeltaT: " << this->TimeStep << endl;
  os << indent << "SolverType: " << this->SolverType << endl;
}
