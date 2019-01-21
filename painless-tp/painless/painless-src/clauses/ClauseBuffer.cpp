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

#include "ClauseBuffer.h"

using namespace std;

//-------------------------------------------------
// Constructor & Destructor
//-------------------------------------------------
ClauseBuffer::ClauseBuffer()
{
	// Instancier la liste
	ListElement *my_buffer = new *ListElement;

}

ClauseBuffer::~ClauseBuffer()
{
   // TODO
}

//-------------------------------------------------
//  Add clause(s)
//-------------------------------------------------
void
ClauseBuffer::addClause(ClauseExchange * clause)
{
   // TODO
}

void
ClauseBuffer::addClauses(const vector<ClauseExchange *> & clauses)
{
   // TODO
}


//-------------------------------------------------
//  Get clause(s)
//-------------------------------------------------
bool
ClauseBuffer::getClause(ClauseExchange ** clause)
{
   // TODO
   return false;
}

void
ClauseBuffer::getClauses(vector<ClauseExchange *> & clauses)
{
   // TODO
}

//-------------------------------------------------
//  Get size of the buffer.
//-------------------------------------------------
int
ClauseBuffer::size()
{
   // TODO
   return 0;
}
