#include "vtkVSP.h"
vtkCxxRevisionMacro(vtkVSP, "$Revision: 0.1 $");
vtkStandardNewMacro(vtkVSP);

//--------------------------------------------------------------------------
vtkVSP::vtkVSP(){	

}

//--------------------------------------------------------------------------
vtkVSP::~vtkVSP(){}

//--------------------------------------------------------------------------
void vtkVSP::PrintSelf(ostream& os,vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}

//--------------------------------------------------------------------------
int vtkVSP::Init()
{
	xiSoftwareVendor("LasPalmas");
	XiBool connected;
	char *Description = new char[1024];
	connected = xiCathAcquire();
	xiCathGetDeviceDescription(Description);
	std::cout << Description << std::endl;
	// Return 1 if a VSP device was found
	return connected;
}

//--------------------------------------------------------------------------
void vtkVSP::Update()
{
	this->UpdateDevice();
}

//--------------------------------------------------------------------------
void vtkVSP::Release()
{
	xiCathRelease();
}

//--------------------------------------------------------------------------
void vtkVSP::UpdateDevice()
{
	xiCathQueryStates();
	xiCathGetControlPanelState(&this->DevicePanel);
	xiCathGetState(0,&this->Catheter0);
	xiCathGetState(1,&this->Catheter1);
	xiCathGetState(2,&this->Catheter2);
}

//Joystick Control state
//--------------------------------------------------------------------------
float *vtkVSP::GetLeftJoystickState() {
	float *data;
	data = new float[2];
	data[0] = this->DevicePanel.joyLeft.right;
	data[1] = this->DevicePanel.joyLeft.up;
	return data;
}

//--------------------------------------------------------------------------
void vtkVSP::GetLeftJoystickState(float data[2]) {
	data[0] = this->DevicePanel.joyLeft.right;
	data[1] = this->DevicePanel.joyLeft.up;
}

//--------------------------------------------------------------------------
void vtkVSP::GetLeftJoystickState(float *data1,float *data2) {
	*data1 = this->DevicePanel.joyLeft.right;
	*data2 = this->DevicePanel.joyLeft.up;
}

//--------------------------------------------------------------------------
float *vtkVSP::GetRightJoystickState() {
	float *data;
	data = new float[2];
	data[0] = this->DevicePanel.joyLeft.right;
	data[1] = this->DevicePanel.joyLeft.up;
	return data;
}

//--------------------------------------------------------------------------
void vtkVSP::GetRightJoystickState(float data[2]) {
	data[0] = this->DevicePanel.joyLeft.right;
	data[1] = this->DevicePanel.joyLeft.up;
}

//--------------------------------------------------------------------------
void vtkVSP::GetRightJoystickState(float *data1,float *data2) {
	*data1 = this->DevicePanel.joyLeft.right;
	*data2 = this->DevicePanel.joyLeft.up;
}

//Control panel buttons state
//--------------------------------------------------------------------------
int vtkVSP::GetZoomState()
{
	switch (this->DevicePanel.buttons)
	{
	case XIP_ZoomPlus:
		return 1;
	case XIP_ZoomMinus:
		return -1;
	default:
		return 0;
	}
}

//--------------------------------------------------------------------------
int vtkVSP::GetVisualizationMode()
{
	switch (this->DevicePanel.buttons)
	{
	case XIP_Mosaic:
		return 1;
	case XIP_Dsa:
		return -1;
	default:
		return 0;
	}
}

//--------------------------------------------------------------------------
int vtkVSP::GetSidState()
{
	switch (this->DevicePanel.buttons)
	{
	case XIP_SidUp:
		return 1;
	case XIP_SidDown:
		return -1;
	default:
		return 0;
	}
}

//--------------------------------------------------------------------------
int vtkVSP::GetTableState()
{
	switch (this->DevicePanel.buttons)
	{
	case XIP_TableUp:
		return 1;
	case XIP_TableDown:
		return -1;
	default:
		return 0;
	}
}

//Pedal state
//--------------------------------------------------------------------------
int vtkVSP::GetLeftPedalState()
{
	return (this->DevicePanel.buttons == XIP_LeftPedal);
}

//--------------------------------------------------------------------------
int vtkVSP::GetCenterPedalState()
{
	return (this->DevicePanel.buttons == XIP_CenterPedal);
}

//--------------------------------------------------------------------------
int vtkVSP::GetRightPedalState()
{
	return (this->DevicePanel.buttons == XIP_RightPedal);
}

//--------------------------------------------------------------------------
int vtkVSP::GetCatheter0State() {
	return this->Catheter0.flags;
}

//--------------------------------------------------------------------------
float vtkVSP::GetCatheter0Depth() {
	return this->Catheter0.insertDepth;
}

//--------------------------------------------------------------------------
float vtkVSP::GetCatheter0Roll() {
	return this->Catheter0.cwRoll;
}

//--------------------------------------------------------------------------
int vtkVSP::GetCatheter1State() {
	return this->Catheter1.flags;
}

//--------------------------------------------------------------------------
float vtkVSP::GetCatheter1Depth() {
	return this->Catheter1.insertDepth;
}

//--------------------------------------------------------------------------
float vtkVSP::GetCatheter1Roll() {
	return this->Catheter1.cwRoll;
}

//--------------------------------------------------------------------------
int vtkVSP::GetCatheter2State() {
	return this->Catheter2.flags;
}

//--------------------------------------------------------------------------
float vtkVSP::GetCatheter2Depth() {
	return this->Catheter2.insertDepth;
}

//--------------------------------------------------------------------------
float vtkVSP::GetCatheter2Roll() {
	return this->Catheter2.cwRoll;
}

/*void vtkVSP::PrintSelf(ostream & os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

//	os << indent << "Name: " << this->Name << endl;
//	os << indent << "Id: " << this->Id << endl;
//	os << indent << "Type: " << this->Type << endl;
//	os << indent << "FileName: " << this->FileName << endl;
//	os << indent << "PolyData (#points): " << this->PolyData->GetNumberOfPoints() << endl;

}*/

