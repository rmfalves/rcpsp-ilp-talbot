#ifndef PLANNING
#define PLANNING

#include <vector.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ilcplex/ilocplex.h>

#include "graph.cpp"

ILOSTLBEGIN

#define FALSE 0
#define TRUE  1

#define REDIRECT
#define LINEAR_RELAXATION

class ProjectPlanning {

    public:

	typedef enum {SS,SF,FS,FF} SequenceType;

	struct Activity {
		int earliest_start,latest_start,earliest_finish,latest_finish;
		int duration,set_start;
		int start,finish; 	// Optimize at verification time
		vector<int> successor,predecessor; 

	Activity() {duration = -1; set_start = -1; earliest_start = latest_start = earliest_finish = latest_finish = 0;}

	};

	struct ActivitySequence {
		int activity1,activity2;
		SequenceType sequence;

		ActivitySequence(int activity1,int activity2,SequenceType sequence)
		{
			this->activity1 = activity1;
			this->activity2 = activity2;
			this->sequence = sequence;
		}

	};
 
	int times,activities,resources;
	vector<vector<int> > act_res;
	vector<int> availability;
	vector<Activity> activity;
	vector<ActivitySequence> activity_sequence;

	void Build(int num_activities,int num_resources) {
		act_res.resize(num_resources);
		for (unsigned res = 0; res < num_resources; res++) {
			act_res[res].resize(num_activities);
			for (unsigned act = 0 ; act < num_activities; act++)
        			act_res[res][act] = 0;
		}
		availability.resize(num_resources);
		for (unsigned res = 0 ; res < num_resources; res++)
			availability[res] = 1;
		for (unsigned act = 0; act < num_activities; act++)
			activity.push_back(Activity());
		resources = num_resources;
		activities = num_activities;
	}

	~ProjectPlanning() {
		activity.clear();
		activity_sequence.clear();
		for (unsigned res = 0; res < act_res.size(); res++)
			act_res[res].clear();
		act_res.clear();
		availability.clear();
	}

	inline int index(int activity,int time) {return((times + 1) * activity + time);}

	void defineActivity(int act,int duration,int start) {
		activity[act].duration = duration;
		activity[act].set_start = start;
	}

	void defineActivitySequence(int act1,int act2,SequenceType seq) {
		activity_sequence.push_back(ActivitySequence(act1,act2,seq));
		if (seq == FS) {
			activity[act1].successor.push_back(act2);
			activity[act2].predecessor.push_back(act1);
		}
	}

	void connectStartActivity() {
		for (int act = 1; act < activities - 1; act++)
			if (activity[act].predecessor.size() == 0)
				defineActivitySequence(0,act,FS);
	}

	void connectTerminalActivity() {
		for (int act = 1; act < activities - 1; act++)
			if (activity[act].successor.size() == 0)
				defineActivitySequence(act,activities - 1,FS);
	}

	bool checkActivities() {
		for (int i = 0; i < activity.size(); i++)
			if (activity[i].duration == -1)	return false;
		return true;
	}


	void setResourceAvailability(int resource,int amount) {
		availability[resource] = amount;
	}


	void allocResource(int activity,int resource,int amount) {
		act_res[resource][activity] = amount;
	}

	/***************************** Formulation ***********************************/

	void buildObjectiveFunction(IloEnv &env,IloModel &model,IloNumVarArray& x) {
		IloExpr expr(env);
		int st = activity[activities - 1].earliest_finish;
		int fi = activity[activities - 1].latest_finish;
		for (int t = st; t <= fi; t++)
			expr += t * x[index(activities - 1,t)];
		model.add(IloMinimize(env,expr));
		expr.end();
	}

	void buildUniqueFinish(IloEnv& env,IloModel& model,IloNumVarArray& x) {
		for (int act = 0; act < activities; act++) {
			IloExpr expr(env);
			int st = activity[act].earliest_finish;
			int fi = activity[act].latest_finish;
			for (int t = st; t <= fi; t++)
				expr += x[index(act,t)];
			model.add(IloRange(env,1,expr,1));
			expr.end();
		}
	}

	void buildActivitySequencing(IloEnv& env,IloModel& model,IloNumVarArray& x) {
		for (int seq = 0; seq < activity_sequence.size(); seq++) {
			IloExpr expr(env);
			int act1 = activity_sequence[seq].activity1;
			int act2 = activity_sequence[seq].activity2;
			int st1 = activity[act1].earliest_finish;
			int fi1 = activity[act1].latest_finish;
			int st2 = activity[act2].earliest_finish;
			int fi2 = activity[act2].latest_finish;
			for (int t = st1; t <= fi1; t++)
				expr -= t * x[index(act1,t)];
			for (int t = st2; t <= fi2; t++)
				expr += (t - activity[act2].duration) * x[index(act2,t)];
			model.add(IloRange(env,0,expr,IloInfinity));
			expr.end();
		}
	}

	void buildResourcesAvailability(IloEnv& env,IloModel& model,IloNumVarArray& x) {
		for (int res = 0; res < act_res.size(); res++)
			for (int t = 0; t <= times; t++) {
				IloExpr expr(env);
				for (int act = 0; act < activities; act++) {
					if (act_res[res][act] == 0) continue;
					for (int q = t; q < t + activity[act].duration && q < times; q++)
						expr += act_res[res][act] * x[index(act,q)];
				}
				model.add(IloRange(env,-IloInfinity,expr,availability[res]));
				expr.end();
			}
	}

	void buildFormula(IloEnv& env,IloModel& model,IloNumVarArray& x) {
		buildObjectiveFunction(env,model,x);
		buildUniqueFinish(env,model,x);
		buildActivitySequencing(env,model,x);
		buildResourcesAvailability(env,model,x);
	}

	/*****************************************************************************/

	void dump() {
		printf("\n==============================================================================\n");
		printf("Activities Duration:\n");
		for (unsigned i = 0; i < activities; i++)
			printf("(%d --> %d) ",i,activity[i].duration);
		printf("\n\nActivities Sequence:\n");
		for (unsigned i = 0; i < activity_sequence.size(); i++)
			printf("(%d,%d,%d) ",activity_sequence[i].activity1,activity_sequence[i].activity2,activity_sequence[i].sequence);
		printf("\n==============================================================================\n");
	}

	void report(FILE *f) {
		fprintf(f,"\n     ");
		for (int t = 1; t <= activity[activities - 1].finish; t++)
			fprintf(f,"T%02d ",t);		
		fprintf(f,"\n");
		for (int t = 0; t <= activity[activities - 1].finish; t++)
			fprintf(f,"====");
		fprintf(f,"\n");
		for (int act = 1; act < activities - 1; act++) {
			fprintf(f,"A%02d|",act);		
			for (int t = 1; t < activity[act].start; t++)
				fprintf(f,"    ");
			for (int t = activity[act].start; t <= activity[act].finish; t++)
				fprintf(f,"####");
			fprintf(f,"\n"); }
		fprintf(f,"\n");
		fprintf(f,"minimal time = %d\n",activity[activities - 1].earliest_finish);
		fprintf(f,"maximal time = %d\n",times);
		fprintf(f,"optimal time = %d\n",activity[activities - 1].finish);
	}

	int estimateMaxTime() {
	// Estimate a maximum time horizon by adding up every activity duration.
		int sum = 0;
		for (int i = 0; i < activities; i++,sum += activity[i].duration);
		return(sum);
	}

	void criticalPath() {
	// Set earliest start time and latest finish time for every activity.
		// Walk list ahead

		activity[0].earliest_start = 0;
		activity[0].earliest_finish = activity[0].earliest_start + activity[0].duration;

		for (int i = 0; i < activities; i++)
			for (int j = 0; j < activity[i].predecessor.size(); j++) {
				int pred = activity[i].predecessor[j];
				if (activity[i].earliest_start < activity[pred].earliest_finish)
					activity[i].earliest_start = activity[pred].earliest_finish;
				activity[i].earliest_finish = activity[i].earliest_start + activity[i].duration;
			}
		
		// Walk list back
		times = estimateMaxTime();
		activity[activities - 1].latest_finish = times;
		activity[activities - 1].latest_start = activity[activities - 1].latest_finish - activity[activities - 1].duration;
		for (int i = activities - 2; i >= 0; i--) {
			for (int j = 0; j < activity[i].successor.size(); j++) {
				int succ = activity[i].successor[j];
				if (activity[i].latest_finish == 0)
					activity[i].latest_finish = activity[succ].latest_start;
				else if (activity[i].latest_finish > activity[succ].latest_start)
					activity[i].latest_finish = activity[succ].latest_start;
			}
			activity[i].latest_start = activity[i].latest_finish - activity[i].duration;
		}
	}

	int extractResult(IloNumVarArray& x,IloCplex& cplex) {
		int t;
		for (int act = 0; act < activities; act++) {
			for (t = index(act,activity[act].latest_finish); !cplex.getValue(x[t]); t--);
			activity[act].finish = t % (times + 1);
			activity[act].start = activity[act].finish - activity[act].duration + 1; 
		}
		return activity[activities - 1].finish;
	}

	bool solve(char *report_file) {

		IloEnv env;
		IloModel model(env);
		IloCplex cplex(model);

		criticalPath();

#ifndef LINEAR_RELAXATION
		IloNumVarArray x(env,activities * (times + 1),0,1,ILOINT);
#else
		IloNumVarArray x(env,activities * times,0,1,ILOFLOAT);
		x.add(IloNumVarArray(env,times,0,10,ILOINT));
#endif
		buildFormula(env,model,x);

		// Redirect CPLEX output to stderr
		int save_stdout = dup(1);
		close(1);
		dup(2);

		cplex.solve();

		// Restore output
		close(1);
		dup(save_stdout);

#ifdef REDIRECT
		printf("%d\n",activity[activities - 1].earliest_finish);
		printf("%d\n",activity[activities - 1].latest_finish);
		printf("%d\n",cplex.getStatus() == IloAlgorithm::Infeasible ? 0 : extractResult(x,cplex));
#else 
		FILE *f = report_file ? fopen(report_file,"wt") : stdout;
		if (!f) return false;
		if (cplex.getStatus() == IloAlgorithm::Infeasible)
			fprintf(f,"Problema impossivel\n");
		else {
			extractResult(x,cplex);
			report(f);
		}
		if (f != stdout) fclose(f);
#endif
		close(save_stdout);
		x.end();
		cplex.end();
		model.end();
		env.end();

		verify();
		return true;
	}

	ActivitySequence *buildSequenceCycle(int *path) {
		ActivitySequence *buffer = (ActivitySequence *)malloc((activities + 1) * sizeof(ActivitySequence));
		int size = 0;
		for (int i = 1; path[i] != -1; i++) {
			int act1 = path[i - 1] / 2;
			int act2 = path[i] / 2;
			if (act1 == act2) continue;
			buffer[size].activity1 = act1; 
			buffer[size].activity2 = act2; 
			buffer[size++].sequence = (SequenceType)((path[i - 1] & 1) * 2 + (path[i] & 1));
		}
		buffer[size].activity1 = -1; 
		buffer[size++].activity2 = -1;
		return (ActivitySequence *)realloc(buffer,size * sizeof(ActivitySequence)); 
	}

	ActivitySequence *checkSequences() {
		int *cycle;
		graph g(activities * 2);
		for (int i = 0,sz = activities << 1; i < sz; i += 2)
			g.newEdge(i,i + 1);
		for (int i = 0; i < activity_sequence.size(); i++) {
			int lin = (activity_sequence[i].activity1 << 1) + (activity_sequence[i].sequence >> 1); 
			int col = (activity_sequence[i].activity2 << 1) + (activity_sequence[i].sequence & 1); 
			g.newEdge(lin,col);
		}
		cycle = g.hasCycle();
		return cycle == NULL ? NULL : buildSequenceCycle(cycle);
	}

	/***************************** Verification ***********************************/

	void verify() {
    		if (!verifyStartFinishIntervals()) return;
	    	if (!verifyActivitySequencing()) return;
    		if (!verifyResourcesAvailability()) return;
	    	fprintf(stderr,"Verifies ok\n");
	}

	bool verifyStartFinishIntervals() {
		for (int act = 0; act < activities; act++) {
			if (activity[act].start < activity[act].earliest_start || activity[act].start > activity[act].latest_start ||
			    activity[act].finish < activity[act].earliest_finish || activity[act].finish > activity[act].latest_finish) {
					fprintf(stderr,"Error in start finish interval, activity %d\n",act);
					return false;
			}
		}
		return true;
	}

	bool verifyActivitySequencing() {
		for (int seq = 0; seq < activity_sequence.size(); seq++) {
			int act1 = activity_sequence[seq].activity1;
			int act2 = activity_sequence[seq].activity2;
			if (activity[act1].finish >= activity[act2].start) {
					fprintf(stderr,"Error in sequencing, activities %d and %d\n",act1,act2);
					return false;
			}
		}
		return true;
	}

	bool verifyResourcesAvailability() {
		for (int res = 0; res < resources; res++)
			for (int t = 0; t <= activity[activities - 1].finish; t++) {
				int sum = 0;
				for (int act = 0; act < activities; act++) {
					if (act_res[res][act] == 0 || t < activity[act].start || t > activity[act].finish) continue;
					sum += act_res[res][act];
				}
				if (sum > availability[res]) {
					fprintf(stderr,"Error in resource allocation, resource %d at time %d\n",res + 1,t);
					return false;
				}
			}
		return true;
	}

};

#endif
