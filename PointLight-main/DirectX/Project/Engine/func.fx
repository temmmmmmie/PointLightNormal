#ifndef _FUNC
#define _FUNC

#include "value.fx"
#include "struct.fx"

#define Light2D g_Light2D[_LightIdx]

void CalcLight2D(int _LightIdx, float3 _vWorldPixelPos, inout float3 _LightColor)
{   
    // Directional Light
    if (0 == Light2D.Type)
    {
        _LightColor += Light2D.LightColor;
    }
    
    // Point Light
    else if (1 == Light2D.Type)
    {
        // 거리에 따른 빛의 감쇄 비율
        float DistRatio = 1.f;
        
        float Dist = distance(_vWorldPixelPos.xy, Light2D.WorldPos.xy);       
                
        if (Dist < Light2D.Radius)
        {
            //DistRatio = saturate(1.f - (Dist / Light2D.Radius));
            DistRatio = saturate(cos(Dist * ((PI / 2.f) / Light2D.Radius)));            
            _LightColor += Light2D.LightColor * DistRatio;
        }
    }
    
    // Spot Light
    else
    {
        
    }   
}

void CalcLight2D(float3 _WorldPixelPos, inout float3 _LightColor)
{
    for (int i = 0; i < Light2DCount; ++i)
    {
        CalcLight2D(i, _WorldPixelPos, _LightColor);
    }
}


void CalcLight3D(float3 _ViewPos, float3 _ViewNormal, int _LightIdx
               , inout float3 _Diffuse, inout float3 _Ambient, inout float3 _Specular)
{
    // 빛 계산
    float LightPow = 0.f;
    float ReflectPow = 0.f;
    float DistRatio = 1.f;
    float3 LightViewDir = (float3) 0.f;
    
    // Directional
    if (0 == g_Light3D[_LightIdx].Type)
    {
        // 빛의 진입각도에 따른 세기(램버트 코사인 법칙) 구하기
        LightViewDir = -mul(float4(normalize(g_Light3D[_LightIdx].Direction), 0.f), g_matView).xyz;
        LightPow = saturate(dot(_ViewNormal, LightViewDir));
    }
    
    // Point
    else if (1 == g_Light3D[_LightIdx].Type)
    {
        // View 공간에서의 광원의 중심 위치
        float3 vLightViewPos = mul(float4(g_Light3D[_LightIdx].WorldPos, 1.f), g_matView).xyz;
        
        // 빛의 진입각도에 따른 세기(램버트 코사인 법칙) 구하기
        LightViewDir = normalize(vLightViewPos - _ViewPos);
        LightPow = saturate(dot(_ViewNormal, LightViewDir));
        
        // 거리에 따른 빛의 세기 감소비율
        // 픽셀 위치와 광원 위치 사이의 거리
        float Dist = distance(_ViewPos, vLightViewPos);
        
        // 거리에 따른 감소된 빛의 세기
        DistRatio = saturate(cos(Dist * ((PI * 0.5f) / g_Light3D[_LightIdx].Radius)));
    }
    
    // Spot
    else
    {
        
    }
    
    // 뷰스페이스에서는 카메라가 원점이기 때문에, pixel 의 View좌표가 곧 카메라가 pixel 을 향하는 방향벡터
    float3 vEye = normalize(_ViewPos);
    
    // 반사벡터
    float3 vReflect = normalize(-LightViewDir + 2.f * dot(LightViewDir, _ViewNormal) * _ViewNormal);
    
    // 반사광 세기 == 시선벡터 dot 반사벡터
    ReflectPow = saturate(dot(-vEye, vReflect));
    ReflectPow = pow(ReflectPow, 10);
        
    // 빛의 총량
    _Diffuse += (g_Light3D[_LightIdx].LightColor * LightPow * DistRatio);    
    
    // 환경광 세기
    _Ambient += g_Light3D[_LightIdx].Ambient;    
    
    // 반사광 세기
    _Specular += ReflectPow * g_Light3D[_LightIdx].LightColor * DistRatio * g_SpecCoef.xyz;
}


int IntersectsRay(float3 _Pos[3], float3 _vStart, float3 _vDir
                  , out float3 _CrossPos, out uint _Dist)
{
    // 삼각형 표면 방향 벡터
    float3 Edge[2] = { (float3) 0.f, (float3) 0.f };
    Edge[0] = _Pos[1] - _Pos[0];
    Edge[1] = _Pos[2] - _Pos[0];
    
    // 삼각형에 수직방향인 법선(Normal) 벡터
    float3 Normal = normalize(cross(Edge[0], Edge[1]));
    
    // 삼각형 법선벡터와 Ray 의 Dir 을 내적
    // 광선에서 삼각형으로 향하는 수직벡터와, 광선의 방향벡터 사이의 cos 값
    float NdotD = -dot(Normal, _vDir);
        
    float3 vStoP0 = _vStart - _Pos[0];
    float VerticalDist = dot(Normal, vStoP0); // 광선을 지나는 한점에서 삼각형 평면으로의 수직 길이
            
    // 광선이 진행하는 방향으로, 삼각형을 포함하는 평면까지의 거리
    float RtoTriDist = VerticalDist / NdotD;
        
    // 광선이, 삼각형을 포함하는 평면을 지나는 교점
    float3 vCrossPoint = _vStart + RtoTriDist * _vDir;
        
    // 교점이 삼각형 내부인지 테스트
    float3 P0toCross = vCrossPoint - _Pos[0];
    
    float3 Full = cross(Edge[0], Edge[1]);
    float3 U = cross(Edge[0], P0toCross);
    float3 V = cross(Edge[1], P0toCross);
       
    // 직선과 삼각형 평면의 교점이 삼각형 1번과 2번 사이에 존재하는지 체크
    //      0
    //     /  \
    //    1 -- 2    
    if (dot(U, Full) < 0.f || 0.f < dot(V, Full))
        return 0;
    
    // 교점이 삼각형 내부인지 체크
    if (length(Full) < length(U) + length(V))
        return 0;
        
    _CrossPos = vCrossPoint;
    _Dist = (uint) RtoTriDist;
    
    return 1;
}

matrix GetBoneMat(int _iBoneIdx, int _iRowIdx)
{
    return g_arrBoneMat[(g_BoneCount * _iRowIdx) + _iBoneIdx];
}

void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
    , inout float4 _vWeight, inout float4 _vIndices
    , int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
        info.vTangent += (mul(float4(_vTangent, 0.f), matBone) * _vWeight[i]).xyz;
        info.vBinormal += (mul(float4(_vBinormal, 0.f), matBone) * _vWeight[i]).xyz;
        info.vNormal += (mul(float4(_vNormal, 0.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = info.vPos;
    _vTangent = normalize(info.vTangent);
    _vBinormal = normalize(info.vBinormal);
    _vNormal = normalize(info.vNormal);
}


void Skinning_LocalPos(inout float3 _vPos, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx)
{
    tSkinningInfo info = (tSkinningInfo) 0.f;

    if (_iRowIdx == -1)
        return;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx);

        info.vPos += (mul(float4(_vPos, 1.f), matBone) * _vWeight[i]).xyz;
    }

    _vPos = info.vPos;
}

#endif