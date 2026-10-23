#ifndef GRAPH_H
#define GRAPH_H

#define ASIZE 256

struct _graph;
typedef struct _graph *Graph;

Graph newAutomaton(int v, int e);
int newVertex(Graph g);
int getInitial(Graph g);
void setTerminal(Graph g, int v);
int isTerminal(Graph g, int v);
int getTarget(Graph g, int v, unsigned char c);
void setTarget(Graph g, int v, unsigned char c, int t);
void copyVertex(Graph g, int target, int source);

#endif