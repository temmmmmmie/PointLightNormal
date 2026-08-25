#pragma once

template<typename T>
class singleton
{
private:
	static T* m_This;

public:
	static T* GetInst()
	{
		if (nullptr == m_This)
		{
			m_This = new T;
		}

		return m_This;		
	}

	static void Destroy()
	{
		if (nullptr != m_This)
		{
			delete m_This;
			m_This = nullptr;
		}
	}

protected:
	singleton()
	{
		typedef void(*DESTORY)(void);
		atexit((DESTORY)&Destroy);
	}

	virtual ~singleton()
	{

	}
};


template<typename T>
T* singleton<T>::m_This = nullptr;