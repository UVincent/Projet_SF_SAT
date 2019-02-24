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

/* Le but du Portfolio est de lancer plusieurs SequentialWorkers en parallèle pour une formule donnée */

Portfolio::Portfolio()
{
   strategyEnding = false;
}

Portfolio::~Portfolio()
{
   strategyEnding = true;
   globalEnding = false;
}

void
Portfolio::solve(const vector<int> & cube)
{
   for(auto &sw : slaves){
        sw->solve(cube);
   }
}

void
Portfolio::join(WorkingStrategy * strat, SatResult res,
                const vector<int> & model)
{
   for(auto &sw : slaves){
        sw->join(strat, res, model);
   }

}

void
Portfolio::setInterrupt()
{
   for(auto &sw : slaves){
        sw->setInterrupt();
   }
}

void
Portfolio::unsetInterrupt()
{
   for(auto &sw : slaves){
        sw->unsetInterrupt();
   }
}


void
Portfolio::waitInterrupt()
{
   for(auto &sw : slaves){
        sw->waitInterrupt();
   }
}

int
Portfolio::getDivisionVariable()
{
   for(auto &sw : slaves){
        sw->getDivisionVariable();
   }

   return 0;
}

void
Portfolio::setPhase(int var, bool value)
{
   for(auto &sw : slaves){
        sw->setPhase(var, value);
   }
}

void
Portfolio::bumpVariableActivity(int var, int times)
{
   for(auto &sw : slaves){
        sw->bumpVariableActivity(var, times);
   }
}
