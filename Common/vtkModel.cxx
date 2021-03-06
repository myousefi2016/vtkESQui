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
#include "vtkModel.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkSyncPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkActor.h"
#include "vtkMatrix4x4.h"
#include "vtkProperty.h"

vtkCxxRevisionMacro(vtkModel, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkModel);

//--------------------------------------------------------------------------
vtkModel::vtkModel()
{
  this->Id = -1;
  this->Type = Visualization;
  this->ObjectId = -1;
  this->ObjectType = Tool;
  this->Name = NULL;
  this->FileName = NULL;
  this->Initialized = 0;
  this->Status = 1;
  this->Color[0]=this->Color[1]=this->Color[2]=1.0;
  this->Opacity = 1.0;
  this->Visibility = 1;

  this->Matrix = NULL;
  this->Actor = NULL;
  this->Mapper = NULL;

  //optional second input
  this->SetNumberOfInputPorts(2);
}

//--------------------------------------------------------------------------
vtkModel::~vtkModel()
{
  if(this->Initialized)
  {
    this->Mapper->Delete();
    this->Actor->Delete();
    this->SyncFilter->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkModel::SetSource(vtkPolyData *source)
{
  this->SetInput(1, source);
}

//----------------------------------------------------------------------------
vtkPolyData *vtkModel::GetSource()
{
  if (this->GetNumberOfInputConnections(1) < 1)
  {
    return NULL;
  }
  return vtkPolyData::SafeDownCast(
      this->GetExecutive()->GetInputData(1, 0));
}


//---------------------------------------------------------------------------
int vtkModel::FillInputPortInformation(int port, vtkInformation *info)
{
  if(!this->Superclass::FillInputPortInformation(port, info))
  {
    return 0;
  }

  if( port == 1 ) // sync mesh
  {
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  }
  return 1;
}

//----------------------------------------------------------------------------
int vtkModel::FillOutputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
  return 1;
}

//----------------------------------------------------------------------------
void vtkModel::SetMatrix(vtkMatrix4x4 * m)
{
  this->Matrix = m;
}

//----------------------------------------------------------------------------
vtkMatrix4x4 * vtkModel::GetMatrix()
{
  return this->Matrix;
}

//----------------------------------------------------------------------------
vtkActor * vtkModel::GetActor()
{
  return this->Actor;
}

//----------------------------------------------------------------------------
vtkPolyDataMapper* vtkModel::GetMapper()
{
  return this->Mapper;
}

//--------------------------------------------------------------------------
void vtkModel::Initialize()
{
  if(!this->Initialized)
  {
    if(!this->Name) this->Name = "";
    //Displaying purposes
    this->Actor = vtkActor::New();
    this->Mapper = vtkPolyDataMapper::New();

    this->SyncFilter = vtkSyncPolyDataFilter::New();

    //Set actor transformation matrix
    if(this->Matrix)
    {
      this->Actor->SetUserMatrix(this->Matrix);
    }

    this->Actor->SetMapper(this->Mapper);

    //Set as initialized
    this->Initialized = 1;
  }
  this->Modified();
}

//----------------------------------------------------------------------------
int vtkModel::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector) {

  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  //Get the input and output
  vtkPolyData *input = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  //Optional input
  vtkPolyData * source = 0;

  if(sourceInfo){
    source = vtkPolyData::SafeDownCast(sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
  }
  //Output
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  //Initialize model
  if(!this->Initialized) this->Initialize();

  //If source is defined -> Synchronize mesh
  if(source)
  {
    vtkDebugMacro("Model source is present\n");

    this->SyncFilter->SetInput(input);
    this->SyncFilter->SetSource(source);
    this->SyncFilter->Update();

    output->ShallowCopy(this->SyncFilter->GetOutput());
  }
  else
  {
    output->ShallowCopy(input);
  }

  //Set visualization parameters
  this->Actor->SetVisibility(this->Visibility);

  if(this->Status){
    this->Actor->GetProperty()->SetColor(this->Color);
    this->Actor->GetProperty()->SetOpacity(this->Opacity);

    this->Mapper->SetInput(output);
    this->Mapper->Modified();
  }

  return 1;
}

//--------------------------------------------------------------------------
void vtkModel::Hide()
{
  this->Visibility = 0;
  this->Modified();
}

//--------------------------------------------------------------------------
void vtkModel::Show()
{
  this->Visibility = 1;
  this->Modified();
}

//--------------------------------------------------------------------------
void vtkModel::Disable()
{
  this->Status = 0;
  this->Visibility = 0;
  this->Modified();
}

//--------------------------------------------------------------------------
void vtkModel::Enable()
{
  this->Status = 1;
  this->Visibility = 1;
  this->Modified();
}

//--------------------------------------------------------------------------
void vtkModel::PrintSelf(ostream&os, vtkIndent indent)
{
  os << indent << "Id: " << this->Id << "\n";
  os << indent << "ObjectId: " << this->ObjectId << "\n";
  os << indent << "Model Type: " << this->Type << "\n";
  if(this->Name) os << indent << "Name: " << this->Name << "\n";
  if(this->FileName) os << indent << "FileName: " << this->FileName << "\n";
  os << indent << "Status: " << this->Status << "\n";
  os << indent << "Visibility: " << this->Visibility << endl;
  os << indent << "Opacity: " << this->Opacity << endl;
  os << indent << "Color: " << this->Color[0] << ", " << this->Color[1] << ", " << this->Color[2] << endl;


}
