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

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkScenario.h"
#include "vtkTool.h"
#include "vtkToolEndovascular.h"
#include "vtkToolCatheter.h"

//!This test perform a test of the vtkToolLaparoscopy class

int main(int argc, char * argv[])
{

  const char * filename0 = "/home/jballesteros/Workspace/data/vtkESQuiData/Scenario/Tools/Catheter/catheterTube.vtp";

  if (argc > 3)
  {
    filename0 = argv[1];
  }

  /**********  Render Window Definitions  ********/
  vtkRenderer * ren1 = vtkRenderer::New();
  ren1->SetBackground(1.0,1.0,1.0);

  vtkRenderWindow *renWin = vtkRenderWindow::New();
  renWin->AddRenderer(ren1);
  renWin->SetSize(840,480);

  vtkRenderWindowInteractor * iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);

  vtkToolCatheter * cath = vtkToolCatheter::New();
  //Set tool identifier
  cath->SetId(0);
  //Set source data filename
  cath->SetFileName(filename0);
  //Set geometric parameters
  cath->SetPosition(0, 0, 0);
  cath->SetOrientation(0, 0, 0);
  cath->SetOrigin(0, 0, 0);

  //Set tool scale (size)
  cath->SetScale(1.0, 1.0, 1.0);

  //Assign render window for display purposes
  cath->SetRenderWindow(renWin);
  cath->Initialize();

  iren->Initialize();

  renWin->Render();

  iren->Start();

  return 0;
}
