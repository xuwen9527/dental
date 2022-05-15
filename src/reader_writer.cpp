#include "external/vcgmesh/ml_mesh_type.h"

#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/import_obj.h>
#include <wrap/io_trimesh/import_stl.h>

#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/export_obj.h>
#include <wrap/io_trimesh/export_stl.h>
#include <wrap/io_trimesh/export.h>

#include <list>
#include <reader_writer.h>
#include <texture.h>
#include <filesystem>

#if defined _MSC_VER
#include <io.h>
#include <direct.h>
#include "external/dirent/dirent.hxx"
#elif defined __GNUC__
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb/stb_image_write.h"

extern "C" {
#include "external/dds/dds.h"
}

namespace {
    std::string toLowerCase(const std::string& str) {
    std::string lowcase_str(str);
    for (auto itr = lowcase_str.begin(); itr != lowcase_str.end(); ++itr) {
      *itr = tolower(*itr);
    }
    return lowcase_str;
  }

  std::string fileExtensionLowerCase(const std::string& file_name) {
    return toLowerCase(std::filesystem::path(file_name).extension().string());
  }
}

namespace vcg {
  class PEdge;
  class PFace;
  class PVertex;
  struct PUsedTypes : public UsedTypes<Use<PVertex>::AsVertexType, Use<PEdge>::AsEdgeType, Use<PFace>::AsFaceType> {};

  class PVertex : public Vertex<PUsedTypes, vertex::Coord3f, vertex::Normal3f, vertex::Qualityf, vertex::Color4b, vertex::BitFlags  >{};
  class PEdge : public Edge< PUsedTypes, edge::VertexRef, edge::BitFlags>{};
  class PFace :public vcg::Face<
    PUsedTypes,
    face::PolyInfo, // this is necessary  if you use component in vcg/simplex/face/component_polygon.h
    face::PFVAdj,   // Pointer to the vertices (just like FVAdj )
    face::Color4b,
    face::BitFlags, // bit flags
    face::Normal3f, // normal
    face::WedgeTexCoord2f
  > {};

  class PMesh : public tri::TriMesh< vector<PVertex>, vector<PEdge>, vector<PFace> > {};

  class MeshModel {
  public:
    enum MeshElement{
      MM_NONE = 0x00000000,
      MM_VERTCOORD = 0x00000001,
      MM_VERTNORMAL = 0x00000002,
      MM_VERTFLAG = 0x00000004,
      MM_VERTCOLOR = 0x00000008,
      MM_VERTQUALITY = 0x00000010,
      MM_VERTMARK = 0x00000020,
      MM_VERTFACETOPO = 0x00000040,
      MM_VERTCURV = 0x00000080,
      MM_VERTCURVDIR = 0x00000100,
      MM_VERTRADIUS = 0x00000200,
      MM_VERTTEXCOORD = 0x00000400,
      MM_VERTNUMBER = 0x00000800,

      MM_FACEVERT = 0x00001000,
      MM_FACENORMAL = 0x00002000,
      MM_FACEFLAG = 0x00004000,
      MM_FACECOLOR = 0x00008000,
      MM_FACEQUALITY = 0x00010000,
      MM_FACEMARK = 0x00020000,
      MM_FACEFACETOPO = 0x00040000,
      MM_FACENUMBER = 0x00080000,
      MM_FACECURVDIR = 0x00100000,

      MM_WEDGTEXCOORD = 0x00200000,
      MM_WEDGNORMAL = 0x00400000,
      MM_WEDGCOLOR = 0x00800000,

      // 	Selection
      MM_VERTFLAGSELECT = 0x01000000,
      MM_FACEFLAGSELECT = 0x02000000,

      // Per Node Stuff....
      MM_CAMERA = 0x08000000,
      MM_TRANSFMATRIX = 0x10000000,
      MM_COLOR = 0x20000000,
      MM_POLYGONAL = 0x40000000,
      MM_UNKNOWN = 0x80000000,

      MM_ALL = 0xffffffff
    };

    CMeshO cm;

    MeshModel() {
      currentDataMask = MM_NONE;
      currentDataMask |= MM_VERTCOORD | MM_VERTNORMAL | MM_VERTFLAG;
      currentDataMask |= MM_FACEVERT | MM_FACENORMAL | MM_FACEFLAG;
    }

    bool hasDataMask(const int maskToBeTested) const {
      return ((currentDataMask & maskToBeTested) != 0);
    }

    void enable(int openingFileMask) {
      if (openingFileMask & vcg::tri::io::Mask::IOM_VERTTEXCOORD) {
          updateDataMask(MM_VERTTEXCOORD);
      }
      if (openingFileMask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD) {
          updateDataMask(MM_WEDGTEXCOORD);
      }
      if (openingFileMask & vcg::tri::io::Mask::IOM_VERTCOLOR) {
          updateDataMask(MM_VERTCOLOR);
      }
      if (openingFileMask & vcg::tri::io::Mask::IOM_FACECOLOR) {
          updateDataMask(MM_FACECOLOR);
      }
      if (openingFileMask & vcg::tri::io::Mask::IOM_VERTRADIUS) {
        updateDataMask(MM_VERTRADIUS);
      }
      if (openingFileMask & vcg::tri::io::Mask::IOM_CAMERA) {
        updateDataMask(MM_CAMERA);
      }
      if (openingFileMask & vcg::tri::io::Mask::IOM_VERTQUALITY) {
        updateDataMask(MM_VERTQUALITY);
      }
      if (openingFileMask & vcg::tri::io::Mask::IOM_FACEQUALITY) {
        updateDataMask(MM_FACEQUALITY);
      }
      if (openingFileMask & vcg::tri::io::Mask::IOM_BITPOLYGONAL) {
        updateDataMask(MM_POLYGONAL);
      }
    }

    void process(int mask) {
      // In case of polygonal meshes the normal should be updated accordingly
      if (mask & vcg::tri::io::Mask::IOM_BITPOLYGONAL) {
        updateDataMask(MeshModel::MM_POLYGONAL); // just to be sure. Hopefully it should be done in the plugin...
        int degNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(cm);
        if (degNum) {
            // LOG_WARN("Warning model contains %d degenerate faces. Removed them.", degNum);
        }

        updateDataMask(MeshModel::MM_FACEFACETOPO);
        vcg::tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(cm);
        vcg::tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(cm);
      } else {
        vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(cm);
        if (!(mask & vcg::tri::io::Mask::IOM_VERTNORMAL)) {
          vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(cm);
        }
        else {
          vcg::tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFaceNormalized(cm);
        }
      }

      //vcg::tri::UpdateBounding<CMeshO>::Box(cm);					// updates bounding box
      if (cm.fn == 0 && cm.en == 0) {
        if (mask & vcg::tri::io::Mask::IOM_VERTNORMAL) {
          updateDataMask(MeshModel::MM_VERTNORMAL);
        }
      }

      if (cm.fn == 0 && cm.en > 0) {
        if (mask & vcg::tri::io::Mask::IOM_VERTNORMAL) {
          updateDataMask(MeshModel::MM_VERTNORMAL);
        }
      }

      if (mask & vcg::tri::io::Mask::IOM_VERTCOLOR) {
        updateDataMask(MeshModel::MM_VERTCOLOR);
      }

      if (mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD) {
        //updateDataMask(MeshModel::MM_WEDGTEXCOORD);
        updateDataMask(MeshModel::MM_VERTTEXCOORD);
        //updateDataMask(MeshModel::MM_FACECOLOR);
      }

      if (mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD) {
        updateDataMask(MeshModel::MM_WEDGTEXCOORD);
      }

      if (mask & vcg::tri::io::Mask::IOM_WEDGNORMAL) {
        updateDataMask(MeshModel::MM_WEDGNORMAL);
      }

      if (mask & vcg::tri::io::Mask::IOM_FACECOLOR) {
        updateDataMask(MeshModel::MM_FACECOLOR);
      }

      //int delVertNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateVertex(cm);
      //int delFaceNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(cm);
      //vcg::tri::Allocator<CMeshO>::CompactEveryVector(cm);
      //if (delVertNum > 0 || delFaceNum > 0)
      //  WARN << "Warning mesh contains " << delVertNum << " vertices with NAN coords and " << delFaceNum << " degenerated faces.\nCorrected.";
    }

    void initBoundaryTextDecoratorData() {
      if (!hasDataMask(MeshModel::MM_WEDGTEXCOORD) && !hasDataMask(MeshModel::MM_VERTTEXCOORD)) {
        return;
      }

      updateDataMask(MeshModel::MM_FACEFACETOPO);
      CMeshO::PerMeshAttributeHandle< std::vector<Point3m> > btvH = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute< std::vector<Point3m> >(cm, "BoundaryTexVector");
      std::vector<Point3m> *BTVp = &btvH();
      BTVp->clear();
      std::vector<std::pair<CMeshO::FacePointer, int> > SaveTopoVec;
      CMeshO::FaceIterator fi;
      for (fi = cm.face.begin(); fi != cm.face.end(); ++fi) if (!(*fi).IsD()) {
        for (int i = 0; i < 3; ++i) {
          SaveTopoVec.emplace_back(std::make_pair((*fi).FFp(i), (*fi).FFi(i)));
        }
      }

      vcg::tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(cm);
      for (fi = cm.face.begin(); fi != cm.face.end(); ++fi) if (!(*fi).IsD()) {
        for (int i = 0; i < 3; ++i) {
          if (vcg::face::IsBorder(*fi, i)) {
            BTVp->emplace_back((*fi).V0(i)->P());
            BTVp->emplace_back((*fi).V1(i)->P());
          }
        }
      }

      std::vector<std::pair<CMeshO::FacePointer, int> >::iterator iii;
      for (fi = cm.face.begin(), iii = SaveTopoVec.begin(); fi != cm.face.end(); ++fi) if (!(*fi).IsD()) {
        for (int i = 0; i < 3; ++i) {
          (*fi).FFp(i) = iii->first;
          (*fi).FFi(i) = iii->second;
        }
      }
    }

    void updateDataMask(int neededDataMask) {
      if ((neededDataMask & MM_FACEFACETOPO) != 0) {
        //cm.face.EnableFFAdjacency();
        vcg::tri::UpdateTopology<CMeshO>::FaceFace(cm);
      }
      if ((neededDataMask & MM_VERTFACETOPO) != 0) {
        //cm.vert.EnableVFAdjacency();
        //cm.face.EnableVFAdjacency();
        vcg::tri::UpdateTopology<CMeshO>::VertexFace(cm);
      }

      if ((neededDataMask & MM_WEDGTEXCOORD) != 0) {
        cm.face.EnableWedgeTexCoord();
      }
      if ((neededDataMask & MM_FACECOLOR) != 0) {
        cm.face.EnableColor();
      }
      if ((neededDataMask & MM_FACEQUALITY) != 0) {
        cm.face.EnableQuality();
      }
      if ((neededDataMask & MM_FACECURVDIR) != 0) {
        cm.face.EnableCurvatureDir();
      }
      if ((neededDataMask & MM_FACEMARK) != 0) {
        cm.face.EnableMark();
      }
      if ((neededDataMask & MM_VERTMARK) != 0) {
        cm.vert.EnableMark();
      }
      if ((neededDataMask & MM_VERTCURV) != 0) {
        cm.vert.EnableCurvature();
      }
      if ((neededDataMask & MM_VERTCURVDIR) != 0) {
        cm.vert.EnableCurvatureDir();
      }
      if ((neededDataMask & MM_VERTRADIUS) != 0) {
        cm.vert.EnableRadius();
      }
      if ((neededDataMask & MM_VERTTEXCOORD) != 0) {
        cm.vert.EnableTexCoord();
      }

      currentDataMask |= neededDataMask;

      //vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(cm); // 去除重合的顶点
      //vcg::tri::Smooth<CMeshO>::VertexNormalLaplacian(mesh, 5); // 平滑顶点法向量
    }

  private:

    int currentDataMask;

    void clearDataMask(int unneededDataMask) {
      if (((unneededDataMask & MM_VERTFACETOPO) != 0) && hasDataMask(MM_VERTFACETOPO)) {
        cm.face.DisableVFAdjacency();
        cm.vert.DisableVFAdjacency();
      }

      if (((unneededDataMask & MM_FACEFACETOPO) != 0) && hasDataMask(MM_FACEFACETOPO)) {
        cm.face.DisableFFAdjacency();
      }

      if (((unneededDataMask & MM_WEDGTEXCOORD) != 0) && hasDataMask(MM_WEDGTEXCOORD)) {
        cm.face.DisableWedgeTexCoord();
      }

      if (((unneededDataMask & MM_FACECOLOR) != 0) && hasDataMask(MM_FACECOLOR)) {
        cm.face.DisableColor();
      }

      if (((unneededDataMask & MM_FACEQUALITY) != 0) && hasDataMask(MM_FACEQUALITY)) {
        cm.face.DisableQuality();
      }

      if (((unneededDataMask & MM_FACEMARK) != 0) && hasDataMask(MM_FACEMARK)) {
        cm.face.DisableMark();
      }

      if (((unneededDataMask & MM_VERTMARK) != 0) && hasDataMask(MM_VERTMARK)) {
        cm.vert.DisableMark();
      }

      if (((unneededDataMask & MM_VERTCURV) != 0) && hasDataMask(MM_VERTCURV)) {
        cm.vert.DisableCurvature();
      }

      if (((unneededDataMask & MM_VERTCURVDIR) != 0) && hasDataMask(MM_VERTCURVDIR)) {
        cm.vert.DisableCurvatureDir();
      }

      if (((unneededDataMask & MM_VERTRADIUS) != 0) && hasDataMask(MM_VERTRADIUS)) {
        cm.vert.DisableRadius();
      }

      if (((unneededDataMask & MM_VERTTEXCOORD) != 0) && hasDataMask(MM_VERTTEXCOORD)) {
        cm.vert.DisableTexCoord();
      }

      currentDataMask = currentDataMask & (~unneededDataMask);
    }
  };
  using MeshModelPtr = std::shared_ptr<MeshModel>;

  bool mesh2Geometry(MeshModelPtr& mesh_model, const std::string& file_name, Dental::GeometryPtr& geometry) {
    if (!mesh_model) {
      return false;
    }

    CMeshO& mesh = mesh_model->cm;

    if (!mesh.vert.size()) {
      // LOG_INFO("number of vertex is 0");
      return false;
    }

    if (mesh.VN() == 0 && mesh.FN() == 0) {
      // LOG_INFO("number of FN is 0");
      return false;
    }

    if (!mesh_model->hasDataMask(MeshModel::MM_VERTCOORD)) {//顶点
      return false;
    }

    bool normal_flag, color_flag, texture_flag;
    normal_flag = mesh_model->hasDataMask(MeshModel::MM_VERTNORMAL);
    color_flag = mesh_model->hasDataMask(MeshModel::MM_VERTCOLOR);

    auto vertexArray = geometry->vertexArray();
    auto normalArray = geometry->normalArray();
    auto colorArray = geometry->colorArray();
    auto texcoordArray = geometry->texcoordArray();

    if (!mesh_model->hasDataMask(MeshModel::MM_WEDGTEXCOORD)) {
      texture_flag = mesh_model->hasDataMask(MeshModel::MM_VERTTEXCOORD);

      size_t size = mesh.vert.size();

      vertexArray->reserve(size);
      if (normal_flag) {
        normalArray->reserve(size);
      }
      if (color_flag) {
        colorArray->reserve(size);
      }
      if (texture_flag) {
        texcoordArray->reserve(size);
      }

      CMeshO::VertexIterator vi = mesh.vert.begin();
      for (unsigned int i = 0; i < mesh.vert.size(); ++i, ++vi) {
        vertexArray->emplace_back(vi->P()[0], vi->P()[1], vi->P()[2]);

        if (normal_flag) {
          normalArray->emplace_back(vi->N()[0], vi->N()[1], vi->N()[2]);
        }

        if (color_flag) {
          colorArray->emplace_back(vi->C()[0] / 255.0f, vi->C()[1] / 255.0f, vi->C()[2] / 255.0f, 1.f);
        }

        if (texture_flag) {
          texcoordArray->emplace_back(vi->T().P()[0], vi->T().P()[1]);
        }
      }

      if (mesh.face.size()) {
        Dental::DrawElementsUInt* elements = new Dental::DrawElementsUInt(Dental::PrimitiveSet::Mode::TRIANGLES);
        elements->reserve(mesh.face.size() * 3);

        vi = mesh.vert.begin();
        for (CMeshO::FaceIterator fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
          elements->emplace_back((GLuint)((*fi).V(0) - &(*vi)));
          elements->emplace_back((GLuint)((*fi).V(1) - &(*vi)));
          elements->emplace_back((GLuint)((*fi).V(2) - &(*vi)));
        }

        Dental::PrimitiveSetPtr ptr(elements);
        geometry->setPrimitiveSet(ptr);
      } else {
        Dental::DrawElementsUInt* elements = new Dental::DrawElementsUInt(Dental::PrimitiveSet::Mode::POINTS);
        elements->reserve(size);

        for (unsigned int i = 0; i < size; ++i) {
          elements->emplace_back((GLuint)i);
        }

        Dental::PrimitiveSetPtr ptr(elements);
        geometry->setPrimitiveSet(ptr);

        if (!normal_flag) {
          normalArray->emplace_back(0.f, 0.f, 1.f);
        }
      }
    } else {
      texture_flag = mesh_model->hasDataMask(MeshModel::MM_WEDGTEXCOORD);

      size_t size = mesh.face.size() * 3;

      Dental::DrawElementsUInt* elements = new Dental::DrawElementsUInt(Dental::PrimitiveSet::Mode::TRIANGLES);
      elements->reserve(size);

      vertexArray->reserve(size);
      if (normal_flag) {
        normalArray->reserve(size);
      }
      if (color_flag) {
        colorArray->reserve(size);
      }
      if (texture_flag) {
        texcoordArray->reserve(size);
      }

      unsigned int i = 0;
      for (auto fi = mesh.face.begin(); fi != mesh.face.end(); ++fi) {
        CVertexO* v0 = (*fi).V(0);
        CVertexO* v1 = (*fi).V(1);
        CVertexO* v2 = (*fi).V(2);

        {
          vertexArray->emplace_back(v0->P()[0], v0->P()[1], v0->P()[2]);

          if (normal_flag) {
            normalArray->emplace_back(v0->N()[0], v0->N()[1], v0->N()[2]);
          }

          if (color_flag) {
            colorArray->emplace_back(v0->C()[0] / 255.0f, v0->C()[1] / 255.0f, v0->C()[2] / 255.0f, 1.f);
          }

          if (texture_flag) {
              texcoordArray->emplace_back((*fi).WT(0).U(), (*fi).WT(0).V());
          }

          elements->push_back(i);
        }

        ++i;

        {
          vertexArray->emplace_back(v1->P()[0], v1->P()[1], v1->P()[2]);

          if (normal_flag) {
            normalArray->emplace_back(v1->N()[0], v1->N()[1], v1->N()[2]);
          }

          if (color_flag) {
            colorArray->emplace_back(v1->C()[0] / 255.0f, v1->C()[1] / 255.0f, v1->C()[2] / 255.0f, 1.f);
          }

          if (texture_flag) {
            texcoordArray->emplace_back((*fi).WT(1).U(), (*fi).WT(1).V());
          }

          elements->push_back(i);
        }

        ++i;

        {
          vertexArray->emplace_back(v2->P()[0], v2->P()[1], v2->P()[2]);

          if (normal_flag) {
              normalArray->emplace_back(v2->N()[0], v2->N()[1], v2->N()[2]);
          }

          if (color_flag) {
              colorArray->emplace_back(v2->C()[0] / 255.0f, v2->C()[1] / 255.0f, v2->C()[2] / 255.0f, 1.f);
          }

          if (texture_flag) {
              texcoordArray->emplace_back((*fi).WT(2).U(), (*fi).WT(2).V());
          }
          elements->emplace_back(i);
        }

        ++i;
      }

      Dental::PrimitiveSetPtr ptr(elements);
      geometry->setPrimitiveSet(ptr);
    }

    if (!mesh.textures.empty()) {
      for (size_t i = 0; i < mesh.textures.size(); ++i) {
        std::string texFile(mesh.textures[i]);
        texFile = std::filesystem::canonical(file_name).string();

        Dental::TexturePtr tex2D(new Dental::Texture());
        tex2D->wrap(Dental::Texture::Wrap::WRAP_S, Dental::Texture::WrapMode::CLAMP_TO_EDGE);
        tex2D->wrap(Dental::Texture::Wrap::WRAP_T, Dental::Texture::WrapMode::CLAMP_TO_EDGE);

        tex2D->filter(Dental::Texture::Filter::MIN_FILTER, Dental::Texture::FilterMode::NEAREST);
        tex2D->filter(Dental::Texture::Filter::MAG_FILTER, Dental::Texture::FilterMode::LINEAR);

        auto image_result = Dental::ReaderWriter::readImage(texFile);
        auto image = std::get<0>(image_result);
        if (image) {
          tex2D->image(image);
          geometry->texture(tex2D, i);
        } else {
          // std::cout << "%s image file read failed!" << texFile << std::get<1>(image_result);
        }
      }
    }
    return true;
  }

  bool geometry2mesh(const Dental::GeometryPtr& geometry, MeshModelPtr& mesh_model, int& mask, const std::string& file_name) {
    if (!geometry || !mesh_model) {
      return false;
    }

    size_t size = geometry->vertexArray()->size();
    if (!size) {
      return false;
    }

    CMeshO& mesh = mesh_model->cm;

    mask = vcg::tri::io::Mask::IOM_VERTCOORD;

    if (geometry->colorArray()->size()) {
      mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
    }

    if (geometry->normalArray()->size()) {
      mask |= vcg::tri::io::Mask::IOM_VERTNORMAL;
    }

    if (geometry->texcoordArray()->size()) {
      mask |= vcg::tri::io::Mask::IOM_VERTTEXCOORD;
      mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
    }

    mesh_model->process(mask);

    CMeshO::VertexIterator vi = vcg::tri::Allocator<CMeshO>::AddVertices(mesh, size);

    glm::vec3* vec = geometry->vertexArray()->data();
    glm::vec3* nor = geometry->normalArray()->data();
    glm::vec4* clr = geometry->colorArray()->data();
    glm::vec2* tex = geometry->texcoordArray()->data();
    for (unsigned int i = 0; i < size; ++i, ++vi, ++vec) {
      (*vi).P()[0] = vec->x;
      (*vi).P()[1] = vec->y;
      (*vi).P()[2] = vec->z;

      if (nor) {
        (*vi).N()[0] = nor->x;
        (*vi).N()[1] = nor->y;
        (*vi).N()[2] = nor->z;
        ++nor;
      }

      if (clr) {
        (*vi).C()[0] = clr->x * 255;
        (*vi).C()[1] = clr->y * 255;
        (*vi).C()[2] = clr->z * 255;
        (*vi).C()[3] = clr->w * 255;
        ++clr;
      }

      if (tex) {
        (*vi).T().P()[0] = tex->x;
        (*vi).T().P()[1] = tex->y;
        ++tex;
      }
    }

    Dental::DrawElementsUInt* tris = dynamic_cast<Dental::DrawElementsUInt*>(geometry->primitiveSet().get());
    if (tris && tris->mode() == Dental::PrimitiveSet::Mode::TRIANGLES) {
      CMeshO::FaceIterator fi = vcg::tri::Allocator<CMeshO>::AddFaces(mesh, tris->size() / 3);

      CVertexO *ver_ptr = mesh.vert.data();
      for (unsigned int i = 0; i < tris->size(); i += 3, ++fi) {
        (*fi).Alloc(3);

        CVertexO *v0 = ver_ptr + (*tris)[i];
        CVertexO *v1 = ver_ptr + (*tris)[i + 1];
        CVertexO *v2 = ver_ptr + (*tris)[i + 2];

        (*fi).V(0) = v0;
        (*fi).V(1) = v1;
        (*fi).V(2) = v2;

        if (geometry->texcoordArray()->size()) {
          (*fi).WT(0).U() = v0->T().P()[0];
          (*fi).WT(0).V() = v0->T().P()[1];

          (*fi).WT(1).U() = v1->T().P()[0];
          (*fi).WT(1).V() = v1->T().P()[1];

          (*fi).WT(2).U() = v2->T().P()[0];
          (*fi).WT(2).V() = v2->T().P()[1];

          (*fi).WT(0).n() = 0;
        }
      }
    } else if (tris && tris->mode() == Dental::PrimitiveSet::Mode::POINTS) {

    }

    Dental::TexturePtr texture2D = geometry->texture();
    if (texture2D) {
      const Dental::ImagePtr image = texture2D->image();
      if (image) {
        std::string image_file_name = std::filesystem::path(file_name).replace_extension("jpg").string();
        std::string stripped_file_name = std::filesystem::path(file_name).filename().string();

        auto result = Dental::ReaderWriter::writeImage(image_file_name, *image);
        if (std::get<0>(result) != Dental::ReaderWriter::Status::FILE_SAVED) {
          std::cout << image_file_name + " save failed! " + std::get<1>(result);
        }

        mesh.textures.emplace_back(stripped_file_name);
      }
    }

    mask = vcg::tri::io::Mask::IOM_VERTCOORD;
    if (geometry->colorArray()->size()) {
      mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
    }

    if (geometry->normalArray()->size()) {
      mask |= vcg::tri::io::Mask::IOM_VERTNORMAL;
    }

    if (geometry->texcoordArray()->size()) {
      mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
    }

    mesh_model->process(mask);

    vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(mesh_model->cm); // 去除重合的顶点

    return true;
  }

  bool read(const std::string& file_name, Dental::GeometryPtr& geometry, std::string& error) {
    std::string ext = fileExtensionLowerCase(file_name);

    MeshModelPtr mesh_model(std::make_shared<MeshModel>());

    int mask = 0;
    vcg::CallBackPos *cb = 0;

    if (ext == ".ply") {
      vcg::tri::io::ImporterPLY<CMeshO>::LoadMask(file_name.c_str(), mask);
      // small patch to allow the loading of per wedge color into faces.
      if (mask & vcg::tri::io::Mask::IOM_WEDGCOLOR) {
        mask |= vcg::tri::io::Mask::IOM_FACECOLOR;
      }
      mesh_model->enable(mask);

      int flag = vcg::tri::io::ImporterPLY<CMeshO>::Open(mesh_model->cm, file_name.c_str(), mask, cb);
      if (flag != 0) {// all the importers return 0 on success
        if (vcg::tri::io::ImporterPLY<CMeshO>::ErrorCritical(flag)) {
          error = "fail to load " + file_name + ", because of " + vcg::tri::io::ImporterPLY<CMeshO>::ErrorMsg(flag);
          return 0;
        }
      }
    } else if (ext == ".obj" || ext == ".qobj") {
      vcg::tri::io::ImporterOBJ<CMeshO>::Info oi;
      oi.cb = cb;
      if (!vcg::tri::io::ImporterOBJ<CMeshO>::LoadMask(file_name.c_str(), oi)) {
        error = file_name + " format is error";
        return 0;
      }

      mesh_model->enable(oi.mask);

      int flag = vcg::tri::io::ImporterOBJ<CMeshO>::Open(mesh_model->cm, file_name.c_str(), oi);
      if (flag != vcg::tri::io::ImporterOBJ<CMeshO>::E_NOERROR) {// all the importers return 0 on success
        error = "fail to load " + file_name + " , because of " + vcg::tri::io::ImporterOBJ<CMeshO>::ErrorMsg(flag);
        return 0;
      }

      mask = oi.mask;
    } else if (ext == ".stl") {
      if (!vcg::tri::io::ImporterSTL<CMeshO>::LoadMask(file_name.c_str(), mask)) {
        error = "fail to load " + file_name + ", because of " + vcg::tri::io::ImporterSTL<CMeshO>::ErrorMsg(vcg::tri::io::ImporterSTL<CMeshO>::E_CANTOPEN);
        return 0;
      }

      int flag = vcg::tri::io::ImporterSTL<CMeshO>::Open(mesh_model->cm, file_name.c_str(), mask, cb);
      if (flag != 0) { // all the importers return 0 on success
        error = "fail to load " + file_name + ", because of " + vcg::tri::io::ImporterSTL<CMeshO>::ErrorMsg(flag);
        return 0;
      }
    }

    mesh_model->process(mask);

    return mesh2Geometry(mesh_model, file_name, geometry);
  }

  bool write(const std::string &file_name, const Dental::GeometryPtr &geometry, const Dental::ReaderWriter::WriteOptions& options, std::string& error) {
    if (!geometry) {
      return false;
    }

    size_t size = geometry->vertexArray()->size();
    if (!size) {
      // LOG_WARN("vertex size is not same");
      return false;
    }

    if ((geometry->colorArray()->size() && size != geometry->colorArray()->size()) ||
      (geometry->normalArray()->size() && size != geometry->normalArray()->size()) ||
      (geometry->texcoordArray()->size() && size != geometry->texcoordArray()->size())) {
      // LOG_WARN("vertex, color, normal, texcoord size is not same");
      return false;
    }

    std::string ext = fileExtensionLowerCase(file_name);

    int mask = 0, ret = -1;
    vcg::CallBackPos *cb = 0;
    vcg::MeshModelPtr m = std::make_shared<vcg::MeshModel>();

    bool flag = vcg::geometry2mesh(geometry, m, mask, file_name);
    if (!flag) {
      return false;
    }

    bool binaryFlag = options.option("Binary") == "1";
    bool magicsFlag = false;
    if (geometry->colorArray()->size()) {
      magicsFlag = options.option("ColorMode") == "1";
    }

    if (ext == ".ply") {
      ret = vcg::tri::io::ExporterPLY<CMeshO>::Save(m->cm, file_name.c_str(), mask, binaryFlag, cb);
      if (ret != 0) {
        error.append(vcg::tri::io::ExporterPLY<CMeshO>::ErrorMsg(ret));
      }
    } else if (ext == ".stl") {
      if (!magicsFlag) {
        mask &= ~vcg::tri::io::Mask::IOM_VERTCOLOR;
        mask &= ~vcg::tri::io::Mask::IOM_FACECOLOR;
      } else {
        mask |= vcg::tri::io::Mask::IOM_FACECOLOR;
        m->cm.face.EnableColor();

        for (CMeshO::FaceIterator fi = m->cm.face.begin(); fi !=  m->cm.face.end(); ++fi) {
          (*fi).C() = ((*fi).V(0)->C() + (*fi).V(1)->C() + (*fi).V(2)->C()) / 3;
        }
      }

      if (m->cm.fn) {
        const time_t t = time(0);
        struct tm *current_time = localtime(&t);

        std::stringstream headerstream;
        headerstream << "STL generated by Dental" << " ";
        headerstream << current_time->tm_year + 1900 << '-' << current_time->tm_mon + 1
                     << '-' << current_time->tm_mday;

        ret = vcg::tri::io::ExporterSTL<CMeshO>::Save(m->cm, file_name.c_str(), binaryFlag,
                                                      mask, headerstream.str().c_str(),
                                                      magicsFlag);
        if (ret != 0) {
          error.append(vcg::tri::io::ExporterSTL<CMeshO>::ErrorMsg(ret));
        }
      }
      } else if (ext == ".obj") {
        vcg::tri::Allocator<CMeshO>::CompactEveryVector(m->cm);
        if (mask & vcg::tri::io::Mask::IOM_BITPOLYGONAL) {
          m->updateDataMask(vcg::MeshModel::MM_FACEFACETOPO);
          vcg::PMesh pm;
          vcg::tri::PolygonSupport<CMeshO, vcg::PMesh>::ImportFromTriMesh(pm, m->cm);
          ret = vcg::tri::io::ExporterOBJ<vcg::PMesh>::Save(pm, file_name.c_str(), mask, cb);
        } else
          ret = vcg::tri::io::ExporterOBJ<CMeshO>::Save(m->cm, file_name.c_str(), mask, cb);

        if (ret != 0) {
          error.append(vcg::tri::io::ExporterOBJ<CMeshO>::ErrorMsg(ret));
        }
    }

    return ret == 0;
  }
}

#define failure_reason() \
  stbi_failure_reason() ? stbi_failure_reason() : ""

namespace Dental::ReaderWriter {
  bool acceptsExtension(const std::string &extension) {
    // check for an exact match
    using FormatDescriptionMap = std::unordered_map<std::string, std::string>;
    static FormatDescriptionMap supportedExtensions_ = {
      {".ply", "Stanford Polygon Library"},
      {".stl", "Stereolithography"},
      {".obj", "Wavefront Object"}
    };

    std::string ext = toLowerCase(extension);
    return (supportedExtensions_.count(ext) != 0);
  }

  void WriteOptions::option(std::string parma, std::string value) {
    (*this)[parma] = value;
  }

  std::string WriteOptions::option(std::string parma) const {
    auto itr = find(parma);
    if (itr != end()) {
      return itr->second;
    }
    return "";
  }

  void WriteOptions::binary(bool flag) {
    option("Binary", flag ? "1" : "0");
  }

  //stl文件是否是彩色模式保存
  void WriteOptions::colorMode(bool flag) {
    option("ColorMode", flag ? "1" : "0");
  }

  std::tuple<GeometryPtr, Status, std::string>
  read(const std::string &file_name) {
    GeometryPtr geometry(std::make_shared<Geometry>());

    auto path = std::filesystem::path(file_name);
    if (!std::filesystem::exists(path)) {
      return { nullptr, Status::FILE_NOT_FIND, file_name + " not found!" };
    }

    if (!acceptsExtension(path.extension().string())) {
      return { nullptr, Status::FILE_NOT_HANDLED, file_name + " not support!" };
    }

    std::string error;
    if (!vcg::read(file_name, geometry, error)) {
      return { nullptr, Status::ERROR_IN_READING_FILE, error };
    }

    return { geometry, Status::FILE_LOADED, "" };
  }

  std::tuple<Status, std::string>
  write(const std::string &file_name, const GeometryPtr& geometry, const WriteOptions& options) {
    auto path = std::filesystem::path(file_name);
    if (!acceptsExtension(path.extension().string())) {
      return { Status::FILE_NOT_HANDLED, file_name + " do not support!"};
    }

    size_t size = geometry->vertexArray()->size() / 3;
    if (!size) {
      return { Status::FILE_NOT_HANDLED, "geometry's data is invalid!"};
    }

    std::string error;
    if (!vcg::write(file_name, geometry, options, error)) {
      return { Status::ERROR_IN_WRITING_FILE, error };
    }

    return { Status::FILE_SAVED, ""};
  }

  std::tuple<ImagePtr, Status, std::string> readImage(unsigned char* data, std::size_t size) {
    if (!data) {
      return { nullptr, Status::FILE_NOT_HANDLED, "" };
    }

    stbi_set_flip_vertically_on_load(true);

    int width, height, components;
    unsigned char* bytes = stbi_load_from_memory((const stbi_uc*)data, size,
      &width, &height, &components, 0);
    if (!bytes) {
      dds_image_t dds_image = dds_load_from_memory((const char*)data, size);
      if (!dds_image) {
        return { nullptr, Status::ERROR_IN_READING_FILE, failure_reason() };
      }

      bytes = dds_image->pixels;
      width = dds_image->header.width;
      height = dds_image->header.height;
      components = 4;
    }

    auto image = std::make_shared<Image>();
    image->image(width, height, 1, components, GL_UNSIGNED_BYTE, bytes,
      Image::AllocationMode::USE_MALLOC_FREE);

    return { image, Status::FILE_LOADED, "" };
  }

  std::tuple<ImagePtr, Status, std::string>
  readImage(const std::string& file_name) {
    if (!std::filesystem::exists(file_name)) {
      return { nullptr, Status::FILE_NOT_FIND, "" };
    }

    int width, height, components;
    unsigned char* bytes = nullptr;

    std::string ext = fileExtensionLowerCase(file_name);
    if (ext == ".dds") {
      dds_image_t dds_image = dds_load_from_file(file_name.c_str());
      if (!dds_image) {
        return { nullptr, Status::ERROR_IN_READING_FILE, "" };
      }

      bytes = dds_image->pixels;
      width = dds_image->header.width;
      height = dds_image->header.height;
      components = 4;

      // free dds_image_t, don't free dds_image_t's pixels
      free(dds_image);
    } else {
      stbi_set_flip_vertically_on_load(true);
      bytes = stbi_load(file_name.c_str(),
        &width, &height, &components, 0);
      if (!bytes) {
        return { nullptr, Status::ERROR_IN_READING_FILE, failure_reason() };
      }
    }

    auto image = std::make_shared<Image>();
    image->fileName(file_name);
    image->verticallyFliped(true);
    image->image(width, height, 1, components, GL_UNSIGNED_BYTE, bytes,
      Image::AllocationMode::USE_MALLOC_FREE);

    return { image,  Status::FILE_LOADED, "" };
  }

  std::tuple<Status, std::string>
  writeImage(const std::string &file_name, const Image &image) {
    stbi_flip_vertically_on_write(image.verticallyFliped() ? 1 : 0);

    std::string ext = fileExtensionLowerCase(file_name);
    int components = Image::computeNumComponents(image.pixelFormat());

    int ret = 0;
    if (ext == ".png") {
      ret = stbi_write_png(file_name.c_str(), image.s(), image.t(), components, image.data(), 0);
    } else if (ext == ".jpg" || ext == ".jpeg") {
      ret = stbi_write_jpg(file_name.c_str(), image.s(), image.t(), components, image.data(), 100);
    } else if (ext == ".bmp") {
      ret = stbi_write_bmp(file_name.c_str(), image.s(), image.t(), components, image.data());
    } else if (ext == ".hdr") {
      ret = stbi_write_hdr(file_name.c_str(), image.s(), image.t(), components, (const float*)image.data());
    } else if (ext == ".tga") {
      ret = stbi_write_tga(file_name.c_str(), image.s(), image.t(), components, image.data());
    }

    return { ret ? Status::FILE_SAVED : Status::ERROR_IN_WRITING_FILE, failure_reason() };
  }
}