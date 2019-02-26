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
   ListElement *node = new ListElement(NULL);

   /* Initialisation variables */
   this->buffer.head = node;
   this->buffer.tail = node;
}

ClauseBuffer::~ClauseBuffer()
{
  delete this->buffer.head;
  delete this->buffer.tail;
}

//-------------------------------------------------
//  Add clause(s)
//-------------------------------------------------
void
ClauseBuffer::addClause(ClauseExchange * clause)
{
   ListElement *node = new ListElement(clause);
   ListElement *next = new ListElement(NULL);
   ListElement *tail = new ListElement(NULL);

   while(1){
      tail = this->buffer.tail;
      next = tail->next;
      if(tail == this->buffer.tail){
         if(next->next == NULL){
            if(!(tail->next.compare_exchange_strong(next, node))){
               next->clause->size++;
               break;
            }
            else{
               this->buffer.tail.compare_exchange_strong(tail, next);
               tail->clause->size++;
            }
         }
      }
   }
   this->buffer.tail.compare_exchange_strong(tail, node);
   this->buffer.size++;
   tail->clause->size++;
}

void
ClauseBuffer::addClauses(const vector<ClauseExchange *> & clauses)
{
   for(auto &clause : clauses){
      addClause(clause);
   }
}


//-------------------------------------------------
//  Get clause(s)
//-------------------------------------------------
bool
ClauseBuffer::getClause(ClauseExchange ** clause)
{
   ListElement *head = new ListElement(NULL);
   ListElement *tail = new ListElement(NULL);
   ListElement *next = new ListElement(NULL);
   while(1){
      head = this->buffer.head;
      tail = this->buffer.tail;
      next = head->next;
      if(head == this->buffer.head){
         if(head == tail){
            if(next == NULL){
               return false;
            }
            this->buffer.tail.compare_exchange_strong(tail, next);
            tail->clause->size++;
         }
         else{
            clause = &(next->clause);
            if(this->buffer.head.compare_exchange_strong(head, next)){
               head->clause->size++;
               break;
            }
         }
      }
   }
   free(head);
   return true;
}

void
ClauseBuffer::getClauses(vector<ClauseExchange *> & clauses)
{
   for(auto &clause : clauses){
      getClause(&clause);
   }
}

//-------------------------------------------------
//  Get size of the buffer.
//-------------------------------------------------
int
ClauseBuffer::size()
{
   return this->buffer.size;
}

