#include "pch.h"
#include "ConstBuffer.h"

#include "Device.h"

ConstBuffer::ConstBuffer()
	: m_Desc{}
	, m_Type(CB_TYPE::END)
{
}

ConstBuffer::~ConstBuffer()
{
}

void ConstBuffer::Create(CB_TYPE _Type, UINT _BufferSize)
{
	m_Type = _Type;

	m_Desc = {};
	m_Desc.ByteWidth = _BufferSize;
	m_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DEVICE->CreateBuffer(&m_Desc, nullptr, m_CB.GetAddressOf());
}

void ConstBuffer::SetData(void* _SysMem, UINT _Size)
{
	assert(m_Desc.ByteWidth >= _Size);

	if(0 == _Size)
		_Size = m_Desc.ByteWidth;

	D3D11_MAPPED_SUBRESOURCE mapSub = {};
	CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSub);

	memcpy(mapSub.pData, _SysMem, _Size);

	CONTEXT->Unmap(m_CB.Get(), 0);
}

void ConstBuffer::Binding()
{
	CONTEXT->VSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->HSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->DSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->GSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
	CONTEXT->PSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
}

void ConstBuffer::Binding_CS()
{
	CONTEXT->CSSetConstantBuffers((UINT)m_Type, 1, m_CB.GetAddressOf());
}
