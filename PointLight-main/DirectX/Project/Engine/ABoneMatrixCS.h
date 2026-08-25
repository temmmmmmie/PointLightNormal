#pragma once
#include "AComputeShader.h"
#include "StructuredBuffer.h"

class ABoneMatrixCS :
    public AComputeShader
{
private:
    Ptr<StructuredBuffer>  m_FrameDataBuffer;	// t16
    Ptr<StructuredBuffer>  m_OffsetMatBuffer;	// t17 
    Ptr<StructuredBuffer>  m_OutputBuffer;		// u0

public:
    // g_int_0 : BonCount, g_int_1 : Frame Index
    void SetBoneCount(int _iBoneCount) { m_Const.iArr[0] = _iBoneCount; }
    void SetFrameIndex(int _iFrameIdx) { m_Const.iArr[1] = _iFrameIdx; }
    void SetNextFrameIdx(int _iFrameIdx) { m_Const.iArr[2] = _iFrameIdx; }
    void SetFrameRatio(float _fFrameRatio) { m_Const.fArr[0] = _fFrameRatio; }
    void SetFrameDataBuffer(Ptr<StructuredBuffer> _buffer) { m_FrameDataBuffer = _buffer; }
    void SetOffsetMatBuffer(Ptr<StructuredBuffer> _buffer) { m_OffsetMatBuffer = _buffer; }
    void SetOutputBuffer(Ptr<StructuredBuffer> _buffer) { m_OutputBuffer = _buffer; }

public:
    virtual int Binding() override;
    virtual void CalcGroupCount() override;
    virtual void Clear() override;

public:
    ABoneMatrixCS();
    virtual ~ABoneMatrixCS();
};

