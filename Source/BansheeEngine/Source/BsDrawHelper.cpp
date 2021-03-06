//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsDrawHelper.h"
#include "BsMesh.h"
#include "BsAABox.h"
#include "BsSphere.h"
#include "BsVertexDataDesc.h"
#include "BsMeshHeap.h"
#include "BsShapeMeshes3D.h"
#include "BsTextData.h"
#include "BsVector2.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	const UINT32 DrawHelper::VERTEX_BUFFER_GROWTH = 4096;
	const UINT32 DrawHelper::INDEX_BUFFER_GROWTH = 4096 * 2;

	DrawHelper::DrawHelper()
		:mLayer(1), mNumActiveMeshes(0)
	{
		mTransform = Matrix4::IDENTITY;

		mSolidVertexDesc = bs_shared_ptr_new<VertexDataDesc>();
		mSolidVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mSolidVertexDesc->addVertElem(VET_FLOAT3, VES_NORMAL);
		mSolidVertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		mWireVertexDesc = bs_shared_ptr_new<VertexDataDesc>();
		mWireVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mWireVertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		mLineVertexDesc = bs_shared_ptr_new<VertexDataDesc>();
		mLineVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mLineVertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		mTextVertexDesc = bs_shared_ptr_new<VertexDataDesc>();
		mTextVertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);
		mTextVertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD);
		mTextVertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		mSolidMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mSolidVertexDesc);
		mWireMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mWireVertexDesc);
		mLineMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mLineVertexDesc);
		mTextMeshHeap = MeshHeap::create(VERTEX_BUFFER_GROWTH, INDEX_BUFFER_GROWTH, mTextVertexDesc);
	}

	DrawHelper::~DrawHelper()
	{
		BS_ASSERT(mNumActiveMeshes == 0 && "Not all DrawHelper meshes were freed on shutdown.");
	}

	void DrawHelper::setColor(const Color& color)
	{
		mColor = color;
	}

	void DrawHelper::setTransform(const Matrix4& transform)
	{
		mTransform = transform;
	}

	void DrawHelper::setLayer(UINT64 layer)
	{
		mLayer = layer;
	}

	void DrawHelper::cube(const Vector3& position, const Vector3& extents)
	{
		mSolidCubeData.push_back(CubeData());
		CubeData& cubeData = mSolidCubeData.back();

		cubeData.position = position;
		cubeData.extents = extents;
		cubeData.color = mColor;
		cubeData.transform = mTransform;
		cubeData.layer = mLayer;
		cubeData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::sphere(const Vector3& position, float radius, UINT32 quality)
	{
		mSolidSphereData.push_back(SphereData());
		SphereData& sphereData = mSolidSphereData.back();

		sphereData.position = position;
		sphereData.radius = radius;
		sphereData.quality = quality;
		sphereData.color = mColor;
		sphereData.transform = mTransform;
		sphereData.layer = mLayer;
		sphereData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::wireCube(const Vector3& position, const Vector3& extents)
	{
		mWireCubeData.push_back(CubeData());
		CubeData& cubeData = mWireCubeData.back();

		cubeData.position = position;
		cubeData.extents = extents;
		cubeData.color = mColor;
		cubeData.transform = mTransform;
		cubeData.layer = mLayer;
		cubeData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::wireSphere(const Vector3& position, float radius, UINT32 quality)
	{
		mWireSphereData.push_back(SphereData());
		SphereData& sphereData = mWireSphereData.back();

		sphereData.position = position;
		sphereData.radius = radius;
		sphereData.quality = quality;
		sphereData.color = mColor;
		sphereData.transform = mTransform;
		sphereData.layer = mLayer;
		sphereData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::line(const Vector3& start, const Vector3& end)
	{
		mLineData.push_back(LineData());
		LineData& lineData = mLineData.back();

		lineData.start = start;
		lineData.end = end;
		lineData.color = mColor;
		lineData.transform = mTransform;
		lineData.layer = mLayer;
		lineData.center = mTransform.multiplyAffine((start + end) * 0.5f);
	}

	void DrawHelper::lineList(const Vector<Vector3>& lines)
	{
		if (lines.size() < 2)
			return;

		mLineListData.push_back(LineListData());
		LineListData& lineListData = mLineListData.back();

		Vector3 center;
		for (auto& point : lines)
			center += point;

		lineListData.lines = lines;
		lineListData.color = mColor;
		lineListData.transform = mTransform;
		lineListData.layer = mLayer;
		lineListData.center = center / (float)lines.size();;
	}

	void DrawHelper::frustum(const Vector3& position, float aspect, Degree FOV, float near, float far)
	{
		mFrustumData.push_back(FrustumData());
		FrustumData& frustumData = mFrustumData.back();

		frustumData.position = position;
		frustumData.aspect = aspect;
		frustumData.FOV = FOV;
		frustumData.near = near;
		frustumData.far = far;
		frustumData.color = mColor;
		frustumData.transform = mTransform;
		frustumData.layer = mLayer;
		frustumData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::cone(const Vector3& base, const Vector3& normal, float height, float radius, const Vector2& scale, 
		UINT32 quality)
	{
		mConeData.push_back(ConeData());
		ConeData& coneData = mConeData.back();

		coneData.base = base;
		coneData.normal = normal;
		coneData.height = height;
		coneData.radius = radius;
		coneData.scale = scale;
		coneData.quality = quality;
		coneData.color = mColor;
		coneData.transform = mTransform;
		coneData.layer = mLayer;
		coneData.center = mTransform.multiplyAffine(base + normal * height * 0.5f);
	}

	void DrawHelper::wireCone(const Vector3& base, const Vector3& normal, float height, float radius, const Vector2& scale,
		UINT32 quality)
	{
		mWireConeData.push_back(ConeData());
		ConeData& coneData = mWireConeData.back();

		coneData.base = base;
		coneData.normal = normal;
		coneData.height = height;
		coneData.radius = radius;
		coneData.scale = scale;
		coneData.quality = quality;
		coneData.color = mColor;
		coneData.transform = mTransform;
		coneData.layer = mLayer;
		coneData.center = mTransform.multiplyAffine(base + normal * height * 0.5f);
	}

	void DrawHelper::disc(const Vector3& position, const Vector3& normal, float radius, UINT32 quality)
	{
		mDiscData.push_back(DiscData());
		DiscData& discData = mDiscData.back();

		discData.position = position;
		discData.normal = normal;
		discData.radius = radius;
		discData.quality = quality;
		discData.color = mColor;
		discData.transform = mTransform;
		discData.layer = mLayer;
		discData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::wireDisc(const Vector3& position, const Vector3& normal, float radius, UINT32 quality)
	{
		mWireDiscData.push_back(DiscData());
		DiscData& discData = mWireDiscData.back();

		discData.position = position;
		discData.normal = normal;
		discData.radius = radius;
		discData.quality = quality;
		discData.color = mColor;
		discData.transform = mTransform;
		discData.layer = mLayer;
		discData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::arc(const Vector3& position, const Vector3& normal, float radius, 
		Degree startAngle, Degree amountAngle, UINT32 quality)
	{
		mArcData.push_back(ArcData());
		ArcData& arcData = mArcData.back();

		arcData.position = position;
		arcData.normal = normal;
		arcData.radius = radius;
		arcData.startAngle = startAngle;
		arcData.amountAngle = amountAngle;
		arcData.quality = quality;
		arcData.color = mColor;
		arcData.transform = mTransform;
		arcData.layer = mLayer;
		arcData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::wireArc(const Vector3& position, const Vector3& normal, float radius, 
		Degree startAngle, Degree amountAngle, UINT32 quality)
	{
		mWireArcData.push_back(ArcData());
		ArcData& arcData = mWireArcData.back();

		arcData.position = position;
		arcData.normal = normal;
		arcData.radius = radius;
		arcData.startAngle = startAngle;
		arcData.amountAngle = amountAngle;
		arcData.quality = quality;
		arcData.color = mColor;
		arcData.transform = mTransform;
		arcData.layer = mLayer;
		arcData.center = mTransform.multiplyAffine(position);
	}

	void DrawHelper::rectangle(const Rect3& area)
	{
		mRect3Data.push_back(Rect3Data());
		Rect3Data& rectData = mRect3Data.back();

		rectData.area = area;
		rectData.color = mColor;
		rectData.transform = mTransform;
		rectData.layer = mLayer;
		rectData.center = mTransform.multiplyAffine(area.getCenter());
	}

	void DrawHelper::text(const Vector3& position, const WString& text, const HFont& font, UINT32 size)
	{
		if (!font.isLoaded() || text.empty())
			return;

		mText2DData.push_back(Text2DData());
		Text2DData& textData = mText2DData.back();

		textData.position = position;
		textData.color = mColor;
		textData.transform = mTransform;
		textData.layer = mLayer;
		textData.center = mTransform.multiplyAffine(position);
		textData.text = text;
		textData.font = font;
		textData.size = size;
	}

	void DrawHelper::wireMesh(const SPtr<MeshData>& meshData)
	{
		if (meshData == nullptr)
			return;

		mWireMeshData.push_back(WireMeshData());
		WireMeshData& wireMeshData = mWireMeshData.back();

		wireMeshData.meshData = meshData;
		wireMeshData.color = mColor;
		wireMeshData.transform = mTransform;
		wireMeshData.layer = mLayer;
		wireMeshData.center = mTransform.multiplyAffine(Vector3::ZERO);
	}

	void DrawHelper::clear()
	{
		mSolidCubeData.clear();
		mWireCubeData.clear();
		mSolidSphereData.clear();
		mWireSphereData.clear();
		mLineData.clear();
		mLineListData.clear();
		mRect3Data.clear();
		mFrustumData.clear();
		mFrustumData.clear();
		mDiscData.clear();
		mWireDiscData.clear();
		mArcData.clear();
		mWireArcData.clear();
		mConeData.clear();
		mWireConeData.clear();
		mText2DData.clear();
		mWireMeshData.clear();
	}

	void DrawHelper::buildMeshes(SortType sorting, const Vector3& reference, UINT64 layers)
	{
		mMeshes.clear();

		enum class ShapeType
		{
			Cube, Sphere, WireCube, WireSphere, WireCone, Line, LineList, Frustum, 
			Cone, Disc, WireDisc, Arc, WireArc, Rectangle, Text, WireMesh
		};

		struct RawData
		{
			ShapeType shapeType;
			MeshType meshType;
			UINT32 idx;
			UINT32 textIdx;
			float distance;
			UINT32 numVertices;
			UINT32 numIndices;
		};

		/************************************************************************/
		/* 			Sort everything according to specified sorting rule         */
		/************************************************************************/

		UINT32 idx = 0;
		Vector<RawData> allShapes;

		UINT32 localIdx = 0;
		for (auto& shapeData : mSolidCubeData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Cube;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsAABox(rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mSolidSphereData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Sphere;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsSphere(shapeData.quality, 
				rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mConeData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Cone;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsCone(shapeData.quality, 
				rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mDiscData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Disc;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsDisc(shapeData.quality,
				rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mArcData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Arc;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsArc(shapeData.quality, 
				rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mRect3Data)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Solid;
			rawData.shapeType = ShapeType::Rectangle;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsQuad(rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mWireCubeData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Line;
			rawData.shapeType = ShapeType::WireCube;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsWireAABox(rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mWireSphereData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Line;
			rawData.shapeType = ShapeType::WireSphere;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsWireSphere(shapeData.quality,
				rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mWireConeData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Line;
			rawData.shapeType = ShapeType::WireCone;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsWireCone(shapeData.quality,
				rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mLineData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Line;
			rawData.shapeType = ShapeType::Line;
			rawData.distance = shapeData.center.distance(reference);
			rawData.numVertices = 2;
			rawData.numIndices = 2;
		}

		localIdx = 0;
		for (auto& shapeData : mLineListData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			UINT32 numLines = (UINT32)shapeData.lines.size() / 2;
			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Line;
			rawData.shapeType = ShapeType::LineList;
			rawData.distance = shapeData.center.distance(reference);
			rawData.numVertices = numLines * 2;
			rawData.numIndices = numLines * 2;
		}

		localIdx = 0;
		for (auto& shapeData : mFrustumData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Line;
			rawData.shapeType = ShapeType::Frustum;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsFrustum(rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mWireDiscData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Line;
			rawData.shapeType = ShapeType::WireDisc;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsWireDisc(shapeData.quality, 
				rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mWireArcData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Line;
			rawData.shapeType = ShapeType::WireArc;
			rawData.distance = shapeData.center.distance(reference);

			ShapeMeshes3D::getNumElementsWireArc(shapeData.quality,
				rawData.numVertices, rawData.numIndices);
		}

		localIdx = 0;
		for (auto& shapeData : mWireMeshData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			allShapes.push_back(RawData());
			RawData& rawData = allShapes.back();

			rawData.idx = localIdx++;
			rawData.textIdx = 0;
			rawData.meshType = MeshType::Wire;
			rawData.shapeType = ShapeType::WireMesh;
			rawData.distance = shapeData.center.distance(reference);
			rawData.numVertices = shapeData.meshData->getNumVertices();
			rawData.numIndices = shapeData.meshData->getNumIndices();
		}

		struct TextRenderData
		{
			UINT32 page;
			SPtr<TextData<>> textData;
		};

		UnorderedMap<UINT32, TextRenderData> textRenderData;
		UINT32 textIdx = 0;

		localIdx = 0;
		for (auto& shapeData : mText2DData)
		{
			if ((shapeData.layer & layers) == 0)
			{
				localIdx++;
				continue;
			}

			SPtr<TextData<>> textData = bs_shared_ptr_new<TextData<>>(shapeData.text, shapeData.font, shapeData.size);

			UINT32 numPages = textData->getNumPages();
			for (UINT32 j = 0; j < numPages; j++)
			{
				UINT32 numQuads = textData->getNumQuadsForPage(j);

				allShapes.push_back(RawData());
				RawData& rawData = allShapes.back();

				rawData.idx = localIdx;
				rawData.textIdx = textIdx;
				rawData.meshType = MeshType::Text;
				rawData.shapeType = ShapeType::Text;
				rawData.distance = shapeData.center.distance(reference);
				rawData.numVertices = numQuads * 4; 
				rawData.numIndices = numQuads * 6;

				TextRenderData& renderData = textRenderData[textIdx];
				renderData.page = j;
				renderData.textData = textData;

				textIdx++;
				idx++;
			}

			localIdx++;
		}

		if (sorting == SortType::FrontToBack)
		{
			std::sort(begin(allShapes), end(allShapes),
				[&](const RawData& x, const RawData& y)
			{
				return x.distance < y.distance;
			});
		}
		else if (sorting == SortType::BackToFront)
		{
			std::sort(begin(allShapes), end(allShapes),
				[&](const RawData& x, const RawData& y)
			{
				return y.distance < x.distance;
			});
		}

		/************************************************************************/
		/* 							Create batches                      		*/
		/************************************************************************/
		struct Batch
		{
			MeshType type;
			HTexture texture;
			UINT32 startIdx;
			UINT32 endIdx;
			UINT32 numVertices;
			UINT32 numIndices;
		};

		UINT32 numShapes = (UINT32)allShapes.size();

		Vector<Batch> batches;
		if (numShapes > 0)
		{
			batches.push_back(Batch());

			{
				Batch& currentBatch = batches.back();
				currentBatch.startIdx = 0;
				currentBatch.type = allShapes[0].meshType;
				currentBatch.numVertices = allShapes[0].numVertices;
				currentBatch.numIndices = allShapes[0].numIndices;

				if (allShapes[0].meshType == MeshType::Text)
				{
					TextRenderData& renderData = textRenderData[allShapes[0].textIdx];
					currentBatch.texture = renderData.textData->getTextureForPage(renderData.page);
				}
			}

			for (UINT32 i = 1; i < numShapes; i++)
			{
				Batch& currentBatch = batches.back();

				HTexture texture;
				if (allShapes[i].meshType == MeshType::Text)
				{
					TextRenderData& renderData = textRenderData[allShapes[i].textIdx];
					texture = renderData.textData->getTextureForPage(renderData.page);
				}

				bool startNewBatch = allShapes[i].meshType != currentBatch.type || texture != currentBatch.texture;
				if (startNewBatch)
				{
					currentBatch.endIdx = i - 1;

					batches.push_back(Batch());

					Batch& newBatch = batches.back();
					newBatch.startIdx = i;
					newBatch.type = allShapes[i].meshType;
					newBatch.numVertices = allShapes[i].numVertices;
					newBatch.numIndices = allShapes[i].numIndices;
					newBatch.texture = texture;
				}
				else
				{
					currentBatch.endIdx = i;
					currentBatch.numVertices += allShapes[i].numVertices;
					currentBatch.numIndices += allShapes[i].numIndices;
				}
			}

			{
				Batch& currentBatch = batches.back();
				currentBatch.endIdx = numShapes - 1;
			}
		}

		/************************************************************************/
		/* 					Generate geometry for each batch                    */
		/************************************************************************/
		for (auto& batch : batches)
		{
			if (batch.type == MeshType::Solid)
			{
				SPtr<MeshData> meshData = bs_shared_ptr_new<MeshData>(batch.numVertices, batch.numIndices, mSolidVertexDesc);

				UINT32 curVertexOffset = 0;
				UINT32 curIndexOffet = 0;

				auto positionIter = meshData->getVec3DataIter(VES_POSITION);
				auto normalIter = meshData->getVec3DataIter(VES_NORMAL);
				auto colorIter = meshData->getDWORDDataIter(VES_COLOR);

				for (UINT32 i = batch.startIdx; i <= batch.endIdx; i++)
				{
					RawData& shapeData = allShapes[i];

					Matrix4* transform = nullptr;
					RGBA color = 0;

					switch (shapeData.shapeType)
					{
					case ShapeType::Cube:
					{
						CubeData& cubeData = mSolidCubeData[shapeData.idx];
						AABox box(cubeData.position - cubeData.extents, cubeData.position + cubeData.extents);
						ShapeMeshes3D::solidAABox(box, meshData, curVertexOffset, curIndexOffet);

						transform = &cubeData.transform;
						color = cubeData.color.getAsRGBA();
					}
						break;
					case ShapeType::Sphere:
					{
						SphereData& sphereData = mSolidSphereData[shapeData.idx];
						Sphere sphere(sphereData.position, sphereData.radius);
						ShapeMeshes3D::solidSphere(sphere, meshData, curVertexOffset, curIndexOffet, sphereData.quality);

						transform = &sphereData.transform;
						color = sphereData.color.getAsRGBA();
					}
						break;
					case ShapeType::Cone:
					{
						ConeData& coneData = mConeData[shapeData.idx];
						ShapeMeshes3D::solidCone(coneData.base, coneData.normal, coneData.height, coneData.radius, 
							coneData.scale, meshData, curVertexOffset, curIndexOffet, coneData.quality);

						transform = &coneData.transform;
						color = coneData.color.getAsRGBA();
					}
						break;
					case ShapeType::Disc:
					{
						DiscData& discData = mDiscData[shapeData.idx];
						ShapeMeshes3D::solidDisc(discData.position, discData.radius, discData.normal,
							meshData, curVertexOffset, curIndexOffet, discData.quality);

						transform = &discData.transform;
						color = discData.color.getAsRGBA();
					}
						break;
					case ShapeType::Arc:
					{
						ArcData& arcData = mArcData[shapeData.idx];
						ShapeMeshes3D::solidArc(arcData.position, arcData.radius, arcData.normal,
							arcData.startAngle, arcData.amountAngle, meshData, curVertexOffset, curIndexOffet, arcData.quality);

						transform = &arcData.transform;
						color = arcData.color.getAsRGBA();
					}
						break;
					case ShapeType::Rectangle:
					{
						Rect3Data rectData = mRect3Data[shapeData.idx];
						ShapeMeshes3D::solidQuad(rectData.area, meshData, curVertexOffset, curIndexOffet);

						transform = &rectData.transform;
						color = rectData.color.getAsRGBA();
					}
						break;
					default:
						break;
					}

					Matrix4 transformIT = transform->inverseAffine().transpose();
					for (UINT32 i = 0; i < shapeData.numVertices; i++)
					{
						Vector3 worldPos = transform->multiplyAffine(positionIter.getValue());
						Vector3 worldNormal = transformIT.multiplyAffine(normalIter.getValue());

						positionIter.addValue(worldPos);
						normalIter.addValue(worldNormal);
						colorIter.addValue(color);
					}

					curVertexOffset += shapeData.numVertices;
					curIndexOffet += shapeData.numIndices;
				}

				mMeshes.push_back(ShapeMeshData());
				ShapeMeshData& newMesh = mMeshes.back();
				newMesh.mesh = mSolidMeshHeap->alloc(meshData, DOT_TRIANGLE_LIST);
				newMesh.type = MeshType::Solid;
			}
			else if (batch.type == MeshType::Wire)
			{
				SPtr<MeshData> meshData = bs_shared_ptr_new<MeshData>(batch.numVertices, batch.numIndices, mWireVertexDesc);

				UINT32 curIndexOffset = 0;

				auto positionIter = meshData->getVec3DataIter(VES_POSITION);
				auto colorIter = meshData->getDWORDDataIter(VES_COLOR);

				for (UINT32 i = batch.startIdx; i <= batch.endIdx; i++)
				{
					RawData& shapeData = allShapes[i];

					Matrix4* transform = nullptr;
					RGBA color = 0;

					switch (shapeData.shapeType)
					{
					case ShapeType::WireMesh:
					{
						WireMeshData& wireMeshData = mWireMeshData[shapeData.idx];

						transform = &wireMeshData.transform;
						color = wireMeshData.color.getAsRGBA();

						auto vertIterRead = wireMeshData.meshData->getVec3DataIter(VES_POSITION);
						for (UINT32 i = 0; i < vertIterRead.getNumElements(); i++)
						{
							Vector3 worldPos = transform->multiplyAffine(vertIterRead.getValue());

							positionIter.addValue(worldPos);
							colorIter.addValue(color);

							vertIterRead.moveNext();
						}

						UINT32* srcIndexData = wireMeshData.meshData->getIndices32();
						UINT32* destIndexData = meshData->getIndices32() + curIndexOffset;

						memcpy(destIndexData, srcIndexData, sizeof(UINT32) * shapeData.numIndices);
						curIndexOffset += shapeData.numIndices;
					}
					break;
					default:
						break;
					}

					mMeshes.push_back(ShapeMeshData());
					ShapeMeshData& newMesh = mMeshes.back();
					newMesh.mesh = mWireMeshHeap->alloc(meshData, DOT_TRIANGLE_LIST);
					newMesh.type = MeshType::Wire;
				}
			}
			else if(batch.type == MeshType::Line)
			{
				SPtr<MeshData> meshData = bs_shared_ptr_new<MeshData>(batch.numVertices,
					batch.numIndices, mLineVertexDesc);

				UINT32 curVertexOffset = 0;
				UINT32 curIndexOffet = 0;

				auto positionIter = meshData->getVec3DataIter(VES_POSITION);
				auto colorIter = meshData->getDWORDDataIter(VES_COLOR);

				for (UINT32 i = batch.startIdx; i <= batch.endIdx; i++)
				{
					RawData& shapeData = allShapes[i];

					Matrix4* transform = nullptr;
					RGBA color = 0;

					switch (shapeData.shapeType)
					{
					case ShapeType::WireCube:
					{
						CubeData& cubeData = mWireCubeData[shapeData.idx];

						AABox box(cubeData.position - cubeData.extents, cubeData.position + cubeData.extents);
						ShapeMeshes3D::wireAABox(box, meshData, curVertexOffset, curIndexOffet);

						transform = &cubeData.transform;
						color = cubeData.color.getAsRGBA();
					}
						break;
					case ShapeType::WireSphere:
					{
						SphereData& sphereData = mWireSphereData[shapeData.idx];

						Sphere sphere(sphereData.position, sphereData.radius);
						ShapeMeshes3D::wireSphere(sphere, meshData, curVertexOffset, curIndexOffet, sphereData.quality);

						transform = &sphereData.transform;
						color = sphereData.color.getAsRGBA();
					}
						break;
					case ShapeType::WireCone:
					{
						ConeData& coneData = mWireConeData[shapeData.idx];
						ShapeMeshes3D::wireCone(coneData.base, coneData.normal, coneData.height, coneData.radius,
							coneData.scale, meshData, curVertexOffset, curIndexOffet, coneData.quality);

						transform = &coneData.transform;
						color = coneData.color.getAsRGBA();
					}
					break;
					case ShapeType::Line:
					{
						LineData& lineData = mLineData[shapeData.idx];

						ShapeMeshes3D::pixelLine(lineData.start, lineData.end, meshData, curVertexOffset, curIndexOffet);

						transform = &lineData.transform;
						color = lineData.color.getAsRGBA();
					}
						break;
					case ShapeType::LineList:
					{
						LineListData& lineListData = mLineListData[shapeData.idx];

						ShapeMeshes3D::pixelLineList(lineListData.lines, meshData, curVertexOffset, curIndexOffet);

						transform = &lineListData.transform;
						color = lineListData.color.getAsRGBA();
					}
					break;
					case ShapeType::Frustum:
					{
						FrustumData& frustumData = mFrustumData[shapeData.idx];

						ShapeMeshes3D::wireFrustum(frustumData.position, frustumData.aspect, frustumData.FOV, frustumData.near,
							frustumData.far, meshData, curVertexOffset, curIndexOffet);

						transform = &frustumData.transform;
						color = frustumData.color.getAsRGBA();
					}
						break;
					case ShapeType::WireDisc:
					{
						DiscData& discData = mWireDiscData[shapeData.idx];

						ShapeMeshes3D::wireDisc(discData.position, discData.radius, discData.normal,
							meshData, curVertexOffset, curIndexOffet, discData.quality);

						transform = &discData.transform;
						color = discData.color.getAsRGBA();
					}
						break;
					case ShapeType::WireArc:
					{
						ArcData& arcData = mWireArcData[shapeData.idx];

						ShapeMeshes3D::wireArc(arcData.position, arcData.radius, arcData.normal,
							arcData.startAngle, arcData.amountAngle, meshData, curVertexOffset, curIndexOffet, arcData.quality);

						transform = &arcData.transform;
						color = arcData.color.getAsRGBA();
					}
						break;
					default:
						break;
					}

					for (UINT32 i = 0; i < shapeData.numVertices; i++)
					{
						Vector3 worldPos = transform->multiplyAffine(positionIter.getValue());

						positionIter.addValue(worldPos);
						colorIter.addValue(color);
					}

					curVertexOffset += shapeData.numVertices;
					curIndexOffet += shapeData.numIndices;
				}

				mMeshes.push_back(ShapeMeshData());
				ShapeMeshData& newMesh = mMeshes.back();
				newMesh.mesh = mLineMeshHeap->alloc(meshData, DOT_LINE_LIST);
				newMesh.type = MeshType::Line;
			}
			else // Text
			{
				SPtr<MeshData> meshData = bs_shared_ptr_new<MeshData>(batch.numVertices,
					batch.numIndices, mTextVertexDesc);

				UINT32 curVertexOffset = 0;
				UINT32 curIndexOffet = 0;

				auto positionIter = meshData->getVec3DataIter(VES_POSITION);
				auto uvIter = meshData->getVec2DataIter(VES_TEXCOORD);
				auto colorIter = meshData->getDWORDDataIter(VES_COLOR);

				for (UINT32 i = batch.startIdx; i <= batch.endIdx; i++)
				{
					RawData& shapeData = allShapes[i];
					Text2DData& text2DData = mText2DData[shapeData.idx];

					TextRenderData& renderData = textRenderData[shapeData.textIdx];
					UINT32 numQuads = renderData.textData->getNumQuadsForPage(renderData.page);

					UINT32* indices = meshData->getIndices32();

					// Note: Need temporary buffers because TextLine doesn't support arbitrary vertex stride. Eventually
					// that should be supported (should be almost trivial to implement)
					Vector2* tempVertices = bs_stack_alloc<Vector2>(shapeData.numVertices);
					Vector2* tempUVs = bs_stack_alloc<Vector2>(shapeData.numVertices);

					UINT32 numLines = renderData.textData->getNumLines();
					UINT32 quadOffset = 0;
					for (UINT32 j = 0; j < numLines; j++)
					{
						const TextDataBase::TextLine& line = renderData.textData->getLine(j);
						UINT32 writtenQuads = line.fillBuffer(renderData.page, tempVertices, tempUVs, indices, quadOffset, numQuads);

						quadOffset += writtenQuads;
					}

					Vector3 translation = text2DData.transform.getTranslation();
					
					Vector2 accum;
					for (UINT32 j = 0; j < shapeData.numVertices; j++)
						accum += tempVertices[j];

					Vector2 center2D = accum / (float)shapeData.numVertices;
					Vector3 lookAt = Vector3::normalize(reference - translation);

					Quaternion rotation;
					rotation.lookRotation(lookAt, Vector3::UNIT_Y);

					float scale = translation.distance(reference) * 0.0025f; // 0.0025 = arbitrary scale to make the text look okay in world space

					// Scale by negative because we want to flip the vertices (they're upside down because GUI shader expects them as such)
					Matrix4 transform = Matrix4::TRS(translation, rotation, Vector3::ONE);

					for (UINT32 j = 0; j < shapeData.numVertices; j++)
					{
						Vector2 localPos2D = tempVertices[j] - center2D;
						localPos2D = localPos2D * -scale;

						Vector3 localPos(localPos2D.x, localPos2D.y, 0.0f);
						Vector3 worldPos = transform.multiplyAffine(localPos);

						positionIter.addValue(worldPos);
						uvIter.addValue(tempUVs[j]);
						colorIter.addValue(text2DData.color.getAsRGBA());
					}

					bs_stack_free(tempUVs);
					bs_stack_free(tempVertices);

					curVertexOffset += shapeData.numVertices;
					curIndexOffet += shapeData.numIndices;
				}

				mMeshes.push_back(ShapeMeshData());
				ShapeMeshData& newMesh = mMeshes.back();
				newMesh.mesh = mTextMeshHeap->alloc(meshData, DOT_TRIANGLE_LIST);
				newMesh.type = MeshType::Text;
				newMesh.texture = batch.texture;
			}
		}

		mNumActiveMeshes += (UINT32)mMeshes.size();
	}

	void DrawHelper::clearMeshes(const Vector<ShapeMeshData>& meshes)
	{
		for (auto meshData : meshes)
		{
			if (meshData.type == MeshType::Solid)
				mSolidMeshHeap->dealloc(meshData.mesh);
			if (meshData.type == MeshType::Wire)
				mWireMeshHeap->dealloc(meshData.mesh);
			else if (meshData.type == MeshType::Line)
				mLineMeshHeap->dealloc(meshData.mesh);
			else // Text
				mTextMeshHeap->dealloc(meshData.mesh);
		}

		mNumActiveMeshes -= (UINT32)meshes.size();
	}
}