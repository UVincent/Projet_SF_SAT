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

/* Le but du Portfolio est de lancer plusieurs SequentialWorkers en parallèle
 pour une formule donnée */

Portfolio::Portfolio()
{

}

Portfolio::~Portfolio()
{
	for(auto &sw : slaves){
		delete sw;
	}
}

void
Portfolio::solve(const vector<int> & cube)
{
	strategyEnding = false;

	/* Start the solving from all SW */
	for(auto &sw : slaves){
		sw->solve(cube);
	}
}

/* Following the given paper: 
– void join(SatResult res, int[*] model): used to notify the parent strat-
egy of the solving end. If the result is sat, model will contain an assignment
that satisfies the sub-formula treated by this node.
It is worth noting that the workflow must start by a call to the root’s solve
method and eventually ends by a call to the root’s join method. The propagation
of solving orders from a parent to one of its child nodes, is done by a call to the
solve method of this last. The results are propagated back from a child to its
parent by a call to the join method of this last. The solving can not be effective
without a call to the leaves’ solve methods.
*/
void
Portfolio::join(WorkingStrategy * strat, SatResult res,
				const vector<int> & model)
{
	setInterrupt();
	strategyEnding = true;

	if(parent == NULL){
		globalEnding = true;
		finalResult = res;
		if(res = SAT){
			finalModel = model;
		}
		else{ 
			parent->join(this, res, model);
		}
	}

	/* Make all SequantialWorker execute join */
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
		if(sw->getDivisionVariable() <= 0)
			return -1;
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
