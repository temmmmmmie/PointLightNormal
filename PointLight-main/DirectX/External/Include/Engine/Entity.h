#pragma once

class Entity
{
private:
	static UINT	g_NextID;

private:
	wstring		m_Name;
	UINT		m_ID;
	int			m_RefCount;

public:
	GET_SET(wstring, Name);
	GET(UINT, ID);

public:
	Entity& operator =(const Entity& _Other) {  m_Name = _Other.m_Name; return *this; }

public:
	void AddRef() { m_RefCount++; }
	void Release() 
	{
		--m_RefCount;
		if (m_RefCount <= 0)
			delete this;
	}

public:
	
	virtual Entity* Clone() = 0;

public:
	Entity();
	Entity(const Entity& _Origin);
	virtual ~Entity();
};


#define CLONE(Type) public: virtual Type* Clone() override { return new Type(*this); };
#define CLONE_DISABLE(Type) private: virtual Type* Clone() override { return nullptr; };