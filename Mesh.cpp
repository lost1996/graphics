#include "Mesh.h"
#include<iostream>

namespace MeshLib
{
	Mesh::Mesh() :vertices_list(NULL), faces_list(NULL), edges_list(NULL)
	{
	}


	Mesh::~Mesh()
	{
	}

	

	HE_vert* Mesh::insert_vertex(glm::vec3 &v)
	{
		if (vertices_list == NULL)
			vertices_list = new VERTEX_LIST;

		/*HE_vert* hv=NULL;
		VERTEX_ITER iter = vertices_list->begin();
		for (; iter != vertices_list->end(); iter++)
		{
			HE_vert* hv0 = (*iter);
			if (hv0->pos == v)
			{
				hv=hv0;
				break;
			}
		}
		if (hv==NULL)
			hv = new HE_vert(v);*/
	/*	if (this->m_vertmap.find(&v) != m_vertmap.end())
		{
			cout << m_vertmap[&v] ->id<< ":" << v.x << "." << v.y << "," << v.z << endl;
			cout << m_vertmap[&v]->id << m_vertmap[&v]->pos.x << "." << m_vertmap[&v]->pos.y << "." << m_vertmap[&v]->pos.z << endl;
			return m_vertmap[&v];
		}*/
	
		HE_vert* hv = new HE_vert(v);

		hv->id = vertices_list->size();
		vertices_list->push_back(hv);
	//	m_vertmap[&v] = hv;
	//		m_vertmap.insert(std::map<glm::vec3*, HE_vert*>::value_type(&v, hv));
	//	cout << hv->id<<endl;
		return hv;
	}

	HE_face* Mesh::insert_face(VERTEX_LIST& vec_hv)
	{
		int vsize = vec_hv.size();
		if (vsize < 3)
			return NULL;
		if (faces_list == NULL)
			faces_list = new FACE_LIST;
		HE_face *hf = new HE_face;
		hf->valence = vsize;

		VERTEX_ITER viter = vec_hv.begin();
		VERTEX_ITER nviter = vec_hv.begin();
		nviter++;

		HE_edge *he1, *he2;
		std::vector<HE_edge*> v_he;
		int i;
		for (i = 0; i<vsize - 1; i++) {
			he1 = insert_edge(*viter, *nviter);
			he2 = insert_edge(*nviter, *viter);

			if (hf->edge == NULL) {
				hf->edge = he1;
			}
			he1->face = hf;

			he1->pair = he2;
			he2->pair = he1;
			v_he.push_back(he1);
			viter++, nviter++;
		}

		nviter = vec_hv.begin();

		he1 = insert_edge(*viter, *nviter);
		he2 = insert_edge(*nviter, *viter);
		he1->face = hf;
		if (hf->edge == NULL) {
			hf->edge = he1;
		}
		he1->pair = he2;
		he2->pair = he1;
		v_he.push_back(he1);

		for (i = 0; i < vsize - 1; i++)
		{
			v_he[i]->next = v_he[i + 1];
			v_he[i + 1]->prev = v_he[i];
		}
		v_he[i]->next = v_he[0];
		v_he[0]->prev = v_he[i];

		hf->id = faces_list->size();
		
		faces_list->push_back(hf);
		return hf;
	}

	HE_edge* Mesh::insert_edge(HE_vert* vstart, HE_vert* vend)
	{
		if (vstart == NULL || vend == NULL)
		{
			return NULL;
		}
		if (edges_list == NULL)
			edges_list = new EDGE_LIST;

		if (m_edgemap[PAIR_VERTEX(vstart, vend)] != NULL)
			return m_edgemap[PAIR_VERTEX(vstart, vend)];


		HE_edge* he = new HE_edge;
		he->vert = vend;
		he->vert->degree++;

		vstart->edge = he;

		he->id = edges_list->size();

		m_edgemap[PAIR_VERTEX(vstart, vend)] = he;
		edges_list->push_back(he);

		return he;
	}

	void Mesh::createMeshByFunction(const glm::vec3& point, const glm::vec3& cube, const GLfloat& length, void function(GLfloat& x, GLfloat& y, GLfloat& z, GLfloat& result))
	{


		glm::vec3 point0 = glm::vec3(point.x - cube.x*length/2.0f, point.y - cube.y*length/2.0f, point.z - cube.z*length/2.0f);
		Mesh mesh;
		mesh.load_obj("Obj/cube2.obj");
		mesh.scale(length/2.0f);
		mesh.translate(point0);

		glm::vec3 point1 = glm::vec3(0.0f);
		glm::vec3 point2 = glm::vec3(0.0f);
		int i = 0, j = 0, k = 0;
		for (i=0; i < (int)(cube.z); i++)
		{
			for (j = 0; j < (int)(cube.y); j++)
			{
				for (k = 0; k < (int)(cube.x); k++)
				{
					point1 = glm::vec3(k*length,j*length,i*length);
					mesh.translate(point1-point2);
					VERTEX_ITER iter = mesh.vertices_list->begin();
					for (; iter != mesh.vertices_list->end(); iter++)
					{
						HE_vert* vert = (*iter);
						function(vert->pos.x, vert->pos.y,vert->pos.z,vert->gray);
						vert->tag = GL_FALSE;
					}

					iter = mesh.vertices_list->begin();
					for (; iter != mesh.vertices_list->end(); iter++)
					{
						if (!(*iter)->tag)
						{
							VERTEX_LIST vlist;
							this->marchingcube((*iter), (*iter)->edge, &vlist,function);
						//	std::reverse(vlist.begin(), vlist.end());
							if (vlist.size()>2)
								this->insert_face(vlist);
						}
					}

					point2 = point1;
				}
			}
		}
	//	this->removeVolid();


	}

	void Mesh::marchingcube(HE_vert* vert, HE_edge* edge, PTR_VERTEX_LIST vlist, void function(GLfloat& x, GLfloat& y, GLfloat& z, GLfloat& result))
	{
		HE_edge* e=edge;
		if (vert->gray <= 0 && !vert->tag)
		{
			vert->tag = GL_TRUE;
			do{
				if (e->vert->gray <= 0)
				{
					marchingcube(e->vert, e->next, vlist,function);
				}
				else
				{

					/*GLfloat x = (vert->pos.x - e->vert->pos.x) == 0 ? vert->pos.x : -9999.99;
					GLfloat y = (vert->pos.y - e->vert->pos.y) == 0 ? vert->pos.y : -9999.99;
					GLfloat z = (vert->pos.z - e->vert->pos.z) == 0 ? vert->pos.z : -9999.99;

					glm::vec3 pos;

					GLfloat result = 0.0f;
					function(x, y, z, result);

					pos = glm::vec3(vert->pos.x>0 ? fabs(x) : -fabs(x), vert->pos.y>0 ? fabs(y) : -fabs(y), vert->pos.z>0 ? fabs(z) : -fabs(z));
*/
					glm::vec3 pos = (vert->pos * fabs(e->vert->gray) + e->vert->pos * fabs(vert->gray)) / (fabs(vert->gray)+ fabs(e->vert->gray));

					HE_vert* vert0 = insert_vertex(pos);
					vlist->push_back(vert0);
				}
				e = e->pair->next;
			} while (e != edge);
		}
	}


	GLboolean Mesh::load_obj(const char* fins)
	{
#if	_MSC_VER > 1310
		FILE *m_pFile;
		fopen_s(&m_pFile, fins, "r");
#else
		FILE *m_pFile = fopen(fins, "r");
#endif

		try
		{
			//read vertices
			fseek(m_pFile, 0, SEEK_SET);
			char pLine[512];
			while (fgets(pLine, 512, m_pFile))
			{
				if (pLine[0] == 'v'&&pLine[1] != 't'&& pLine[1] != 'n')
				{
					float x, y, z;
#if	_MSC_VER > 1310
					if (sscanf_s(pLine, "v %e %e %e", &x, &y, &z) == 3)
#else
					if (sscanf(pLine, "v %e %e %e", &x, &y, &z) == 3)
#endif
					{
						glm::vec3 nvv(x, y, z);
						insert_vertex(nvv);
					}
				}
			}

		//	read vertex normals
			/*fseek(m_pFile, 0, SEEK_SET);
			VERTEX_ITER viter = vertices_list->begin();
			while (fgets(pLine, 512, m_pFile))
			{
				char *pTmp = pLine;
				if (pTmp[0] == 'v' && pTmp[1] == 'n')
				{
					float x, y, z;
#if	_MSC_VER > 1310
					if (sscanf_s(pLine, "v %e %e %e", &x, &y, &z) == 3)
#else
					if (sscanf(pLine, "v %e %e %e", &x, &y, &z) == 3)
#endif
					{
						glm::vec3 nvv(x, y, z);
						(*viter)->normal = nvv;
					}

					viter++;
				}
			}
*/
			//read facets

			char *tok;
			char temp[128];

			fseek(m_pFile, 0, SEEK_SET);
			bool has_curvature = false;

			//the first entry stores the number of doubles
			//the second entry stores the valid type (K1,K2,k1,k2))
			std::vector<std::pair<int, int> > eobjtag;
			std::pair<int, int> mpair;

			while (fgets(pLine, 512, m_pFile))
			{
				char *pTmp = pLine;
				char *pTok;
				if (pTmp[0] == 'f')
				{
					VERTEX_LIST s_faceid;

					tok = strtok_s(pLine, " ",&pTok);
					while ((tok = strtok_s(NULL, " ",&pTok)) != NULL)
					{
						strcpy_s(temp, tok);
						temp[strcspn(temp, "/")] = 0;
						int id = (int)strtol(temp, NULL, 10) - 1;
						HE_vert * hv = get_vertex(id);
						bool findit = false;
						for (int i = 0; i <(int)s_faceid.size(); i++)
						{
							if (hv == s_faceid[i])	//remove redundant vertex id if it exists
							{
								findit = true;
								break;
							}
						}
						if (findit == false && hv != NULL)
						{
							s_faceid.push_back(hv);
						}
					}
					if ((int)s_faceid.size() >= 3)
						insert_face(s_faceid);
				}
			}
		}


		catch (...)
		{
			
			fclose(m_pFile);
			return false;
		}


		fclose(m_pFile);


		return true;
	}

	void Mesh::setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), NULL , GL_STATIC_DRAW);


		void* data1=glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(data1, &this->vertices[0],vertices.size()*sizeof(GLfloat));
		glUnmapBuffer(GL_ARRAY_BUFFER);

		void* data2 = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(data2, &this->indices[0], indices.size()*sizeof(GLfloat));
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),(GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		glBindVertexArray(0);
	}
	void Mesh::DrawModel(Shader shader)
	{
		shader.Use();
		glBindVertexArray(this->VAO);
		glPrimitiveRestartIndex(this->vertices_list->size() + 1);

		//glPolygonMode(GL_FRONT, GL_LINE);
		glDrawElements(GL_TRIANGLE_FAN, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void Mesh::DrawLine(Shader shader)
	{
		shader.Use();
		glBindVertexArray(this->VAO);
		glPrimitiveRestartIndex(this->vertices_list->size() + 1);

		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_LINE_LOOP, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	glm::vec3  Mesh::update()
	{
		compute_all_normal();
		//获取顶点数据
		update_vertices();
		//获取索引数据
		update_indices();

		//
	//	return getCenter();
		return glm::vec3(0.0f);
	}

	Mesh* Mesh::Doo_Sabin()
	{

		Mesh* mesh=new Mesh();
		FACE_ITER fIter=faces_list->begin();
	//	std::map<HE_vert*,VERTEX_LIST> vertmap;
		std::map<HE_edge*, HE_vert*> edgemap1;
		std::map<HE_edge*, HE_vert*> edgemap2;


		EDGE_ITER eIter = this->edges_list->begin();
		for (; eIter != edges_list->end(); eIter++)
		{
			(*eIter)->tag = GL_FALSE;
		}


		for (; fIter < faces_list->end(); fIter++)
		{
			HE_face*  face = (*fIter);
			HE_edge* edge = face->edge;		
			VERTEX_LIST mvertexList;
			do{
				if (!edge->tag)
				{
					//判断该点是否在空面的边界上
					if (!isborder(edge->vert) || !isFaceBorder(edge))
					{
						glm::vec3 mpos = edge->vert->pos + face->GetCentroid();
						glm::vec3 pos = glm::vec3(mpos.x / 2, mpos.y / 2, mpos.z / 2);
						HE_vert* vert = mesh->insert_vertex(pos);
						//		vertmap[edge->vert].push_back(vert);
						edgemap1[edge] = vert;
						edgemap2[edge->next] = vert;

						edge->tag = GL_TRUE;
						edge = edge->next;
						mvertexList.push_back(vert);

					}
					else
					{
						glm::vec3 pos1, pos2;
						//判断哪条边在空面上
						if (isborder(edge->next->vert))
						{
							mesh->getBorderVerts(edge->vert, edge->next->vert, pos1, pos2);

							HE_vert* vert1 = mesh->insert_vertex(pos1);
							//		vertmap[edge->vert].push_back(vert);
							edgemap1[edge] = vert1;
							edgemap2[edge->next] = vert1;

							edge->tag = GL_TRUE;
							edge = edge->next;
							mvertexList.push_back(vert1);

							HE_vert* vert2 = mesh->insert_vertex(pos2);
							//		vertmap[edge->vert].push_back(vert);
							edgemap1[edge] = vert2;
							edgemap2[edge->next] = vert2;

							edge->tag = GL_TRUE;
							edge = edge->next;
							mvertexList.push_back(vert2);

						}
						else
						{
							edge = edge->prev;
						}
					}
				}
				else
				{
					if (edge == face->edge)
						break;
					else
						edge = edge->next;
				}
			} while (edge != face->edge);

			//F-face
			mesh->insert_face(mvertexList);
		}

		////V-face
		//VERTEX_ITER vIter = this->vertices_list->begin();
		//for (; vIter < this->vertices_list->end(); vIter++)
		//{
		//	mesh->insert_face(vertmap[(*vIter)]);
		//}

		//E-face
		eIter = this->edges_list->begin();
		PTR_EDGE_LIST elist=new EDGE_LIST;

		for (; eIter != edges_list->end(); eIter++)
		{
			(*eIter)->tag = GL_FALSE;
		}
		
		eIter = this->edges_list->begin();

		for (; eIter < this->edges_list->end(); eIter ++ )
		{
			
			if (!(*eIter)->pair->tag && !(*eIter)->tag && (*eIter)->face!=nullptr && (*eIter)->pair->face!=nullptr)
			{
				VERTEX_LIST eFaceVertexs;
				eFaceVertexs.push_back(edgemap1[(*eIter)]);
				eFaceVertexs.push_back(edgemap2[(*eIter)]);
				eFaceVertexs.push_back(edgemap1[(*eIter)->pair]);
				eFaceVertexs.push_back(edgemap2[(*eIter)->pair]);
				


				elist->push_back((*eIter));
				mesh->insert_face(eFaceVertexs);

				(*eIter)->tag = GL_TRUE;
				(*eIter)->pair->tag = GL_TRUE;
			}

		}

		removeVolid();//把空面的边连起来
		//V-face
		VERTEX_ITER vIter = this->vertices_list->begin();
		
		for (; vIter != this->vertices_list->end(); vIter++)
		{
				VERTEX_LIST vList;
				HE_edge* edge = (*vIter)->edge;
				do
				{
					if (edgemap2[edge]!=NULL)
						vList.push_back(edgemap2[edge]);
					edge = edge->pair->next;
				} while (edge != (*vIter)->edge);

				std::reverse(vList.begin(), vList.end());

				if (vList.size()>2)
					mesh->insert_face(vList);
				
		}
		return mesh;
	}

	glm::vec3 HE_face::GetCentroid()
	{
		glm::vec3 V(0.0f);
		HE_edge* he = edge;
		GLfloat i = 0.0f;
		do
		{
			V += he->vert->pos;
			he = he->next;
			i++;
		} while (he != edge);
		if (i > 2)
		{
			return V / i;
		}
		else
		{
			return glm::vec3(0.0f);
		}
	};

	glm::vec3 Mesh::getCenter()
	{
		FACE_ITER iter = faces_list->begin();
		glm::vec3 center = glm::vec3(0.0f);
		for (; iter != faces_list->end(); iter++)
		{
			center += (*iter)->GetCentroid();
		}
		return center / (GLfloat)faces_list->size();
	}

	void Mesh::translate(glm::vec3 pos)
	{
		VERTEX_ITER iter = vertices_list->begin();
		for (; iter != vertices_list->end(); iter++)
		{
			(*iter)->pos += pos;
		}
	}

	void Mesh::scale(GLfloat size)
	{
		VERTEX_ITER iter = vertices_list->begin();
		for (; iter != vertices_list->end(); iter++)
		{
			(*iter)->pos = (*iter)->pos*size;
		}
	}

	void Mesh::update_vertices()
	{
		VERTEX_ITER iter = vertices_list->begin();
		for (; iter < vertices_list->end(); iter++)
		{
			vertices.push_back((*iter)->pos.x);
			vertices.push_back((*iter)->pos.y);
			vertices.push_back((*iter)->pos.z);
			vertices.push_back((*iter)->normal.x);
			vertices.push_back((*iter)->normal.y);
			vertices.push_back((*iter)->normal.z);

		}
	}

	void Mesh::update_indices()
	{
		FACE_ITER iter = faces_list->begin();
		for (; iter < faces_list->end(); iter++)
		{
			HE_edge* edge = (*iter)->edge;
			//	cout << "First::::" << edge->vert->id<<endl;
			do
			{
				indices.push_back(edge->vert->id);
				edge = edge->next;
			} while (edge->vert->id != (*iter)->edge->vert->id);
			indices.push_back(vertices_list->size() + 1);
		}
	}

	bool Mesh::isborder(HE_vert* hv)
	{
		HE_edge* edge = hv->edge;
		do {
			if (edge == NULL || edge->pair->face == NULL || edge->face == NULL) {
				return true;
			}
			edge = edge->pair->next;

		} while (edge != hv->edge);

		return false;
	}

	bool Mesh::isFaceBorder(HE_edge* edge)
	{
		HE_edge* he = edge;
		do {
			if (edge == NULL || edge->pair->face == NULL || edge->face == NULL) {
				return true;
			}
			edge = edge->next;

		} while (edge != he);

		return false;
	}

	void Mesh::removeVolid()
	{
		EDGE_ITER iter = edges_list->begin();
		for (; iter != edges_list->end(); iter++)
		{
			if ((*iter)->prev == NULL)
			{
				insertNext((*iter));
			}
		}
	}

	void Mesh::insertNext(HE_edge* edge)
	{
		HE_edge* e = edge->pair;
		do{
			if (e->next == NULL)
			{
				edge->prev=e;
				e->next = edge;
				insertNext(e);
				break;
			}
			e = e->next->pair;

		} while (e != edge->pair);
	}

	void Mesh ::compute_faces_list_normal()
	{
		FACE_ITER fiter = faces_list->begin();

		for (; fiter != faces_list->end(); fiter++)
		{
			compute_perface_normal(*fiter);
		}
	}
	//--------------------------------------------------------------
	void Mesh::compute_perface_normal(HE_face * hf)
	{
		int i = 0;
		HE_edge * pedge = hf->edge;
		HE_edge * nedge = hf->edge->next;

		hf->normal = glm::vec3(0.0f);
		for (i = 0; i<hf->valence; i++) {

			//cross product
			HE_vert * p = pedge->vert;
			HE_vert * c = pedge->next->vert;
			HE_vert * n = nedge->next->vert;
			glm::vec3  pc, nc;
			pc = p->pos - c->pos;
			nc = n->pos - c->pos;

			hf->normal -= glm::cross(pc,nc);
			pedge = nedge;
			nedge = nedge->next;
			if (hf->valence == 3) {
				break;
			}
		}
		glm::normalize(hf->normal);

	}
	//--------------------------------------------------------------
	void Mesh::compute_vertices_list_normal()
	{
		VERTEX_ITER viter = vertices_list->begin();

		for (; viter != vertices_list->end(); viter++)
		{
			compute_pervertex_normal(*viter);
		}

	}
	//--------------------------------------------------------------
	void Mesh::compute_pervertex_normal(HE_vert * hv)
	{

		HE_edge * edge = hv->edge;

		if (edge == NULL) {
			hv->normal = glm::vec3(0.0f);
			return;
		}

		hv->normal = glm::vec3(0.0f);


		do {
			if (edge->face != NULL) {
				hv->normal += edge->face->normal;
			}

			edge = edge->pair->next;

		} while (edge != hv->edge && edge!=NULL );

		glm::normalize(hv->normal);
	}

	//--------------------------------------------------------------
	void Mesh::compute_all_normal()
	{
		compute_faces_list_normal();
		compute_vertices_list_normal();
	}

}

