#pragma once

union COLLIDER_ID
{
	struct
	{
		UINT LEFT;
		UINT Right;
	};
	LONGLONG ID;
};


class CollisionMgr
	: public singleton<CollisionMgr>
{
	SINGLE(CollisionMgr);
private:
	UINT				m_Matrix[MAX_LAYER];
	map<LONGLONG, bool>	m_mapColInfo;



public:
	void Progress();

public:
	void CollisionCheck(UINT _LeftLayer, UINT _RightLayer);
	void CollisionCheckClear();

private:
	void CollisionBtwLayer(UINT _Left, UINT _Right);
	bool IsOverlap(class CCollider2D* _LeftCol, class CCollider2D* _RightCol);


};

