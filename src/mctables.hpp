//Author: Paul Bourke http://paulbourke.net/geometry/polygonise/

#ifndef MCTABLES_H
#define MCTABLES_H

class MCTables {
public:
	static const int EDGE_TABLE[256];
	static const int TRI_TABLE[256][16];
};

#endif