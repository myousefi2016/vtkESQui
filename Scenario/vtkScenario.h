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
#ifndef _vtkScenario_h
#define _vtkScenario_h

#include "vtkESQuiScenarioWin32Header.h"
#include "vtkObject.h"

class vtkRenderWindow;
class vtkRenderer;

class vtkOrgan;
class vtkOrganCollection;
class vtkTool;
class vtkToolCollection;

//!Implementation of the simulation scenario. All the elements in the scenario (organs, tools...) are stored in collections.
class VTK_ESQUI_SCENARIO_EXPORT vtkScenario: public vtkObject
{

public:
	//!Type Revision Macro
	vtkTypeRevisionMacro(vtkScenario, vtkObject);
	//!Create a new Scenario object
	static vtkScenario * New();
	//!Return class name as a string
	const char *GetClassName() {return "vtkScenario";}

	//!Set the collection of organs of the scenario
	/*!
	* \param collection collection of organs to be inserted to the scenario
	* \sa GetOrganCollection()
	*/
	void SetOrganCollection(vtkOrganCollection * collection);

	//!Get the collection of organs of the scenario
	/*!
	* \sa SetOrganCollection(vtkOrganCollection * collection)
	*/
	vtkOrganCollection * GetOrganCollection();

	//!Insert organ to the scenario
	/*!
	* The organ will be inserted at the end of the list
	* \param organ vtkOrgan object to be inserted
	* \return position of the added object.
	* \sa InsertOrgan(vtkOrgan * organ)
	*/
	void InsertNextOrgan(vtkOrgan * organ);

	//!Insert an organ in the specified position
	/*!
	* If the given position is not empty, the content is replaced by the new model
	* \param index index of the list where the organ will be added
	* \param organ vtkOrgan object to be inserted
	* \sa InsertNextOrgan(vtkOrgan * organ)
	* \return false if the insert operation was not correctly completed
	*/
	void InsertOrgan(vtkIdType index, vtkOrgan * organ);


	//!Delete the organ at the specified id from the scenario
	/*!
	* It will only remove the organ from the collection, it does not free memory
	* \param index index of the organ to be removed
	* \return false if the model could not be deleted
	*/
	void RemoveOrgan(vtkIdType index);

	//!Return the organ at the specified position
	/*!
	* \param id index of the organ to be returned
	* \return vtkOrgan object
	*/
	vtkOrgan * GetOrgan(vtkIdType id);

	//!Return number of organs in the scenario
	vtkIdType GetNumberOfOrgans();

	//!Set the collection of tools of the scenario
	/*!
	* \param collection collection of tools
	*/
	void SetToolCollection(vtkToolCollection * collection);

	//!Return the collection of tools of the scenario
	vtkToolCollection * GetToolCollection();

	//!Insert tool to the scenario
	/*!
	* \param tool vtkTool object to be inserted
	* \return the position of the inserted object in the collection
	*/
	void InsertNextTool(vtkTool * tool);

	//!Insert a tool in a given position in the scenario
	/*!
	* If the given position is not empty, the content is replaced by the new model
	* \param index index of the tool to be inserted
	* \param tool vtkTool object to be inserted
	* \return false if the insert operation was not correctly completed
	*/
	void InsertTool(vtkIdType index, vtkTool * tool);

	//!Delete the tool at the specified id from the scenario
	/*!
	It will only remove the tool from the collection, it does not liberate the memory
	\return false if the model could not be deleted
	*/
	void RemoveTool(vtkIdType index);

	//!Return the tool stored with the specified id
	/*!
	* \param id index of the tool to be returned
	*/
	vtkTool * GetTool(vtkIdType id);

	//!Return the number of tools contained in the scenario
	vtkIdType GetNumberOfTools();

	//!Assign the render window of the tool
	/*!
	* \param Renderer render window for displaying purposes
	* \sa GetRenderWindow()
	*/
	void SetRenderWindow(vtkRenderWindow *Renderer);

	//!Return the render window of the tool
	/*!
	* \sa SetRenderWindow(vtkRenderWindow *Renderer)
	*/
	vtkRenderWindow *GetRenderWindow();

	//!Update the whole Scenario
	/*!
	* Collision Detection is processed. If any contact has occurred the related tools and organs will be updated
	*/
	void Update();

protected:

	vtkScenario();
	~vtkScenario();

private:

	vtkScenario(const vtkScenario&);
	void operator=(const vtkScenario&);

	//!Collection of the scenario organs
	vtkOrganCollection * Organs;
	//!Collection of the scenario tools
	vtkToolCollection * Tools;

	//!Scenario render window
	vtkRenderWindow * RenderWindow;
	//!Scenario renderer
	vtkRenderer * Renderer;

};

#endif
