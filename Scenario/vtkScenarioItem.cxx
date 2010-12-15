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
#include "vtkScenarioItem.h"

vtkCxxRevisionMacro(vtkScenarioItem, "$Revision: 0.1 $");

//--------------------------------------------------------------------------
vtkScenarioItem::vtkScenarioItem() {
	this->Id = -1;
	this->Name = NULL;
	this->RenderWindow = NULL;
	this->Renderer = NULL;

	this->Scale = 0.0;

	this->Position[0]=this->Position[1]=this->Position[2]=0.0;
	this->Velocity[0]=this->Velocity[1]=this->Velocity[2]=0.0;
	this->Orientation[0]=this->Orientation[1]=this->Orientation[2]=0.0;
	this->Origin[0]=this->Origin[1]=this->Origin[2]=0.0;
	this->Direction[0]=this->Direction[1]=this->Direction[2]=0.0;

}

//--------------------------------------------------------------------------
vtkScenarioItem::~vtkScenarioItem()
{
}

//--------------------------------------------------------------------------
void vtkScenarioItem::PrintSelf(ostream& os,vtkIndent indent) {

	this->Superclass::PrintSelf(os, indent);

	os << indent << "Name: " << this->GetName() << "\n";
	os << indent << "Id: " << this->GetId() << "\n";
	os << indent << "Position: " << this->Position[0] << ", " << this->Position[1] << ", " << this->Position[2] << endl;
	os << indent << "Origin: " << this->Origin[0] << ", " << this->Origin[1] << ", " << this->Origin[2] << endl;
	os << indent << "Orientation: " << this->Orientation[0] << ", " << this->Orientation[1] << ", " << this->Orientation[2] <<  endl;
	os << indent << "Direction: " << this->Direction[0] << ", " << this->Direction[1] << ", " << this->Direction[2] <<  endl;
	os << indent << "Velocity: " << this->Velocity[0] << ", " << this->Velocity[1] << ", " << this->Velocity[2] <<  endl;

}
