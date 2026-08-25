#pragma once


template<typename T>
class Ptr
{
private:
	T*	m_Target;

public:
	T* Get() const { return m_Target; }
	T* operator ->() const { return m_Target; }
	T** GetAdressOf() const { return &m_Target; }


public:
	Ptr& operator = (const Ptr<T>& _OtherPtr)
	{
		if (nullptr != m_Target)
			m_Target->Release();

		m_Target = _OtherPtr.m_Target;

		if (nullptr != m_Target)
			m_Target->AddRef();

		return *this;
	}

	bool operator == (T* _Ptr) const { return m_Target == _Ptr; }
	bool operator != (T* _Ptr) const { return m_Target != _Ptr; }
	bool operator == (const Ptr<T>& _OtherPtr) const { return m_Target == _OtherPtr.m_Target; }
	bool operator != (const Ptr<T>& _OtherPtr) const { return m_Target != _OtherPtr.m_Target; }

public:
	Ptr()
		: m_Target(nullptr)
	{}

	Ptr(T* _Entity)
		: m_Target(_Entity)
	{
		if(nullptr != m_Target)
			m_Target->AddRef();
	}

	Ptr(const Ptr<T>& _OtherPtr)
		: m_Target(_OtherPtr.m_Target)
	{
		if (nullptr != m_Target)
			m_Target->AddRef();
	}

	~Ptr()
	{
		if (nullptr != m_Target)
			m_Target->Release();
	}
};


template<typename T>
bool operator == (void* _pointer, const Ptr<T>& _Ptr)
{
	return _pointer == _Ptr.Get();
}

template<typename T>
bool operator != (void* _pointer, const Ptr<T>& _Ptr)
{
	return _pointer != _Ptr.Get();
}