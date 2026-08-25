#pragma once
#include "Asset.h"

#include "AGraphicShader.h"
#include "ATexture.h"

// 물체의 재질을 설정한다.
// 렌더링 방식(파이프라인, 쉐이더) 와, 쉐이더에 전달할 리소스 데이터(텍스쳐, 상수 등) 를 묶은 개념
class AMaterial :
	public Asset
{
private:
	tMtrlConst              m_Const;
	Ptr<AGraphicShader>     m_Shader;
	Ptr<ATexture>           m_Tex[TEX_END];
	RENDER_DOMAIN			m_Domain; // 해당 재질의 렌더링 시점

public:
	GET_SET(Ptr<AGraphicShader>, Shader);

	template<typename T>
	void SetScalar(SCALAR_PARAM _Type, const T& _Data);

	template<typename T>
	T& GetScalar(SCALAR_PARAM _Type);

	void SetDiffuseCoeffcient(Vec3 _Diff) { m_Const.DiffCoef = _Diff; }
	void SetSpecularCoeffcient(Vec3 _Spec) { m_Const.SpecCoef = _Spec; }
	void SetAmbientuseCoeffcient(Vec3 _Amb) { m_Const.AmbCoef = _Amb; }
	void SetEmissiveuseCoeffcient(Vec3 _Emiss) { m_Const.EmissCoef = _Emiss; }

	Vec3 GetDiffuseCoeffcient() { return m_Const.DiffCoef; }
	Vec3 GetSpecularCoeffcient() { return  m_Const.SpecCoef; }
	Vec3 GetAmbientuseCoeffcient() { return m_Const.AmbCoef; }
	Vec3 GetEmissiveuseCoeffcient() { return m_Const.EmissCoef; }

	void SetDomain(RENDER_DOMAIN _Domain) { m_Domain = _Domain; }
	RENDER_DOMAIN GetDomain() { return m_Domain; }

	// 재질끼리 대입할때 사용할 대입연산자
	void operator = (const AMaterial& _OtherMtrl)
	{
		SetName(_OtherMtrl.GetName());

		m_Const = _OtherMtrl.m_Const;

		for (UINT i = 0; i < TEX_END; ++i)
		{
			m_Tex[i] = _OtherMtrl.m_Tex[i];
		}

		m_Shader = _OtherMtrl.m_Shader;

		m_Domain = _OtherMtrl.m_Domain;
	}

	// 재질의 계수 부분 세팅하는 함수
	void SetMaterialCoefficient(Vec4 _vDiff, Vec4 _vSpec, Vec4 _vAmb, Vec4 _vEmis)
	{
		m_Const.DiffCoef = _vDiff;
		m_Const.SpecCoef = _vSpec;
		m_Const.AmbCoef = _vAmb;
		m_Const.EmissCoef = _vEmis;
	}

	void SetAnim3D(bool _bTrue) { m_Const.arrAnimData[0] = (int)_bTrue; }
	void SetBoneCount(int _iBoneCount) { m_Const.arrAnimData[1] = _iBoneCount; }
	void SetShadowMat(Matrix& _start) {
		m_Const.ViewProj = _start;
	}

public:
	void SetTexture(TEX_PARAM _Param, Ptr<ATexture> _Tex) { m_Tex[_Param] = _Tex; }
	Ptr<ATexture> GetTexture(TEX_PARAM _Param) { return m_Tex[(UINT)_Param]; }

	void Binding();

public:
	virtual int Save(const wstring& _FilePath) override;
	virtual int Load(const wstring& _FilePath) override;


	CLONE(AMaterial);
public:
	AMaterial(bool _EngineAsset = false);
	virtual ~AMaterial();

};

// 함수템플릿 안에서 T 타입 구별하기, T 타입에 따른 예외처리
template<typename T>
void AMaterial::SetScalar(SCALAR_PARAM _Type, const T& _Data)
{
	switch (_Type)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		if constexpr (std::is_same_v<T, int> || std::is_same_v<T, UINT>)
		{
			m_Const.iArr[_Type - INT_0] = _Data;
		}
		break;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		if constexpr (std::is_same_v<T, float>)
		{
			m_Const.fArr[_Type - FLOAT_0] = _Data;
		}
		break;
	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		if constexpr (std::is_same_v<T, Vec2>)
		{
			m_Const.v2Arr[_Type - VEC2_0] = _Data;
		}
		break;
	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		if constexpr (std::is_same_v<T, Vec4> || std::is_same_v<T, Vec3>)
		{
			m_Const.v4Arr[_Type - VEC4_0] = _Data;
		}
		break;
	case MAT_0:
	case MAT_1:
		if constexpr (std::is_same_v<T, Matrix>)
		{
			m_Const.matArr[_Type - MAT_0] = _Data;
		}
		break;
	}
}

template<typename T>
inline T& AMaterial::GetScalar(SCALAR_PARAM _Type)
{
	switch (_Type)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		if constexpr (std::is_same_v<T, int> || std::is_same_v<T, UINT>)
		{
			return m_Const.iArr[_Type - INT_0];
		}
		break;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		if constexpr (std::is_same_v<T, float>)
		{
			return m_Const.fArr[_Type - FLOAT_0];
		}
		break;
	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		if constexpr (std::is_same_v<T, Vec2>)
		{
			return m_Const.v2Arr[_Type - VEC2_0];
		}
		break;
	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		if constexpr (std::is_same_v<T, Vec4> || std::is_same_v<T, Vec3>)
		{
			return m_Const.v4Arr[_Type - VEC4_0];
		}
		break;
	case MAT_0:
	case MAT_1:
		if constexpr (std::is_same_v<T, Matrix>)
		{
			return m_Const.matArr[_Type - MAT_0];
		}
		break;
	}
}
