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

#include "painless.h"

#include "clauses/ClauseManager.h"
#include "sharing/SimpleSharing.h"
#include "sharing/Sharer.h"
#include "solvers/SolverFactory.h"
#include "utils/Logger.h"
#include "utils/Parameters.h"
#include "utils/SatUtils.h"
#include "utils/System.h"
#include "working/SequentialWorker.h"
#include "working/Portfolio.h"
#include "working/DivideAndConquer.h"

#include <unistd.h>

using namespace std;


// -------------------------------------------
// Declaration of global variables
// -------------------------------------------
atomic<bool> globalEnding(false);

Sharer ** sharers = NULL;

int nSharers = 0;

WorkingStrategy * working = NULL;

SatResult finalResult = UNKNOWN;

vector<int> finalModel;


// -------------------------------------------
// Main du framework
// -------------------------------------------
int main(int argc, char ** argv)
{
	Parameters::init(argc, argv);
	
	if (Parameters::getFilename() == NULL ||
		Parameters::isSet("h"))
	{
		printf("USAGE: %s [parameters] input.cnf\n", argv[0]);
		printf("Parameters:\n");
		printf("\t-solver=type\t\t glucose, maple, or combo, default maple\n");
		printf("\t-lbd-limit=<INT>\t lbd limit of exported clauses\n");
		printf("\t-d=0...7\t\t diversification 0=none, 1=sparse, 2=dense," \
			   " 3=random, 4=native, 5=1&4, 6=sparse-random, 7=6&4," \
			   " default is 0.\n");
		printf("\t-c=<INT>\t\t number of cpus, default is 4.\n");
		printf("\t-wkr-strat=1...3\t 1=divide and conquer (DC), 2=portfolio" \
			   " (PF), 3=PF of DC, default is DC");
		printf("\t-shr-strat=0...1\t 0=no sharing, 1=simple sharing, default is" \
			   " 0\n");
		printf("\t-shr-sleep=<INT>\t time in usecond a sharer sleep each" \
			   " round, default 500000 (0.5s)\n");
		printf("\t-no-model\t\t won't print the model if the problem is SAT\n");
		printf("\t-t=<INT>\t\t timeout in second, default is no limit\n");
		printf("\t-split-heur=1...3\t for D&C: splitting heuristic," \
			   " 1=VSIDS, 2=#flips, 3=propagation rate, default is 1\n");
		printf("\t-copy-mode=1...2\t for D&C: method to allocate new subspaces " \
			   "to workers, 1=no copy: reuses the old solver, 2=clone: clones " \
			   "the solver and delete old solver, default is 1\n");
		
		return 0;
	}
	
	log(2, "CNF file found !\n");
	Parameters::printParams();
	
	setVerbosityLevel(Parameters::getIntParam("v", 0));
	
	int cpus = Parameters::getIntParam("c", 4);
	
	srand(time(NULL));
	
	// Create solvers
	log(1, "painless.cpp\t - Create solvers\n");
	vector<SolverInterface *> solvers;
	
	const string solverType = Parameters::getParam("solver");
	const int wkrStrat      = Parameters::getIntParam("wkr-strat", 1);
	
	int nSolvers = cpus;
	if (wkrStrat == 1 && Parameters::getIntParam("copy-mode", 1) == 2) {
		nSolvers = 1;
	}
	
	if (solverType == "glucose") {
		SolverFactory::createGlucoseSolvers(nSolvers, solvers);
	} else if(solverType == "combo") {
		SolverFactory::createComboSolvers(nSolvers, solvers);
	} else {
		SolverFactory::createMapleSolvers(nSolvers, solvers);
	}
	
	// Diversifycation
	log(1, "painless.cpp\t - Diversification\n");
	int diversification = Parameters::getIntParam("d", 0);
	
	switch (diversification) {
		case 1 :
			SolverFactory::sparseDiversification(solvers);
			break;
			
		case 2 :
			SolverFactory::binValueDiversification(solvers);
			break;
			
		case 3 :
			SolverFactory::randomDiversification(solvers, 2015);
			break;
			
		case 4 :
			SolverFactory::nativeDiversification(solvers);
			break;
			
		case 5 :
			SolverFactory::sparseDiversification(solvers);
			SolverFactory::nativeDiversification(solvers);
			break;
			
		case 6 :
			SolverFactory::sparseRandomDiversification(solvers);
			break;
			
		case 7 :
			SolverFactory::sparseRandomDiversification(solvers);
			SolverFactory::nativeDiversification(solvers);
			break;
			
		case 0 :
			break;
	}
	
	// Start sharing threads
	log(1, "painless.cpp\t - Start sharing threads\n");
	switch(Parameters::getIntParam("shr-strat", 0)) {
		case 1 :
			nSharers   = 1;
			sharers    = new Sharer*[nSharers];
			sharers[0] = new Sharer(0, new SimpleSharing(), solvers, solvers);
			break;
			
		case 0 :
		default:
			break;
	}
	
	// Working strategy creation
	log(1, "painless.cpp\t - Working strategy creation\n");
	switch(wkrStrat) {
		// Question 4 :
		// We can use it withe "combo" solver option
		case 3 :
			log(1, "painless.cpp\t - DivideAndConquer strategy creation\n");
			
			int middle = 1;
			if (cpus > 2) {
				middle = (int)(cpus / 2);
			}
			WorkingStrategy * ws1, * ws 2;
			
			working = new Portfolio();
			ws1 = new DivideAndConquer();
			ws2 = new DivideAndConquer();
			for (size_t i = 0; i < middle; i++) {
				ws1->addSlave(new SequentialWorker(solvers[i]));
			}
			for (size_t i = middle; i < cpus; i++) {
				ws2->addSlave(new SequentialWorker(solvers[i]));
			}
			working.addSlave(ws1);
			Working.addSlave(ws2);
			break;
		// END question 4.
		case 2 :
			working = new Portfolio();
			for (size_t i = 0; i < cpus; i++) {
				working->addSlave(new SequentialWorker(solvers[i]));
			}
			break;
			
		case 1 :
		default :
			working = new DivideAndConquer();
			if(Parameters::getIntParam("copy-mode",1) == 2) {
				working->addSlave(new SequentialWorker(solvers[0]));
				for(size_t i = 1; i < cpus; i++) {
					working->addSlave(new SequentialWorker(NULL));
				}
			} else {
				for(size_t i = 0; i < cpus; i++) {
					working->addSlave(new SequentialWorker(solvers[i]));
				}
			}
			break;
	}
	
	// Init the management of clauses
	log(1, "painless.cpp\t - initClauseManager()\n");
	ClauseManager::initClauseManager();
	
	// Launch working
	log(1, "painless.cpp\t - Launch working\n");
	vector<int> cube;
	working->solve(cube);
	
	// Wait until end or timeout
	log(1, "painless.cpp\t - Wait until end or timeout\n");
	int timeout   = Parameters::getIntParam("t", -1);
	int maxMemory = Parameters::getIntParam("max-memory", -1) * 1024 * 1024;
	
	while(globalEnding == false) {
		sleep(1);
		
		if (maxMemory > 0 && getMemoryUsed() > maxMemory) {
			cout << "c Memory used is going too large!!!!" << endl;
		}
		
		if (timeout > 0 && getRelativeTime() >= timeout) {
			globalEnding = true;
			working->setInterrupt();
		}
	}
	
	// Delete sharers
	log(1, "painless.cpp\t - Delete sharers\n");
	for (int i=0; i < nSharers; i++) {
		delete sharers[i];
	}
	delete sharers;
	
	// Delete working strategy
	delete working;
	
	// Delete shared clauses
	ClauseManager::joinClauseManager();
	
	// Print solver stats
	SolverFactory::printStats(solvers);
	
	// Print the result and the model if SAT
	if (finalResult == SAT) {
		printf("s SATISFIABLE\n");
		
		if (Parameters::isSet("no-model") == false) {
			printModel(finalModel);
		}
	} else if (finalResult == UNSAT) {
		printf("s UNSATISFIABLE\n");
	} else {
		printf("s UNKNOWN\n");
	}
	
	return finalResult;
}
