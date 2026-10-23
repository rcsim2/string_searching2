#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

/////////////////////////////////////////////////////////////////////
// NOTE: There was a bug in the original code which allowed XFrame 1]
// to be found in the text even though it was not present.
//
// State 0 is the automaton’s initial state, but newAutomaton also 
// initializes vertexNum to 0. So the first call to newVertex returns 0.
// In preAut, that makes the transition for the pattern’s first character
// point back to the initial state, instead of advancing to a new state.
// That lets the automaton match the rest of the pattern without requiring
// its first character.
//
// The fix is to reserve state 0 for the initial state by starting vertexNum
// at 1. Then the first new state is 1, and the existing m + 1 allocation
// has room for the initial state plus all m pattern states.
/////////////////////////////////////////////////////////////////////

struct _graph {
    int vertexNum;
    int edgeNum;
    int initial;
    int *terminal;
    int **target;
};

Graph newAutomaton(int v, int e) {
    Graph g = (Graph)malloc(sizeof(struct _graph));
    g->vertexNum = 1; //0; // Reserve state 0 for the initial state
    g->edgeNum = 0;
    g->initial = 0;
    g->terminal = (int *)calloc(v, sizeof(int));
    g->target = (int **)malloc(v * sizeof(int *));
    for (int i = 0; i < v; i++) {
        g->target[i] = (int *)calloc(ASIZE, sizeof(int));
    }
    return g;
}

int newVertex(Graph g) {
    return g->vertexNum++;
}

int getInitial(Graph g) {
    return g->initial;
}

void setTerminal(Graph g, int v) {
    g->terminal[v] = 1;
}

int isTerminal(Graph g, int v) {
    return g->terminal[v];
}

int getTarget(Graph g, int v, unsigned char c) {
    return g->target[v][c];
}

void setTarget(Graph g, int v, unsigned char c, int t) {
    g->target[v][c] = t;
}

void copyVertex(Graph g, int target, int source) {
    for (int i = 0; i < ASIZE; i++) {
        g->target[target][i] = g->target[source][i];
    }
}