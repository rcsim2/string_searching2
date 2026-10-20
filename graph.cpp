#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

struct _graph {
    int vertexNum;
    int edgeNum;
    int initial;
    int *terminal;
    int **target;
};

Graph newAutomaton(int v, int e) {
    Graph g = (Graph)malloc(sizeof(struct _graph));
    g->vertexNum = 0;
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