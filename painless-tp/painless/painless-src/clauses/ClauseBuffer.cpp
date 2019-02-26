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
   puts("ClauseBuffer.cpp\t - new ClauseBuffer()");
   ListElement *node = new ListElement(NULL);

   /* Initialisation variables */
   this->buffer.head = node;
   this->buffer.tail = node;
   buffer.size = 0;
}

ClauseBuffer::~ClauseBuffer()
{
  puts("ClauseBuffer.cpp\t - delete ClauseBuffer()");
  delete this->buffer.head;
  delete this->buffer.tail;
}

//-------------------------------------------------
//  Add clause(s)
//-------------------------------------------------
void
ClauseBuffer::addClause(ClauseExchange * clause)
{
   //puts("ClauseBuffer.cpp\t - addClause(...)");
   ListElement *node = new ListElement(clause);
   ListElement *next = new ListElement(NULL);
   ListElement *tail = new ListElement(NULL);

   while(1){
      tail = this->buffer.tail;
      next = tail->next;
      //puts("ClauseBuffer.cpp\t - while(1)");
      //printf("ClauseBuffer.cpp\t - tailClauseSize : %d, tail->next : %s, nextClauseSize : %d.\n",
      //    tail->clause->size, tail->next, next->clause->size);
      if(tail == this->buffer.tail){
         //puts("*");
         if(next == NULL){
            //puts("**");
            if((tail->next.compare_exchange_strong(next, node))){
               //puts("if");
               int tmpb = buffer.size++;
               printf("buffSize : %d.\n", tmpb);
               break;
            }
         }
         else{
            //puts("else");
            this->buffer.tail.compare_exchange_weak(tail, next);
            printf("tailClauseSize : %d.\n", tail->clause->size);
            tail->clause->size++;
            //puts("else END");
         }
      }
   }
   //puts("END while");
   this->buffer.tail.compare_exchange_strong(tail, node);
   //int tmpB = this->buffer.size++;
   //int tmpT = tail->clause->size++;
   //printf("BufferSize : %d,\t", tmpB);
   //printf("tailClauseSize : %d.\n", tmpT);
   //puts("ClauseBuffer.cpp\t - addClause(...) : END");
}

void
ClauseBuffer::addClauses(const vector<ClauseExchange *> & clauses)
{
   puts("ClauseBuffer.cpp\t - addClauses(...)");
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
   //puts("ClauseBuffer.cpp\t - getClause(...)");
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
            *clause = next->clause;
            if(this->buffer.head.compare_exchange_strong(head, next)){
               head->clause->size++;
               break;
            }
         }
      }
   }
   free(head);
   this->buffer.size --;
   return true;
}

void
ClauseBuffer::getClauses(vector<ClauseExchange *> & clauses)
{
   puts("ClauseBuffer.cpp\t - getClauses(...)");
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

