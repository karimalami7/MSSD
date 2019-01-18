#include "bskytree.h"


/*
void ExecuteSSkyline(vector<int>& AttList, vector<Point>& PointList, vector<Point>& Skyline);
void InsertSkyline(vector<Point>& SkylineList, SNode& SkyNode);
void ClearSkyTree(SNode& SkyTree);
void PushStack(stack<SNode>& Stack, SNode& SkyNode);
void SelectPivotPoint(vector<int>& AttList, vector<Point>& PointList);
int ComputeDistance(vector<int>& AttList, Point CPoint);
bool EvaluatePoint(vector<int>& AttList, int nCPos, vector<Point>& PointList);
extern int nGlobalAtt;
extern unsigned long long nGMeasure;
extern vector<int> GlobalAttList;
bool ComparePointID(Point FirPoint, Point SecPoint);
bool CompareAtt(Point FirPoint, Point SecPoint);
bool CompareMultipleAtt(Point FirPoint, Point SecPoint);
void SetSubspaceList(int nNumAtt, vector<int>* SubspaceList);
void SortPointList(int nNumAtt, vector<Point>& PointList, vector<vector<Point> >& SPointList);
int ExecuteBSkyTree(vector<int>& AttList, vector<Point>& PointList, vector<Point>& Skyline);*/
//void ComputeSubBSkyTree(vector<int>& AttList, vector<Point>& PointList, SNode& SkyTree);
//void MapPointToRegion(vector<int>& AttList, vector<Point>& PointList, map<int, vector<Point> >& PointMap, SNode& SkyTree);
//void PartialDominance(vector<int>& AttList, int nBase, vector<Point>& PointList, SNode& SkyTree);
//bool FilterPoint(Point& CPoint, vector<int>& AttList, SNode& SkyNode);


int ComputeDistance(vector<int>& AttList, Point CPoint)
{
	int nCurAtt, nNumAtt = (int)AttList.size();
	Space dMax, dMin;

	dMax = dMin = CPoint[AttList[0]];
	for (int nAttID = 1; nAttID < nNumAtt; nAttID++)
	{
		nCurAtt = AttList[nAttID];
		if (dMax < CPoint[nCurAtt])
			dMax = CPoint[nCurAtt];
		else if (dMin > CPoint[nCurAtt])
			dMin = CPoint[nCurAtt];
	}

	return dMax - dMin;
}


bool EvaluatePoint(vector<int>& AttList, int nCPos, vector<Point>& PointList)
{
	bool bSkyline = true, bDominate;
	int nCurAtt, nNumAtt = (int)AttList.size();

	Point CPoint = PointList[nCPos], SPoint;
	for (int nPnt = 1; nPnt < nCPos; nPnt++)
	{

		SPoint = PointList[nPnt];

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bDominate = true;
		for (int nAttID = 0; nAttID < nNumAtt; nAttID++)
		{
			nCurAtt = AttList[nAttID];
			if (CPoint[nCurAtt] < SPoint[nCurAtt])
			{
				bDominate = false;
				break;
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (bDominate)
		{
			bSkyline = false;
			break;
		}
	}

	return bSkyline;
}

void SelectPivotPoint(vector<int>& AttList, vector<Point>& PointList)
{
	int nHead = 0, nTail = (int)PointList.size() - 1, nCPos = 1;
	int nCurAtt, nNumAtt = (int)AttList.size();

	Point CPoint, HPoint = PointList[nHead], Temp;
	double dCurDist, dMinDist = ComputeDistance(AttList, HPoint);

	while (nCPos <= nTail)
	{
		CPoint = PointList[nCPos];
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool bDominate = false, bDominated = false;
		for (int nAttID = 0; nAttID < nNumAtt; nAttID++)
		{
			nCurAtt = AttList[nAttID];
			if (HPoint[nCurAtt] < CPoint[nCurAtt])
				bDominate = true;
			else if (HPoint[nCurAtt] > CPoint[nCurAtt])
				bDominated = true;

			if (bDominate && bDominated)
				break;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (bDominate && !bDominated)
		{
			PointList[nCPos] = PointList[nTail];
			PointList.pop_back();
			nTail--;
		}
		else if (!bDominate && bDominated)
		{
			PointList[nHead] = PointList[nCPos];
			PointList[nCPos] = PointList[nTail];
			PointList.pop_back();
			nTail--;

			HPoint = PointList[nHead];
			dMinDist = ComputeDistance(AttList, HPoint);
		}
		else
		{
			dCurDist = ComputeDistance(AttList, CPoint);

			if (dCurDist < dMinDist)
			{
				if (EvaluatePoint(AttList, nCPos, PointList))
				{
					Temp = PointList[nHead];
					PointList[nHead] = PointList[nCPos];
					PointList[nCPos] = Temp;

					HPoint = PointList[nHead];
					dMinDist = dCurDist;
					nCPos++;
				}
				else
				{
					PointList[nCPos] = PointList[nTail];
					PointList.pop_back();
					nTail--;
				}
			}
			else
				nCPos++;
		}
	}
}

void MapPointToRegion(vector<int>& AttList, vector<Point>& PointList, map<int, vector<Point> >& PointMap, SNode& SkyTree)
{
	int nCurAtt, nNumAtt = (int)AttList.size();
	int nNumPnt = (int)PointList.size();
	int nLattice, nEqlLattice, nPruned = (1 << nNumAtt)  - 1;

	Point CPoint, BasisPoint = PointList[0];
	SkyTree.NodePointList.push_back(PointList[0]);

	for (int nPnt = 1; nPnt < nNumPnt; nPnt++)
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		nLattice = 0, nEqlLattice = nPruned;
		CPoint = PointList[nPnt];
		for (int nAttID = 0; nAttID < nNumAtt; nAttID++)
		{
			nCurAtt = AttList[nAttID];
			if (BasisPoint[nCurAtt] < CPoint[nCurAtt])
				nLattice |= 1 << nAttID;
			else if (BasisPoint[nCurAtt] == CPoint[nCurAtt])
			{
				nLattice |= 1 << nAttID;
				nEqlLattice ^= 1 << nAttID;
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (nLattice < nPruned)
		{
			nLattice &= nEqlLattice;
			if (PointMap.find(nLattice) != PointMap.end())
				PointMap[nLattice].push_back(PointList[nPnt]);
			else
			{
				vector<Point> CPointList;
				CPointList.push_back(PointList[nPnt]);
				PointMap.insert( pair<int, vector<Point> >(nLattice, CPointList));
			}
		}
		else if (nEqlLattice == 0)
			SkyTree.NodePointList.push_back(PointList[nPnt]);
	}
}


bool FilterPoint(Point& CPoint, vector<int>& AttList, SNode& SkyNode)
{
	int nCurAtt, nNumAtt = AttList.size();
	Point SPoint = SkyNode.NodePointList[0];

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int nPruned = (1 << nNumAtt) - 1;
	int nLattice = 0, nEqlLattice = nPruned;
	for (int nAttID = 0; nAttID < nNumAtt; nAttID++)
	{
		nCurAtt = AttList[nAttID];
		if (SPoint[nCurAtt] < CPoint[nCurAtt])
			nLattice |= 1 << nAttID;
		else if (SPoint[nCurAtt] == CPoint[nCurAtt])
		{
			nLattice |= 1 << nAttID;
			nEqlLattice ^= 1 << nAttID;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (nLattice == nPruned)
		return false;
	else
	{
		nLattice &= nEqlLattice;
		if (SkyNode.nNumChild > 0)
		{
			int nCLattice, nNumChild = SkyNode.nNumChild;
			for (int nChild = 0; nChild < nNumChild; nChild++)
			{
				nCLattice = SkyNode.ChildNode[nChild].nLatticeID;
				if (nCLattice <= nLattice)
				{
					if ((nCLattice & nLattice) == nCLattice)
					{
						if (!FilterPoint(CPoint, AttList, SkyNode.ChildNode[nChild]))
							return false;
					}
				}
				else
					break;
			}
		}

		return true;
	}
}



void PartialDominance(vector<int>& AttList, int nBase, vector<Point>& PointList, SNode& SkyTree)
{
	int nCLattice;//, nNumAtt = (int)AttList.size();
	int nNumPnt, nNumChild = SkyTree.nNumChild;

	for (int nChild = 0; nChild < nNumChild; nChild++)
	{
		nCLattice = SkyTree.ChildNode[nChild].nLatticeID;
		if (nCLattice <= nBase)
		{
			if ((nCLattice & nBase) == nCLattice)
			{
				nNumPnt = (int)PointList.size();
				for (int nPnt = 0; nPnt < nNumPnt; nPnt++)
				{
					if (!FilterPoint(PointList[nPnt], AttList, SkyTree.ChildNode[nChild]))
					{
						PointList[nPnt] = PointList[nNumPnt-1];
						PointList.pop_back();

						nPnt--, nNumPnt--;
					}
				}

				if (PointList.size() == 0)
					break;
			}
		}
		else
			break;
	}
}

void ComputeSubBSkyTree(vector<int>& AttList, vector<Point>& PointList, SNode& SkyTree)
{
	int nLatticeID, nNumChild = 0;
	vector<Point> CPointList;
	map<int, vector<Point> > PointMap;

	SelectPivotPoint(AttList, PointList);											// Pivot selection
	MapPointToRegion(AttList, PointList, PointMap, SkyTree);		// Map points to binary vectors representing subregions.

	if (!PointMap.empty())
		SkyTree.ChildNode = new SNode[PointMap.size()];

	for (map<int, vector<Point> >::iterator it = PointMap.begin(); it != PointMap.end(); it++)
	{
		nLatticeID = (*it).first;
		CPointList = (*it).second;

		if (nNumChild > 0)
		{
			SkyTree.nNumChild = nNumChild;
			PartialDominance(AttList, nLatticeID, CPointList, SkyTree);			// Partial dominance check
		}

		if (CPointList.size() == 1)
		{
			SkyTree.ChildNode[nNumChild].nLatticeID = nLatticeID;
			SkyTree.ChildNode[nNumChild].NodePointList = CPointList;
			SkyTree.ChildNode[nNumChild++].nNumChild = 0;
		}
		else if (CPointList.size() > 1)
		{
			SkyTree.ChildNode[nNumChild].nLatticeID = nLatticeID;
			ComputeSubBSkyTree(AttList, CPointList, SkyTree.ChildNode[nNumChild++]);	// Recursive call.
		}
	}

	SkyTree.nNumChild = nNumChild;
}


void InsertSkyline(vector<Point>& SkylineList, SNode& SkyNode)
{
	int nNumChild = (int)SkyNode.nNumChild;
	if (nNumChild > 0)
	{
		int nNumPnt = (int)SkyNode.NodePointList.size();
		for (int nPnt = 0; nPnt < nNumPnt; nPnt++)
			SkylineList.push_back(SkyNode.NodePointList[nPnt]);

		for (int nChild = 0; nChild < nNumChild; nChild++)
			InsertSkyline(SkylineList, SkyNode.ChildNode[nChild]);
	}
	else
	{
		int nNumPnt = (int)SkyNode.NodePointList.size();
		for (int nPnt = 0; nPnt < nNumPnt; nPnt++)
			SkylineList.push_back(SkyNode.NodePointList[nPnt]);
	}
}


void PushStack(stack<SNode>& Stack, SNode& SkyNode)
{
	int nNumChild = (int)SkyNode.nNumChild;
	if (nNumChild > 0)
	{
		Stack.push(SkyNode);
		for (int nChild=0; nChild<nNumChild; nChild++)
			PushStack(Stack, SkyNode.ChildNode[nChild]);
	}
}


void ClearSkyTree(SNode& SkyTree)
{
	stack<SNode> Stack;
	PushStack(Stack, SkyTree);

	while (!Stack.empty())
	{
		delete[] Stack.top().ChildNode;
		Stack.pop();
	}
}


void ExecuteBSkyTree(vector<int>& AttList, vector<Point>& PointList, vector<Point>& skyline)
{
	SNode SkyTree;
	SkyTree.nLatticeID = 0;

	vector<Point> CPointList = PointList;

	ComputeSubBSkyTree(AttList, CPointList, SkyTree);
	InsertSkyline(skyline, SkyTree);
	ClearSkyTree(SkyTree);
}

vector<Point> subspaceSkylineSize_TREE(vector<int>& AttList, vector<Point>& PointList){
    vector<Point> skyline;
    ExecuteBSkyTree(AttList, PointList, skyline);
    return skyline;
}


