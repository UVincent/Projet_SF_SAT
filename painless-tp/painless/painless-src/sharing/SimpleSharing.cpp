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
	/* Params: from = prod / to = cons */
	int i, j, k;

	/* Add all clauses from producers in tmp */
	for(i=0; i<from.size(); i++){
		from[i]->getLearnedClauses(tmp);
		stats.sharedClauses += tmp.size();
		stats.receivedClauses += tmp.size();
		/* Update all shared clauses refs to the total number of consumers */
		for(j=0; j<to.size(); j++){
			if(j != i){
				for(k=0; k<tmp.size(); k++){
					ClauseManager::increaseClause(tmp[k], 1);
				}
				to[j]->addLearnedClauses(tmp);
			} 
		}
		/* Put them to all consumers */
		for(auto &shared_clause : tmp){
			ClauseManager::releaseClause(shared_clause);
		}
	}
}

SharingStatistics
SimpleSharing::getStatistics()
{
   return stats;
}
