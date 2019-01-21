// -----------------------------------------------------------------------------
// Copyright (C) 2017  Ludovic LE FRIOUX
//
// This file is part of PaInleSS.
//
// PaInleSS is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
// -----------------------------------------------------------------------------

#include "../utils/Parameters.h"
#include "../utils/Logger.h"
#include "../working/Portfolio.h"
#include "../working/SequentialWorker.h"

using namespace std;

Portfolio::Portfolio()
{
   // TODO
}

Portfolio::~Portfolio()
{
   // TODO
}

void
Portfolio::solve(const vector<int> & cube)
{
   // TODO
}

void
Portfolio::join(WorkingStrategy * strat, SatResult res,
                const vector<int> & model)
{
   // TODO
}

void
Portfolio::setInterrupt()
{
   // TODO
}

void
Portfolio::unsetInterrupt()
{
   // TODO
}

void
Portfolio::waitInterrupt()
{
   // TODO
}

int
Portfolio::getDivisionVariable()
{
   // TODO
   return 0;
}

void
Portfolio::setPhase(int var, bool value)
{
   // TODO
}

void
Portfolio::bumpVariableActivity(int var, int times)
{
   // TODO
}
