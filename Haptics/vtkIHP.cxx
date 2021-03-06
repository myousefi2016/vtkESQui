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
#include "vtkIHP.h"

#include "vtkObjectFactory.h"
#include "vtkMath.h"

#include "vtkScenarioObjectCollection.h"
#include "vtkScenarioObject.h"
#include "vtkTool.h"
#include "vtkToolGrasper.h"
#include "vtkToolProbe.h"

vtkCxxRevisionMacro(vtkIHP, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkIHP);

//--------------------------------------------------------------------------
vtkIHP::vtkIHP()
{
  this->InitialPitch = 0;
}

//--------------------------------------------------------------------------
vtkIHP::~vtkIHP(){

}

//--------------------------------------------------------------------------
void vtkIHP::PrintSelf(ostream& os,vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//--------------------------------------------------------------------------
int vtkIHP::Init()
{
  xiSoftwareVendor("itc_las_palmas");
  int  connected;
  bool initialized = false;
  char *Description = new char[1024];
  connected = xiTrocarAcquire();

  xiTrocarGetDeviceDescription(0, Description);
  xiTrocarGetDeviceDescription(1, Description);

  std::cout << "Waiting for tool initialization\n";
  while (!initialized)
  {
    this->RetrieveDeviceState();
    std::string str;

    if (!!(this->Trocar0.flags & XI_InitNeedToolOut))
    {
      str += "Take the tool out. ";
    }
    if (!!(this->Trocar0.flags & XI_InitNeedLeftIPulse) || !!(this->Trocar0.flags & XI_InitNeedRightIPulse))
    {
      str += "Tilt the trocar. ";
    }

    initialized = !!(this->Trocar0.flags & XI_TrocarIsInitialized);
    if(!initialized) std::cout << str.c_str() << std::endl;
  }

  //Set Initial Forces
  this->Trocar0Force.tipForce[0] = 0;   ///< force in Newtons applied to a virtual instrument tip of unit length
  this->Trocar0Force.tipForce[1] = 0;
  this->Trocar0Force.tipForce[2] = 0;
  this->Trocar0Force.rollForce = 0;     ///< force applied to rotate the instrument (clockwise Nm).
  this->Trocar0Force.opening = 0;
  this->Trocar1Force.tipForce[0] = 0;   ///< force in Newtons applied to a virtual instrument tip of unit length
  this->Trocar1Force.tipForce[1] = 0;
  this->Trocar1Force.tipForce[2] = 0;
  this->Trocar1Force.rollForce = 0;     ///< force applied to rotate the instrument (clockwise Nm).
  this->Trocar1Force.opening = 0;

  std::cout << "Device was successfully initialized.\n";

  // Return > 0 if a IHP device was found
  return connected;
}

//--------------------------------------------------------------------------
void vtkIHP::Update()
{
  this->UpdateDevice();

  //Scenario tools will be updated
  this->Tools->InitTraversal();
  vtkIdType id = 0;
  while(vtkScenarioObject * o = this->Tools->GetNextObject())
  {
    vtkTool * t = vtkTool::SafeDownCast(o);
    if(t && t->GetToolType() ==  vtkTool::Laparoscopy)
    {
      vtkToolLaparoscopy * tool = vtkToolLaparoscopy::SafeDownCast(t);

      //Trocar state
      float * state = this->GetTrocarState(id);

      //Trocar's direction angles
      //Haptic coordinate system differs  from VTK system
      // |  Haptic  |  VTK  |
      // |      X      |    Y    |
      // |      Y      |    Z    |
      // |      Z      |    X    |
      tool->Yaw(this->GetTrocarYaw(id));
      tool->Pitch(this->GetTrocarPitch(id));
      tool->Roll(this->GetToolRoll(id));

      //Tool-in-the-trocar parameters
      //Set tool's depth
      tool->SetDepth(20*this->GetToolDepth(id));

      if(tool->GetToolModel() == vtkToolLaparoscopy::Grasper){
        vtkToolGrasper * grasper = vtkToolGrasper::SafeDownCast(t);
        //Set tool's opening.
        grasper->SetOpening(this->GetToolOpening(id));
      }

      //Display tool buttons/pedal state
      if(this->GetToolButtonState(id)){
        std::cout << "Tool("<<id<< ") Main button is pressed...\n";
      }
      if(this->GetLeftPedalState()){
        std::cout << "Tool("<<id<< ") Left pedal is pressed...\n";
      }
      if(this->GetRightPedalState()){
        std::cout << "Tool("<<id<< ") Right pedal is pressed...\n";
      }

      //Set Tool Feedback Forces
      float force [3];
      force[0] = force[1] = force[2] = 0;

      if(tool->GetDepth() > 3)
      {
        std::cout << "Tool Depth > 3. Force Applied" << std::endl;
        force[2] = 1;
      }
      else force[2] = 0;

      this->SetToolTipForce(id, force);
      this->ApplyForce(id);

      id++;
    }
  }

  // Flush forces to the haptic device
  this->FlushForces();
}

//--------------------------------------------------------------------------
void vtkIHP::Release()
{
  //Clear force from previous executions
  xiTrocarRelease();
}

//--------------------------------------------------------------------------
void vtkIHP::UpdateDevice()
{
  this->RetrieveDeviceState();
}

//--------------------------------------------------------------------------
void  vtkIHP::RetrieveDeviceState()
{
  //Query Tools' State
  xiTrocarQueryStates();
  xiTrocarGetState(0,&this->Trocar0);
  xiTrocarGetState(1,&this->Trocar1);

}

//--------------------------------------------------------------------------
void vtkIHP::SetInitialPitchAngle(float angle)
{
  this->InitialPitch = angle;
}
//--------------------------------------------------------------------------
float vtkIHP::GetInitialPitchAngle()
{
  return this->InitialPitch;
}


//--------------------------------------------------------------------------
float *vtkIHP::GetTrocarState(int id)
{
  if(id == 0)
  {
    return this->GetLeftTrocarState();
  }
  else
  {
    return this->GetRightTrocarState();
  }
}

//--------------------------------------------------------------------------
float *vtkIHP::GetLeftTrocarState() {
//  0..2  trocarDir[3];  ///< Unit vector defining the trocar's direction
//  3     toolDepth;     ///< depth in meters of the tool's insertion (may be negative)
//  4       toolRoll;      ///< roll around the tool/trocar axis in radians
//  5       opening;       ///< [0..1] value for opening of instrument handle (or angled optics roll angle)

  float *data;
  data = new float[6];
  data[0] = this->Trocar0.trocarDir[0];
  data[1] = this->Trocar0.trocarDir[1];
  data[2] = this->Trocar0.trocarDir[2];
  data[3] = this->Trocar0.toolDepth;
  data[4] = this->Trocar0.toolRoll;
  data[5] = this->Trocar0.opening;

  return data;
}

//--------------------------------------------------------------------------
float *vtkIHP::GetRightTrocarState() {
//  0..2  trocarDir[3];  ///< Unit vector defining the trocar's direction
//  3     toolDepth;     ///< depth in meters of the tool's insertion (may be negative)
//  4       toolRoll;      ///< roll around the tool/trocar axis in radians
//  5       opening;       ///< [0..1] value for opening of instrument handle (or angled optics roll angle)

  float *data;
  data = new float[6];
  data[0] = this->Trocar1.trocarDir[0];
  data[1] = this->Trocar1.trocarDir[1];
  data[2] = this->Trocar1.trocarDir[2];
  data[3] = this->Trocar1.toolDepth;
  data[4] = this->Trocar1.toolRoll;
  data[5] = this->Trocar1.opening;

  return data;
}

//--------------------------------------------------------------------------
float *vtkIHP::GetTrocarDirection(int id)
{
  if(id == 0)
  {
    return this->GetLeftTrocarDirection();
  }
  else
  {
    return this->GetRightTrocarDirection();
  }
}

//--------------------------------------------------------------------------
float *vtkIHP::GetLeftTrocarDirection() {
//  0..2  trocarDir[3];  ///< Unit vector defining the trocar's direction
  float *data;
  data = new float[6];
  data[0] = this->Trocar0.trocarDir[0];
  data[1] = this->Trocar0.trocarDir[1];
  data[2] = this->Trocar0.trocarDir[2];

  return data;
}

//--------------------------------------------------------------------------
float *vtkIHP::GetRightTrocarDirection() {
//  0..2  trocarDir[3];  ///< Unit vector defining the trocar's direction
  float *data;
  data = new float[6];
  data[0] = this->Trocar1.trocarDir[0];
  data[1] = this->Trocar1.trocarDir[1];
  data[2] = this->Trocar1.trocarDir[2];

  return data;
}

//--------------------------------------------------------------------------
float vtkIHP::GetTrocarYaw(int id)
{
  if(id == 0)
  {
    return this->GetLeftTrocarYaw();
  }
  else
  {
    return this->GetRightTrocarYaw();
  }
}

//--------------------------------------------------------------------------
float vtkIHP::GetLeftTrocarYaw()
{
  // Yaw angle is obtained from the x unit vector component
  // Range [-90,90]
  // Angle in degrees must be translated to VTK coordinate system
  return -(vtkMath::DegreesFromRadians(acos(this->Trocar0.trocarDir[0]))-90);
}

//--------------------------------------------------------------------------
float vtkIHP::GetRightTrocarYaw()
{
  //  Yaw angle is obtained from the x unit vector component
  // Range [-90,90]
  // Angle in degrees must be translated to VTK coordinate system
  return -(vtkMath::DegreesFromRadians(acos(this->Trocar1.trocarDir[0]))-90);
}

//--------------------------------------------------------------------------
float vtkIHP::GetTrocarPitch(int id)
{
  if(id == 0)
  {
    return this->GetLeftTrocarPitch();
  }
  else
  {
    return this->GetRightTrocarPitch();
  }
}

//--------------------------------------------------------------------------
float vtkIHP::GetLeftTrocarPitch()
{
  //  Pitch angle is obtained from the y unit vector component
  // Haptic Range [-90,30]
  // Angle in degrees must be translated to VTK coordinate system
  // Range = InitialPitch + [0, -120]. Horizontal plane set as 0 degrees reference
  float pitch = this->InitialPitch;
  if (this->Trocar0.trocarDir[2] > 0)
  {
    //Haptic Pitch > 0
    pitch += vtkMath::DegreesFromRadians(-acos(this->Trocar0.trocarDir[1]))-90;
  }
  else
  {
    //Haptic Pitch < 0
    pitch += vtkMath::DegreesFromRadians(acos(this->Trocar0.trocarDir[1]))-90;
  }
  return pitch;
}

//--------------------------------------------------------------------------
float vtkIHP::GetRightTrocarPitch()
{
  //  Pitch angle is obtained from the y unit vector component
  // Haptic Range [-90,30]
  // Angle in degrees must be translated to VTK coordinate system
  // Range = InitialPitch + [0, -120]. Horizontal plane set as 0 degrees reference
  float pitch = this->InitialPitch;
  if (this->Trocar0.trocarDir[2] > 0)
  {
    //Haptic Pitch > 0
    pitch += vtkMath::DegreesFromRadians(-acos(this->Trocar1.trocarDir[1]))-90;
  }
  else
  {
    //Haptic Pitch < 0
    pitch += vtkMath::DegreesFromRadians(acos(this->Trocar1.trocarDir[1]))-90;
  }
  return pitch;
}

//--------------------------------------------------------------------------
float vtkIHP::GetToolDepth(int id)
{
  if(id == 0)
  {
    return this->GetLeftToolDepth();
  }
  else
  {
    return this->GetRightToolDepth();
  }
}

//--------------------------------------------------------------------------
float vtkIHP::GetLeftToolDepth()
{
  // Depth in meters
  // Range [-0.04, 0.32]
  // Return Range [0, 1] -> Correction Factor (K) = 3.57
  float K = 3.57;
  return (this->Trocar0.toolDepth +0.04)*K;
}

//--------------------------------------------------------------------------
float vtkIHP::GetRightToolDepth()
{
  // Depth in meters
  // Range [-0.04, 0.32]
  // Return Range [0, 1] -> Correction Factor (K) = 3.57
  float K = 3.57;
  return (this->Trocar1.toolDepth +0.04)*K;
}

//--------------------------------------------------------------------------
float vtkIHP::GetToolRoll(int id)
{
  if(id == 0)
  {
    return this->GetLeftToolRoll();
  }
  else
  {
    return this->GetRightToolRoll();
  }
}

//--------------------------------------------------------------------------
float vtkIHP::GetLeftToolRoll()
{
  // Roll Angle is returned in degrees
  // Haptic  Range [-0.04, 0.32]
  return - vtkMath::DegreesFromRadians(this->Trocar0.toolRoll);
}

//--------------------------------------------------------------------------
float vtkIHP::GetRightToolRoll() {
  // Roll Angle is returned in degrees
  // Haptic Range [-0.04, 0.32]
  return - vtkMath::DegreesFromRadians(this->Trocar1.toolRoll);
}

//--------------------------------------------------------------------------
float vtkIHP::GetToolOpening(int id)
{
  if(id == 0)
  {
    return this->GetLeftToolOpening();
  }
  else
  {
    return this->GetRightToolOpening();
  }
}

//--------------------------------------------------------------------------
float vtkIHP::GetLeftToolOpening() {
  // Tool Opening
  // Range [0,1]
  return this->Trocar0.opening;
}

//--------------------------------------------------------------------------
float vtkIHP::GetRightToolOpening() {
  // Tool Opening
  // Range [0,1]
  return this->Trocar1.opening;
}

//--------------------------------------------------------------------------
int vtkIHP::GetToolButtonState(int id)
{
  if(id == 0)
  {
    return this->GetLeftToolButtonState();
  }
  else
  {
    return this->GetRightToolButtonState();
  }
}

//--------------------------------------------------------------------------
int vtkIHP::GetLeftToolButtonState(){
  return (this->Trocar0.flags & XI_ToolButtonMain);
}

//--------------------------------------------------------------------------
int vtkIHP::GetRightToolButtonState(){
  return (this->Trocar1.flags & XI_ToolButtonMain);
}

//--------------------------------------------------------------------------
int vtkIHP::GetToolLeftButtonState(int id){
  if(id == 0)
  {
    return this->GetLeftToolLeftButtonState();
  }
  else
  {
    return this->GetRightToolLeftButtonState();
  }
}

//--------------------------------------------------------------------------
int vtkIHP::GetLeftToolLeftButtonState(){
  return (this->Trocar0.flags & XI_ToolButtonLeft);
}

//--------------------------------------------------------------------------
int vtkIHP::GetRightToolLeftButtonState(){
  return (this->Trocar1.flags & XI_ToolButtonLeft);
}

//--------------------------------------------------------------------------
int vtkIHP::GetToolRightButtonState(int id){
  if(id == 0)
  {
    return this->GetLeftToolRightButtonState();
  }
  else
  {
    return this->GetRightToolRightButtonState();
  }
}

//--------------------------------------------------------------------------
int vtkIHP::GetLeftToolRightButtonState(){
  return (this->Trocar0.flags & XI_ToolButtonRight);
}

//--------------------------------------------------------------------------
int vtkIHP::GetRightToolRightButtonState(){
  return (this->Trocar1.flags & XI_ToolButtonRight);
}

//--------------------------------------------------------------------------
int vtkIHP::GetLeftPedalState(){
  return (this->GetLeftToolLeftButtonState() || this->GetRightToolLeftButtonState());
}

//--------------------------------------------------------------------------
int vtkIHP::GetRightPedalState(){
  return (this->GetLeftToolRightButtonState() || this->GetRightToolRightButtonState());
}

//--------------------------------------------------------------------------
void vtkIHP::ApplyForce(int id)
{
  XiToolForce trocar;
  if(id == 0)
  {
    trocar = this->Trocar0Force;
  }
  else
  {
    trocar = this->Trocar1Force;
  }

  xiTrocarSetForce(id, &trocar);
}

//--------------------------------------------------------------------------
void vtkIHP::FlushForces()
{
  xiTrocarFlushForces();
}

//--------------------------------------------------------------------------
void vtkIHP::SetToolTipForce(int id, float * tipForce)
{
  if(id == 0)
  {
    this->SetLeftToolTipForce(tipForce);
  }
  else
  {
    this->SetRightToolTipForce(tipForce);
  }
}

//--------------------------------------------------------------------------
void vtkIHP::SetLeftToolTipForce(float * tipForce){
  this->Trocar0Force.tipForce[0] = tipForce[0];
  this->Trocar0Force.tipForce[1] = tipForce[1];
  this->Trocar0Force.tipForce[2] = tipForce[2];
}

//--------------------------------------------------------------------------
void vtkIHP::SetRightToolTipForce(float * tipForce){
  this->Trocar1Force.tipForce[0] = tipForce[0];
  this->Trocar1Force.tipForce[1] = tipForce[1];
  this->Trocar1Force.tipForce[2] = tipForce[2];
}

//--------------------------------------------------------------------------
void vtkIHP::SetToolTipForce(int id, float tipForceX, float tipForceY, float tipForceZ)
{
  if(id == 0)
  {
    this->SetLeftToolTipForce(tipForceX, tipForceY, tipForceZ);
  }
  else
  {
    this->SetRightToolTipForce(tipForceX, tipForceY, tipForceZ);
  }
}

//--------------------------------------------------------------------------
void vtkIHP::SetLeftToolTipForce(float tipForceX, float tipForceY, float tipForceZ){
  this->Trocar0Force.tipForce[0] = tipForceX;
  this->Trocar0Force.tipForce[1] = tipForceY;
  this->Trocar0Force.tipForce[2] = tipForceZ;
}

//--------------------------------------------------------------------------
void vtkIHP::SetRightToolTipForce(float tipForceX, float tipForceY, float tipForceZ){
  this->Trocar1Force.tipForce[0] = tipForceX;
  this->Trocar1Force.tipForce[1] = tipForceY;
  this->Trocar1Force.tipForce[2] = tipForceZ;
}

//--------------------------------------------------------------------------
void vtkIHP::SetToolRollForce(int id, float rollForce){
  if(id == 0)
    {
      this->SetLeftToolRollForce(rollForce);
    }
    else
    {
      this->SetRightToolRollForce(rollForce);
    }
}

//--------------------------------------------------------------------------
void vtkIHP::SetLeftToolRollForce(float rollForce){
  this->Trocar0Force.rollForce = rollForce;
}

//--------------------------------------------------------------------------
void vtkIHP::SetRightToolRollForce(float rollForce){
  this->Trocar1Force.rollForce = rollForce;
}

//--------------------------------------------------------------------------
void vtkIHP::SetToolOpeningForce(int id, float openingForce)
{
  if(id == 0)
  {
    this->SetLeftToolOpeningForce(openingForce);
  }
  else
  {
    this->SetRightToolOpeningForce(openingForce);
  }
}

//--------------------------------------------------------------------------
void vtkIHP::SetLeftToolOpeningForce(float openingForce){
  this->Trocar0Force.opening = openingForce;
}

//--------------------------------------------------------------------------
void vtkIHP::SetRightToolOpeningForce(float openingForce){
  this->Trocar0Force.opening = openingForce;
}

//--------------------------------------------------------------------------
/*void vtkIHP::PrintSelf(ostream & os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

//  os << indent << "Name: " << this->Name << endl;
//  os << indent << "Id: " << this->Id << endl;
//  os << indent << "Type: " << this->Type << endl;
//  os << indent << "FileName: " << this->FileName << endl;
//  os << indent << "PolyData (#points): " << this->PolyData->GetNumberOfPoints() << endl;

}*/

