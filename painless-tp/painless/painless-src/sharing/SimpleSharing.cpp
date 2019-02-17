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

#include "../sharing/SimpleSharing.h"
#include "../clauses/ClauseManager.h"

SimpleSharing::SimpleSharing()
{
   // TODO
}

void
SimpleSharing::doSharing(int idSharer, const vector<SolverInterface *> & from,
                         const vector<SolverInterface *> & to)
{
	/* from = prod - to = cons */

	/* Add all clauses from producers in tmp */
	for(auto &solverInt : from){
		solverInt->getLearnedClauses(tmp);
	}
	/* Put them to all consumers */
	for(auto &solverInt : to){
		solverInt->addLearnedClauses(tmp);
	}
}

SharingStatistics
SimpleSharing::getStatistics()
{
   return stats;
}
