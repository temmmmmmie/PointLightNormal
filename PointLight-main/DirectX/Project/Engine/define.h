#pragma once

#define SINGLE(Type) private:\
						Type();\
						Type(const Type& _Origin) = delete;\
						virtual ~Type();\
						Type& operator =(const Type& _Other) = delete;\
						friend class singleton<Type>;

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;

#define GET(Type, MemName) Type Get##MemName() const {return m_##MemName;}
#define SET(Type, MemName) void Set##MemName(Type _Data) { m_##MemName = _Data;}
#define GET_SET(Type, MemName) GET(Type, MemName); SET(Type, MemName)

#define GET_REF(Type, MemName) const Type& Get##MemName() const {return m_##MemName;}
#define SET_REF(Type, MemName) void Set##MemName(const Type& _Data){m_##MemName = _Data;}
#define GET_SET_REF(Type, MemName) GET_REF(Type, MemName); SET_REF(Type, MemName)



#define DEVICE Device::GetInst()->GetDevice().Get()
#define CONTEXT Device::GetInst()->GetContext().Get()

#define MAX_LAYER 32