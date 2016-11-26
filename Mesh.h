#ifndef MESH_H
#define MESH_H

#include<vector>
#include<map>
#include<glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include"Shader.h"

namespace MeshLib
{
	class HE_vert;
	class HE_face;
	class HE_edge;

	template<class Type>
	struct less : public binary_function <Type, Type, bool>
	{
		bool operator()(     ///实现比较功能.
		const Type* _Left,
		const Type* _Right
		) const
		{
			return (_Left->x - _Right->x<0) && (_Left->y - _Right->y<0) && (_Left->z - _Right->z<0) ? true : false;
		};
	};

	class HE_vert
	{
	public: 
		glm::vec3 pos;
		HE_edge* edge;
		glm::vec3 normal;
		GLint id;
		GLint degree;
		GLboolean tag;
		GLfloat gray;

	public:
		HE_vert(glm::vec3 &v) :pos(v), edge(0), id(-1), degree(0), tag(GL_FALSE)
		{
		}
	};

	class HE_edge
	{
	public:
		HE_vert* vert;
		HE_edge* pair;
		HE_face* face;
		HE_edge* next;
		HE_edge* prev;
		GLint id;
		GLboolean tag;

	public:
		HE_edge() :vert(0), pair(0), face(0), next(0), prev(0), id(-1), tag(GL_FALSE)
		{}
	};

	class HE_face
	{
	public:
		HE_edge* edge;
		int valence;
		glm::vec3 normal;
		GLint id;
		GLboolean tag;
		
	public:
		HE_face() :edge(0), id(-1), tag(GL_FALSE)
		{}

		glm::vec3 GetCentroid();
	};

	class Mesh
	{

	public:
		typedef std::vector<HE_vert*> VERTEX_LIST;
		typedef std::vector<HE_edge*> EDGE_LIST;
		typedef std::vector<HE_face*> FACE_LIST;

		typedef VERTEX_LIST* PTR_VERTEX_LIST;
		typedef EDGE_LIST* PTR_EDGE_LIST;
		typedef FACE_LIST* PTR_FACE_LIST;

		typedef VERTEX_LIST::iterator VERTEX_ITER;
		typedef FACE_LIST::iterator FACE_ITER;
		typedef EDGE_LIST::iterator EDGE_ITER;

		typedef VERTEX_LIST::reverse_iterator VERTEX_RITER;
		typedef FACE_LIST::reverse_iterator FACE_RITER;
		typedef EDGE_LIST::reverse_iterator EDGE_RITER;

		typedef std::pair<HE_vert*, HE_vert*> PAIR_VERTEX;

		PTR_VERTEX_LIST vertices_list;
		PTR_EDGE_LIST edges_list;
		PTR_FACE_LIST faces_list;

		std::map<PAIR_VERTEX, HE_edge*> m_edgemap;
		std::map<glm::vec3*, HE_vert*,less<glm::tvec3<GLfloat,glm::precision::defaultp>>> m_vertmap;

		std::vector<GLfloat> vertices;
		std::vector<GLuint> indices;

		GLuint VAO, VBO, EBO;


	public:
		Mesh();
		~Mesh();

		void setupMesh();

		void DrawModel(Shader shader);

		void DrawLine(Shader shader);

		Mesh* Doo_Sabin();

		HE_vert* insert_vertex(glm::vec3 &v);

		HE_face* insert_face(VERTEX_LIST &vec_hv);

		HE_edge* insert_edge(HE_vert* vstart, HE_vert* vend);

		void createMeshByFunction(const glm::vec3& point, const glm::vec3& cube, const  GLfloat& length, void function(GLfloat& x, GLfloat& y, GLfloat& z, GLfloat& result));

		void marchingcube(HE_vert* vert, HE_edge* edge, PTR_VERTEX_LIST vlist, void function(GLfloat& x, GLfloat& y, GLfloat& z, GLfloat& result));

		GLboolean load_obj(const char* fins);

		void compute_faces_list_normal();

		void compute_perface_normal(HE_face* hf);

		void compute_vertices_list_normal();

		void compute_pervertex_normal(HE_vert* hv);

		void compute_all_normal();

		glm::vec3 update();

		glm::vec3 getCenter();

		void translate(glm::vec3 pos);

		void scale(GLfloat size);


		inline void getBorderVerts(HE_vert* vstart,HE_vert* vend,glm::vec3 &pos1,glm::vec3 &pos2)
		{
			glm::vec3 mpos = (vstart->pos + vend->pos) / 2.0f;
			pos1 = (mpos + vstart->pos) / 2.0f;
			pos2 = (mpos + vend->pos) / 2.0f;
		};

		void update_vertices();

		void update_indices();
		
		bool isborder(HE_vert* hv);

		bool isFaceBorder(HE_edge* edge);

		void removeVolid();

		void insertNext(HE_edge* edge);

		inline int get_num_of_vertices_list()
		{
			return vertices_list ? (int)vertices_list->size() : 0;
		}

		inline int get_num_of_faces_list()
		{
			return faces_list ? (int)faces_list->size() : 0;
		}
		inline int get_num_of_edges_list()
		{
			return edges_list ? (int)edges_list->size() : 0;
		}

		inline HE_vert* get_vertex(int id) //get a vertex by id
		{
			return id >= get_num_of_vertices_list() || id<0 ? NULL : (*vertices_list)[id];
		}

		inline HE_edge* get_edge(int id) //get an edge by id
		{
			return id >= get_num_of_edges_list() || id<0 ? NULL : (*edges_list)[id];
		}
		inline HE_face* get_face(int id) //get a face by id
		{
			return id >= get_num_of_faces_list() || id<0 ? NULL : (*faces_list)[id];
		}
	};

}

#endif
