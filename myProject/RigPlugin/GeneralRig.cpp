#include "StdAfx.h"
#include "GeneralRig.h"

using namespace RigFEM;

GeneralRig::GeneralRig(RigSimulationNode* node, int nParam, int nPnts):RigBase(nParam), m_node(node), m_nPnts(nPnts)
{
}

GeneralRig::~GeneralRig(void)
{
}

MStatus RigFEM::GeneralRig::getMeshPoints( double* points )
{
	// 获取网格对象
	MStatus s;
	MPlug meshPlug = Global::getPlug(m_node, RigSimulationNode::meshLongName());
	MObject meshObj = meshPlug.asMObject(MDGContext::fsNormal, &s);
	MFnMesh meshFn(meshObj, &s);
	CHECK_MSTATUS_AND_RETURN_IT(s);

	int nVtx = meshFn.numVertices(&s);
	if (nVtx != m_nPnts)
		return MStatus::kFailure;

	// 获取变换矩阵
	MPlug matrixPlug = Global::getPlug(m_node, RigSimulationNode::transformLongName());
	MObject matrixObj = matrixPlug.asMObject();
	MFnMatrixData matrixData(matrixObj, &s);
	CHECK_MSTATUS_AND_RETURN_IT(s);
	MMatrix matrix    = matrixData.matrix();

	MItMeshVertex it(meshObj, &s);
	for (int i=0; !it.isDone(&s) && i < m_nPnts*3; it.next())
	{
		MPoint p = it.position(MSpace::kWorld)*matrix;
		points[i++] = p.x;
		points[i++] = p.y;
		points[i++] = p.z;
	}
	return s;
}

MStatus RigFEM::GeneralRig::setParamPlug()
{
	MStatus s = m_node->setParamPlug(m_param, m_nParam);
	return s;
}

bool RigFEM::GeneralRig::computeValue( double* result, const double* params /*= 0*/ )
{
	if (params)
	{
		setFreeParam(params);
	}
	MStatus s = setParamPlug();
	if (s == MStatus::kSuccess)
	{
		s = getMeshPoints(result);
	}
	return s == MStatus::kSuccess ? true : false;
}

MStatus RigFEM::GeneralRig::fetchParamFromNode()
{
	MStatus s;
	MPlug paramArrayPlug = Global::getPlug(m_node, RigSimulationNode::initParamLongName());
	for (int ithParam = 0; ithParam < m_nParam; ++ithParam)
	{
		MPlug paramPlug = paramArrayPlug.elementByLogicalIndex(ithParam,&s);
		if (!s)
			break;
		paramPlug.getValue(m_param[ithParam]);
	}
	return s;
}

MStatus RigFEM::GeneralRig::getMesh( MObject& meshObj )
{
	MStatus s;
	MPlug meshPlug = Global::getPlug(m_node, RigSimulationNode::meshLongName());
	meshObj = meshPlug.asMObject(MDGContext::fsNormal, &s);
	return s;
}

bool RigFEM::GeneralRig::computeExternalForce( const EigVec& pos, const EigVec& vel, const EigVec& m, double time, EigVec& extForce, EigVec& surfForce )
{
	if (!m_node)
		return false;

	return m_node->computeExternalForce(pos, vel, m, time, extForce, surfForce);
}

bool RigFEM::GeneralRig::getControlParams( EigVec& targetParam, EigVec& propGain, EigVec& deriGain )
{
	if (!m_node)
	{
		return false;
	}

	return m_node->getControlParams(targetParam, propGain, deriGain);
}

bool RigFEM::GeneralRig::getControlTarget( EigVec& targetParam )
{
	if (!m_node)
	{
		return false;
	}

	return m_node->getControlTarget(targetParam);
}

bool RigFEM::GeneralRig::getControlGain( EigVec& propGain, EigVec& deriGain )
{
	if (!m_node)
	{
		return false;
	}

	return m_node->getControlGain(propGain, deriGain);
}
