#ifndef GRAPH_H
#define GRAPH_H

#define ASIZE 256


struct _graph; 
typedef struct _graph *Graph;

Graph newGraph(int v, int e);
Graph newAutomaton(int v, int e);
Graph newSuffixAutomaton(int v, int e);
int newVertex(Graph g);
int getInitial(Graph g);
bool isTerminal(Graph g, int v);
void setTerminal(Graph g, int v);
int getTarget(Graph g, int v, unsigned char c);
void setTarget(Graph g, int v, unsigned char c, int t);
int getSuffixLink(Graph g, int v);
void setSuffixLink(Graph g, int v, int s);
int getLength(Graph g, int v);
void setLength(Graph g, int v, int ell);
int getPosition(Graph g, int v);
void setPosition(Graph g, int v, int p);
int getShift(Graph g, int v, unsigned char c);
void setShift(Graph g, int v, unsigned char c, int s);
void copyVertex(Graph g, int target, int source);

#endif