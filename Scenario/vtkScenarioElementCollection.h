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
#ifndef __vtkScenarioElementCollection_h
#define __vtkScenarioElementCollection_h

#include "vtkESQuiScenarioWin32Header.h"
#include "vtkCollection.h"

class vtkScenarioElement;

//!Collection of ScenarioElements
class VTK_ESQUI_SCENARIO_EXPORT vtkScenarioElementCollection : public vtkCollection
{
public:
  //! Type revision macro
  vtkTypeRevisionMacro(vtkScenarioElementCollection,vtkCollection);
  //! Create new element storage collection
  static vtkScenarioElementCollection *New() {return new vtkScenarioElementCollection;}
  //! Return classname
  const char *GetClassName() {return "vtkScenarioElementCollection";}
  //! Print collection values
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  //! Insert (replace) an element into the collection
  /*!
   * Element will be added at the specified id replacing its old value.
   * \param id index of the list where the element will be added
   * \param ScenarioElement vtkScenarioElement object to be inserted
   * \sa InsertNextScenarioElement(vtkScenarioElement * ScenarioElement);
   */
  void ReplaceElement(vtkIdType id, vtkScenarioElement * ScenarioElement);

  //! Insert element at the end of the collection
  /*!
   * \param ScenarioElement vtkScenarioElement object to be inserted
   * \sa ReplaceScenarioElement(vtkIdType id, vtkScenarioElement * ScenarioElement)
   */
  void AddElement(vtkScenarioElement * ScenarioElement);

  //! Remove an element of the collection at the specified id
  /*!
   * \param id index of the element in the collection
   * \sa ReplaceScenarioElement(vtkIdType id, vtkScenarioElement * ScenarioElement)
   * \sa AddScenarioElement(vtkScenarioElement * ScenarioElement)
   */
  void RemoveElement(vtkIdType id);

  //! Return element located the specified id
  /*!
   * Get the element in the list with the position id. NULL is returned when the collection is exhausted.
   * \param id list index where the element is stored
   */
  vtkScenarioElement * GetElement(vtkIdType id);

  //! Return element on the collection
  /*!
   * Get the element at the pointer position. Beware of calling InitTraversal() to init the pointer. NULL is returned when the collection is exhausted.
   */
  vtkScenarioElement * GetNextElement();

  //! Return number of elements in the collection
  vtkIdType GetNumberOfElements();

protected:
  vtkScenarioElementCollection() {};
  ~vtkScenarioElementCollection() {};

private:
  vtkScenarioElementCollection(const vtkScenarioElementCollection&);  // Not implemented.
  void operator=(const vtkScenarioElementCollection&);  // Not implemented.
};


#endif
